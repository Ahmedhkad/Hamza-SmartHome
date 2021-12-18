#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Comment this out to disable prints and save space
// #define BLYNK_PRINT Serial

char auth[] = MainAUTH;
// Your WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

const uint16_t kIrLed = IRPin; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);         // Set the GPIO to be used to sending the message.
// data captured by IRrecvDumpV2.ino
uint16_t rawData[71] = {9036, 4526, 552, 562, 570, 562, 572, 562, 570, 562, 570, 562, 570, 562, 572, 562, 572, 562, 570, 1598, 666, 1712, 550, 1692, 572, 1692, 572, 1680, 584, 1692, 572, 1712, 550, 1636, 628, 562, 570, 562, 570, 562, 570, 1642, 620, 562, 570, 562, 570, 1662, 602, 562, 570, 1640, 626, 1676, 586, 1712, 550, 564, 570, 1648, 616, 1692, 570, 564, 570, 1710, 554, 39470, 9036, 2290, 552};
uint16_t rawData2[3] = {9034, 2290, 552};

int frontState;

int outsideState;
int enteryState;

int lightSwitch01;
int lightSwitch02;
int lightSwitch03;
int lightSwitch04;
int lightSwitch05;
int lightSwitch06;
int lightSwitch07;
int lightSwitch08;
int lightSwitch09;
int bluetoothButton;

boolean bluetoothState = false;

WidgetBridge bridgeBedroom(V10);
WidgetBridge bridgeEntery(V20);

void bluetoothIR()
{
  irsend.sendRaw(rawData, 71, 32); // Send a raw data capture at 38kHz.
  delay(150);

  for (int x = 0; x <= 20; x++)
  {
    irsend.sendRaw(rawData2, 3, 32);
    delay(150);
  }
}
void bluetoothON()
{
  if (bluetoothState == false)
  {
    bluetoothIR();
    bluetoothState = true;
    Blynk.virtualWrite(V20, 1);
  }
}
void bluetoothOFF()
{
  if (bluetoothState == true)
  {
    bluetoothIR();
    bluetoothState = false;
    Blynk.virtualWrite(V20, 0);
  }
}
BLYNK_WRITE(V0)
{
  switch (param.asInt())
  {
  case 1: // Main speakers ON
    bluetoothON();
    digitalWrite(BathAMP, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    delay(100);
    digitalWrite(MainAMP, LOW);
    digitalWrite(MainSpeakers, LOW);
    break;
  case 2: // Bathroom speakers ON
    bluetoothON();
    digitalWrite(MainAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    delay(100);
    digitalWrite(BathAMP, LOW);
    digitalWrite(BathSpeakers, LOW);
    break;
  case 3: // Bedroom speakers ON
    bluetoothON();
    digitalWrite(MainAMP, HIGH);
    digitalWrite(BathAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    delay(100);
    bridgeBedroom.virtualWrite(V1, 1);
    bridgeBedroom.virtualWrite(V2, 1);
    break;
  case 4: // All ON
    bluetoothON();
    digitalWrite(MainAMP, LOW);
    digitalWrite(BathAMP, LOW);
    digitalWrite(MainSpeakers, LOW);
    digitalWrite(BathSpeakers, LOW);
    bridgeBedroom.virtualWrite(V1, 1);
    bridgeBedroom.virtualWrite(V2, 1);
    break;
  case 5: // All OFF
    bluetoothOFF();
    digitalWrite(MainAMP, HIGH);
    digitalWrite(BathAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    break;
  default:
    break;
  }
}

BLYNK_WRITE(V1) // Switch between front and roof's speakers in Hall
{
  frontState = param.asInt();
  digitalWrite(FrontSpeakers, !frontState);
}

BLYNK_WRITE(V2) // Manual control Entery lights - light around house
{
  outsideState = param.asInt();
  bridgeEntery.virtualWrite(V2, outsideState);
}
BLYNK_WRITE(V3) // Manual control Entery lights - Garage lights
{
  enteryState = param.asInt();
  bridgeEntery.virtualWrite(V1, enteryState);
}

BLYNK_WRITE(V4) // Time Enable on Entery board
{
  bridgeEntery.virtualWrite(V6, param.asInt());
}

BLYNK_WRITE(V11)
{ // Wireless Switch 1
  lightSwitch01 = param.asInt();
  bridgeEntery.virtualWrite(V11, lightSwitch01);
}

BLYNK_WRITE(V12)
{ // Wireless Switch 1
  lightSwitch02 = param.asInt();
  bridgeEntery.virtualWrite(V12, lightSwitch02);
}
BLYNK_WRITE(V13) // Wireless Switch 3
{
  lightSwitch03 = param.asInt();
  bridgeEntery.virtualWrite(V13, lightSwitch03);
}
BLYNK_WRITE(V14) // Wireless Switch 4
{
  lightSwitch04 = param.asInt();
  bridgeEntery.virtualWrite(V14, lightSwitch04);
}
BLYNK_WRITE(V15) // Wireless Switch 5
{
  lightSwitch05 = param.asInt();
  bridgeEntery.virtualWrite(V15, lightSwitch05);
}
BLYNK_WRITE(V16) // Wireless Switch 6
{
  lightSwitch06 = param.asInt();
  bridgeEntery.virtualWrite(V16, lightSwitch06);
}
BLYNK_WRITE(V17) // Wireless Switch 7
{
  lightSwitch07 = param.asInt();
  bridgeEntery.virtualWrite(V17, lightSwitch07);
}
BLYNK_WRITE(V18) // Wireless Switch 8
{
  lightSwitch08 = param.asInt();
  bridgeEntery.virtualWrite(V18, lightSwitch08);
}
BLYNK_WRITE(V19) // Wireless Switch 9
{
  lightSwitch09 = param.asInt();
  bridgeEntery.virtualWrite(V19, lightSwitch09);
}

BLYNK_WRITE(V20) // Wireless Switch 9
{
  bluetoothButton = param.asInt();
  if( bluetoothButton == 1){
    bluetoothON();
  }
  else if(bluetoothButton == 0){
    bluetoothOFF();
  }
}
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  bridgeBedroom.setAuthToken(BedroomAUTH);
  bridgeEntery.setAuthToken(EnteryAUTH);
}

void setup()
{
  // Debug console
  // Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  irsend.begin();
  pinMode(MainSpeakers, OUTPUT);
  pinMode(BathSpeakers, OUTPUT);
  pinMode(FrontSpeakers, OUTPUT);
  pinMode(MainAMP, OUTPUT);
  pinMode(BathAMP, OUTPUT);

  digitalWrite(MainSpeakers, HIGH); // Set all relays OFF (signal inverted)
  digitalWrite(BathSpeakers, HIGH);
  digitalWrite(FrontSpeakers, HIGH);
  digitalWrite(MainAMP, HIGH);
  digitalWrite(BathAMP, HIGH);
}

void loop()
{
  Blynk.run();
}
