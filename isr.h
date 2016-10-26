void ICACHE_RAM_ATTR pinupEnergie() {
  if (energiepuls == 0){
    energiepuls = 1;
  }
  if (energiepuls == 1){
    energiepuls = 2;
  }
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
