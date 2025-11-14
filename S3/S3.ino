#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#define PINO_LED 2
#define TRIG 13
#define ECHO 12
#define PINO_SERVO 27
#define PINO_PRESENCA 14

WiFiClientSecure client;
PubSubClient mqtt(client);
Servo servo3;

const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

const char* BROKER_URL  = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int   BROKER_PORT = 8883;
const char* BROKER_USER = "Placa-3-Hisabel";
const char* BROKER_PASS = "123456abX";

const char* TOPIC_PUBLISH_PRESENCA   = "Projeto/S3/Presenca3";
const char* TOPIC_PUBLISH_OBJETO     = "Projeto/S3/Ultrassom3";


const char* TOPIC_SUBSCRIBE_COMANDOS = "Projeto/S3/Controle";

unsigned long lastPublish = 0;
int publishInterval = 3000;

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

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("Comando recebido: ");
  Serial.println(msg);

  if (msg == "Acender") digitalWrite(PINO_LED, HIGH);
  else if (msg == "Apagar") digitalWrite(PINO_LED, LOW);
  else if (msg == "objeto_proximo") servo3.write(90);
}

void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi conectado!");
}

void conectarMQTT() {
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  client.setInsecure();
  mqtt.setCallback(callback);

  while (!mqtt.connected()) {
    Serial.print("Conectando ao broker...");

    String clientId = "S3_" + String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str(), BROKER_USER, BROKER_PASS)) {
      Serial.println("Conectado!");

      mqtt.subscribe(TOPIC_SUBSCRIBE_COMANDOS);
      Serial.print("Subscrito em: ");
      Serial.println(TOPIC_SUBSCRIBE_COMANDOS);

    } else {
      Serial.print("Falha. Código: ");
      Serial.println(mqtt.state());
      delay(1500);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PINO_LED, OUTPUT);
  pinMode(PINO_PRESENCA, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  servo3.attach(PINO_SERVO);
  servo3.write(0);

  conectarWiFi();
  conectarMQTT();
}

void loop() {
  if (!mqtt.connected()) conectarMQTT();
  mqtt.loop();

  long distancia = medirDistancia(TRIG, ECHO);
  Serial.println(distancia);

  if (distancia > 0 && distancia < 10) {
    mqtt.publish(TOPIC_PUBLISH_OBJETO, "objeto_proximo");
  }

  unsigned long agora = millis();
  if (agora - lastPublish >= publishInterval) {
    lastPublish = agora;

    int presenca = digitalRead(PINO_PRESENCA);
    mqtt.publish(TOPIC_PUBLISH_PRESENCA, String(presenca).c_str());

    Serial.print("Presença publicada: ");
    Serial.println(presenca);
  }

  delay(20);
}
