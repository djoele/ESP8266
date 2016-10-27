void ICACHE_RAM_ATTR pinupEnergie() {
  energiepuls = 1;
}

void ICACHE_RAM_ATTR pinupGas() {
  gaspuls = 1;
}

void ICACHE_RAM_ATTR pulsetaskwater() {
  waardenu = digitalRead(pinWater);
  if (waardenu > waarde || waardenu < waarde) {
    if (waardenu == HIGH) {
        waterpuls = 1;
      }
    }
    waarde = waardenu;
}
