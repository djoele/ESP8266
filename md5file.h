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
  #ifdef DEBUG
  serverClient.print(F("[HEAP] FreeHeap : "));
  serverClient.println(ESP.getFreeHeap());
  #endif
  File values = SPIFFS.open("/values.txt", "w+");
  if (!values) {
    #ifdef DEBUG
    Serial.println(F("[VALUES] Error: schrijven van values in niet gelukt..."));
    #endif
    return;
  }
  values.println(String("") + counter + " " + counter1 + " " + counter2); 
  //values.println(String("") + 51375 + " " + 1938 + " " + 20);
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
