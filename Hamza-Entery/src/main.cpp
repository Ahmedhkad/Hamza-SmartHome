#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#include <RCSwitch.h> //for wireless RF433 control
RCSwitch mySwitch = RCSwitch();

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

BlynkTimer timer;

WidgetRTC rtc;
WidgetLED led(V5);


char auth[] = EnteryAUTH;
// Your WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;
int EnteryState;
int OutsideState;
int secondsNow;
int timerEnable = 1;
int setON = 64800;  // default time on is 18:00
int setOFF = 19800; // default time off is 05:30

int lightSwitch01;
int lightSwitch02;
int lightSwitch03;
int lightSwitch04;
int lightSwitch05;
int lightSwitch06;
int lightSwitch07;
int lightSwitch08;
int lightSwitch09;

BLYNK_WRITE(V1)
{
  if (timerEnable == 0)
  {

    EnteryState = param.asInt();
    digitalWrite(EnteryLight, !EnteryState);
    Serial.print("EnteryState = ");
    Serial.println(EnteryState);
  }
}
BLYNK_WRITE(V2)
{
  if (timerEnable == 0)
  {
    OutsideState = param.asInt();
    digitalWrite(OutsideLight, !OutsideState);
  }
}

BLYNK_WRITE(V3)
{
  setON = param.asInt();
}

BLYNK_WRITE(V4)
{
  setOFF = param.asInt();
}

BLYNK_WRITE(V6)
{
  timerEnable = param.asInt();
}

void outDoorLights()
{
  secondsNow = (hour() * 3600) + (minute() * 60) + second(); // count seconds to compare it with setON and setOFF
  if (timerEnable == 1)
  {
    if (secondsNow >= setON && secondsNow < setOFF)
    {
      digitalWrite(OutsideLight, LOW);
      led.on();
    }
    else
    {

      digitalWrite(OutsideLight, HIGH);
      led.off();
    }
  }
}

BLYNK_WRITE(V11)
{ // Wireless Switch 1
  lightSwitch01 = param.asInt();
  if (lightSwitch01 == 1)
  {
    mySwitch.send(3159510, 24);
  }
  else
  {
    mySwitch.send(3159514, 24);
  }
}
BLYNK_WRITE(V12)
{ // Wireless Switch 2
  lightSwitch02 = param.asInt();
  if (lightSwitch02 == 1)
  {
    mySwitch.send(3159520, 24);
  }
  else
  {
    mySwitch.send(3159524, 24);
  }
}
BLYNK_WRITE(V13)
{ // Wireless Switch 3
  lightSwitch03 = param.asInt();
  if (lightSwitch03 == 1)
  {
    mySwitch.send(3159530, 24);
  }
  else
  {
    mySwitch.send(3159534, 24);
  }
}
BLYNK_WRITE(V14)
{ // Wireless Switch 4
  lightSwitch04 = param.asInt();
  if (lightSwitch04 == 1)
  {
    mySwitch.send(3159540, 24);
  }
  else
  {
    mySwitch.send(3159544, 24);
  }
}
BLYNK_WRITE(V15)
{ // Wireless Switch 5
  lightSwitch05 = param.asInt();
  if (lightSwitch05 == 1)
  {
    mySwitch.send(3159550, 24);
  }
  else
  {
    mySwitch.send(3159554, 24);
  }
}
BLYNK_WRITE(V16)
{ // Wireless Switch 6
  lightSwitch06 = param.asInt();
  if (lightSwitch06 == 1)
  {
    mySwitch.send(3159560, 24);
  }
  else
  {
    mySwitch.send(3159564, 24);
  }
}
BLYNK_WRITE(V17)
{ // Wireless Switch 7
  lightSwitch07 = param.asInt();
  if (lightSwitch07 == 1)
  {
    mySwitch.send(3159570, 24);
  }
  else
  {
    mySwitch.send(3159574, 24);
  }
}
BLYNK_WRITE(V18)
{ // Wireless Switch 8
  lightSwitch08 = param.asInt();
  if (lightSwitch08 == 1)
  {
    mySwitch.send(3159580, 24);
  }
  else
  {
    mySwitch.send(3159584, 24);
  }
}
BLYNK_WRITE(V19)
{ // Wireless Switch 9
  lightSwitch09 = param.asInt();
  if (lightSwitch09 == 1)
  {
    mySwitch.send(3159590, 24);
  }
  else
  {
    mySwitch.send(3159594, 24);
  }
}
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Synchronize time on connection
  rtc.begin();
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  mySwitch.enableTransmit(RFPin); // GPIO15 D7 RF radio pin

  pinMode(EnteryLight, OUTPUT);
  pinMode(OutsideLight, OUTPUT);

  digitalWrite(EnteryLight, HIGH); // all relays off when startup
  digitalWrite(OutsideLight, HIGH);

  timer.setInterval(10000L, outDoorLights);
}

void loop()
{
  Blynk.run();
  timer.run();
}