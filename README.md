# InhabitX main on a XIAO ESP32-S3, with the laptop as its screen

The main (bookings, ghost release, lost & found, answering the web page) runs on the XIAO ESP32-S3 (Sense) instead of the laptop. Its touch screen UI is drawn on the board and shown in a browser: the laptop (or a phone) is the screen, clicks are the touches.

```text
XIAO ESP32-S3  --MQTT/TLS-->  HiveMQ  <--  web page, nodes
      |
      +-- Wi-Fi --> http://inhabitx-main.local/  (browser = the main's screen)
```

## What you need

- XIAO ESP32-S3 or XIAO ESP32-S3 Sense, USB-C cable
- a 2.4 GHz Wi-Fi with internet that the laptop is on too (phone hotspot; on iPhone turn on *Maximize Compatibility*)
- Arduino IDE 2 with the **esp32** boards package by Espressif (3.1.x)
- the libraries **lvgl 9.2.2**, **PubSubClient 2.8**, **WebSockets 2.6.1** (by Markus Sattler / Links2004) from the Library Manager

## How to use

1. **Before you start:** only one main at a time. Stop `main_sim` on the laptop (both log in as `inhabitx-main-Z`).
2. **LVGL settings:** copy `lv_conf.h` from this branch **next to** the lvgl library folder: `~/Arduino/libraries/lv_conf.h` (Windows: `Documents\Arduino\libraries\lv_conf.h`).
3. **Passwords:** in `inhabitx_main/`, copy `secrets.h.example` to `secrets.h` and fill in Wi-Fi name, Wi-Fi password and the broker password. `secrets.h` is never committed.
4. **Open** `inhabitx_main/inhabitx_main.ino` in the Arduino IDE and set under **Tools**:
   - Board: **XIAO_ESP32S3**
   - PSRAM: **OPI PSRAM**
   - Partition Scheme: **Maximum APP (7.9MB APP No OTA/No FS)**: the sketch is ~3.2 MB, the default 3.3 MB would be nearly full
5. **Upload**, then open the Serial Monitor (115200 baud):
   ```text
   wifi: connected, screen at http://192.168.x.y/ or http://inhabitx-main.local/
   time: 10:05
   mqtt: connected as devices, listening on inhabitx/Z/#
   ```
6. **The screen:** open that address in a browser on the laptop. Full screen (F11) makes it look like the real one. Click = touch.
7. **Rooms:** run the fake nodes on the laptop (`.venv/bin/python tools/fake_nodes.py --shuffle 3` on `main`). The page https://nadaabbas444.github.io/inhabitx/ and the XIAO's screen now show the same rooms; booking works on both.

## Files

| File | What |
|---|---|
| `inhabitx_main/inhabitx_main.ino` | Wi-Fi, time (NTP), MQTT over TLS with the last will, the main loop |
| `inhabitx_main/remote_screen.*` | LVGL display + touch that live in the browser: changed areas go out run-length coded over a WebSocket (port 81), clicks come back |
| `inhabitx_main/viewer.html` | the page the laptop shows; `viewer_html.h` is generated from it (`python3 gen_viewer_html.py`) |
| `inhabitx_main/src/inhabitx/` | the main's logic and UI, same code as `main_sim` (1024×600) |
| `lv_conf.h` | LVGL settings (2 MB of LVGL memory in PSRAM, TinyTTF fonts) |

## If it doesn't work

| Problem | Check |
|---|---|
| `wifi: connecting....` forever | name/password in `secrets.h`; the network must be 2.4 GHz and WPA2 |
| `no PSRAM?` | Tools > PSRAM must be **OPI PSRAM** |
| `mqtt: failed (state 4)` or `(state 5)` | broker password in `secrets.h` |
| `mqtt: failed (state -2)` | no internet on that Wi-Fi, or port 8883 blocked |
| `Sketch too big` when compiling | Tools > Partition Scheme: **Maximum APP (7.9MB APP No OTA/No FS)** |
| browser page stays black | laptop and XIAO must be on the same Wi-Fi; try the IP instead of `.local` |
| the laptop's `main_sim` keeps reconnecting | two mains: stop `main_sim` |
| bookings say "Pick a time today…" | NTP not reached yet: the fake nodes also give the time, start them |

Not tested on a real XIAO yet: the build is checked with the ESP32 Arduino tools, the screen page and its protocol with a stand-in on the PC.
