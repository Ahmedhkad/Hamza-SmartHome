#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib

#include <ArduinoJson.h>

#include <ArduinoOTA.h>

#include <RCSwitch.h> //for wireless RF433 control
RCSwitch mySwitch = RCSwitch();

const char *ssid = "Ahmedov" ;       // defined on secret.h
const char *password = "Ahmedov90";   // defined on secret.h
const char *mqtt_server = mqttURL; // defined on secret.h
const char *deviceName = "Hamza-Entry";
WiFiClient espClient;
PubSubClient client(espClient);

StaticJsonDocument<100> doc;

int device;
int valuejson;


 void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Test if parsing succeeds.
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  // Print the values to data types
  device = doc["device"].as<unsigned int>();
  valuejson = doc["value"].as<unsigned int>();
  Serial.print("device: ");
  Serial.print(device);
  Serial.print(" valuejson: ");
  Serial.println(valuejson);

  switch (device)
  {
  case 1:
    if (valuejson == 1)
    {
      digitalWrite(EnteryLight, HIGH);
      client.publish("EnteryLight", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(EnteryLight, LOW);
      client.publish("EnteryLight", "OFF");
    }
    break;

  case 2:
    if (valuejson == 1)
    {
      digitalWrite(OutsideLight, HIGH);
      client.publish("OutsideLight", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(OutsideLight, LOW);
      client.publish("OutsideLight", "OFF");
    }
    break;

  case 3:
    if (valuejson == 1)
    {
      mySwitch.send(3138542, 24);
      client.publish("hamza-light4", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3119513, 24);
      client.publish("hamza-light4", "OFF");
    }
    break;

    case 4:
    if (valuejson == 1)
    {
      mySwitch.send(3128232, 24);
      client.publish("hamza-light5", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3188413, 24);
      client.publish("hamza-light5", "OFF");
    }
    break;
 

     
  default:
    Serial.print("Err device in case-switch invalid.");
    break;
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttClient, mqttName, mqttPASS))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LifeTopic", "{\"device\":\"Hamza-Entry\"}");
      // ... and resubscribe
      client.subscribe("Hamza-Entry");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(EnteryLight, OUTPUT);
  pinMode(OutsideLight, OUTPUT);

  digitalWrite(EnteryLight, HIGH); // all relays off when startup
  digitalWrite(OutsideLight, HIGH);

  Serial.begin(115200); // debug print on Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  mySwitch.enableTransmit(RFPin); // GPIO15 D7 RF radio pin
 ArduinoOTA.setHostname(mqttClient);
  ArduinoOTA.begin();
}

void loop()
{
  ArduinoOTA.handle();

  if (!client.connected())
  {
    Serial.println("reconnecting ...");
    reconnect();
  }
  client.loop();
}
