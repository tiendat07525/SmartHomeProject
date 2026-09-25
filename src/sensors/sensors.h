#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

struct SensorData {
    float temperature;
    float humidity;
    int gas;
    int light;
    bool motion;
};

void initSensors();
SensorData readSensors();

#endif