#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib

#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <RCSwitch.h> //for wireless RF433 control
RCSwitch mySwitch = RCSwitch();
 
 WiFiUDP ntpUDP;
// https://www.ntp-servers.net/servers.html
NTPClient timeClient(ntpUDP, "ntp1.ntp-servers.net", 10200, 600000);

const char *ssid = ssidWifi ;       // defined on secret.h
const char *password = passWifi;   // defined on secret.h
const char *mqtt_server = mqttURL; // defined on secret.h
const char *deviceName = mqttClient;

WiFiClient espClient;
PubSubClient client(espClient);

StaticJsonDocument<150> doc;
StaticJsonDocument<300> updater;

long unsigned int timestamp;

int device;
int valuejson;
int datajson;

bool timerEnable = true;

int count = 0;

String timeStr;
char timeChar[300];
String error_str;
char error_msg[300];

unsigned long WifiDelayMillis = 0;
const long WifiDelayInterval = 5000; // interval to check wifi and mqtt

// unsigned long timerMillis2 = 0;
// const long interval = 8000; // interval at which to blink (milliseconds)


const uint16_t kIrLed = IRPin; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);         // Set the GPIO to be used to sending the message.
// data captured by IRrecvDumpV2.ino
 

 void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(deviceName); 
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
  case 2:
    if (valuejson == 1)
    {
      digitalWrite(MainSpeakers, LOW);
      client.publish("Hamza/MainSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(MainSpeakers, HIGH);
      client.publish("Hamza/MainSpeakers", "OFF");
    }
    break;

  case 3:
    if (valuejson == 1)
    {
      digitalWrite(BathSpeakers, LOW);
      client.publish("Hamza/BathSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BathSpeakers, HIGH);
      client.publish("Hamza/BathSpeakers", "OFF");
    }
    break;

    case 4:
    if (valuejson == 1)
    {
      digitalWrite(FrontSpeakers, LOW);
      client.publish("Hamza/FrontSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(FrontSpeakers, HIGH);
      client.publish("Hamza/FrontSpeakers", "OFF");
    }
    break;

    case 5:
    if (valuejson == 1)
    {
      digitalWrite(MainAMP, LOW);
      client.publish("Hamza/MainAMP", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(MainAMP, HIGH);
      client.publish("Hamza/MainAMP", "OFF");
    }
    break;

    case 6:
    if (valuejson == 1)
    {
      digitalWrite(BathAMP, LOW);
      client.publish("Hamza/BathAMP", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BathAMP, HIGH);
      client.publish("Hamza/BathAMP", "OFF");
    }
    break;

    case 7:
    if (valuejson == 1)
    {
      mySwitch.send(3158530, 24);
      client.publish("Hamza/hamza-light1", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3159414, 24);
      client.publish("Hamza/hamza-light1", "OFF");
    }
    break;

    case 8:
    if (valuejson == 1)
    {
      mySwitch.send(3158590, 24);
      client.publish("Hamza/hamza-light2", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3159494, 24);
      client.publish("Hamza/hamza-light2", "OFF");
    }
    break;

    case 9:
    if (valuejson == 1)
    {
      mySwitch.send(3158532, 24);
      client.publish("Hamza/hamza-light3", "ON");
    }
    else if (valuejson == 0)
    {
      mySwitch.send(3159413, 24);
      client.publish("Hamza/hamza-light3", "OFF");
    }
    break;

    case 0:
    if (valuejson == 1)
    {
      irsend.sendNEC(0xFF12ED, 32, 40);
      client.publish("Hamza/hamza-blutooth", "ON40");
    }
    else if (valuejson == 0)
    {
       irsend.sendNEC(0xFF12ED, 32, 80);
       client.publish("Hamza/hamza-blutooth", "ON80");
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
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttClient, mqttName, mqttPASS, mqttWillTopic ,0 , true,"offline"))
    {
      Serial.println("MainConnected");
      // Once connected, publish an announcement...
      client.publish("Hamza/MainConnected", "online", true);
      // ... and resubscribe
      client.subscribe("Hamza/Hamza-Main");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      // delay(5000);
      count = count + 1;
    }
  }
}

void setup()
{
  pinMode(MainSpeakers, OUTPUT);
  pinMode(BathSpeakers, OUTPUT);
  pinMode(FrontSpeakers, OUTPUT);
  pinMode(MainAMP, OUTPUT);
  pinMode(BathAMP, OUTPUT);

  Serial.begin(115200); // debug print on Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, mqttPORT);
  client.setCallback(callback);

  irsend.begin();
  mySwitch.enableTransmit(RFPin); // GPIO15 D7 RF radio pin

  digitalWrite(MainSpeakers, HIGH); // Set all relays OFF (signal inverted)
  digitalWrite(BathSpeakers, HIGH);
  digitalWrite(FrontSpeakers, HIGH);
  digitalWrite(MainAMP, HIGH);
  digitalWrite(BathAMP, HIGH);

    ArduinoOTA.setHostname(mqttClient);
  ArduinoOTA.setPort(otaPort);
  ArduinoOTA.setPassword(otaPass);

  ArduinoOTA.onError([](ota_error_t error)
                     {
    error_str = String(error);
    error_str.toCharArray(error_msg, error_str.length() + 1);
    client.publish("Hamza/Error", error_msg); });
  timeClient.begin();
  timeClient.update();
  ArduinoOTA.begin();
}

void loop()
{
  ArduinoOTA.handle();
  unsigned long currentMillis = millis();

  if (currentMillis - WifiDelayMillis >= WifiDelayInterval)
  {
    WifiDelayMillis = currentMillis;
    if (!client.connected())
    {
      Serial.println("reconnecting ...");
      reconnect();
    }
    timeClient.update();
    char buffer[300];
    updater["Disconnected"] = count;
    updater["time"] = timeClient.getFormattedTime();
    serializeJson(updater, buffer);
    client.publish("Hamza/MainLifeTopic", buffer);
    // Serial.println(timeClient.getFormattedTime());  //for debug
  }

  client.loop();
}
