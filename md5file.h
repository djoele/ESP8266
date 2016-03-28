String readMD5() {
  File md5 = SPIFFS.open("/md5.txt", "r");
  String line;
  if (!md5) {
    return "Error reading MD5";
  }
  while (md5.available()) {
    line = md5.readStringUntil('\n');
  }
  md5.close();
  return line;
}

