# Master screen UI: PC simulator

Runs the master's LVGL UI on a Linux laptop in a window the size of the board's screen, with the **same** `ui/` and `logic/` code that goes onto the E84. Only `main.c` (window, clock, fake slaves) is PC-specific.

## Build and run

Needs `cmake`, `gcc` and `libx11-dev`. LVGL 9.2.2 is downloaded by CMake on the first build.

```bash
cmake -S firmware/master/sim -B firmware/master/sim/build
cmake --build firmware/master/sim/build -j
./firmware/master/sim/build/master_sim --at 10:05
```

`--at HH:MM` pretends it's that time today. Bookings only work between 07:00 and 22:00.

## Screen size

| Panel | Resolution | Build with |
|---|---|---|
| 10.1" TFT (WF101JTYAHMNB0), **chosen** | 1024×600 | default |
| Waveshare 7" DSI LCD (C) | 1024×600 | default |
| 4.3" panel that ships with the kit | 800×480 | `-DUI_HOR_RES=800 -DUI_VER_RES=480` (regenerate the campus map for that size first) |

All three connect to the E84 Evaluation Kit over MIPI-DSI and are supported by Infineon's LVGL code example.

## Fake slaves

Type commands in the terminal where the simulator runs (`h` for help):

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
├── logic/            plain C, no LVGL: rooms, bookings, ghost release, lost & found
│   ├── ix_model.h/.c
│   └── test_model.c  cc -I. test_model.c ix_model.c -o t && ./t
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
```

## Porting to the board

- Call `ui_init()` after the display driver is up, and provide `time_t ix_now(void)` (SNTP or RTC).
- Feed `ix_slave_update()` from the MQTT callback (M1). `ix_tick()` already runs in the UI's 1 s timer.
- In the board's `lv_conf.h`: `LV_COLOR_DEPTH 16`, `LV_USE_TINY_TTF 1`, Montserrat 14/16/20/24/32 (for icons), and `LV_MEM_SIZE` of at least 2 MB (fonts are rasterised at runtime).
- Map data © OpenStreetMap contributors (ODbL). The credit is shown on the campus page and must stay there.
