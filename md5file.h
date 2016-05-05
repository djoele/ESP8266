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
    #ifdef DEBUG
    Serial.println(F("[VALUES] Error: schrijven van values in niet gelukt..."));
    #endif
    return;
  }
  values.println(String("") + counter + " " + counter1 + " " + counter2); 
  values.close();

  uint32_t a = '3ffffd80';
    uint32_t b = '3fffffb0';
    uint32_t offset = 0;
    offset = '01a0';
    String stack = getStack(a + offset, b);
    Serial.println(String("[PULS] Energiepuls: ") + stack);
}

void saveSHA(String s) {
  File sha = SPIFFS.open("/sha.txt", "w+");
  if (!sha) {
    Serial.println(F("[SHA] Failed to open config file for writing"));
  }
  sha.println(s.c_str());
  sha.close();
}

