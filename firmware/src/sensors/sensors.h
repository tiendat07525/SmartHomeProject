#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

struct SensorData {
    float temperature;
    float humidity;
    int gas;
    int lux;
    bool isDark;
    bool motion;
};

void initSensors();
SensorData readSensors();

#endif