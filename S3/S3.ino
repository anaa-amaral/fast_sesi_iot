<<<<<<< HEAD
#include <WiFi.h>
#include <PubSubClient.h>
=======
 #include <WiFi.h>

 const String SSID = "FIESC_IOT_EDU";
 const String PASS = "8120gv08";

 void setup() {
  Serial.begin(115200);
  Serial.println("Conectando ao WiFI");
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso!");
>>>>>>> 16d3aa6fdfd85ac42ce3bad40cc9bff4a927cc00

WiFiClient client;
PubSubClient mqtt(client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String URL   = "test.mosquitto.org";
const int PORT     = 1883;
const String USR   = "";
const String broker_PASS  = "";
const String Topic = "DSM1";



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
  Serial.println("\nConectado com sucesso ao broker!");
}

void loop() {
  String mensagem = "Hisabel: ";
  mensagem += "Olá";

  mqtt.publish(Topic.c_str(),mensagem.c_str());
  mqtt.loop();
  delay(1000);
}
