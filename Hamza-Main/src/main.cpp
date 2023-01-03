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

const char *ssid = "Ahmedov" ;       // defined on secret.h
const char *password = "Ahmedov90";   // defined on secret.h
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

// void bluetoothIR()
// {
// <<<<<<< HEAD
//   // irsend.sendRaw(rawData, 71, 32); // Send a raw data capture at 38kHz.
//   // irsend.sendNEC(0xFF12ED, 32);   //Power
//   // delay(50);
//   irsend.sendNEC(0xFF12ED, 32, 40);
  
// =======
//   irsend.sendNEC(0xFF12ED, 32, 40); // Send a raw data capture at 38kHz.
// >>>>>>> e6e5d22f431ca0b017324e8ff174d3cb6eb5c3c2
// }
// void bluetoothON()
// {
//   if (bluetoothState == false)
//   {
//     bluetoothIR();
//     bluetoothState = true;
//   }
// }
// void bluetoothOFF()
// {
//   if (bluetoothState == true)
//   {
//     bluetoothIR();
//     bluetoothState = false;
    
//   }
// }

// BLYNK_WRITE(V0)
// {
//   switch (param.asInt())
//   {
//   case 1: // Main speakers ON
//     bluetoothON();
//     digitalWrite(BathAMP, HIGH);
//     digitalWrite(BathSpeakers, HIGH);
//     bridgeBedroom.virtualWrite(V1, 0);
//     bridgeBedroom.virtualWrite(V2, 0);
//     delay(100);
//     digitalWrite(MainAMP, LOW);
//     digitalWrite(MainSpeakers, LOW);
//     break;
//   case 2: // Bathroom speakers ON
//     bluetoothON();
//     digitalWrite(MainAMP, HIGH);
//     digitalWrite(MainSpeakers, HIGH);
//     bridgeBedroom.virtualWrite(V1, 0);
//     bridgeBedroom.virtualWrite(V2, 0);
//     delay(100);
//     digitalWrite(BathAMP, LOW);
//     digitalWrite(BathSpeakers, LOW);
//     break;
//   case 3: // Bedroom speakers ON
//     bluetoothON();
//     digitalWrite(MainAMP, HIGH);
//     digitalWrite(BathAMP, HIGH);
//     digitalWrite(MainSpeakers, HIGH);
//     digitalWrite(BathSpeakers, HIGH);
//     delay(100);
//     bridgeBedroom.virtualWrite(V1, 1);
//     bridgeBedroom.virtualWrite(V2, 1);
//     break;
//   case 4: // All ON
//     bluetoothON();
//     digitalWrite(MainAMP, LOW);
//     digitalWrite(BathAMP, LOW);
//     digitalWrite(MainSpeakers, LOW);
//     digitalWrite(BathSpeakers, LOW);
//     bridgeBedroom.virtualWrite(V1, 1);
//     bridgeBedroom.virtualWrite(V2, 1);
//     break;
//   case 5: // All OFF
//     bluetoothOFF();
//     digitalWrite(MainAMP, HIGH);
//     digitalWrite(BathAMP, HIGH);
//     digitalWrite(MainSpeakers, HIGH);
//     digitalWrite(BathSpeakers, HIGH);
//     bridgeBedroom.virtualWrite(V1, 0);
//     bridgeBedroom.virtualWrite(V2, 0);
//     break;
//   default:
//     break;
//   }
// }

// BLYNK_WRITE(V1) // Switch between front and roof's speakers in Hall
// {
//   frontState = param.asInt();
//   digitalWrite(FrontSpeakers, !frontState);
// }

// BLYNK_WRITE(V2) // Manual control Entery lights - light around house
// {
//   outsideState = param.asInt();
//   bridgeEntery.virtualWrite(V2, outsideState);
// }
// BLYNK_WRITE(V3) // Manual control Entery lights - Garage lights
// {
//   enteryState = param.asInt();
//   bridgeEntery.virtualWrite(V1, enteryState);
// }

// BLYNK_WRITE(V4) // Time Enable on Entery board
// {
//   bridgeEntery.virtualWrite(V6, param.asInt());
// }

// BLYNK_WRITE(V5) // Set Hold IR blutooth's power button
// {
//   holdBtnTime = param.asInt();
// }

// BLYNK_WRITE(V11)
// { // Wireless Switch 1
//   lightSwitch01 = param.asInt();
//   bridgeEntery.virtualWrite(V11, lightSwitch01);
// }

// BLYNK_WRITE(V12)
// { // Wireless Switch 1
//   lightSwitch02 = param.asInt();
//   bridgeEntery.virtualWrite(V12, lightSwitch02);
// }
// BLYNK_WRITE(V13) // Wireless Switch 3
// {
//   lightSwitch03 = param.asInt();
//   bridgeEntery.virtualWrite(V13, lightSwitch03);
// }
// BLYNK_WRITE(V14) // Wireless Switch 4
// {
//   lightSwitch04 = param.asInt();
//   bridgeEntery.virtualWrite(V14, lightSwitch04);
// }
// BLYNK_WRITE(V15) // Wireless Switch 5
// {
//   lightSwitch05 = param.asInt();
//   bridgeEntery.virtualWrite(V15, lightSwitch05);
// }
// BLYNK_WRITE(V16) // Wireless Switch 6
// {
//   lightSwitch06 = param.asInt();
//   bridgeEntery.virtualWrite(V16, lightSwitch06);
// }
// BLYNK_WRITE(V17) // Wireless Switch 7
// {
//   lightSwitch07 = param.asInt();
//   bridgeEntery.virtualWrite(V17, lightSwitch07);
// }
// BLYNK_WRITE(V18) // Wireless Switch 8
// {
//   lightSwitch08 = param.asInt();
//   bridgeEntery.virtualWrite(V18, lightSwitch08);
// }
// BLYNK_WRITE(V19) // Wireless Switch 9
// {
//   lightSwitch09 = param.asInt();
//   bridgeEntery.virtualWrite(V19, lightSwitch09);
// }

// BLYNK_WRITE(V20) // Wireless Switch 9
// {
//   bluetoothButton = param.asInt();
//   if( bluetoothButton == 1){
//     bluetoothON();
//   }
//   else if(bluetoothButton == 0){
//     bluetoothOFF();
//   }
// }

// BLYNK_WRITE(V21)
// { // Wireless Switch 9
//   lightSwitch21 = param.asInt();
//   if (lightSwitch09 == 1)
//   {
//     mySwitch.send(3158590, 24);
//   }
//   else
//   {
//     mySwitch.send(3159494, 24);
//   }
// }
// BLYNK_WRITE(V22)
// { // Wireless Switch 9
//   lightSwitch22 = param.asInt();
//   if (lightSwitch09 == 1)
//   {
//     mySwitch.send(3158530, 24);
//   }
//   else
//   {
//     mySwitch.send(3159414, 24);
//   }
// }
// BLYNK_WRITE(V23)
// { // Wireless Switch 9
//   lightSwitch23 = param.asInt();
//   if (lightSwitch09 == 1)
//   {
//     mySwitch.send(3159355, 24);
//   }
//   else
//   {
//     mySwitch.send(3159194, 24);
//   }
// }
// // This function is called every time the device is connected to the Blynk.Cloud
// BLYNK_CONNECTED()
// {
//   bridgeBedroom.setAuthToken(BedroomAUTH);
//   bridgeEntery.setAuthToken(EnteryAUTH);
// }


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
       
    }
    else if (valuejson == 0)
    {
      
    }
    break;

  case 3:
    if (valuejson == 1) // Rolling Screen UP
    {
       
    }
    else if (valuejson == 2) // Rolling Screen UP
    {
       
    }
    else if (valuejson == 0)
    {
      
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
