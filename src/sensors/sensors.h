#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

struct SensorData
{
    int motion;
    int lightValue;

    float temperature;
    float humidity;

    bool isDark;
};

void sensorsInit();

SensorData readSensors();

#endif