#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

char auth[] = MainAUTH;
// Your WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

int frontState;

int outsideState;
int enteryState;

WidgetBridge bridgeBedroom(V10);
WidgetBridge bridgeEntery(V20);

BLYNK_WRITE(V0)
{
  switch (param.asInt())
  {
  case 1: // Main speakers ON
    digitalWrite(BathAMP, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    delay(100);
    digitalWrite(MainAMP, LOW);
    digitalWrite(MainSpeakers, LOW);
    break;
  case 2: // Bathroom speakers ON
    digitalWrite(MainAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    bridgeBedroom.virtualWrite(V1, 0);
    bridgeBedroom.virtualWrite(V2, 0);
    delay(100);
    digitalWrite(BathAMP, LOW);
    digitalWrite(BathSpeakers, LOW);
    break;
  case 3: // Bedroom speakers ON
    digitalWrite(MainAMP, HIGH);
    digitalWrite(BathAMP, HIGH);
    digitalWrite(MainSpeakers, HIGH);
    digitalWrite(BathSpeakers, HIGH);
    delay(100);
    bridgeBedroom.virtualWrite(V1, 1);
    bridgeBedroom.virtualWrite(V2, 1);
    break;
  case 4: // All OFF
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

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  bridgeBedroom.setAuthToken(BedroomAUTH);
  bridgeEntery.setAuthToken(EnteryAUTH);
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
