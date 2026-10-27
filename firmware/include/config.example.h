// =====================================================================
// config.example.h — FILE MẪU, được commit lên git.
// Cách dùng: copy thành include/config.h rồi điền giá trị thật.
//   Linux/macOS: cp include/config.example.h include/config.h
//   Windows    : copy include\config.example.h include\config.h
// include/config.h đã nằm trong .gitignore — KHÔNG commit file đó.
// Tên hằng ở đây phải khớp 1-1 với config.h mà code đang dùng.
// =====================================================================
#ifndef CONFIG_H
#define CONFIG_H

// ---------------- Wi-Fi ----------------
// Wokwi: SSID "Wokwi-GUEST", mật khẩu rỗng.
#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASS       ""

// ---------------- MQTT ----------------
// Broker local (docker compose trong backend/): IP máy chạy Docker trong LAN.
// Wokwi không truy cập được IP LAN -> dùng broker công cộng hoặc tunnel.
#define MQTT_SERVER     "YOUR_MQTT_HOST"   // vd: "192.168.1.10" hoặc "broker.hivemq.com"
#define MQTT_PORT       1883
#define MQTT_CLIENT_ID  "ESP32_SmartHome_Client"

// ---------------- MQTT Topics ----------------
#define TOPIC_TELEMETRY "smarthome/telemetry"
#define TOPIC_MODE      "smarthome/mode"          // Nhận lệnh: "NORMAL" hoặc "NIGHT"
#define TOPIC_RELAY     "smarthome/relay/control" // Nhận lệnh: "R1:ON", "R1:OFF", "R2:ON", "R2:OFF"

// ---------------- GPIO ----------------
#define PIN_MQ2         35  // ADC1
#define PIN_LDR         34  // ADC1
#define PIN_DHT         4
#define PIN_PIR         27
#define PIN_BUZZER      14
#define PIN_LED_LIGHT   18  // Đèn chiếu sáng (Cyan)
#define PIN_LED_TEMP    21  // Đèn cảnh báo nhiệt (Vàng) — LƯU Ý: diagram.json đang nối LED vàng vào GPIO19
#define PIN_RELAY1      26
#define PIN_RELAY2      25

// ---------------- Ngưỡng ----------------
#define TEMP_THRESHOLD      35.0  // °C
#define GAS_THRESHOLD       100   // ADC 0-4095 (bridge đang dùng 1800 — chưa thống nhất)
#define LDR_DARK_THRESHOLD  2000  // ADC > ngưỡng => tối

#endif
