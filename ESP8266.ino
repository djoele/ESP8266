#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <user_interface.h>
#include <SoftwareSerial.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <stdio.h>
#include <string.h>
#include "cont.h"
#include "constants.h"
#include "functies.h"
#include "isr.h"
#include "Base64.h"
#include "eeprom.h"
#include "crash.h"

void setup() {
  EEPROM.begin(512);
  
  //generate base64 string from credentials, for http basic auth
  memset(unameenc,0,sizeof(unameenc));
  base64_encode(unameenc, uname, strlen(uname));

  Serial.begin(115200);
  connectWifi();
  WiFi.onEvent(WiFiEvent);

  determineStartValues();
  
  uploadError();

  if(counter<0){
    ESP.restart();
  }

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

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("ESP8266-Meterkast");
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
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
    //uint32_t a = 3fffdf00;
    //uint32_t b=3fffffb0;
    //String stack = getStack(a, b);
    //serverClient.println(String("Stack: ") + stack);
    eeprom_read_string(0, buf, EEPROM_MAX_ADDR);
    Serial.println(String("eeprom read: "));
    Serial.println(buf);
    serverClient.println(String("eeprom read: "));
    serverClient.println(buf);
    
    energiepuls = 0;
    counter++;
    Serial.println(String("Energiepuls gedetecteerd: ") + counter);
    serverClient.println(String("Energiepuls gedetecteerd: ") + counter);
    pulsetijd = now();
    tijdsduur = pulsetijd - begintijd;
    begintijd = pulsetijd;
    huidigverbruik = floor(3600 / tijdsduur);
    Serial.println(String("Huidig verbruik: ") + huidigverbruik);
    serverClient.println(String("Huidig verbruik: ") + huidigverbruik);
  }
  
  if (gaspuls == 1){
    gaspuls = 0;
    counter2++;
    Serial.println(String("Gaspuls gedetecteerd: ") + counter2);
    serverClient.println(String("Gaspuls gedetecteerd: ") + counter2);
  }
  
  pulsetaskwater();
  if (waterpuls == 1){
    triggernu = now();
    tijdsduur2 = triggernu - triggertijd;
    Serial.println(String("Tijdsduur tot vorige waterpuls: ") + tijdsduur2);
    serverClient.println(String("Tijdsduur tot vorige waterpuls: ") + tijdsduur2);
    if (tijdsduur2 > 3) {
      counter1++;
      Serial.println(String("Waterpuls gedetecteerd: ") + counter1);
      serverClient.println(String("Waterpuls gedetecteerd: ") + counter1);
    }  
    triggertijd = now();
    waterpuls = 0;
  }
  
  ESP.wdtFeed();
  Alarm.delay(1000);
}
