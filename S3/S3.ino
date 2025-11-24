#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// -------------------- DEFINIÇÃO DE PINOS --------------------
#define PINO_LED 2
#define TRIG 26
#define ECHO 25
#define PINO_SERVO3 19       // Servo 3
#define PINO_SERVO4 18       // Servo 4
#define PINO_PRESENCA 14

// -------------------- OBJETOS MQTT E SERVO --------------------
WiFiClientSecure client;
PubSubClient mqtt(client);
Servo servo3;
Servo servo4;

// -------------------- CREDENCIAIS WI-FI --------------------
const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

// -------------------- BROKER MQTT --------------------
const char* BROKER_URL  = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int   BROKER_PORT = 8883;
const char* BROKER_USER = "Placa-3-Hisabel";
const char* BROKER_PASS = "123456abX";

// -------------------- TÓPICOS MQTT --------------------
const char* TOPIC_PUBLISH_PRESENCA   = "Projeto/S3/Presenca3";
const char* TOPIC_PUBLISH_OBJETO     = "Projeto/S3/Ultrassom3";
const char* TOPIC_PUBLISH_1 = "Projeto/S2/Distancia1";
const char* TOPIC_PUBLISH_2 = "Projeto/S2/Distancia2";

const char* TOPICO_SUBSCRIBE = "S1/iluminacao";

// -------------------- CONTROLE DE PUBLICAÇÃO --------------------
unsigned long lastPublish = 0;        // guarda o último instante em que publicou presença
int publishInterval = 3000;          // publica presença a cada 3 segundos

// -------------------- FUNÇÃO DE LEITURA DO ULTRASSOM --------------------
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 30000);              // mede o tempo do eco
  long distancia = (duracao * 0.034) / 2;                    // converte tempo = distância em cm
  return distancia;
}

// -------------------- CALLBACK MQTT --------------------
void callback(char* topic, byte* payload, unsigned int length) {

  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Tópico recebido: ");
  Serial.println(topic);
  Serial.print("Mensagem recebida: ");
  Serial.println(mensagem);

  // ---------- CONTROLE DO LED ----------
  if (String(topic) == TOPICO_SUBSCRIBE) {
    if (mensagem == "acender")  digitalWrite(PINO_LED, HIGH);
    if (mensagem == "apagar")   digitalWrite(PINO_LED, LOW);
  }

  // ---------- CONTROLE DO SERVO 3 ----------
  if (String(topic) == TOPIC_PUBLISH_1) {
    if (mensagem == "objeto_proximo") servo3.write(90);
    if (mensagem == "objeto_longe")   servo3.write(45);
  }

  // ---------- CONTROLE DO SERVO 4 ----------
  if (String(topic) == TOPIC_PUBLISH_2) {
    if (mensagem == "objeto_proximo") servo4.write(90);
    if (mensagem == "objeto_longe")   servo4.write(45);
  }
}


// -------------------- CONEXÃO COM WI-FI --------------------
void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi conectado!");
}

// -------------------- CONEXÃO COM BROKER MQTT --------------------
void conectarMQTT() {
  mqtt.setServer(BROKER_URL, BROKER_PORT);
 client.setInsecure();                // ignora certificado (requerido em alguns brokers)
 mqtt.setCallback(callback);          // registra função callback

 // mantém tentativa de conexão até conseguir
 while (!mqtt.connected()) {
    Serial.print("Conectando ao broker...");

 // ID aleatório para identificar a placa
 String clientId = "S3_" + String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str(), BROKER_USER, BROKER_PASS)) {
      Serial.println("Conectado!");

      // ---------- Assinaturas MQTT ----------
      mqtt.subscribe(TOPICO_SUBSCRIBE);         // LED
      mqtt.subscribe(TOPIC_PUBLISH_1);          // servo3
      mqtt.subscribe(TOPIC_PUBLISH_2);          // servo4
      mqtt.subscribe("Projeto/S3/Controle");    // comandos gerais


      Serial.print("Subscrito em: ");
      Serial.println(TOPICO_SUBSCRIBE);

    } else {
      Serial.print("Falha. Código: ");
      Serial.println(mqtt.state());
      delay(1500);
    }
  }
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);

  // Configuração de pinos
  pinMode(PINO_LED, OUTPUT);
  pinMode(PINO_PRESENCA, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  servo3.attach(PINO_SERVO3);
  servo4.attach(PINO_SERVO4);

  servo3.write(0);
  servo4.write(0);


   conectarWiFi();       // conecta ao Wi-Fi
   conectarMQTT();       // conecta ao broker MQTT
}

// -------------------- LOOP PRINCIPAL --------------------
void loop() {

// garante que o MQTT sempre esteja conectado
  if (!mqtt.connected()) conectarMQTT();
  mqtt.loop();      // mantém comunicação MQTT ativa

  // ---------- LEITURA DO SENSOR ULTRASSÔNICO ----------
  long distancia = medirDistancia(TRIG, ECHO);
  Serial.println(distancia);

  // Publica de acordo com a distância medida
  if (distancia > 0 && distancia < 10) {
    mqtt.publish(TOPIC_PUBLISH_1, "objeto_proximo");
  } else if (distancia > 10) {
    mqtt.publish(TOPIC_PUBLISH_2, "objeto_longe");
  }

  // ---------- ENVIO PERIÓDICO DA PRESENÇA (a cada 3s) ----------
  unsigned long agora = millis();
  if (agora - lastPublish >= publishInterval) {
    lastPublish = agora;
    int presenca = digitalRead(PINO_PRESENCA);
    mqtt.publish(TOPIC_PUBLISH_PRESENCA, String(presenca).c_str());
    Serial.print("Presença publicada: ");
    Serial.println(presenca);
  }

  delay(20);   // pequeno delay para estabilidade de leitura
}
