//#define DEBUG

#include <EEPROM.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <user_interface.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include "constants.h"
#include "cont.h"
#include "eeprom.h"
#include "crash.h"
#include "md5file.h"
#include "functies.h"
#include "isr.h"
#include "Base64.h"
#include "update.h"

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  #endif
  
  EEPROM.begin(EEPROM_MAX_ADDR);
  SPIFFS.begin();
  
  stack = loadStack();
  
  version = readFile("/md5.txt");
  strcpy(md5value, version.c_str());
  #ifdef DEBUG
  Serial.println(String("Gelezen md5 uit file: ") + md5value);
  #endif

  memset(unameenc,0,sizeof(unameenc));
  base64_encode(unameenc, uname, strlen(uname));

  connectWifi();
  WiFi.onEvent(WiFiEvent);

  //FOR RESET eerst alleen saveValues, daarna die uit en dan weer determineStartValues
  //saveValues();
  determineStartValues();
  
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
  Alarm.timerRepeat(30, saveValues);

  #ifdef DEBUG
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  #endif

  server.on("/update_esp8266", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "Login Succes, updating start..");
    doUpdate();
  });
  server.begin();

  #ifdef DEBUG
  Serial.println("Start uploading stack...");
  #endif
  uploadStack();
  #ifdef DEBUG
  Serial.println("Klaar uploading stack...");
  #endif
}

void loop() { 
  server.handleClient();
  #ifdef DEBUG
  handleTelnet();
  #endif
  if (energiepuls == 1){
    energiepuls = 0;
    counter++;
    pulsetijd = now();
    tijdsduur = pulsetijd - begintijd;
    begintijd = pulsetijd;
    huidigverbruik = floor(3600 / tijdsduur);
    #ifdef DEBUG
    serverClient.println(String("Energiepuls: ") + huidigverbruik);
    #endif
  }
  if (gaspuls == 1){
    gaspuls = 0;
    counter2++;
    #ifdef DEBUG
    serverClient.println(String("Gaspuls: ") + counter2);
    #endif
  }
  pulsetaskwater();
  if (waterpuls == 1){
    triggernu = now();
    tijdsduur2 = triggernu - triggertijd;
    #ifdef DEBUG
    serverClient.println(String("Tijdsduur tot vorige waterpuls: ") + tijdsduur2);
    #endif
    if (tijdsduur2 > 3) {
      counter1++;
      #ifdef DEBUG
      serverClient.println(String("Waterpuls gedetecteerd: ") + counter1);
      #endif
    }  
    triggertijd = now();
    waterpuls = 0;
  }
  ESP.wdtFeed();
  Alarm.delay(1000);
}
