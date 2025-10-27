
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient client;
PubSubClient mqtt(client);

#define PINO_LED 2

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String URL   = "test.mosquitto.org";
const int PORT     = 1883;
const String USR   = "";
const String broker_PASS  = "";
const String MyTopic = "HisabelGata";
const String OtherTopic = "JuliaLinda";



void setup() {
  Serial.begin(115200);
  Serial.println("Conectando ao WiFi");
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso");
  Serial.println("Conectando ao Broker");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "S3_";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(),USR.c_str(),broker_PASS.c_str());
    Serial.print(".");
    delay(200);
  }
  pinMode(PINO_LED, OUTPUT);
  mqtt.subscribe(MyTopic.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nConectado com sucesso ao broker!");
}

void loop() {
  String mensagem = "Hisabel: ";
  if (Serial.available()>0){
    mensagem += Serial.readStringUntil('\n');
    mqtt.publish(OtherTopic.c_str(),mensagem.c_str());
    Serial.println(mensagem);
  }
  mqtt.loop();
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int lenght){
  String mensagem = "";
  for(int i = 0; i < lenght; i ++){
    mensagem += (char)payload[i];
  }
  Serial.println("Recebido: ");
  Serial.println(mensagem);
  if(mensagem == "Julia: Acender"){
    digitalWrite(PINO_LED, HIGH);
  }else if(mensagem == "Julia: Apagar"){
    digitalWrite(PINO_LED, LOW);
  }
}


