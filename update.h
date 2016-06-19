void doUpdate(){
  if((WiFi.status() == WL_CONNECTED)) {
    #ifdef DEBUG    
    serverClient.println((String("[HTTP] url: ") + updateurl + ", MD5: " + md5value + ", Fingerprint: " + fingerprint));
    Serial.println((String("[HTTP] url: ") + updateurl + ", MD5: " + md5value + ", Fingerprint: " + fingerprint));
    #endif
    t_httpUpdate_return ret = ESPhttpUpdate.update(updateurl, md5value, fingerprint);
    #ifdef DEBUG    
    Serial.println((String("[HTTP] url: ") + updateurl + ", MD5: " + md5value + ", Fingerprint: " + fingerprint));
    #endif
    if(ret == HTTP_UPDATE_OK) {
      //Dit helpt tegen een vroegtijdige reboot
      delete &mDisconnectHandler;
      
      ret = ESPhttpUpdate.update(updateurl, md5value, fingerprint);
     }
  }
}
