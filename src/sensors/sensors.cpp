#include "sensors.h"
#include "../include/config.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT22);

void initSensors() {
    pinMode(PIN_MQ2, INPUT);
    pinMode(PIN_LDR, INPUT);
    pinMode(PIN_PIR, INPUT);
    dht.begin();
}

SensorData readSensors() {
    SensorData data;
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();
    data.gas = analogRead(PIN_MQ2);
    data.light = analogRead(PIN_LDR);
    data.motion = digitalRead(PIN_PIR) == HIGH;
    return data;
}