#include <Ethernet.h>

#define DEBUG
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
#include <Wire.h>

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  Serial.setDebugOutput(true);
#endif

  EEPROM.begin(2000);
  SPIFFS.begin();

  version = readFile("/md5.txt");
  strcpy(md5value, version.c_str());
  Serial.println((String("[MD5] md5: ") + md5value));

  sha = readFile("/sha.txt");
  fingerprint = sha.c_str();
  strcpy(shavalue, sha.c_str());
  Serial.println((String("[SHA] sha: ") + shavalue));

  memset(unameenc, 0, sizeof(unameenc));
  base64_encode(unameenc, uname, strlen(uname));

  determineStartValues();

  connectWifi();

  pinMode(pinWater, INPUT_PULLUP);
  waarde = digitalRead(pinWater);
  waardenu = waarde;

  Alarm.timerRepeat(60, uploadWater);
  Alarm.timerRepeat(120, saveValues);
  Alarm.timerRepeat(20, reconnectWifi);

#ifdef DEBUG
  telnetServer.begin();
  telnetServer.setNoDelay(true);
#endif

  server.on("/update_esp8266", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "Login Succes, updating start..");
    doUpdate();
  });
  server.on("/reset", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "ESP8266 gaat resetten..");
    ESP.reset();
  });
  server.on("/resetvalues", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    int reset = atoi(server.arg("reset").c_str());
    resetStartValues(reset);
  });
  server.on("/stack", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", loadStack());
  });
  server.on("/resetinfo", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", ESP.getResetInfo());
    //Nu hebben we de stack trace ook echt verstuurd
    error_sent = 1;
    eeprom_erase_all();
    EEPROM.commit();
  });
  server.on("/update_sha", HTTP_POST, []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    saveSHA(server.arg("sha"));
    sha = readFile("/sha.txt");
    strcpy(shavalue, sha.c_str());

    sha = readFile("/sha.txt");
    strcpy(shavalue, sha.c_str());
    ESP.restart();
  });
  server.begin();

  //triggerStack();
}


void loop() {
  trigger_nu = now();
  tijdsduur = trigger_nu - trigger_begin;
  //#ifdef DEBUG
  //serverClient.println(String("[DUUR] Tijdsduur geen puls gezien: ") + tijdsduur);
  //#endif

#ifdef DEBUG
  server.handleClient();
  handleTelnet();
#endif

  pulsetaskwater();
  if (waterpuls == 1) {
    counter++;
#ifdef DEBUG
    serverClient.println(String("[PULS] Waterpuls gedetecteerd: ") + counter);
#endif
    waterpuls = 0;
    trigger_begin = now();
  }
  Alarm.delay(1000);
}
