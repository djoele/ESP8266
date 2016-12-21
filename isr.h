void ICACHE_RAM_ATTR pulsetaskwater() {
  waardenu = digitalRead(pinWater);
  if (waardenu > waarde || waardenu < waarde) {
    if (waardenu == HIGH) {
        waterpuls = 1;
      }
    }
    waarde = waardenu;
}
