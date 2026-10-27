#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "sensors/sensors.h"
#include "actuators/actuators.h"
#include "mqtt_handler/mqtt_handler.h"

unsigned long lastSend = 0;
unsigned long lastSensorRead = 0;
unsigned long pirMotionTimer = 0;
SensorData currentData;
bool buzzerState = false;

void setup() {
    Serial.begin(115200);
    initSensors();
    initActuators();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("\r\nDang ket noi Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\r\nWi-Fi da ket noi! IP: %s\r\n", WiFi.localIP().toString().c_str());

    initMQTT();

    // Tiêu đề bảng chuẩn hóa căn lề cố định (Cột Gas phản ánh trạng thái cảnh báo)
    Serial.print("\r\n+-----------+-----------+-----------+-----------+-------+----------+-----------+\r\n");
    Serial.print("|  GAS STAT |  LDR(D0) |  TEMP(°C) |  HUM(%)   |  PIR  |  BUZZER  | LED_LIGHT |\r\n");
    Serial.print("+-----------+-----------+-----------+-----------+-------+----------+-----------+\r\n");
}

void loop() {
    handleMQTT();
    unsigned long now = millis();

    // 1. Kiểm tra PIR liên tục thời gian thực
    if (digitalRead(PIN_PIR) == HIGH) {
        pirMotionTimer = now;
    }

    // 2. Đọc cảm biến định kỳ mỗi 1000ms
    if (now - lastSensorRead >= 1000) {
        lastSensorRead = now;
        currentData = readSensors();

        // Xử lý cảnh báo rò rỉ khí/khói
        if (currentData.gas > GAS_THRESHOLD) {
            setBuzzer(true);
            buzzerState = true;
        } else {
            setBuzzer(false);
            buzzerState = false;
        }

        // Xử lý cảnh báo quá nhiệt (Đèn LED vàng)
        if (!isnan(currentData.temperature) && currentData.temperature > TEMP_THRESHOLD) {
            setTempAlertLed(true);
        } else {
            setTempAlertLed(false);
        }

        // Trạng thái đèn chiếu sáng (LED Cyan)
        bool isLightOn = isNightMode 
                         ? (now - pirMotionTimer < 5000 && pirMotionTimer > 0)
                         : (currentData.isDark);

        // Xuất thông tin trực quan ra Serial Monitor
        Serial.printf("| %-9s | %9d | %9.1f | %9.1f | %5d | %5d | %-8s | %-9s |\r\n",
                      (currentData.gas > GAS_THRESHOLD) ? "LEAK (!)" : "SAFE",
                      currentData.gas,
                      isnan(currentData.temperature) ? 0.0f : currentData.temperature,
                      isnan(currentData.humidity) ? 0.0f : currentData.humidity,
                      currentData.lux,
                      currentData.motion ? 1 : 0,
                      buzzerState ? "ON (!)" : "OFF",
                      isLightOn ? "ON" : "OFF");
    }

    // 3. Cập nhật trạng thái đèn chiếu sáng theo chế độ
    if (isNightMode) {
        if (now - pirMotionTimer < 5000 && pirMotionTimer > 0) {
            setLightLed(true);
        } else {
            setLightLed(false);
        }
    } else {
        setLightLed(currentData.isDark);
    }

    // 4. Gửi telemetry qua MQTT định kỳ mỗi 2 giây
    if (now - lastSend > 2000) {
        lastSend = now;
        publishTelemetry(
            currentData.temperature,
            currentData.humidity,
            currentData.gas,
            currentData.lux,
            currentData.motion
        );
    }
}