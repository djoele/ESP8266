byte readEeprom(){
  // read a byte from the current address of the EEPROM
 
}

void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    ESP.wdtFeed();
  }
  Serial.println("WiFi connected..");
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      Serial.println("WiFi DHCP Timeout");
      connectWifi();
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connectWifi();
      break;
  }
}

void determineStartValues() {
  WiFiClient client;
  if (!client.connect(host, httpPort2)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/meterkast";
  Serial.print("Requesting URL: ");
  Serial.println(url);

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
    Serial.print(buffer);
  }
  
  char *p;
  Serial.println(String("Split in tokens: ") + buffer);

  p = strtok (buffer, " ");
  int teller = 0;
  while (p != NULL && teller < 3)
  {
    Serial.println(String("Deel string: ") + p);
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
    Serial.println("connection failed");
    return;
  }

  String url = String(updateString) + id + String(updateString2) + value;
  if (type == "Huidig energieverbruik") {
    url = String(updateString) + id + String(updateString2) + value + ";" + value2;
  }
  Serial.print("Request: ");
  Serial.println(url);
  Serial.println(String("Uploading ") + type + ": " + value);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + unameenc + " \r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
}

void uploadResetinfoToDomoticz(int id, const char* updateString2, const char* type, String value, int value2) {
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = String(updateString) + id + String(updateString2) + value;
  if (type == "Huidig energieverbruik") {
    url = String(updateString) + id + String(updateString2) + value + ";" + value2;
  }
  Serial.print("Request: ");
  Serial.println(url);
  Serial.println(String("Uploading ") + type + ": " + value);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + unameenc + " \r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
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

void replaceSpaces(const char* string,char* newstring) {
  #define MAX_STR_LENGTH 1000
  
  // Copy first MAX_STR_LENGTH characters into newstring
  strncpy(newstring, string, MAX_STR_LENGTH);
  //explicitly set the last byte of the buffer to 0 after calling strncpy
  newstring[MAX_STR_LENGTH - 1] = 0;

  unsigned int i = 0;
  for (i = 0; i < MAX_STR_LENGTH; ++i) {
    if (newstring[i] == ' ') { // Look for a space
      newstring[i] = ','; // replace with a %20
    }
  }
}

void uploadError() {
  //value = readEeprom();

  Serial.print("\t");
  //Serial.print(value, DEC);
  Serial.println();
  
  const char* rinfo;
  char rinfo2[1000];
  rinfo = ESP.getResetInfo().c_str();
  replaceSpaces(rinfo,rinfo2);
  uploadResetinfoToDomoticz(ID3, updateElectricityOrText, type3, rinfo2, -1);
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
