#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib

#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoOTA.h>
 
WiFiUDP ntpUDP;
// https://www.ntp-servers.net/servers.html
NTPClient timeClient(ntpUDP, "ntp1.ntp-servers.net", 10800, 610000);

int minuteNow;
int hourNow;

int hourStart = 16;
int minuteStart = 47;

int hourEnd = 7;
int minuteEnd = 41;

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
const long WifiDelayInterval = 6000; // interval to check wifi and mqtt

unsigned long timerMillis2 = 0;
const long interval = 9000; // interval at which to blink (milliseconds)


 void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 WiFi.hostname(deviceName); // DHCP Hostname (useful for finding device for static lease)
  // if (!WiFi.config(local_IP, gateway, subnet))
  // {
  //   Serial.println("STA Failed to configure");
  // }

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
  datajson = doc["data"].as<unsigned int>();
  Serial.print("device: ");
  Serial.print(device);
  Serial.print(" valuejson: ");
  Serial.print(valuejson);
  Serial.print(" data: ");
  Serial.println(datajson);

  switch (device)
  {
  case 1:
    if (valuejson == 1)
    {
      digitalWrite(NVRPower, LOW);
      client.publish("NVRPower", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(NVRPower, HIGH);
      client.publish("NVRPower", "OFF");
    }
    break;

  case 2:
    if (valuejson == 1)
    {
      digitalWrite(BedroomAmp, LOW);
      client.publish("BedroomAmp", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BedroomAmp, HIGH);
      client.publish("BedroomAmp", "OFF");
    }
    break;

  case 3:
    if (valuejson == 1)
    {
      digitalWrite(BedroomSpeakers, LOW);
      client.publish("BedroomSpeakers", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(BedroomSpeakers, HIGH);
      client.publish("BedroomSpeakers", "OFF");
    }
    break;

    case 5:
    hourStart = valuejson;
    minuteStart = datajson;
    client.publish("BedroomTimerStart", "OK");
    break;

  case 6:
    hourEnd = valuejson;
    minuteEnd = datajson;
    client.publish("BedroomTimerEnd", "OK");
    break;

  case 9:
    if (valuejson == 9)
    {
      timerEnable = true;
      client.publish("BedroomTimerEnable", "ON");
    }
    else if (valuejson == 5)
    {
      timerEnable = false;
      client.publish("BedroomTimerEnable", "OFF");
      client.publish("BedroomTimerStart", "OFF");
      client.publish("BedroomTimerEnd", "OFF");
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
      Serial.println("BedroomConnected");
      // Once connected, publish an announcement...
      client.publish("BedroomConnected", "online", true);
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
  pinMode(NVRPower, OUTPUT);

  digitalWrite(BedroomSpeakers,HIGH);
  digitalWrite(BedroomAmp, HIGH);

  Serial.begin(115200); // debug print on Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, mqttPORT);
  client.setCallback(callback);
   
 ArduinoOTA.setHostname(mqttClient);
  ArduinoOTA.setPort(otaPort);
  ArduinoOTA.setPassword(otaPass);

  ArduinoOTA.onError([](ota_error_t error)
                     {
    error_str = String(error);
    error_str.toCharArray(error_msg, error_str.length() + 1);
    client.publish("Error", error_msg); });
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
    client.publish("BedroomLifeTopic", buffer);
    // Serial.println(timeClient.getFormattedTime());  //for debug
  }

  if (currentMillis - timerMillis2 >= interval && timerEnable == true)
  {
    timerMillis2 = currentMillis;

    int minuteNow = timeClient.getMinutes();
    int hourNow = timeClient.getHours();

    if ((hourNow == hourStart) && (minuteNow == minuteStart))
    {
      digitalWrite(NVRPower, LOW);
      client.publish("NVRPower", "ON");
    }
    else if ((hourNow == hourEnd) && (minuteNow == minuteEnd))
    {
      digitalWrite(NVRPower, HIGH);
      client.publish("NVRPower", "OFF");
    }
  }

  client.loop();
}


 