#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>

void initActuators();
void setBuzzer(bool state);
void setLightLed(bool state);
void setTempAlertLed(bool state);
void setRelay(int relayNum, bool state);

#endif