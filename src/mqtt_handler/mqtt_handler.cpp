#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "mqtt_handler.h"
#include "config.h"
#include "actuators/actuators.h"

static WiFiClient espClient;
static PubSubClient client(espClient);
bool isNightMode = false;
static unsigned long lastReconnectAttempt = 0;

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    message.reserve(length);
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.printf("[MQTT] Nhận tin [%s]: %s\n", topic, message.c_str());

    if (String(topic) == TOPIC_MODE) {
        if (message == "NIGHT") {
            isNightMode = true;
            Serial.println("-> Chuyển sang CHẾ ĐỘ BAN ĐÊM");
        } else {
            isNightMode = false;
            Serial.println("-> Chuyển sang CHẾ ĐỘ THƯỜNG");
        }
    } else if (String(topic) == TOPIC_RELAY) {
        // Cú pháp payload mẫu: "R1:ON", "R1:OFF", "R2:ON", "R2:OFF"
        if (message == "R1:ON") setRelay(1, true);
        else if (message == "R1:OFF") setRelay(1, false);
        else if (message == "R2:ON") setRelay(2, true);
        else if (message == "R2:OFF") setRelay(2, false);
    }
}

void initMQTT() {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqttCallback);
}

static bool reconnectNonBlocking() {
    if (client.connect(MQTT_CLIENT_ID)) {
        Serial.println("[MQTT] Kết nối Broker thành công!");
        client.subscribe(TOPIC_MODE);
        client.subscribe(TOPIC_RELAY);
        return true;
    }
    Serial.printf("[MQTT] Thất bại, rc=%d. Thử lại sau...\n", client.state());
    return false;
}

void handleMQTT() {
    if (!client.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            if (reconnectNonBlocking()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        client.loop();
    }
}

void publishTelemetry(float temp, float hum, int gas, int lux, bool motion) {
    if (!client.connected()) return;
    char payload[256];
    snprintf(payload, sizeof(payload),
        "{\"temp\":%.1f,\"hum\":%.1f,\"gas\":%d,\"lux\":%d,\"motion\":%d}",
        isnan(temp) ? 0.0f : temp,
        isnan(hum) ? 0.0f : hum,
        gas,
        lux,
        motion ? 1 : 0
    );
    client.publish(TOPIC_TELEMETRY, payload);
}