#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <stdio.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <user_interface.h>
#include <ESP8266httpUpdate.h>
#include "md5file.h"
#include "cont.h"
#include "constants.h"
#include "eeprom.h"
#include "crash.h"
#include "functies.h"
#include "isr.h"
#include "Base64.h"
#include "update.h"

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  EEPROM.begin(4096);
  SPIFFS.begin();
  
  version = readMD5();
  strcpy(md5value, version.c_str());
  
  memset(unameenc,0,sizeof(unameenc));
  base64_encode(unameenc, uname, strlen(uname));

  connectWifi();
  WiFi.onEvent(WiFiEvent);

  //determineStartValues();
  uploadStack();

  pinMode(pinGas, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinGas), pinupGas, FALLING);

  pinMode(pinEnergie, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinEnergie), pinupEnergie, FALLING);

  pinMode(pinWater, INPUT_PULLUP);
  waarde = digitalRead(pinWater);
  waardenu = waarde;
  triggertijd = now();

  Alarm.timerRepeat(250, uploadEnergie2);
  Alarm.timerRepeat(55, uploadEnergie1);
  Alarm.timerRepeat(300, uploadWater);
  Alarm.timerRepeat(350, uploadGas);
  Alarm.timerRepeat(60, doUpdate);

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("ESP8266-Meterkast");
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  telnetServer.begin();
  telnetServer.setNoDelay(true);
}

void loop() { 
  ArduinoOTA.handle();
  handleTelnet();
  if (energiepuls == 1){
    energiepuls = 0;
    counter++;
    pulsetijd = now();
    tijdsduur = pulsetijd - begintijd;
    begintijd = pulsetijd;
    huidigverbruik = floor(3600 / tijdsduur);
    serverClient.println(String("Huidig verbruik: ") + huidigverbruik);
  }
  if (gaspuls == 1){
    gaspuls = 0;
    counter2++;
    serverClient.println(String("Gaspuls gedetecteerd: ") + counter2);
  }
  pulsetaskwater();
  if (waterpuls == 1){
    triggernu = now();
    tijdsduur2 = triggernu - triggertijd;
    serverClient.println(String("Tijdsduur tot vorige waterpuls: ") + tijdsduur2);
    if (tijdsduur2 > 3) {
      counter1++;
      serverClient.println(String("Waterpuls gedetecteerd: ") + counter1);
    }  
    triggertijd = now();
    waterpuls = 0;
  }
  ESP.wdtFeed();
  Alarm.delay(1000);
}
