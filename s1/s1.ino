#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure client;
PubSubClient mqtt(client);

#define PINO_LED 2

// HiveMQ Cloud
const char* MQTT_SERVER = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "Placa-1-Gustavo";
const char* MQTT_PASS = "123456abX";

// Wi-Fi
const char* SSID = "FIESC_IOT_EDU";
const char* WIFI_PASS = "8120gv08";

// Tópicos MQTT
const char* TOPICO_UMIDADE = "S1/umidade";
const char* TOPICO_TEMPERATURA = "S1/temperatura";
const char* TOPICO_LED = "S1/iluminacao";

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Recebido no tópico ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(mensagem);

  if (mensagem == "acender") {
    digitalWrite(PINO_LED, HIGH);
    Serial.println("LED aceso!");
  } else if (mensagem == "apagar") {
    digitalWrite(PINO_LED, LOW);
    Serial.println("LED apagado!");
  }
}

void setup() {
  pinMode(PINO_LED, OUTPUT);
  Serial.begin(115200);

  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" conectado!");

  client.setInsecure(); // sem certificado
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);

  Serial.print("Conectando ao HiveMQ...");
  while (!mqtt.connected()) {
    String clientId = "ESP32-S1-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println(" conectado!");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }

  mqtt.subscribe(TOPICO_LED);
  Serial.println("Assinado no tópico de LED.");
}

void loop() {
  if (!mqtt.connected()) {
    mqtt.connect("ESP32-S1", MQTT_USER, MQTT_PASS);
    mqtt.subscribe(TOPICO_LED);
  }
  mqtt.loop();

  // Publica valores simulados
  int umidade = random(40, 70);
  int temperatura = random(20, 30);

  String msgUmidade = String(umidade);
  String msgTemperatura = String(temperatura);

  mqtt.publish(TOPICO_UMIDADE, msgUmidade.c_str());
  mqtt.publish(TOPICO_TEMPERATURA, msgTemperatura.c_str());

  Serial.println("Publicado:");
  Serial.println("  Umidade: " + msgUmidade);
  Serial.println("  Temperatura: " + msgTemperatura);
  delay(5000);
}
