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


WidgetBridge bridgeBedroom(V10);

BLYNK_WRITE(V0)
{
  switch (param.asInt())
  {
  case 1:
    digitalWrite(BathAMP, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    delay(100);
    digitalWrite(MainAMP, LOW);
    digitalWrite(MainSpeakers, LOW);
    break;
  case 2:
    digitalWrite(MainAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    delay(100);
    digitalWrite(BathAMP, LOW);
    digitalWrite(BathSpeakers, LOW);
    break;
      case 3:
    digitalWrite(MainAMP, HIGH);
    digitalWrite(BathAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    delay(100);
    bridgeBedroom.virtualWrite(V1, 1);
    bridgeBedroom.virtualWrite(V2, 1);
    break;
  default:
    break;
  }
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  bridgeBedroom.setAuthToken(BedroomAUTH);
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
}
