String DisplayAddress(IPAddress address)
{
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
}

void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1);
  }
  Serial.print(String("[WIFI] IP: "));
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP();
  ipadres = DisplayAddress(ip);
}

void reconnectWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
  }
}

void callURL2(String url, String host, const int port) {
  HTTPClient * http = new HTTPClient();
  http->begin(host, port, url, fingerprint);
  http->setAuthorization(www_username,www_password);
  http->addHeader("X-ESP8266-IP", ipadres);

  int httpCode = http->GET();
  #ifdef DEBUG    
    serverClient.println((String("[HTTP] url: ") + url + ", return code: " + httpCode));
  #endif
  http->end();
  http->~HTTPClient();
  delete http;
}
               
void determineStartValues() {
  if(!SPIFFS.exists("/values.txt")){
    saveValues();
  }
  String bufferin = readFile("/values.txt");
  char buffer[1024];
  strcpy(buffer,bufferin.c_str());
  char *p;
  p = strtok (buffer, " ");
  counter = atoi(p);
  Serial.println((String("[VALUE] counter: ") + counter));
}

void resetStartValues(int reset) {
  if(SPIFFS.exists("/values.txt")){
    SPIFFS.remove("/values.txt");
    counter = reset;
    saveValues();
    ESP.restart();
  }
}
  
void triggerStack(){
  if (error_sent == 0){
    callURL2(trigger_stack, host, httpsPort);
  }  
}

void uploadValueToDomoticz(int id, const char* updateString2, const char* type, int value, int value2) {
  String url = String(updateString) + id + String(updateString2) + value;
  if (type == "Huidig energieverbruik") {
    url = String(updateString) + id + String(updateString2) + value + ";" + value2;
  }
  callURL2(url, host, httpsPort);
}

void uploadWater() {  
  if (tijdsduur > 180)
  {
    uploadValueToDomoticz(ID1, updateCounter, type1, counter, -1);
  } 
  else
  {
     #ifdef DEBUG    
        serverClient.println((String("[HTTP]: geen upload want tijdsduur geen puls gezien is te kort")));
    #endif
  }
}

String loadStack(){
  String stack = "";
  eeprom_read_string(0, buf, 2000);
  if (buf[0]=='c'){
    stack = String(buf);
  }
  return stack;
}

#ifdef DEBUG
void handleTelnet(){
  if (telnetServer.hasClient()) {
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) {
        serverClient.stop();
      }
      serverClient = telnetServer.available();
      serverClient.print("");
      serverClient.flush();
    }
  }
  if (millis() - startTimeWifi > 2000) {
    startTimeWifi = millis();
  }
  delay(10);  
}
#endif
