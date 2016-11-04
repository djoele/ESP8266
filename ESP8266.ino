#define DEBUG
//#define DEBUG2
#include <EEPROM.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <user_interface.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "constants.h"
#include <cont.h>
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
  
  EEPROM.begin(2000);
  SPIFFS.begin();
  
  version = readFile("/md5.txt");
  strcpy(md5value, version.c_str());
  
  sha = readFile("/sha.txt");
  fingerprint = sha.c_str();
  strcpy(shavalue, sha.c_str());
  #ifdef DEBUG
    Serial.println(String("[MD5] Gelezen md5: ") + md5value + "\n[SHA] Gelezen sha: " + shavalue);
  #endif
  
  memset(unameenc,0,sizeof(unameenc));
  base64_encode(unameenc, uname, strlen(uname));

  connectWifi();    
    
  #ifdef DEBUG
    Serial.print(F("[WIFI] Verbonden met Wifi."));
  #endif
   
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
  Alarm.timerRepeat(60, uploadHeap);
  Alarm.timerRepeat(60, reconnectWifi);

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
  server.on("/reset", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat resetten..");
    ESP.reset();
  });
  server.on("/resetvalues", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat values resetten naar [0 0 0]");
    resetStartValues();
  });
  #ifdef DEBUG2
  server.on("/crash", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat crashen met EXCEPTION..");
    char linea[]="0x123456",**ap;
    int num;
    num=strtol(linea,ap,0);
    printf("%d\n%s",num,*ap);
    int k;
  });
  server.on("/crash2", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat crashen met SOFT_WDT..");
    while (true){
      serverClient.println("Crashing...");
    }
  });
  server.on("/crash3", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat crashen met WDT..");
    ESP.wdtDisable();
    while (true){
      serverClient.println("Crashing...");
    }
  });
  server.on("/crash4", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat crashen met EXCEPTION..");
      crashme();
  });
  server.on("/crash5", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat crashen met EXCEPTION..");
      crashme2();
  });
  #endif

  server.on("/stack", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
      server.send(200, "text/plain", loadStack());
  });
  server.on("/resetinfo", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", ESP.getResetInfo());
    //Nu hebben we de stack trace ook echt verstuurd
    error_sent = 1;
    eeprom_erase_all();
    EEPROM.commit();
  });
  server.on("/update_sha", HTTP_POST, [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "Login Succes, updating sha..");    
    #ifdef DEBUG
      serverClient.println(F("[SHA] Login Succes, updating sha.."));
    #endif 
    saveSHA(server.arg("sha"));
    sha = readFile("/sha.txt");
    strcpy(shavalue, sha.c_str());

    sha = readFile("/sha.txt");
    strcpy(shavalue, sha.c_str());
    #ifdef DEBUG
    serverClient.println(String("[SHA] Gelezen sha uit file na update: ") + shavalue);
    #endif
    ESP.restart();
  });
  server.begin();

  triggerStack();
}

void loop() { 
  server.handleClient();
  #ifdef DEBUG
    handleTelnet();
  #endif
  if (energiepuls > 0){
    //counter ophogen met het aantal getelde energiepulsen
    //dan bij versturen delen door 2
    //want er zitten 2000 pulsen in een kWh
    counter = counter + energiepuls;
    pulsetijd = now();
    tijdsduur = pulsetijd - begintijd;
    begintijd = pulsetijd;
    //huidig verbruik wel hier corrigeren
    //tel het aantal pulsen sinds de vorige loop en dan delen door 2 vanwege 2000 pulsen in een kWh
    huidigverbruik = energiepuls * 3600 / tijdsduur / 2;
    #ifdef DEBUG 
      serverClient.println(String("[PULS] Energiepuls: ") + huidigverbruik);
    #endif
    energiepuls = 0;
  }
  if (gaspuls > 0){
    counter2 = counter2 + gaspuls;
    #ifdef DEBUG
      serverClient.println(String("[PULS] Gaspuls: ") + counter2);
    #endif
    gaspuls = 0;
  }
  pulsetaskwater();
  if (waterpuls == 1){
    triggernu = now();
    tijdsduur2 = triggernu - triggertijd;
    #ifdef DEBUG
      serverClient.println(String("[PULS] Tijdsduur tot vorige waterpuls: ") + tijdsduur2);
    #endif
    
    if (tijdsduur2 > 1) {
      counter1++;
      #ifdef DEBUG
        serverClient.println(String("[PULS] Waterpuls gedetecteerd: ") + counter1);
      #endif
    }  
    triggertijd = now();
    waterpuls = 0;
  }
  if (error_count == 3){
   ESP.restart(); 
  }
  Alarm.delay(1000);
}
