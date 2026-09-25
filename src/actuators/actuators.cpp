#include "actuators.h"
#include "../include/config.h"

void initActuators() {
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_LED_LIGHT, OUTPUT);
    pinMode(PIN_LED_TEMP, OUTPUT);
    pinMode(PIN_RELAY1, OUTPUT);
    pinMode(PIN_RELAY2, OUTPUT);

    // Mặc định tắt các cơ cấu chấp hành
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED_LIGHT, LOW);
    digitalWrite(PIN_LED_TEMP, LOW);
    // Relay module kích mức LOW: HIGH là ngắt
    digitalWrite(PIN_RELAY1, HIGH);
    digitalWrite(PIN_RELAY2, HIGH);
}

void setBuzzer(bool state) {
    if (state) {
        tone(PIN_BUZZER, 1000); // Phát âm 1000Hz cảnh báo
    } else {
        noTone(PIN_BUZZER);
    }
}

void setLightLed(bool state) {
    digitalWrite(PIN_LED_LIGHT, state ? HIGH : LOW);
}

void setTempAlertLed(bool state) {
    digitalWrite(PIN_LED_TEMP, state ? HIGH : LOW);
}

void setRelay(int relayNum, bool state) {
    int pin = (relayNum == 1) ? PIN_RELAY1 : PIN_RELAY2;
    digitalWrite(pin, state ? LOW : HIGH); // Active LOW[cite: 1]
}