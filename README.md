#  IoT Smart Home – Giám sát an toàn và tự động hóa

##  Giới thiệu

Dự án **IoT Smart Home – Nhà thông minh giám sát an toàn và tự động hóa** được xây dựng trên nền tảng **ESP32**, nhằm mô phỏng hệ thống nhà thông minh có khả năng thu thập dữ liệu cảm biến, xử lý tự động và điều khiển các thiết bị trong nhà.

Hệ thống hiện tập trung vào:

*  Đo nhiệt độ và độ ẩm bằng DHT22.
*  Phát hiện chuyển động bằng cảm biến PIR.
*  Đo cường độ ánh sáng bằng LDR.
*  Điều khiển đèn tự động.
*  Điều khiển quạt tự động.
*  Kết nối ESP32 với Wi-Fi.
*  Gửi dữ liệu cảm biến qua MQTT.
*  Nhận lệnh điều khiển thiết bị qua MQTT.
*  Mô phỏng phần cứng và mạng trên Wokwi.
*  Phát triển firmware bằng PlatformIO.

---

#  Kiến trúc hệ thống

```text
┌──────────────────────────────────────┐
│              SENSORS                 │
│                                      │
│   PIR        LDR        DHT22        │
│    │          │           │          │
└────┼──────────┼───────────┼──────────┘
     │          │           │
     └──────────┼───────────┘
                ▼
        ┌───────────────┐
        │     ESP32     │
        │               │
        │ Sensor Read   │
        │ Automation    │
        │ MQTT Client   │
        └───────┬───────┘
                │
              Wi-Fi
                │
                ▼
        ┌───────────────┐
        │  MQTT Broker  │
        └───────┬───────┘
                │
          ┌─────┴─────┐
          ▼           ▼
     Dashboard    Controller
```

ESP32 đồng thời điều khiển trực tiếp:

```text
                 ESP32
                   │
          ┌────────┴────────┐
          │                 │
          ▼                 ▼
   Relay Light         Relay Fan
          │                 │
          ▼                 ▼
        💡 Đèn             🌀 Quạt
```

---

# Phần cứng

## ESP32

ESP32 là bộ điều khiển trung tâm của hệ thống.

Nhiệm vụ:

* Đọc cảm biến.
* Xử lý dữ liệu.
* Thực hiện logic tự động hóa.
* Điều khiển relay.
* Kết nối Wi-Fi.
* Giao tiếp MQTT.

---

## PIR Motion Sensor

PIR được sử dụng để phát hiện chuyển động.

```text
PIR
 │
 ▼
ESP32
 │
 ▼
Motion detected?
 │
 ├── YES → Xử lý tự động hóa
 │
 └── NO  → Tiếp tục giám sát
```

---

## LDR

LDR được sử dụng để đo mức ánh sáng môi trường.

Giá trị đọc từ chân ADC của ESP32 được so sánh với:

```cpp
#define LDR_THRESHOLD 2000
```

Ngưỡng này có thể điều chỉnh tùy theo mô hình mô phỏng hoặc phần cứng thực tế.

---

## DHT22

DHT22 cung cấp:

* Nhiệt độ.
* Độ ẩm.

Hai dữ liệu này được gửi lên MQTT Broker theo các topic tương ứng.

---

# Sơ đồ GPIO

Cấu hình GPIO hiện tại được khai báo trong `config.h`:

| Thiết bị   |    GPIO | Loại           |
| ---------- | ------: | -------------- |
| PIR        | GPIO 27 | Digital Input  |
| LDR        | GPIO 34 | Analog Input   |
| DHT22      |  GPIO 4 | Digital        |
| Relay đèn  | GPIO 26 | Digital Output |
| Relay quạt | GPIO 25 | Digital Output |
| LED 1      | GPIO 19 | Digital Output |
| LED 2      | GPIO 18 | Digital Output |

### Sơ đồ kết nối logic

```text
PIR ───────────── GPIO27
LDR ───────────── GPIO34
DHT22 ─────────── GPIO4

Relay Light ───── GPIO26
Relay Fan ─────── GPIO25

LED 1 ─────────── GPIO19
LED 2 ─────────── GPIO18
```

---

#  Điều khiển thiết bị

## Đèn

Relay đèn được điều khiển thông qua:

```cpp
#define RELAY_LIGHT_PIN 26
```

Relay hiện tại trong mô phỏng Wokwi sử dụng:

```cpp
#define RELAY_ACTIVE_HIGH true
```

Có nghĩa:

```text
HIGH → Relay ON
LOW  → Relay OFF
```

Logic điều khiển đèn có thể dựa trên:

* Chuyển động.
* Mức ánh sáng.
* Thời gian.
* Lệnh MQTT.

---

## Quạt

Relay quạt sử dụng:

```cpp
#define RELAY_FAN_PIN 25
```

Quạt có thể được điều khiển:

* Tự động theo nhiệt độ.
* Bằng lệnh MQTT.
* Theo trạng thái phòng.

Ví dụ:

```text
Temperature >= threshold
          │
          ▼
       Fan ON

Temperature < threshold
          │
          ▼
       Fan OFF
```

---

# Wi-Fi

ESP32 hiện sử dụng mạng Wi-Fi của Wokwi:

```cpp
#define WIFI_SSID "Wokwi-GUEST"
```

Trong môi trường Wokwi, mạng `Wokwi-GUEST` được sử dụng để mô phỏng kết nối Internet.

Nếu triển khai trên phần cứng thật, cần thay đổi cấu hình Wi-Fi tương ứng.

---

#  MQTT

Dự án sử dụng **MQTT** làm giao thức truyền thông giữa ESP32 và MQTT Broker.

Mô hình:

```text
ESP32
 │
 │ MQTT Publish
 │
 ▼
MQTT Broker
 │
 │ MQTT Subscribe
 ▼
Dashboard / Client
```

ESP32 có thể:

* Publish dữ liệu cảm biến.
* Subscribe topic điều khiển.
* Publish trạng thái thiết bị.
* Tự động reconnect khi mất kết nối.

---

# MQTT Broker hiện tại

Trong quá trình phát triển và mô phỏng, project hiện sử dụng **Pinggy** để tạo đường hầm TCP phục vụ kiểm thử MQTT.

Cấu hình hiện tại có dạng:

```cpp
#define MQTT_HOST "YOUR_PINGGY_HOST"
#define MQTT_PORT YOUR_PINGGY_PORT
```

> ⚠️ Endpoint Pinggy là endpoint tạm thời và có thể thay đổi sau mỗi lần chạy hoặc theo thời gian sử dụng. Không nên ghi endpoint hiện tại cố định vào `config.example.h`.

Khi triển khai ổn định, nên thay Pinggy bằng một MQTT Broker cố định.

Ví dụ:

```text
ESP32
  │
  ▼
MQTT Broker
  │
  ├── Dashboard
  ├── Database
  └── Application
```

---

# Sensor Topics

Các topic sử dụng để gửi dữ liệu cảm biến:

| Topic                          | Dữ liệu     |
| ------------------------------ | ----------- |
| `smarthome/sensor/temperature` | Nhiệt độ    |
| `smarthome/sensor/humidity`    | Độ ẩm       |
| `smarthome/sensor/motion`      | Chuyển động |
| `smarthome/sensor/light`       | Ánh sáng    |
---

#  Control Topics

Các topic nhận lệnh điều khiển:

| Topic                     | Chức năng       |
| ------------------------- | --------------- |
| `smarthome/control/light` | Điều khiển đèn  |
| `smarthome/control/fan`   | Điều khiển quạt |
---

#  Actuator Topics

ESP32 có thể gửi trạng thái thực tế của thiết bị lên:

| Topic                      | Trạng thái      |
| -------------------------- | --------------- |
| `smarthome/actuator/light` | Trạng thái đèn  |
| `smarthome/actuator/fan`   | Trạng thái quạt |
---

# Cấu hình tự động hóa

Các thông số hiện tại:

```cpp
#define LDR_THRESHOLD 2000
#define LIGHT_OFF_DELAY 10000
#define SENSOR_INTERVAL 5000
```

| Tham số             |  Giá trị | Ý nghĩa                   |
| ------------------- | -------: | ------------------------- |
| `LDR_THRESHOLD`     |     2000 | Ngưỡng ánh sáng           |
| `LIGHT_OFF_DELAY`   | 10000 ms | Thời gian chờ tắt đèn     |
| `SENSOR_INTERVAL`   |  5000 ms | Chu kỳ đọc/gửi cảm biến   |
| `RELAY_ACTIVE_HIGH` |   `true` | Relay kích hoạt bằng HIGH |

---

#  Chu trình hoạt động

```text
          START
            │
            ▼
    Initialize GPIO
            │
            ▼
     Initialize DHT22
            │
            ▼
      Connect Wi-Fi
            │
            ▼
      Connect MQTT
            │
            ▼
      ┌─────────────┐
      │ Read Sensors│
      └──────┬──────┘
             │
             ▼
      Process Data
             │
             ▼
    Automation Logic
             │
       ┌─────┴─────┐
       ▼           ▼
    Light        Fan
    Control      Control
       │           │
       └─────┬─────┘
             ▼
      Publish MQTT
             │
             ▼
      Check MQTT
      Connection
             │
             ▼
          LOOP
```

---

# Logic tự động hóa

## Phát hiện chuyển động

```text
PIR = HIGH
   │
   ▼
Có chuyển động
   │
   ▼
Kiểm tra điều kiện ánh sáng
   │
   ├── Tối → Bật đèn
   │
   └── Sáng → Không bật đèn
```

Khi không còn chuyển động, hệ thống sử dụng:

```cpp
LIGHT_OFF_DELAY = 10000
```

để tránh tắt đèn ngay lập tức.

---

## Điều khiển theo ánh sáng

```text
LDR
 │
 ▼
Read ADC
 │
 ▼
Compare LDR_THRESHOLD
 │
 ├── Điều kiện tối
 │       ↓
 │    Có thể bật đèn
 │
 └── Điều kiện sáng
         ↓
      Không cần đèn
```

> Lưu ý: quan hệ giữa giá trị ADC và "sáng/tối" phụ thuộc cách mắc LDR trong mạch. Khi triển khai phần cứng thực tế cần hiệu chuẩn lại `LDR_THRESHOLD`.

---

## Điều khiển quạt

Nhiệt độ từ DHT22 được sử dụng làm đầu vào cho logic điều khiển quạt.

```text
DHT22
  │
  ▼
Temperature
  │
  ▼
Automation
  │
  ├── Nhiệt độ cao → Fan ON
  │
  └── Nhiệt độ bình thường → Fan OFF
```

Ngưỡng nhiệt độ có thể được cấu hình riêng trong chương trình.

---

#  Mô phỏng Wokwi

Dự án được mô phỏng trên **Wokwi** nhằm kiểm thử:

* ESP32.
* PIR.
* LDR.
* DHT22.
* Relay.
* LED.
* Wi-Fi.
* MQTT.

Các file quan trọng:

```text
diagram.json
wokwi.toml
```

---

#  Môi trường phát triển

## Công cụ

* Visual Studio Code
* PlatformIO
* Wokwi
* Git / GitHub

## Framework

* ESP32
* Arduino Framework

## Giao thức

* Wi-Fi
* MQTT

---

#  Cấu trúc project

```text
IoT-Smart-Home/
│
├── README.md
├── platformio.ini
├── wokwi.toml
├── diagram.json
│
├── include/
│   ├── config.h
│   └── config.example.h
│
├── src/
│
├── lib/
│
├── test/
│
└── .gitignore
```

---

#  Quản lý cấu hình

Project sử dụng hai file:

```text
config.example.h
config.h
```

## `config.example.h`

Đây là **file mẫu** dùng để chia sẻ trên GitHub.

File này không chứa:

* MQTT endpoint thực tế.
* MQTT username/password.
* Wi-Fi password cá nhân.
* Các thông tin cấu hình riêng.

Ví dụ:

```cpp
#define WIFI_SSID "Wokwi-GUEST"

#define MQTT_HOST "YOUR_MQTT_HOST"
#define MQTT_PORT 1883
```

---

## `config.h`

Đây là file cấu hình thực tế dùng khi chạy project.

Ví dụ:

```cpp
#define WIFI_SSID "Wokwi-GUEST"

#define MQTT_HOST "your-real-mqtt-host"
#define MQTT_PORT 1883
```

Nếu MQTT Broker yêu cầu xác thực:

```cpp
#define MQTT_USERNAME "YOUR_MQTT_USERNAME"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"
```

---

#  Cài đặt

## 1. Clone project

```bash
git clone <repository-url>
cd IoT-Smart-Home
```

## 2. Mở project

Mở thư mục bằng Visual Studio Code.

Đảm bảo đã cài PlatformIO.

---

## 3. Tạo `config.h`

Copy file mẫu:

### Linux / macOS

```bash
cp include/config.example.h include/config.h
```

### Windows

```cmd
copy include\config.example.h include\config.h
```

Sau đó chỉnh các thông số cần thiết trong:

```text
include/config.h
```

---

#  Build

Sử dụng PlatformIO:

```bash
pio run
```

Hoặc trong Visual Studio Code:

```text
PlatformIO
    ↓
Build
```

Firmware được tạo tại:

```text
.pio/build/esp32doit-devkit-v1/
```

Các file quan trọng:

```text
firmware.bin
firmware.elf
```

---

#  Chạy Wokwi

File `wokwi.toml` sử dụng firmware được build bởi PlatformIO:

```toml
[wokwi]
version = 1
firmware = ".pio/build/esp32doit-devkit-v1/firmware.bin"
elf = ".pio/build/esp32doit-devkit-v1/firmware.elf"
```

Quy trình:

```text
Edit code
   ↓
PlatformIO Build
   ↓
firmware.bin + firmware.elf
   ↓
Run Wokwi
   ↓
Kiểm tra mạch
   ↓
Kiểm tra Serial
   ↓
Kiểm tra MQTT
```

---

#  Kiểm thử

Các trường hợp kiểm thử chính:

| STT | Điều kiện                        | Kết quả mong đợi             |
| --: | -------------------------------- | ---------------------------- |
|   1 | Không chuyển động                | Không kích hoạt đèn          |
|   2 | Có chuyển động + môi trường tối  | Đèn ON                       |
|   3 | Có chuyển động + môi trường sáng | Không bật đèn tự động        |
|   4 | Hết chuyển động                  | Sau thời gian delay, đèn OFF |
|   5 | Nhiệt độ cao                     | Quạt ON                      |
|   6 | Nhiệt độ bình thường             | Quạt OFF                     |
|   7 | MQTT hoạt động                   | Publish sensor data          |
|   8 | Có lệnh MQTT                     | ESP32 điều khiển thiết bị    |
|   9 | Mất MQTT                         | Thực hiện reconnect          |
|  10 | Mất Wi-Fi                        | Thực hiện reconnect          |

---

#  Kiến trúc hướng tới

```text
┌───────────────┐
│    Sensors    │
│ PIR / LDR /   │
│ DHT22 / ...   │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│     ESP32     │
│ Automation    │
└───────┬───────┘
        │ Wi-Fi
        ▼
┌───────────────┐
│ MQTT Broker   │
└───────┬───────┘
        │
   ┌────┼────┐
   ▼    ▼    ▼
Dashboard DB  App
```

---

#  Trạng thái dự án

### Phần cứng

* [x] ESP32
* [x] PIR
* [x] LDR
* [x] DHT22
* [x] Relay đèn
* [x] Relay quạt
* [x] LED

### Firmware

* [x] Cấu hình GPIO
* [x] Cấu hình Wi-Fi
* [x] Cấu hình MQTT
* [x] Cấu hình Sensor Topics
* [x] Cấu hình Control Topics
* [x] Cấu hình Actuator Topics
* [x] Cấu hình tự động hóa
* [x] Tách `config.example.h`
* [ ] Hoàn thiện toàn bộ logic tự động hóa
* [ ] Hoàn thiện xử lý lỗi cảm biến
* [ ] Hoàn thiện reconnect Wi-Fi/MQTT

### IoT

* [x] Kết nối Wi-Fi
* [x] MQTT
* [x] Publish dữ liệu cảm biến
* [x] Subscribe lệnh điều khiển
* [ ] MQTT Broker ổn định
* [ ] Dashboard
* [ ] Database
* [ ] Điều khiển từ xa

### Mô phỏng

* [x] Wokwi
* [x] PlatformIO
* [x] `diagram.json`
* [x] `wokwi.toml`

---

#  License

Dự án được thực hiện phục vụ mục đích **học tập, nghiên cứu và mô phỏng hệ thống IoT**.

---

