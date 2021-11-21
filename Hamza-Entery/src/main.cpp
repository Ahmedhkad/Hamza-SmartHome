#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

char auth[] = EnteryAUTH;
// Your WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;
 int EnteryState;
 int OutsideState;

BLYNK_WRITE(V1) {
  EnteryState = param.asInt();
  digitalWrite(EnteryLight,!EnteryState);
}
BLYNK_WRITE(V2) {
  OutsideState = param.asInt();
  digitalWrite(OutsideLight,!OutsideState);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{

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
}

void loop()
{
  Blynk.run();
}


