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

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1);
    ESP.wdtFeed();
  }
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

void determineStartValues() {
  WiFiClient client;
  if (!client.connect(host, httpPort2)) {
    return;
  }

  String url = "/meterkast";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + unameenc + " \r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);

  const int MAX_PAGENAME_LEN = 1024;
  char buffer[MAX_PAGENAME_LEN + 1];
  memset(buffer, 0, sizeof(buffer));
  while (client.available()) {
    client.readBytesUntil('\r', buffer, sizeof(buffer));
  }
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
      counter2 = (atoi(p) / 10);
    }
    p = strtok (NULL, " ");
    teller = teller + 1;
    delay(5);
  }
  Serial.println(String("Start values: ") + counter + "," + counter1 + "," + counter2);
}

void uploadValueToDomoticz(int id, const char* updateString2, const char* type, int value, int value2) {
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    return;
  }

  String url = String(updateString) + id + String(updateString2) + value;
  if (type == "Huidig energieverbruik") {
    url = String(updateString) + id + String(updateString2) + value + ";" + value2;
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + unameenc + " \r\n" + 
               "Connection: close\r\n\r\n");
}

void uploadResetinfoToDomoticz(int id, const char* updateString2, const char* type, String value, int value2) {
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    return;
  }

  String url = String(updateString) + id + String(updateString2) + value;
  if (type == "Huidig energieverbruik") {
    url = String(updateString) + id + String(updateString2) + value + ";" + value2;
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + unameenc + " \r\n" + 
               "Connection: close\r\n\r\n");
}

void uploadGas() {
  uploadValueToDomoticz(ID2, updateGas, type2, counter2, -1);
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

void uploadStack(){
<<<<<<< HEAD
  //char *rinfo;
  //String reset;
  //reset = ESP.getResetInfo();
  //rinfo = &reset[0];
  //char rr[500];
  
=======
  char *rinfo;
  String reset;
  reset = ESP.getResetInfo();
  rinfo = &reset[0];
  char rr[1000];
 
>>>>>>> try
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
<<<<<<< HEAD
  Serial.println(ret);
  //strcpy(rr, rinfo);
  //strcat(rr, (const char *)ret);
  //Serial.println(rr);
  
  Serial.println("****************************STACK AFTER URLENCODE********************************");
  //String bla = urlencode(rr);
  //Serial.println(rr);
  Serial.println("****************************STACK AFTER URLENCODE********************************");
  //uploadResetinfoToDomoticz(ID3, updateElectricityOrText, type3, bla, -1);
=======

  strcpy(rr, rinfo);
  strcat(rr, (const char *)ret);

  String bla = urlencode(rr);
  Serial.println(bla);
  uploadResetinfoToDomoticz(ID3, updateElectricityOrText, type3, bla, -1);
>>>>>>> try
}

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

