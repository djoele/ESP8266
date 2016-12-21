String readFile(String pad) {
  File f = SPIFFS.open(pad, "r");
  String line;
  if (!f) {
    return "Error";
  }
  while (f.available()) {
    line = f.readStringUntil('\n');
  }
  f.close();
  return line;
}

void saveValues() {
  File values = SPIFFS.open("/values.txt", "w+");
  if (!values) {
    return;
  }
  values.println(counter); 
  values.close();
}

void saveSHA(String s) {
  File sha = SPIFFS.open("/sha.txt", "w+");
  if (!sha) {
    Serial.println(F("[SHA] Failed to open config file for writing"));
  }
  sha.println(s.c_str());
  sha.close();
}

