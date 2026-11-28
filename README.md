# InhabitX main on the PSOC™ Edge E84

The main (bookings, ghost release, lost & found, answering the web page) running on the E84 instead of the laptop. Same behaviour as `main_sim`, with its touch screen UI on the board's 4.3-inch display.

## In the Infineon Live Lab: what to run

Each slot can be a fresh lab PC, so plan about 20 minutes (downloads + first build).

1. **Before:** make sure no `main_sim` is running anywhere (only one main at a time). Open the lab in **one browser tab only**: a second login ends the session.
2. **Plug in**, open the Development Environment (the lab PC), start **Visual Studio Code**, then **Terminal → New Terminal**.
3. **Run** (type it in three short lines, long lines get garbled over the remote link):
   ```powershell
   cd ~\Documents
   iwr https://raw.githubusercontent.com/NadaAbbas444/inhabitx/main-board-infineon/lab.ps1 -OutFile lab.ps1
   powershell -ExecutionPolicy Bypass -File lab.ps1
   ```
4. **Type the broker password** when it asks (`broker password (HiveMQ login "devices")`). The lab Wi-Fi (`PSOC-E84-AP`) is read from the lab's own `wifi_connect.bat`.
5. **Wait.** It downloads our code and Infineon's two examples, builds and programs the board. If it stops with an error, run the last line again: finished steps are skipped.
6. **When it says "press RESET"**: Input Panel (left) → Board Controls → **RESET**. It then reads the board for up to 2½ minutes and ends with:
   ```text
     OK      wifi
     OK      broker
     OK      clock
   ```
   A `MISSING` line comes with a hint. To look again later without building: `powershell -ExecutionPolicy Bypass -File lab.ps1 -Check`.
7. **Check:** the UI shows on the board's screen (**EVK Display Livestream** in the lab page). Book a room on https://nadaabbas444.github.io/inhabitx/: it appears on the board's screen and in the serial output.
8. **Before the slot ends**, remove everything that holds the password (`secrets.h` and the built firmware):
   ```powershell
   cd ~\Documents; Remove-Item ixmain, ixscreen, ix, lab.ps1 -Recurse -Force
   ```

Without screen (first test, or if the screen version fails): `powershell -ExecutionPolicy Bypass -File lab.ps1 -NoScreen`.

## What's in this branch

| Folder / file | What |
|---|---|
| `lab.ps1` | the one command above |
| `with-screen/` | the main with its touch screen UI (4.3-inch 832×480, or 7-inch / 10.1-inch 1024×600); `setup_lab.ps1` puts it into Infineon's examples |
| `no-screen/` | the main on the Wi-Fi core, no display |
| `common/` | the main's logic, used by both |

## Before you start (any setup)

- **Only one main at a time.** Stop `main_sim` on the laptop while the board is the main: both log in as `inhabitx-main-Z` and would keep kicking each other off.
- **The time** comes from the internet (SNTP, UDP port 123) once the board is on Wi-Fi; until then bookings are refused with `clock` ("The building's clock isn't set yet" on the page). If the network blocks NTP, a node message with a real `"ts"` sets it too (e.g. the fake nodes: `.venv/bin/python tools/fake_nodes.py --room Z.1.12` on `main`).
- **Wi-Fi:** the board needs a network with internet and port 8883 open (the Live Lab's `PSOC-E84-AP` has both).

The steps below are what `lab.ps1` does, for doing it by hand (e.g. on your own board).

## no-screen

1. **ModusToolbox → Project Creator:** E84 kit + Infineon's example **"Wi-Fi MQTT Client"** (`mtb-example-psoc-edge-wifi-mqtt-client`).
2. **Copy** into the project's `proj_cm33_ns/`:
   - everything from `no-screen/`: replaces Infineon's `mqtt_client_config.h`, `mqtt_client_config.c`, `subscriber_task.c`, `publisher_task.c` and `wifi_config.h`
   - everything from `common/` except `test_main_app.c`
3. **Passwords:** copy `secrets.h.example` to `secrets.h` and fill in Wi-Fi name, Wi-Fi password and the broker password. Never commit `secrets.h`, never share the built `.hex`/`.elf`.
4. **Build + program.** The serial terminal shows:
   ```text
   main: listening on inhabitx/Z/#
   main: running (client id inhabitx-main-Z)
   sntp: time from pool.ntp.org
   main: clock set
   main: node Z.1.08: occupied
   ```
5. **Check** on https://nadaabbas444.github.io/inhabitx/: the rooms follow the nodes, booking works, a 3rd booking with the same email → "limit". Unplug the board → "Building offline" on the page within about 1.5 minutes (the broker waits 1.5 × the 60 s keep-alive).

## with-screen

The E84 has two cores. Here the **CM55** runs the screen, the UI and the main; the **CM33** only does Wi-Fi + MQTT and passes messages through (IPC).

1. **ModusToolbox → Project Creator:** E84 kit + Infineon's example **"Graphics LVGL demo"** (`mtb-example-psoc-edge-gfx-lvgl-demo`). Pick the display in its `common.mk` (`CONFIG_DISPLAY`, see Infineon's README: 4.3-inch is the default) and check that the music demo runs.
2. **Wi-Fi core:** create Infineon's **"Wi-Fi MQTT Client"** example as well, and copy its whole `proj_cm33_ns/` folder over the display project's `proj_cm33_ns/`. Then copy into that `proj_cm33_ns/`:
   - everything from `with-screen/cm33/` and `with-screen/shared/`
   - `common/main_rooms.h`
   - `secrets.h.example` → `secrets.h`, fill in Wi-Fi name, Wi-Fi password, broker password
3. In `proj_cm33_ns/main.c` (Infineon's), add one include and one line before `Cy_SysEnableCM55(...)`:
   ```c
   #include "ix_ipc.h"
   ...
   ix_ipc_cm33_init();
   Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_US);
   ```
4. **Screen core:** copy into the display project's `proj_cm55/source/`:
   - everything from `with-screen/cm55/`, `with-screen/shared/` and `with-screen/ui/`
   - everything from `common/` except `test_main_app.c`

   Delete Infineon's demo files there: `lv_demo_music*.c` and `img_*.c`.
5. In `proj_cm55/source/main.c` (Infineon's), three edits:
   ```c
   #include "inhabitx_screen.h"                       // with the other includes
   inhabitx_screen_start();                          // instead of lv_demo_music();
   time_till_next = inhabitx_screen_loop();          // instead of time_till_next = lv_timer_handler();
   ```
6. In `proj_cm55/include/lv_conf.h`:
   ```c
   #define LV_USE_TINY_TTF 1                          // was 0: our fonts
   #define LV_FONT_MONTSERRAT_32 1                    // was 0: icons
   #define LV_FONT_DEFAULT &lv_font_montserrat_16     // was _14
   ```
7. In `proj_cm55/Makefile`, add the screen size (it has to match the display, the build stops if it doesn't):
   ```make
   DEFINES+=UI_HOR_RES=832 UI_VER_RES=480       # 4.3-inch (default)
   # DEFINES+=UI_HOR_RES=1024 UI_VER_RES=600    # 7-inch or 10.1-inch
   ```
8. **Build + program.** Serial terminal: `main: screen 832x480 up, waiting for the Wi-Fi core`, then `relay: connected, passing messages to the screen core`, `sntp: time from …`, `main: broker connected`, `main: clock set`. The screen shows the same as `main_sim`; booking works on the screen and on the web page.

If text looks wrong or the screen stays black after step 6, switch the GPU drawing off in `lv_conf.h` (`#define LV_USE_DRAW_VG_LITE 0`): slower, but it rules out the graphics engine.

## Leaving the live lab

The lab's PC and board are used by the next person after you. Your code is public anyway; the **passwords** are what matter (`secrets.h`, and the built `.hex`/`.elf` contain them too).

1. Copy anything you changed there to your laptop first.
2. Delete the whole project folder on the lab PC (including `secrets.h` and `build/`), and empty the trash.
3. Program the board with an unchanged Infineon example, so your firmware (with the passwords) isn't left on it.
4. Don't sign in to anything on the lab PC; if you did, sign out.
5. If in doubt, change the `devices` password in HiveMQ afterwards.

## If it doesn't work

| Problem | Check |
|---|---|
| no Wi-Fi | Wi-Fi name/password in `secrets.h`, WPA2 network |
| TLS / handshake error | our `mqtt_client_config.h` and `.c` really replaced Infineon's |
| connection refused | broker password in `secrets.h` |
| connected, but the laptop main keeps reconnecting | `main_sim` is still running: stop it |
| bookings say "The building's clock isn't set yet" | no `main: clock set` yet: look for `sntp: no answer` (NTP blocked), then give the time with a node message (fake nodes) |
| rooms all offline | no node messages: start the fake nodes |
| with-screen: `relay: screen core not answering` | `ix_ipc_cm33_init();` missing in `proj_cm33_ns/main.c`, or the screen core crashed (check its serial output) |
| with-screen: build error about `UI_HOR_RES` | the Makefile size doesn't match the display (4.3-inch = 832 480) |

The logic is checked on the PC: `cd common && cc -I. test_main_app.c main_app.c ix_model.c ix_msg.c -o t && ./t`.
