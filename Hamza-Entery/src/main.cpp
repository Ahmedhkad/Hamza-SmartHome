#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib

#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <RCSwitch.h> //for wireless RF433 control
RCSwitch mySwitch = RCSwitch();

WiFiUDP ntpUDP;
// https://www.ntp-servers.net/servers.html
NTPClient timeClient(ntpUDP, "ntp1.ntp-servers.net", 10800, 600000);

int minuteNow;
int hourNow;

int hourStart = 16;
int minuteStart = 47;

int hourEnd = 7;
int minuteEnd = 41;

const char *ssid = ssidWifi;       // defined on secret.h
const char *password = passWifi;   // defined on secret.h
const char *mqtt_server = mqttURL; // defined on secret.h
const char *deviceName = mqttClient;

// IPAddress local_IP(192, 168, 1, 145);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);   // optional
// IPAddress secondaryDNS(8, 8, 4, 4); // optional

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

unsigned long timerMillis2 = 0;
const long interval = 8000; // interval at which to blink (milliseconds)

void setup_wifi()
{
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

  while (WiFi.status() != WL_CONNECTED)
  {
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
      digitalWrite(EnteryLight, LOW);
      client.publish("EnteryLight", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(EnteryLight, HIGH);
      client.publish("EnteryLight", "OFF");
    }
    break;

  case 2:
    if (valuejson == 1)
    {
      digitalWrite(OutsideLight, LOW);
      client.publish("OutsideLight", "ON");
    }
    else if (valuejson == 0)
    {
      digitalWrite(OutsideLight, HIGH);
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

  case 5:
    hourStart = valuejson;
    minuteStart = datajson;
    client.publish("EntryTimerStart", "OK");
    break;

  case 6:
    hourEnd = valuejson;
    minuteEnd = datajson;
    client.publish("EntryTimerEnd", "OK");
    break;

  case 9:
    if (valuejson == 9)
    {
      timerEnable = true;
      client.publish("EntryTimerEnable", "ON");
    }
    else if (valuejson == 5)
    {
      timerEnable = false;
      client.publish("EntryTimerEnable", "OFF");
      client.publish("EntryTimerStart", "OFF");
      client.publish("EntryTimerEnd", "OFF");
    }
    break;

  default:
    Serial.print("Err device in case-switch invalid.");
    client.publish("Error", "Entry case-switch invalid");
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
    if (client.connect(mqttClient, mqttName, mqttPASS))
    {
      Serial.println("EntryConnected");
      // Once connected, publish an announcement...
      client.publish("EntryConnected", "online", true);
      // ... and resubscribe
      client.subscribe("Hamza-Entry");
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
  pinMode(EnteryLight, OUTPUT);
  pinMode(OutsideLight, OUTPUT);

  Serial.begin(115200); // debug print on Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, mqttPORT);
  client.setCallback(callback);

  mySwitch.enableTransmit(RFPin); // GPIO15 D7 RF radio pin
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
    client.publish("EntryLifeTopic", buffer);
    // Serial.println(timeClient.getFormattedTime());  //for debug
  }

  if (currentMillis - timerMillis2 >= interval && timerEnable == true)
  {
    timerMillis2 = currentMillis;

    int minuteNow = timeClient.getMinutes();
    int hourNow = timeClient.getHours();

    if ((hourNow == hourStart) && (minuteNow == minuteStart))
    {
      digitalWrite(OutsideLight, LOW);
      client.publish("OutsideLight", "ON");
    }
    else if ((hourNow == hourEnd) && (minuteNow == minuteEnd))
    {
      digitalWrite(OutsideLight, HIGH);
      client.publish("OutsideLight", "OFF");
    }
  }

  client.loop();
}