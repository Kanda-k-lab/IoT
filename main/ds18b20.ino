#include "OneWire.h"
#include "DallasTemperature.h"

#include "ds18b20.h"

// Data wire and power line are plugged into port A2,A3 
#define ONE_WIRE_BUS0 16           // Data
#define ONE_WIRE_BUS1 17 
//#define POWER_LINE   A3           // Power & Pullup

OneWire oneWire0(ONE_WIRE_BUS0);
DallasTemperature sensors0(&oneWire0);
OneWire oneWire1(ONE_WIRE_BUS1);
DallasTemperature sensors1(&oneWire1);

int count = 0;

void vInitDs18b20_0(void){
  //pinMode( POWER_LINE, OUTPUT );
  //digitalWrite( POWER_LINE, HIGH );   // Sensor Power ON
  sensors0.begin();                    // Start up the library
  sensors0.requestTemperatures(); // Send the command to get temperatures
  delay(500);
  return;
}

void vInitDs18b20_1(void){
  //pinMode( POWER_LINE, OUTPUT );
  //digitalWrite( POWER_LINE, HIGH );   // Sensor Power ON
  sensors1.begin();                    // Start up the library
  sensors1.requestTemperatures(); // Send the command to get temperatures
  delay(500);
  return;
}

float fGetTemp0(void){ 
  sensors0.requestTemperatures(); // Send the command to get temperatures
  return  sensors0.getTempCByIndex(0);   // Get Current temp
}

float fGetTemp1(void){ 
  sensors1.requestTemperatures(); // Send the command to get temperatures
  return  sensors1.getTempCByIndex(0);   // Get Current temp
}

float fGetCount1(void){
  count += sensors1.getDS18Count();
  return count;
}
