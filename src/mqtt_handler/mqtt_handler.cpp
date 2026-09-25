#include "mqtt_handler.h"
#include "../include/config.h"
#include "../actuators/actuators.h"

static WiFiClient espClient;
static PubSubClient client(espClient);
bool isNightMode = false;

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
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
    }
}

void initMQTT() {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqttCallback);
}

static void reconnect() {
    while (!client.connected()) {
        Serial.print("Đang kết nối MQTT Broker...");
        if (client.connect(MQTT_CLIENT_ID)) {
            Serial.println(" Thành công!");
            client.subscribe(TOPIC_MODE);
            client.subscribe(TOPIC_RELAY);
        } else {
            Serial.printf(" Thất bại, rc=%d. Thử lại sau 3s...\n", client.state());
            delay(3000);
        }
    }
}

void handleMQTT() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

void publishTelemetry(float temp, float hum, int gas, int light, bool motion) {
    if (!client.connected()) return;
    char payload[128];
    snprintf(payload, sizeof(payload), 
        "{\"temp\":%.1f,\"hum\":%.1f,\"gas\":%d,\"lux\":%d,\"motion\":%d}",
        temp, hum, gas, light, motion ? 1 : 0);
    client.publish(TOPIC_TELEMETRY, payload);
}