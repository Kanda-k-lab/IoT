#include <Wire.h>
#include <Digital_Light_TSL2561.h>

void Lux_init() {
  Wire.begin();
  TSL2561.init();
}

int get_lux() {
  int lux = TSL2561.readVisibleLux();

  if(lux >= 144){
    lux = lux * 2.5 - 143;
  }else{
    lux = 0;
  }
  return lux;
}
