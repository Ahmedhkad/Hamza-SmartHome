#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

BLYNK_WRITE(V0)
{
  switch (param.asInt())
  {
  case 1:
    digitalWrite(BathAMP, LOW);
    digitalWrite(BathSpeakers, LOW);
    delay(100);
    digitalWrite(MainAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    break;
  case 2:
    digitalWrite(MainAMP, LOW);
    digitalWrite(MainSpeakers, LOW);
    delay(100);
    digitalWrite(BathAMP, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    break;
  default:
    break;
  }
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

  pinMode(MainSpeakers, OUTPUT);
  pinMode(BathSpeakers, OUTPUT);
  pinMode(FrontSpeakers, OUTPUT);
  pinMode(MainAMP, OUTPUT);
  pinMode(BathAMP, OUTPUT);
}

void loop()
{
  Blynk.run();
  timer.run();
}
