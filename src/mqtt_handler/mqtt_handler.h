#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>

void mqttInit();

void mqttLoop();

bool mqttConnected();

void mqttPublish(
    const char* topic,
    const char* message,
    bool retained = false
);

#endif