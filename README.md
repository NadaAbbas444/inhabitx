# InhabitX main on the PSOC™ Edge E84

The main (bookings, ghost release, lost & found, answering the web page) running on the E84 instead of the laptop. Same behaviour as `main_sim`.

| Folder | What | Use it when |
|---|---|---|
| `no-screen/` | the main on the Wi-Fi core, no display | the board has internet but no screen (or you want the safe first step) |
| `with-screen/` | the main with its touch screen UI | the board has internet **and** a display |
| `common/` | the main's logic, used by both | always |

## Before you start

- **Only one main at a time.** Stop `main_sim` on the laptop while the board is the main: both log in as `inhabitx-main-Z` and would keep kicking each other off.
- **Keep the fake nodes running on the laptop** (`.venv/bin/python tools/fake_nodes.py --shuffle 3` on `main`). Besides the rooms, they give the board the time: it has no clock, and until the first node message with a real time arrives, bookings are refused with `time`.
- **Wi-Fi:** the board needs a network with internet. In a live lab, ask the staff for one and whether port 8883 is open.

## no-screen

1. **ModusToolbox → Project Creator:** E84 kit + Infineon's example **"Wi-Fi MQTT Client"** (`mtb-example-psoc-edge-wifi-mqtt-client`).
2. **Copy** into the project's `proj_cm33_ns/`:
   - everything from `no-screen/`: replaces Infineon's `mqtt_client_config.h`, `mqtt_client_config.c`, `subscriber_task.c`, `publisher_task.c` and `wifi_config.h`
   - everything from `common/` except `test_main_app.c`
3. **Passwords:** copy `secrets.h.example` to `secrets.h` and fill in Wi-Fi name, Wi-Fi password and the broker password. Never commit `secrets.h`, never share the built `.hex`/`.elf`.
4. **Build + program.** The serial terminal shows:
   ```text
   main: listening on inhabitx/Z/#
   main: running (client id inhabitx-main-Z), waiting for the time from a node message
   main: clock set
   main: node Z.1.08: occupied
   ```
5. **Check** on https://nadaabbas444.github.io/inhabitx/: the rooms follow the fake nodes, booking works, a 3rd booking with the same email → "limit". Unplug the board → "Building offline" on the page within about 1.5 minutes (the broker waits 1.5 × the 60 s keep-alive).

## with-screen

_See the with-screen section below once it's added._

## If it doesn't work

| Problem | Check |
|---|---|
| no Wi-Fi | Wi-Fi name/password in `secrets.h`, WPA2 network |
| TLS / handshake error | our `mqtt_client_config.h` and `.c` really replaced Infineon's |
| connection refused | broker password in `secrets.h` |
| connected, but the laptop main keeps reconnecting | `main_sim` is still running: stop it |
| bookings say "Pick a time today…" | no node message with a real time yet: start the fake nodes |
| rooms all offline | no node messages: start the fake nodes |

The logic is checked on the PC: `cd common && cc -I. test_main_app.c main_app.c ix_model.c ix_msg.c -o t && ./t`.
