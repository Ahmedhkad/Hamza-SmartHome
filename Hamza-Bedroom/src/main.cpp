#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib

#include <ArduinoJson.h>

#include <ArduinoOTA.h>
 

const char *ssid = ssidWifi ;       // defined on secret.h
const char *password = passWifi;   // defined on secret.h
const char *mqtt_server = mqttURL; // defined on secret.h
const char *deviceName = "Hamza-Bedroom";
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
      digitalWrite(BedroomAmp, HIGH);
      client.publish("BedroomAmp", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BedroomAmp, LOW);
      client.publish("BedroomAmp", "OFF");
    }
    break;

  case 2:
    if (valuejson == 1)
    {
      digitalWrite(BedroomSpeakers, HIGH);
      client.publish("BedroomSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BedroomSpeakers, LOW);
      client.publish("BedroomSpeakers", "OFF");
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
      client.subscribe("Hamza-Bedroom");
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
   pinMode(BedroomAmp, OUTPUT);
  pinMode(BedroomSpeakers, OUTPUT);

  digitalWrite(BedroomSpeakers,HIGH);
  digitalWrite(BedroomAmp, HIGH);

  Serial.begin(115200); // debug print on Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
   
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


 