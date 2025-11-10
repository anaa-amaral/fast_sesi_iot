#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#define PINO_LED 2
#define PINO_PRESENCA 27
#define PINO_PRESENCA 26
#define PINO_SERVO 18
#define PINO_SERVO 5

WiFiClientSecure client;
PubSubClient mqtt(client);
Servo servo3;

const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

const char* BROKER_URL  = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int   BROKER_PORT = 8883;
const char* BROKER_USER = "Placa-3-Hisabel";
const char* BROKER_PASS = "123456abX";
const char* TOPIC_PUBLISH_PRESENCA = "ProjetoSA/S3/Presenca3";
const char* TOPIC_SUBSCRIBE_COMANDOS = "ProjetoSA/S3/Comandos";

unsigned long lastPublish = 0;
int publishInterval = 3000;


void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("\nMensagem recebida no tópico ");
  Serial.println(topic);
  Serial.print("Conteúdo: ");
  Serial.println(mensagem);

  if (mensagem == "Acender") {
    digitalWrite(PINO_LED, HIGH);
  } 
  else if (mensagem == "Apagar") {
    digitalWrite(PINO_LED, LOW);
  } 
  else if (mensagem == "Abrir servo") {
    servo3.write(90);
  } 
  else if (mensagem == "Fechar servo") {
    servo3.write(0);
  }
}

void conectarWiFi() {
  Serial.print("Conectando ao WiFi ");
  Serial.println(SSID);
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
      Serial.println("Conectado ao broker HiveMQ!");
      mqtt.subscribe(TOPIC_SUBSCRIBE_COMANDOS);
      Serial.print("Inscrito em: ");
      Serial.println(TOPIC_SUBSCRIBE_COMANDOS);
    } else {
      Serial.print("Falha na conexão. Código: ");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(PINO_LED, OUTPUT);
  pinMode(PINO_PRESENCA, INPUT);
  servo3.attach(PINO_SERVO);

  conectarWiFi();
  conectarMQTT();
}


void loop() {
  if (!mqtt.connected()) {
    conectarMQTT();
  }
  mqtt.loop();

 
  unsigned long agora = millis();
  if (agora - lastPublish > publishInterval) {
    lastPublish = agora;
    int presenca = digitalRead(PINO_PRESENCA);
    String mensagem = String(presenca);
    mqtt.publish(TOPIC_PUBLISH_PRESENCA, mensagem.c_str());
    Serial.print("Presença publicada: ");
    Serial.println(mensagem);
  }
}
