#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

//constantes p/ broker
const String URL = "test.mosquitto.org";
const int PORT = 1883;
const String USR = "";
const String broker_PASS = "";
const String Topic = "DSM1";



const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

void setup() {
  Serial.begin(115200);
  Serial.print("Conectado ao Wi-Fi");
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
    mqtt.connect(ID.c_str(),USR.c_str(),broker_PASS.c_str());
    Serial.print(".");
    delay(200);

  }
  Serial.println("\nConecxao com sucesso ao broker!");
}

void loop() {
  String mensagem ="Ana: ";
  mensagem += "olá";

  mqtt.publish(Topic.c_str(),mensagem.c_str());
  mqtt.loop();
  delay(1000);

}
