# Cập nhật GitHub: giữ repo `tiendat07525/SmartHomeProject`, gộp dashboard vào

> Quyết định D-004, D-005: chỉ dùng tài khoản `tiendat07525`; repo `SmartHomeProject` trở thành monorepo.
> Người dùng tự chạy. Quy trình đã được chạy thử trên repo giả.
> Kết quả: lịch sử firmware **liền mạch** (git nhận diện là đổi tên file sang `firmware/`), lịch sử dashboard được ghép vào bằng 1 merge, **push thường, không cần `--force`**.
> Chạy tại `/run/media/dat/Data/IoT/MainV2`.

## Bước 1 — Chốt thay đổi đang dở trong `firmware/`

```bash
cd firmware
git worktree prune          # dọn worktree cũ đã mất thư mục
git pull origin main        # đảm bảo bản local mới nhất (nếu báo xung đột -> dừng, hỏi lại)
git status                  # 8 file src/... + config.example.h + .gitignore; KHÔNG được có include/config.h
git add -A
git commit -m "feat(firmware): MQ2, còi cảnh báo, chế độ đêm, bảng Serial"
cd ..
```

## Bước 2 — Đưa `.git` của SmartHomeProject lên thư mục gốc

```bash
mv firmware/.git .git                 # repo SmartHomeProject giờ bao toàn bộ MainV2
mv dashboard/.git ../_dashboard.git   # cất repo dashboard ra ngoài (backup + nguồn lịch sử)
git remote -v                         # phải là github.com/tiendat07525/SmartHomeProject
```

## Bước 3 — Commit cấu trúc mới (chưa gồm dashboard)

```bash
git add -A -- . ':!dashboard'
git status        # mong đợi: R README.md -> firmware/README.md, R src/... -> firmware/src/...
                  #           A backend/..., docs/..., .claude/..., CLAUDE.md
                  # KHÔNG được thấy: firmware/include/config.h, backend/.env, node_modules, .pio
git commit -m "refactor(repo): chuyển sang monorepo, firmware vào firmware/

- Mã ESP32 chuyển vào firmware/ (giữ lịch sử qua rename)
- Thêm backend/ (docker-compose, bridge, mosquitto), docs/, .claude/, CLAUDE.md
- .gitignore gốc, backend/.env.example, sửa config.example.h
Refs: Giai đoạn 0"
```

## Bước 4 — Ghép lịch sử dashboard vào `dashboard/`

```bash
git fetch ../_dashboard.git main:history/dashboard
git merge -s ours --no-commit --allow-unrelated-histories history/dashboard
git read-tree --prefix=dashboard/ history/dashboard
git commit -m "chore(repo): ghép lịch sử smarthome-iot-node-red vào dashboard/"
```

Sau bước này `git status` sẽ báo `deleted: dashboard/huong_dan_chay_lai_node_red_smart_home.md` (file đã chuyển sang `docs/node-red-windows.md`):

```bash
git add -A
git commit -m "docs: chuyển hướng dẫn Node-RED sang docs/node-red-windows.md"
```

## Bước 5 — Kiểm tra rồi push

```bash
git log --oneline --graph -15
git log --oneline --follow -- firmware/src/main.cpp   # phải thấy cả commit cũ
git ls-files | grep -E "/config\.h$|\.env$"           # phải KHÔNG in ra dòng nào
git push origin main                                  # push thường, không --force
git branch -D history/dashboard                       # nhánh tạm, xóa sau khi push
```

Khi đã ổn trên GitHub có thể xóa backup `../_dashboard.git`.

## Lưu ý
- Repo `github.com/NguyenDat-1010/smarthome-iot-node-red` **không bị đụng tới**; lịch sử chỉ được lấy từ bản `.git` local.
- Sau khi push, trang GitHub sẽ không còn README ở gốc (README cũ nằm ở `firmware/README.md`). README gốc mới thuộc Giai đoạn 5 — có thể làm sớm nếu cần.
- Nếu ai khác trong nhóm đang có bản clone cũ: chỉ cần `git pull` — các file sẽ tự chuyển sang `firmware/`. Code đang sửa dở của họ cần commit trước khi pull.
- Bảo mật: `config.h` và `.env` chưa từng được commit. Mật khẩu/token mặc định trong `backend/docker-compose.yml` sẽ được commit — sẽ chuyển sang `.env` ở Giai đoạn 3.
