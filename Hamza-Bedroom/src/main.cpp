#include <Arduino.h>
#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and blynk server info

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

char auth[] = BedroomAUTH;
// Your WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;
 int ampState;
 int SpeakersState;

BLYNK_WRITE(V1) {
  ampState = param.asInt();
  digitalWrite(BedroomAmp,!ampState);
}
BLYNK_WRITE(V2) {
  SpeakersState = param.asInt();
  digitalWrite(BedroomSpeakers,!ampState);
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

  pinMode(BedroomAmp, OUTPUT);
  pinMode(BedroomSpeakers, OUTPUT);

  digitalWrite(BedroomSpeakers,HIGH);
  digitalWrite(BedroomAmp, HIGH);

}

void loop()
{
  Blynk.run();
}
