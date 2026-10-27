# Hướng dẫn chạy lại Node-RED – Dự án Smart Home IoT

## 1. Mục tiêu

Tài liệu này hướng dẫn khởi động lại Node-RED trên Windows và mở giao diện Dashboard của dự án Smart Home IoT.

## 2. Khởi động Node-RED

1. Nhấn `Windows + R`.
2. Nhập `cmd`, sau đó nhấn `Enter`.
3. Trong cửa sổ Command Prompt, chạy:

```cmd
node-red
```

4. Chờ Node-RED khởi động. Quan sát log trong cửa sổ CMD để xem có lỗi hay không.
5. Giữ cửa sổ CMD mở trong lúc sử dụng Node-RED. Đóng cửa sổ này có thể làm Node-RED dừng hoạt động.

### Nếu báo lỗi `node-red is not recognized`

CMD không tìm thấy lệnh `node-red`. Kiểm tra Node.js và npm đã được cài chưa:

```cmd
node -v
npm -v
```

Nếu hai lệnh trên chạy được nhưng `node-red` không chạy, kiểm tra Node-RED đã được cài đặt toàn cục chưa:

```cmd
npm list -g --depth=0
```

Nếu cần cài Node-RED toàn cục:

```cmd
npm install -g --unsafe-perm node-red
```

Sau khi cài xong, đóng và mở lại CMD rồi chạy `node-red` lần nữa.

## 3. Mở giao diện Node-RED

Mở trình duyệt và truy cập:

- Editor: <http://localhost:1880>

Tại đây, có thể xem và chỉnh sửa các flow đã lưu trên máy.

## 4. Mở Dashboard Smart Home

Truy cập:

- Dashboard: <http://localhost:1880/ui>

Nếu Dashboard không hiển thị:
- Kiểm tra Node-RED có đang chạy không.
- Kiểm tra các node có báo lỗi màu đỏ hay không.
- Kiểm tra gói `node-red-dashboard` đã được cài đặt chưa.
- Mở tab Debug và cửa sổ CMD để xem thông báo lỗi.

## 5. Khởi động các dịch vụ liên quan

Để kiểm tra đầy đủ dự án, khởi động những dịch vụ cần dùng.

### Mosquitto (MQTT)

Mosquitto phải chạy để Node-RED nhận/gửi dữ liệu qua MQTT. Nếu đã cài Mosquitto dưới dạng Windows Service, mở `services.msc` và kiểm tra dịch vụ Mosquitto đang ở trạng thái `Running`.

Có thể gửi dữ liệu thử từ một cửa sổ CMD khác:

```cmd
mosquitto_pub -h localhost -t home/livingroom/sensor -m "{\"temperature\":32,\"humidity\":72,\"light\":50,\"motion\":true}"
```

Lệnh trên gửi một gói dữ liệu cảm biến thử lên topic `home/livingroom/sensor`.

### InfluxDB (nếu sử dụng lưu lịch sử)

Nếu InfluxDB được giải nén tại `D:\InfluxDB`, mở CMD và chạy:

```cmd
cd /d D:\InfluxDB
influxd.exe
```

Sau đó truy cập <http://localhost:8086>. Giữ cửa sổ chạy `influxd.exe` mở trong lúc sử dụng nếu InfluxDB chưa chạy dưới dạng dịch vụ.

## 6. Kiểm tra flow

1. Mở <http://localhost:1880>.
2. Kiểm tra các node MQTT IN, JSON, Function xử lý dữ liệu và các node Dashboard.
3. Nhấn **Deploy** sau khi có thay đổi.
4. Mở Dashboard tại <http://localhost:1880/ui>.
5. Gửi dữ liệu thử bằng lệnh `mosquitto_pub` ở trên.
6. Kiểm tra các widget nhiệt độ, độ ẩm, ánh sáng, chuyển động và trạng thái điều khiển có cập nhật không.

## 7. Lỗi thường gặp

| Hiện tượng | Cách kiểm tra |
|---|---|
| `node-red is not recognized` | Kiểm tra Node.js/npm và cài đặt Node-RED toàn cục. |
| Không mở được `localhost:1880` | Kiểm tra cửa sổ CMD của Node-RED và log khởi động. |
| Port 1880 đang được sử dụng | Kiểm tra có phiên Node-RED khác đang chạy hay không; tránh khởi động nhiều phiên cùng lúc. |
| Dashboard trắng hoặc không vào được `/ui` | Kiểm tra `node-red-dashboard`, lỗi node và log. |
| Không nhận dữ liệu MQTT | Kiểm tra Mosquitto, topic `home/livingroom/sensor`, broker `localhost` và JSON đầu vào. |
| Không lưu được dữ liệu InfluxDB | Kiểm tra InfluxDB đang chạy, địa chỉ, bucket, organization và token trong cấu hình node. |

## 8. Checklist chạy dự án

- [ ] Mosquitto đang chạy (nếu dùng MQTT).
- [ ] Node-RED chạy thành công.
- [ ] Editor mở được tại `http://localhost:1880`.
- [ ] Dashboard mở được tại `http://localhost:1880/ui`.
- [ ] Đã Deploy flow nếu vừa chỉnh sửa.
- [ ] Gửi thử dữ liệu MQTT và xác nhận widget cập nhật.
- [ ] InfluxDB chạy và nhận dữ liệu (nếu cần lưu lịch sử).

---

**Lưu ý:** Flow Node-RED và các dịch vụ như Mosquitto/InfluxDB là những thành phần riêng biệt. Chạy Node-RED không tự động đảm bảo Mosquitto hoặc InfluxDB cũng đang hoạt động.
