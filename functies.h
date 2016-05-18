String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
      }
      yield();
    }
    return encodedString;
    
}

String DisplayAddress(IPAddress address)
{
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
}

void connectWifi() {
  #ifdef DEBUG
  Serial.println(String("[WIFI] ssid: ") + ssid);
  Serial.println(String("[WIFI] password: ") + password);
  #endif
  WiFi.begin(ssid, password);
  #ifdef DEBUG
  Serial.print(F("[WIFI] Verbinden met Wifi"));
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG
    Serial.print(F("."));
    #endif
    delay(1);
    ESP.wdtFeed();
  }
  Serial.print(String("[WIFI] IP: "));
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP();
  ipadres = DisplayAddress(ip);
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      connectWifi();
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      connectWifi();
      break;
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
    Serial.println((String("[HTTP] url: ") + url + ", return code: " + httpCode));
  #endif
  http->end();
  http->~HTTPClient();
  delete http;
}
               
void determineStartValues() {
  if(!SPIFFS.exists("/values.txt")){
    #ifdef DEBUG
    Serial.println(F("[VALUES] values.txt bestaat niet"));
    #endif
    saveValues();
  }
  String bufferin = readFile("/values.txt");
  #ifdef DEBUG
  Serial.println("[VALUES] Bufferin: " + bufferin);
  #endif
  char buffer[1024];
  strcpy(buffer,bufferin.c_str());
  char *p;
  p = strtok (buffer, " ");
  int teller = 0;
  while (p != NULL && teller < 3)
  {
    if (teller == 0) {
      counter = atoi(p);
    }
    if (teller == 1) {
      counter1 = atoi(p);
    }
    if (teller == 2) {
      counter2 = atoi(p);
    }
    p = strtok (NULL, " ");
    teller = teller + 1;
    delay(5);
  }
  #ifdef DEBUG
  Serial.println(String("[VALUES] Gelezen values op basis van bufferin: ") + counter + " " + counter1 + " " + counter2);
  #endif
}

void triggerStack(){
  callURL2(trigger_stack, host, httpsPort);
}

void uploadValueToDomoticz(int id, const char* updateString2, const char* type, int value, int value2) {
  String url = String(updateString) + id + String(updateString2) + value;
  if (type == "Huidig energieverbruik") {
    url = String(updateString) + id + String(updateString2) + value + ";" + value2;
  }
  callURL2(url, host, httpsPort);
}

void uploadGas() {
  uploadValueToDomoticz(ID2, updateCounter, type2, counter2, -1);
}

void uploadWater() {
  uploadValueToDomoticz(ID1, updateCounter, type1, counter1, -1);
}

void uploadEnergie1() {
  if (huidigverbruik > 0 and huidigverbruik < 3000){
    uploadValueToDomoticz(ID4, updateElectricityOrText, type4, huidigverbruik, counter);
  }
}

void  uploadEnergie2() {
  uploadValueToDomoticz(ID, updateElectricityOrText, type, counter, -1);
}

void uploadHeap() {
  uint32_t heapnu = ESP.getFreeHeap();
  float perc = ((float)heapnu/(float)heap)*100;
  #ifdef DEBUG
    serverClient.println(String("[HEAP] FreeHeap : ") + ESP.getFreeHeap() + ", " + perc + "%");
  #endif
  uploadValueToDomoticz(ID5, updateElectricityOrText, type, perc, -1);
}

String loadStack(){
  char *rinfo;
  String reset;
  reset = ESP.getResetInfo();
  rinfo = &reset[0];
  char rr[2500];

  eeprom_read_string(0, buf, EEPROM_MAX_ADDR);
  String stack = urlencode(buf);
  const char find[4] = "ctx";
  const char find2[10] = "Exception";
  const char * stackkie = stack.c_str();
  char *ret;
  ret = strstr(stackkie, find2);
  if (ret==NULL){
    ret = strstr(stackkie, find);
  }
  
  strcpy(rr, rinfo);
  //Deze eruithalen wanneer crash optreedt meteen bij opstarten
  if (ret != NULL){
    strcat(rr, (const char *)ret);
  }
  String bla = urlencode(rr);
  eeprom_erase_all();
  EEPROM.commit();
  return bla;
}

#ifdef DEBUG
void handleTelnet(){
  if (telnetServer.hasClient()) {
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) {
        serverClient.stop();
      }
      serverClient = telnetServer.available();
      serverClient.flush();  // clear input buffer, else you get strange characters 
    }
  }
  if (millis() - startTimeWifi > 2000) { // run every 2000 ms
    startTimeWifi = millis();
  }
  delay(10);  // to avoid strange characters left in buffer
}
#endif
