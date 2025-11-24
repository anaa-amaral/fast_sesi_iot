                                                                                                                                                                                                                                                                                                                         #include <WiFi.h>
#include <WiFiClientSecure.h> //Permite a conexão segura com o broker MQTT
#include <PubSubClient.h>

//---Conexão com wifi e o broker--
const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

const char* BROKER = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int PORT = 8883;

const char* BROKER_USER = "Placa-2-Julia";
const char* BROKER_PASS = "123456abX";

//Declara os pinos
#define TRIG1 12
#define ECHO1 25
#define TRIG2 18
#define ECHO2 21
#define PINO_RGB 14
#define PINO_LED 4

const char* TOPICO_PUBLISH_1 = "Projeto/S2/Distancia1";
const char* TOPICO_PUBLISH_2 = "Projeto/S2/Distancia2";//Publico as duas distâncias (status dos sensores)
const char* TOPICO_SUBSCRIBE = "S1/iluminacao";//recebo o comando do S1
const char* TOPICO_ENVIO_S3   = "Projeto/S3/Controle";//Envia esse status para o S3

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);//Torna mais seguro 

long medirDistancia(int trigPin, int echoPin) {//dois pinos para medir a distância
  digitalWrite(trigPin, LOW);// Envia um pulso curto para iniciar a medição
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracao = pulseIn(echoPin, HIGH, 30000);//medir a duração em microssegundos
  long distancia = duracao * 0.034 / 2;//Calcula a distância em centímetros
  return distancia;
}

void callback(char* topic, byte* payload, unsigned int length) {//Função acionada automaticamente quando uma mensagem chega em um tópico assinado
  String mensagem;
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];//basicamente converte os elemnetos da array para string de texto
  }
  if (mensagem == "acender") {//se a mensagem recebida for acender o pino acende 
    digitalWrite(PINO_LED, HIGH);
  } else if (mensagem == "apagar") {//se a mensagem recebida for apagar o pino apaga
    digitalWrite(PINO_LED, LOW);
  }
  Serial.println(mensagem);//Mostra a mensagem
}

void conectaWiFi() {//Conexão com o wifi
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Conectado ao WiFi!");
}

void conectaMQTT() {//Conexão com o broker
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
  Serial.println("Conectado ao broker!");
}

void setup() {//Define os pinos como entrada e saída
  Serial.begin(115200);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);
  conectaWiFi();
  conectaMQTT();
}

void loop() {
  if (!mqtt.connected()) {//Verifica se a conexão MQTT foi perdida e tenta reconectar
    conectaMQTT();
  }
  long dist1 = medirDistancia(TRIG1, ECHO1);//Mede as duas distâncias
  long dist2 = medirDistancia(TRIG2, ECHO2);
  String msg1 = String(dist1);
  String msg2 = String(dist2);

  Serial.println(dist1);
  Serial.println(dist2);

  if (dist1 < 10) {
    mqtt.publish(TOPICO_PUBLISH_1, "objeto_proximo");//Se a ditância for menor que 10 ele publica objeto_proximo
    mqtt.publish(TOPICO_ENVIO_S3, "objeto_proximo");
  }
  if(dist2 > 10) {
    mqtt.publish(TOPICO_PUBLISH_2, "objeto_longe");//Se a ditância for maior que 10 ele publica objeto_longe
    mqtt.publish(TOPICO_ENVIO_S3, "objeto_longe");
  }
  
  delay(1000);
  mqtt.loop();
}
