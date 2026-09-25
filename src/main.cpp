#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "sensors/sensors.h"
#include "actuators/actuators.h"
#include "mqtt_handler/mqtt_handler.h"

unsigned long lastSend = 0;
unsigned long pirMotionTimer = 0;

void setup() {
    Serial.begin(115200);
    initSensors();
    initActuators();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Đang kết nối Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi đã kết nối! IP: " + WiFi.localIP().toString());

    initMQTT();
}

void loop() {
    handleMQTT();
    SensorData s = readSensors();

    // 1. Logic Khói/Gas vượt ngưỡng -> Còi kêu
    if (s.gas > GAS_THRESHOLD) {
        setBuzzer(true);
    } else {
        setBuzzer(false);
    }

    // 2. Logic Đèn chiếu sáng (Bình thường vs Chế độ Ban đêm)
    if (isNightMode) {
        // Chế độ ban đêm: Mặc định tắt, chỉ bật khi có chuyển động trong 5 giây[cite: 1]
        if (s.motion) {
            pirMotionTimer = millis();
        }
        if (millis() - pirMotionTimer < 5000 && pirMotionTimer > 0) {
            setLightLed(true);
        } else {
            setLightLed(false);
        }
    } else {
        // Chế độ thông thường: Trời tối tự động bật đèn
        if (s.light > LDR_DARK_VAL) {
            setLightLed(true);
        } else {
            setLightLed(false);
        }
    }

    // 3. Logic Nhiệt độ vượt ngưỡng -> Bật LED cảnh báo (sau này thay bằng Motor)
    if (!isnan(s.temperature) && s.temperature > TEMP_THRESHOLD) {
        setTempAlertLed(true);
    } else {
        setTempAlertLed(false);
    }

    // Gửi dữ liệu cảm biến lên MQTT mỗi 2 giây[cite: 1]
    if (millis() - lastSend > 2000) {
        lastSend = millis();
        publishTelemetry(s.temperature, s.humidity, s.gas, s.light, s.motion);
    }
}