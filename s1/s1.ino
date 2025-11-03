#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

#define PINO_LED 2

//constantes p/ broker
const String URL = "7aecec580ecf4e5cbac2d52b35eb85b9.s1.eu.hivemq.cloud";
const int PORT = 8883;
const String USR = "";
const String broker_user = "Placa-1-Gustavo";
const String broker_PASS = "123456abX";

const String MyTopic = "Gustavo_sla";
const String OtherTopic = "Ana";

const String MyTopic = "SA_S1_presenca1";

const String MyTopic = "SA_S1_servo1";

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

void setup() {
  pinMode(PINO_LED, OUTPUT); // define pino do LED como saída
  Serial.begin(115200);
  Serial.print("Conectado ao Wi-Fi");
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.print("\nConectado com sucesso!");
  client.setInsecure();
  Serial.print("Conectando ao broker");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "Trem";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(),USR.c_str(),broker_PASS.c_str());
    Serial.print(".");
    delay(200);

  }
  mqtt.subscribe(MyTopic.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nConecxao com sucesso ao broker!");
}

void loop() {
  String mensagem ="Gustavo: ";
  if(Serial.available()>0){
    mensagem += Serial.readStringUntil('\n');
    mqtt.publish(OtherTopic.c_str(),mensagem.c_str());
  }
  mqtt.loop();
  delay(1000);

}

void callback(char* topic, byte* payload, unsigned int length){
  String mensagem = "";
  for(int i = 0; i < length; i++){
    mensagem += (char)payload[i];//acender led
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);
  if(mensagem == "Ana: Acender"){
    digitalWrite(PINO_LED, HIGH);
  }else if (mensagem == "Ana: Apagar"){
    digitalWrite(PINO_LED, LOW);
  }
}