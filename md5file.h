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
    Serial.println("Error: schrijven van values in niet gelukt...");
    #endif
    return;
  }
  values.println(String("") + counter + " " + counter1 + " " + counter2); 
  //values.println(String("") + 51375 + " " + 1938 + " " + 200);
  values.close();
}
