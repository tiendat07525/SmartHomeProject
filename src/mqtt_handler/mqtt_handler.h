#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include <WiFi.h>

extern bool isNightMode;

void initMQTT();
void handleMQTT();
void publishTelemetry(float temp, float hum, int gas, int light, bool motion);

#endif