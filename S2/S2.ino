 #include <WiFi.h>
 #include<PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);
#define PINO_LED 2

 const String SSID = "FIESC_IOT_EDU";
 const String PASS = "8120gv08";

const String URL = "test.mosquitto.org";
const int PORT = 1883;
const String USR = "";
const String Broker_PASS= "";
const String MyTopic = "JuliaLinda";
const String OtherTopic = "HisabelGata";




 void setup() {
  pinMode(PINO_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Conectando ao WiFI");
  WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso!");
  Serial.println("Conectando ao Broker");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "S2_";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(), USR.c_str(), Broker_PASS.c_str());
    Serial.print(".");
    delay(200);
  }
  mqtt.subscribe(MyTopic.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nCOnectado com sucesso ao broker!");

}


void loop() {
  String mensagem = "Julia: ";
  if (Serial.available()>0){
    mensagem += Serial.readStringUntil('\n');
    mqtt.publish(OtherTopic.c_str(), mensagem.c_str());
  }
  mqtt.loop();
  delay(1000);
}

void callback (char* topic, byte* payload, unsigned int lenght){
  String mensagem = "";
  for(int i = 0 ;i< lenght; i++){
    mensagem += (char)payload[i];

  }
  Serial.print("Recebido:");
  Serial.println(mensagem);
  if(mensagem == "Hisabel: Acender"){
    digitalWrite(PINO_LED, HIGH);
  }else if(mensagem == "Hisabel: Apagar"){
    digitalWrite(PINO_LED, LOW);
  }
}

