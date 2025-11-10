#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

const char* BROKER = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int PORT = 8883;

const char* BROKER_USER = "Placa-2-Julia";
const char* BROKER_PASS = "123456abX";

#define TRIG1 12
#define ECHO1 25
#define TRIG2 18
#define ECHO2 21
#define PINO_RGB 14

const char* TOPICO_PUBLISH_1 = "Projeto/S2/Distancia1";
const char* TOPICO_PUBLISH_2 = "Projeto/S2/Distancia2";
const char* TOPICO_SUBSCRIBE = "Projeto/S2/RGB";
const char* TOPICO_ENVIO_S3   = "Projeto/S3/Controle";

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);

long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracao = pulseIn(echoPin, HIGH, 30000);
  long distancia = duracao * 0.034 / 2;
  return distancia;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  if (mensagem == "ligar") {
    digitalWrite(PINO_RGB, HIGH);
  } else if (mensagem == "desligar") {
    digitalWrite(PINO_RGB, LOW);
  }
}

void conectaWiFi() {
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void conectaMQTT() {
  mqtt.setServer(BROKER, PORT);
  mqtt.setCallback(callback);
  espClient.setInsecure();
  while (!mqtt.connected()) {
    String clientId = "S2_" + String(random(0xffff), HEX);
    if (mqtt.connect(clientId.c_str(), BROKER_USER, BROKER_PASS)) {
      mqtt.subscribe(TOPICO_SUBSCRIBE);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(PINO_RGB, OUTPUT);
  digitalWrite(PINO_RGB, LOW);
  conectaWiFi();
  conectaMQTT();
}

void loop() {
  if (!mqtt.connected()) {
    conectaMQTT();
  }
  mqtt.loop();
  long dist1 = medirDistancia(TRIG1, ECHO1);
  long dist2 = medirDistancia(TRIG2, ECHO2);
  String msg1 = String(dist1);
  String msg2 = String(dist2);
  mqtt.publish(TOPICO_PUBLISH_1, msg1.c_str());
  mqtt.publish(TOPICO_PUBLISH_2, msg2.c_str());
  if (dist1 < 10 || dist2 < 10) {
    mqtt.publish(TOPICO_ENVIO_S3, "objeto_proximo");
  } else {
    mqtt.publish(TOPICO_ENVIO_S3, "area_livre");
  }
  delay(2000);
}
