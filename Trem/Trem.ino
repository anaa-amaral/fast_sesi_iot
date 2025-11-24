//Receber informações de velocidade enviadas por outro dispositivo
//Interpretar esses dados e converter em ações físicas

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure client;
PubSubClient mqtt(client);

// Atuadores
#define LED_VERMELHO 23
#define LED_VERDE 22

//constantes p/ broker e conexão
const String URL = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int PORT = 8883;
const String broker_user = "Placa-4-Ana";
const String broker_PASS = "123456abX";
const String MyTopic = "projeto/trem/velocidade"; // Tópico utilizado ( números inteiros )

// Conexão ao Wi-Fi
const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

void setup() {
  Serial.begin(115200);
  Serial.print("Conectado ao Wi-Fi");
  client.setInsecure();
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.print("\nConectado com sucesso!");
  Serial.print("Conectando ao broker");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "Trem";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(),broker_user.c_str(),broker_PASS.c_str());
    Serial.print(".");
    delay(200);
  }
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  mqtt.subscribe(MyTopic.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nConecxao com sucesso ao broker!");
}

void loop() {
  // leitura da mensagem
  if(Serial.available()>0){
    String mensagem = Serial.readStringUntil('\n');
    mqtt.publish(MyTopic.c_str(),mensagem.c_str());
  }
  mqtt.loop();
  delay(50);

}

void callback(char* topic, byte* payload, unsigned int length){
  String mensagem = "";
  for(int i = 0; i < length; i++){
    mensagem += (char)payload[i];//acender led
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);
  int val = mensagem.toInt(); 
  // Controle dos atuadores:
  if(val > 0){
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
  }else if(val < 0){
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
  }else{
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, LOW);
  }
}
