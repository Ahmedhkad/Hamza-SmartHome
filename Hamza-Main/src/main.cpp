#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib

#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ArduinoOTA.h>

#include <RCSwitch.h> //for wireless RF433 control
RCSwitch mySwitch = RCSwitch();
 
const char *ssid = ssidWifi ;       // defined on secret.h
const char *password = passWifi;   // defined on secret.h
const char *mqtt_server = mqttURL; // defined on secret.h
const char *deviceName = "Hamza-Main";
WiFiClient espClient;
PubSubClient client(espClient);

StaticJsonDocument<100> doc;

int device;
int valuejson;

const uint16_t kIrLed = IRPin; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);         // Set the GPIO to be used to sending the message.
// data captured by IRrecvDumpV2.ino

int frontState;

int lightSwitch01;
int lightSwitch02;
int lightSwitch03;
int lightSwitch04;
int lightSwitch05;
int lightSwitch06;
int lightSwitch07;
int lightSwitch08;
int lightSwitch09;
int lightSwitch21;
int lightSwitch22;
int lightSwitch23;
int bluetoothButton;
int holdBtnTime=40;     //Hold time on IR button

boolean bluetoothState = false;

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
      digitalWrite(NVRPower, HIGH);
      client.publish("NVRPower", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(NVRPower, LOW);
      client.publish("NVRPower", "OFF");
    }
    break;

  case 2:
    if (valuejson == 1)
    {
      digitalWrite(MainSpeakers, HIGH);
      client.publish("MainSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(MainSpeakers, LOW);
      client.publish("MainSpeakers", "OFF");
    }
    break;

  case 3:
    if (valuejson == 1)
    {
      digitalWrite(BathSpeakers, HIGH);
      client.publish("BathSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BathSpeakers, LOW);
      client.publish("BathSpeakers", "OFF");
    }
    break;

    case 4:
    if (valuejson == 1)
    {
      digitalWrite(FrontSpeakers, HIGH);
      client.publish("FrontSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(FrontSpeakers, LOW);
      client.publish("FrontSpeakers", "OFF");
    }
    break;

    case 5:
    if (valuejson == 1)
    {
      digitalWrite(MainAMP, HIGH);
      client.publish("MainAMP", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(MainAMP, LOW);
      client.publish("MainAMP", "OFF");
    }
    break;

    case 6:
    if (valuejson == 1)
    {
      digitalWrite(BathAMP, HIGH);
      client.publish("BathAMP", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BathAMP, LOW);
      client.publish("BathAMP", "OFF");
    }
    break;

    case 7:
    if (valuejson == 1)
    {
      mySwitch.send(3158530, 24);
      client.publish("hamza-light1", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3159414, 24);
      client.publish("hamza-light1", "OFF");
    }
    break;

    case 8:
    if (valuejson == 1)
    {
      mySwitch.send(3158590, 24);
      client.publish("hamza-light2", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3159494, 24);
      client.publish("hamza-light2", "OFF");
    }
    break;

    case 9:
    if (valuejson == 1)
    {
      mySwitch.send(3158532, 24);
      client.publish("hamza-light3", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3159413, 24);
      client.publish("hamza-light3", "OFF");
    }
    break;

    case 0:
    if (valuejson == 1)
    {
      irsend.sendNEC(0xFF12ED, 32, 40);
      client.publish("hamza-blutooth", "ON40");
    }
    else if (valuejson == 0)
    {
       irsend.sendNEC(0xFF12ED, 32, 80);
       client.publish("hamza-blutooth", "ON80");
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
      client.publish("LifeTopic", "{\"device\":\"Hamza-Main\"}");
      // ... and resubscribe
      client.subscribe("Hamza-Main");
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
  pinMode(NVRPower, OUTPUT);
  pinMode(MainSpeakers, OUTPUT);
  pinMode(BathSpeakers, OUTPUT);
  pinMode(FrontSpeakers, OUTPUT);
  pinMode(MainAMP, OUTPUT);
  pinMode(BathAMP, OUTPUT);

  Serial.begin(115200); // debug print on Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ArduinoOTA.setHostname(mqttClient);
  ArduinoOTA.begin();

  irsend.begin();
  mySwitch.enableTransmit(RFPin); // GPIO15 D7 RF radio pin

  digitalWrite(MainSpeakers, HIGH); // Set all relays OFF (signal inverted)
  digitalWrite(BathSpeakers, HIGH);
  digitalWrite(FrontSpeakers, HIGH);
  digitalWrite(MainAMP, HIGH);
  digitalWrite(BathAMP, HIGH);
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
