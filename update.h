void doUpdate(){
  if((WiFi.status() == WL_CONNECTED)) {
    Serial.println("Update SPIFFS...");
    t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(binPath);
    Serial.println(String("ret: ") + ret);
    serverClient.println(String("ret: ") + ret);
    if(ret == HTTP_UPDATE_OK) {
      Serial.println("Update sketch...");
      ret = ESPhttpUpdate.update(binPath);
      Serial.println(String("ret: ") + ret);
      serverClient.println(String("ret: ") + ret);
      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          serverClient.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          serverClient.println("HTTP_UPDATE_NO_UPDATES");
          break;
        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          serverClient.println("HTTP_UPDATE_OK");
          break;
        }
     }
  }
}
