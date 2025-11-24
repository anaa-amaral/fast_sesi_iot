#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

// ---------- PINOS ----------
const byte LDR_PIN = 34;
const byte LED_PIN = 19;

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ULTRASSÔNICO
#define TRIG_PIN 22
#define ECHO_PIN 23

// LED RGB
#define LED_R 14
#define LED_G 26
#define LED_B 25

// ---------- OBJETOS ----------
WiFiClientSecure client;
PubSubClient mqtt(client);

// ---------- MQTT ----------
const char* MQTT_SERVER = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int   MQTT_PORT   = 8883;
const char* MQTT_USER   = "Placa-1-Gustavo";
const char* MQTT_PASS   = "123456abX";

// ---------- WI-FI ----------
const char* SSID      = "FIESC_IOT_EDU";
const char* WIFI_PASS = "8120gv08";

// ---------- TÓPICOS ----------
const char* TOPICO_UMIDADE     = "S1/umidade";
const char* TOPICO_TEMPERATURA = "S1/temperatura";
const char* TOPICO_LED         = "S1/iluminacao";
const char* TOPICO_PRESENCA    = "S1/presenca";

// ---------- FUNÇÃO ULTRASSÔNICO ----------
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 30000);
  long distancia = (duracao * 0.034) / 2;
  return distancia;
}

// ---------- CALLBACK MQTT ----------
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;

  for (int i = 0; i < length; i++) mensagem += (char)payload[i];

  if (strcmp(topic, TOPICO_LED) == 0) {
    if (mensagem == "acender") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("acender");
    }
    else if (mensagem == "apagar"){
      digitalWrite(LED_PIN, LOW);
      Serial.println("apagar");
      }
  }
}

// ---------- CONECTA MQTT ----------
void conectaMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Conectando ao HiveMQ... ");

    String clientId = "ESP32-S1-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("Conectado!");
      mqtt.subscribe(TOPICO_LED);
    } else {
      Serial.println("Falhou. Tentando novamente...");
      delay(1000);
    }
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);

  // Saídas
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // Sensor DHT
  dht.begin();

  // Ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Wi-Fi
  Serial.print("Conectando ao Wi-Fi ");
  WiFi.begin(SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Conectado!");

  // MQTT
  client.setInsecure();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
  conectaMQTT();
}

// ---------- LOOP PRINCIPAL ----------
void loop() {

  if (!mqtt.connected()) conectaMQTT();
  mqtt.loop();

  // ----- LDR -----
  int leituraLDR = analogRead(LDR_PIN);
  Serial.print("LDR: ");
  Serial.println(leituraLDR);

  if (leituraLDR > 3500) {
    digitalWrite(LED_PIN, HIGH);
    mqtt.publish(TOPICO_LED, "acender");
  } else {
    digitalWrite(LED_PIN, LOW);
    mqtt.publish(TOPICO_LED, "apagar");
  }

  // ----- DHT -----
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  mqtt.publish(TOPICO_TEMPERATURA, String(temperatura).c_str());
  mqtt.publish(TOPICO_UMIDADE, String(umidade).c_str());

  Serial.print("Temp: ");
  Serial.println(temperatura);
  Serial.print("Umid: ");
  Serial.println(umidade);

  // ----- ULTRASSÔNICO -----
  long distancia = medirDistancia(TRIG_PIN, ECHO_PIN);

  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");

  if (distancia > 0 && distancia < 50) {
    mqtt.publish(TOPICO_PRESENCA, "objeto proximo");
    Serial.println("objeto proximo");
} else {
    mqtt.publish(TOPICO_PRESENCA, "nada detectado");
    Serial.println("nada detectado");
}

  Serial.println("--------------------");
  delay(1500);
}
