void saveMD5(String s) {
  File md5 = SPIFFS.open("/md5.txt", "w");
  if (!md5) {
    Serial.println("Failed to open config file for writing");
  }
  md5.println(s.c_str());
  md5.close();
}

String readMD5() {
  File md5 = SPIFFS.open("/md5.txt", "r");
  String line;
  if (!md5) {
    Serial.println("Failed to open file for reading");
  }
  while (md5.available()) {
    line = md5.readStringUntil('\n');
  }
  md5.close();
  
  return line;
}
