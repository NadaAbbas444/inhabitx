# Master screen UI: PC simulator

Runs the master's LVGL UI on a Linux laptop in a window the size of the board's screen, with the **same** `ui/` and `logic/` code that goes onto the E84. Only `main.c` (window, clock, fake slaves) and `mqtt_link.c` (broker connection) are PC-specific.

## Build and run

Needs `cmake`, `gcc`, `libx11-dev` and `libssl-dev`. LVGL 9.2.2 and coreMQTT 2.3.1 are downloaded by CMake on the first build.

```bash
cmake -S firmware/master/sim -B firmware/master/sim/build
cmake --build firmware/master/sim/build -j
./firmware/master/sim/build/master_sim --at 10:05
```

| Option | Effect |
|---|---|
| *(none)* | connect to the broker from `broker.conf`, no fake slaves, start with no bookings |
| `--offline` | no broker: built-in fake slaves and demo bookings (also used when `broker.conf` has no password) |
| `--fake` | broker **and** the built-in fake slaves, until real slaves or `tools/fake_slaves.py` run |
| `--conf FILE` | broker settings from another file (or set `IX_BROKER_CONF`) |
| `--at HH:MM` | pretend it's that time today. Bookings only work between 08:00 and 24:00 |

A booking nobody shows up for is released after 30 s (demo). For a test with real people (I2) build with the real 10 min: `cmake -S firmware/master/sim -B firmware/master/sim/build -DGHOST_RELEASE_S=600`.

## Broker

The master logs in as `devices` (README §4). The password stays on the laptop:

```bash
cp firmware/master/sim/broker.conf.example firmware/master/sim/broker.conf
```

then put the password into `broker.conf`, which is in `.gitignore`. Instead of the file, `IX_MQTT_HOST`, `IX_MQTT_PORT`, `IX_MQTT_USER` and `IX_MQTT_PASSWORD` work too. The connection is MQTT 3.1.1 over TLS on port 8883; the broker's certificate is checked against the system's CA certificates.

What the master does on the broker (topics and payloads in README §4):

| Task | Topic | |
|---|---|---|
| M1 | subscribes to `inhabitx/Z/#` | each `inhabitx/Z/<floor>/<room>/state` → `ix_slave_update()`. Unknown rooms are ignored. No message for 10 s → offline |
| M4 | publishes `inhabitx/Z/rooms`, `inhabitx/Z/lostfound` (retained, QoS 1) | when the content changes, checked twice a second; again after every reconnect. No names or emails |
| M6 | `inhabitx/Z/bookings/new`, `inhabitx/Z/lostfound/new` → `inhabitx/Z/replies/<req>` (not retained) | through `ix_book()` / `ix_found_add()`. `error` is one of the README §4 codes; found items can also get `room` or `cat`. A request delivered twice gets the same reply. Names and emails are never logged or published |

The terminal prints one line per change (`slave Z.1.09: occupied`, `web booking Z.1.10 14:00-15:00, 4 people: ok`), never names or emails. `s` also shows whether the broker is connected. If the connection drops, the simulator retries every 5 s and keeps running on what it knows.

Only one master per building: a second simulator with the same broker login and client id takes over the connection from the first.

### Why coreMQTT

[coreMQTT](https://github.com/FreeRTOS/coreMQTT) (MIT) is the client inside Infineon's ModusToolbox `mqtt` library, so the E84 port keeps the same calls and only swaps the transport. It is ~3k lines of C, uses no heap and no threads, and runs from the main loop here. The PC transport is OpenSSL (`mqtt_link.c`); on the board it's Infineon's secure sockets. Version 2.x because that's the MQTT 3.1.1 line the Infineon library is built on.

## Screen size

| Panel | Resolution | Build with |
|---|---|---|
| 10.1" TFT (WF101JTYAHMNB0), **chosen** | 1024×600 | default |
| Waveshare 7" DSI LCD (C) | 1024×600 | default |
| 4.3" panel that ships with the kit | 800×480 | `-DUI_HOR_RES=800 -DUI_VER_RES=480` (regenerate the campus map for that size first) |

All three connect to the E84 Evaluation Kit over MIPI-DSI and are supported by Infineon's LVGL code example.

## Fake slaves

Type commands in the terminal where the simulator runs (`h` for help):

With `--offline` or `--fake`:

| Command | Effect |
|---|---|
| `o 09` | toggle someone there / empty in Z.1.09 |
| `x 12` | toggle the Z.1.12 slave online / offline (offline after 10 s) |
| `g 10` | new booking on Z.1.10 starting now with nobody there (ghost demo) |
| `s` | print all room states |
| `t X Y` / `k text` | tap the screen / type into the focused field (for scripted tests) |

## Files

```text
firmware/master/
├── logic/            plain C, no LVGL, no MQTT library: rooms, bookings, ghost release, lost & found
│   ├── ix_model.h/.c
│   ├── ix_msg.h/.c   broker messages in and out (README §4), JSON by hand
│   ├── test_model.c  cc -I. test_model.c ix_model.c -o t && ./t
│   └── test_msg.c    cc -I. test_msg.c ix_msg.c ix_model.c -o t && ./t
├── ui/               LVGL 9 UI, portable
│   ├── ui.c          shell: sidebar, header, clock, toast, idle reset
│   ├── ui_overview.c hero, rooms table, floor plan, occupancy, room drawer
│   ├── ui_campus.c   real AAU campus map with live building
│   ├── ui_book.c     booking: any / specific room, time, name + email
│   ├── ui_lostfound.c
│   ├── ui_widgets.c  theme + helpers (colours and fonts live in ui_internal.h)
│   ├── campus_map.c  generated from OpenStreetMap (assets/gen_campus_map.py)
│   └── ui_fonts*.c   Roboto (Apache 2.0) for TinyTTF (assets/gen_fonts.py)
├── assets/           generators for the map and fonts
└── sim/              this simulator
    ├── main.c            window, clock, fake slaves, glue to the broker
    ├── mqtt_link.h/.c    coreMQTT over OpenSSL: connect, reconnect, publish
    └── broker.conf.example   copy to broker.conf (not committed) and add the password
```

## Porting to the board

- Call `ui_init()` after the display driver is up, and provide `time_t ix_now(void)` (SNTP or RTC).
- MQTT: add Infineon's `mqtt` library (coreMQTT inside), connect with the `devices` login and HiveMQ's root certificate, subscribe to `IX_TOPIC_SUB`, and pass every message to `ix_msg_handle()`; publish its reply if there is one. Publish `ix_msg_rooms()` / `ix_msg_lostfound()` retained when they change (see `publish_state()` in `sim/main.c`). `ix_tick()` already runs in the UI's 1 s timer.
- In the board's `lv_conf.h`: `LV_COLOR_DEPTH 16`, `LV_USE_TINY_TTF 1`, Montserrat 14/16/20/24/32 (for icons), and `LV_MEM_SIZE` of at least 2 MB (fonts are rasterised at runtime).
- Map data © OpenStreetMap contributors (ODbL). The credit is shown on the campus page and must stay there.
