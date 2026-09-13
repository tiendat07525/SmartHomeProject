#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>

void actuatorsInit();

void setLight(bool state);
bool getLightState();

void setFan(bool state);
bool getFanState();

#endif