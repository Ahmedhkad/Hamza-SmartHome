#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

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
 int setON = 68400;  // default time on is 20:00
 int setOFF = 25200; // default time off is 07:00

BLYNK_WRITE(V1) {
  EnteryState = param.asInt();
  digitalWrite(EnteryLight,!EnteryState);
}
BLYNK_WRITE(V2) {
  OutsideState = param.asInt();
  digitalWrite(OutsideLight,!OutsideState);
}


BLYNK_WRITE(V3) {
  setON = param.asInt();
}

BLYNK_WRITE(V4) {
  setOFF = param.asInt();
}

void outDoorLights(){
  secondsNow = (hour()*3600) + (minute()*60) + second();  //count seconds to compare it with setON and setOFF

  if( secondsNow >= setON && secondsNow < setOFF){
    digitalWrite(OutsideLight,LOW);
    led.on();
  }
  else{

    digitalWrite(OutsideLight,HIGH);
    led.off();
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

  pinMode(EnteryLight, OUTPUT);
  pinMode(OutsideLight, OUTPUT);

  digitalWrite(EnteryLight, HIGH);  //all relays off when startup
  digitalWrite(OutsideLight,HIGH);

  timer.setInterval(10000L, outDoorLights);
}

void loop()
{
  Blynk.run();
  timer.run();
}