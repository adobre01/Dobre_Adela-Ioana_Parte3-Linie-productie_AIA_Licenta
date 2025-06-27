#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// Configurare WiFi

const char* ssid = "MERCUSYS_FA2C";
const char* password = "camin22106a";
//const char* ssid = "Adela";
//const char* password = "rc#2fngs";

// CONFIGURARE MQTT

const char* mqtt_server = "192.168.1.102"; // Punem IP-ul 
//const char* mqtt_server = "172.20.10.3";
const int mqtt_port = 1883;
const char* mqtt_topic = "test";
const char*mqtt_username="";
const char*mqtt_password="";

WiFiClient espClient;
PubSubClient client(espClient);

// SoftwareSerial (D6 = RX, D5 = TX)
SoftwareSerial arduinoSerial(D6, D5);

// Functie conectare WiFi
void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Conectare la retea: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectat la WiFi!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

// Callback MQTT
void callback(char* topic, byte* payload, unsigned int length) 
{
  String mesaj = "";
  for (unsigned int i = 0; i < length; i++) 
  {
    mesaj += (char)payload[i];
  }
  mesaj.trim();

  Serial.print("MQTT primit: ");
  Serial.println(mesaj);

  // Trimite mesaj către Arduino dacă este valid
  if (mesaj == "Obiect 1 a fost amplasat!") 
  {
    arduinoSerial.println(mesaj);
    Serial.println("Trimis către Arduino.");
  }
}

// Conectare la broker MQTT
void reconnect() 
{
  while (!client.connected()) 
  {
    Serial.print("Conectare la MQTT...");
    if (client.connect("PlusivoESP8266",mqtt_username,mqtt_password)) 
    {
      Serial.println(" Conectat la broker!");
      client.subscribe(mqtt_topic);
    } 
    else 
    {
      Serial.print("Eroare: ");
      Serial.print(client.state());
      Serial.println(" – reincercare în 5 secunde");
      delay(5000);
    }
  }
}

void setup() 
{
  Serial.begin(9600);
  arduinoSerial.begin(9600);
  delay(1000);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  // Trimite date de la Arduino → MQTT (dacă vrei să trimiți și înapoi)
 if(arduinoSerial.available())
  {
    String input=arduinoSerial.readStringUntil('\n');
    input.trim();


    int IndexVirgula=input.indexOf(',');

    String culoare="";
    int numar_obiecte=0;

    if(IndexVirgula>0)
    {
       culoare=input.substring(0,IndexVirgula);
       numar_obiecte=input.substring(IndexVirgula+1).toInt();
    }


    // Construim JSON
    String mesajJSON = "{";
    mesajJSON += "\"Type\":\"obiecte_banda\",";
    mesajJSON += "\"culoare\":\"" + culoare + "\",";
    mesajJSON += "\"numar_obiecte\":" + String(numar_obiecte);
    mesajJSON += "}";

    Serial.print("Trimit mesaj JSON: ");
    Serial.println(mesajJSON);

    client.publish(mqtt_topic, mesajJSON.c_str());

  }

}
