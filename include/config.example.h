```cpp
#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
// GPIO
// =====================================================

#define PIR_PIN 27

#define LDR_PIN 34

#define DHT_PIN 4
#define DHT_TYPE DHT22

#define RELAY_LIGHT_PIN 26
#define RELAY_FAN_PIN 25

#define LED_1_PIN 19
#define LED_2_PIN 18


// =====================================================
// WIFI
// =====================================================

// Wokwi virtual Wi-Fi network
#define WIFI_SSID "Wokwi-GUEST"

// Nếu Wi-Fi yêu cầu mật khẩu, khai báo tại đây.
// Để trống nếu sử dụng Wokwi-GUEST.
#define WIFI_PASSWORD ""


// =====================================================
// MQTT
// =====================================================

// Thay bằng MQTT Broker của bạn.
//
// Ví dụ:
// #define MQTT_HOST "broker.hivemq.com"
// #define MQTT_PORT 1883
//
// Hoặc broker/server riêng:
// #define MQTT_HOST "your-mqtt-broker.example.com"
// #define MQTT_PORT 1883

#define MQTT_HOST "YOUR_MQTT_HOST"
#define MQTT_PORT 1883

// Nếu MQTT Broker yêu cầu tài khoản:
// #define MQTT_USERNAME "YOUR_MQTT_USERNAME"
// #define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"


// =====================================================
// SENSOR TOPICS
// =====================================================

#define TOPIC_TEMPERATURE "smarthome/sensor/temperature"

#define TOPIC_HUMIDITY "smarthome/sensor/humidity"

#define TOPIC_MOTION "smarthome/sensor/motion"

#define TOPIC_LIGHT "smarthome/sensor/light"


// =====================================================
// CONTROL TOPICS
// =====================================================

#define TOPIC_CONTROL_LIGHT "smarthome/control/light"

#define TOPIC_CONTROL_FAN "smarthome/control/fan"


// =====================================================
// ACTUATOR TOPICS
// =====================================================

#define TOPIC_ACTUATOR_LIGHT "smarthome/actuator/light"

#define TOPIC_ACTUATOR_FAN "smarthome/actuator/fan"


// =====================================================
// AUTOMATION
// =====================================================

// Ngưỡng ánh sáng của LDR
#define LDR_THRESHOLD 2000

// Thời gian chờ trước khi tắt đèn
// Đơn vị: milliseconds
#define LIGHT_OFF_DELAY 10000

// Chu kỳ đọc cảm biến
// Đơn vị: milliseconds
#define SENSOR_INTERVAL 5000


// =====================================================
// RELAY CONFIGURATION
// =====================================================

// Relay trong mô phỏng Wokwi hiện tại:
// HIGH = ON
// LOW  = OFF

#define RELAY_ACTIVE_HIGH true


#endif
```
