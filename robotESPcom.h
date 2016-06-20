#ifndef _robotESPcom_H_
#define _robotESPcom_H_
#include "Arduino.h"

#include "ESP8266WiFi.h"


#define SOP '<'
#define EOP '>'


void setupWifi();
void setup();
void loop();

void comHandler(char);

void motorCommand();







#endif /* _robotESPcom_H_ */
