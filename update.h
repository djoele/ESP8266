void doUpdate(){
  if((WiFi.status() == WL_CONNECTED)) {
    t_httpUpdate_return ret = ESPhttpUpdate.update(updateurl, md5value, fingerprint);
    if(ret == HTTP_UPDATE_OK) {
      //Dit helpt tegen een vroegtijdige reboot
      WiFi.removeEvent(WiFiEvent);
      ret = ESPhttpUpdate.update(updateurl, md5value, fingerprint);
     }
  }
}

