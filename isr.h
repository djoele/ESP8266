void pinupEnergie() {
  energiepuls = 1;
}

void pinupGas() {
  gaspuls = 1;
}

void pulsetaskwater() {
  waardenu = digitalRead(pinWater);
  if (waardenu > waarde || waardenu < waarde) {
    if (waardenu == HIGH) {
        waterpuls = 1;
      }
    }
    waarde = waardenu;
}


