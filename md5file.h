String readFile(String pad) {
  File f = SPIFFS.open(pad, "r");
  String line;
  if (!f) {
    return "Error reading file";
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
    Serial.println("Failed to open config file for writing");
  }
  values.println(String("") + counter + " " + counter1 + " " + counter2);
  values.close();
}
