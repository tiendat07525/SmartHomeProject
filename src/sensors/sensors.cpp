#include "sensors.h"
#include "config.h"

#include <DHT.h>

DHT dht(DHT_PIN, DHT_TYPE);

void sensorsInit()
{
    pinMode(PIR_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);

    dht.begin();
}

SensorData readSensors()
{
    SensorData data;

    // PIR
    data.motion = digitalRead(PIR_PIN);

    // LDR
    data.lightValue = analogRead(LDR_PIN);

    // DHT22
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();

    // Day / night
    data.isDark = data.lightValue < LDR_THRESHOLD;

    return data;
}