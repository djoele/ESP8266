boolean f(int addr) {
  return ((addr >= 0) && (addr <= 4096));
}

boolean eeprom_is_addr_ok(int addr) {
  return ((addr >= 0) && (addr <= 4096));
}

boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
  int i;
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }

  return true;
}

boolean eeprom_write_string(int addr, const char* string) {
  int numBytes;
  numBytes = strlen(string) + 1;
  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

boolean eeprom_read_bytes(int startAddr, byte array[], int numBytes) {
  int i;
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    array[i] = EEPROM.read(startAddr + i);
  }

  return true;
}

boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
  byte ch;
  int bytesRead;
  if (!eeprom_is_addr_ok(addr)) {
    return false;
  }
  if (bufSize == 0) {
    return false;
  }
  if (bufSize == 1) {
    buffer[0] = 0;
    return true;
  }

  bytesRead = 0;

  ch = EEPROM.read(addr + bytesRead);

  buffer[bytesRead] = ch;

  bytesRead++;

  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= 4096) ) {
    ch = EEPROM.read(addr + bytesRead);
    buffer[bytesRead] = ch;
    bytesRead++;
  }
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }

  return true;
}

void eeprom_erase_all(byte b = 0xFF) {
  int i;

  for (i = 0; i <= 4096; i++) {
    EEPROM.write(i, b);
  }
}
