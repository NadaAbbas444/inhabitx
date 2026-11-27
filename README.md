# InhabitX node on the board (room Z.1.09)

The board reads its sensor and sends *someone there or not* to the cloud every 2 s. The sending is done; **you only add your sensor code in `board/node_sensor.c`.**

This branch only holds the node. The whole project (main, web page, message contract) is on [`main`](https://github.com/NadaAbbas444/inhabitx/tree/main).

```bash
git clone -b feat-run-node-on-board https://github.com/NadaAbbas444/inhabitx.git inhabitx-node
```

## Before you start: Wi-Fi

The board reaches the cloud over Wi-Fi, so it needs a network it can join on its own. Campus Wi-Fi (eduroam) won't work, because it needs a university login.

- **Board next to you:** use your phone's hotspot. The hotspot name is the network name shown in the Wi-Fi list, and the password is the one you set on the phone.
  - iPhone: Settings → Personal Hotspot, turn on **Maximize Compatibility**.
  - Android: Mobile hotspot, security **WPA2** (not WPA3-only), band **2.4 GHz** if offered.
- **Remote board in a live lab:** a phone hotspot can't reach it. Ask the lab staff for a Wi-Fi network the board can use (name + password) and whether it allows outgoing connections on port **8883**.

## How to use

1. **ModusToolbox → Project Creator:** your PSOC™ Edge E84 kit + Infineon's example **"Wi-Fi MQTT Client"** (`mtb-example-psoc-edge-wifi-mqtt-client`).
2. **Copy** everything from [`board/`](board/) into the project's `proj_cm33_ns/` folder. It replaces Infineon's `publisher_task.c`, `mqtt_client_config.h` and `wifi_config.h`; rename those three to `.orig` first if you want to be able to go back.
3. **Passwords:** copy `secrets.h.example` to `secrets.h` (same folder) and fill in:
   - `SECRET_WIFI_SSID` / `SECRET_WIFI_PASSWORD`: the Wi-Fi from above
   - `SECRET_MQTT_PASSWORD`: the broker password, Nada sends it to you directly

   `secrets.h` is never committed. Don't share the built `.hex`/`.elf` either: it contains the passwords.
4. **Your code** in `node_sensor.c`:
   - `node_sensor_init()`: set up the sensor, called once
   - `node_is_occupied()`: your readout + logic, `return true` if someone is there, called every 2 s

   `node_is_occupied()` must return within a few milliseconds. If reading the sensor takes longer, read it in your own task and let this function return the latest result.
5. **Build + program.** The serial terminal shows every 2 s:
   `node: inhabitx/Z/1/Z.1.09/state -> {"room":"Z.1.09","occupied":true,"ts":0}`
6. **Check** https://nadaabbas444.github.io/inhabitx/: Z.1.09 follows your sensor within 2 s (Nada's laptop must be running). Board off → *Offline* after 10 s.

Logic to aim for: `true` after ~3 s of presence, `false` after ~20 s without. Only true/false leaves the board, never raw data or audio. If your sensor code runs on the CM55 core, tell Nada.

## If it doesn't work

| Problem | Check |
|---|---|
| no Wi-Fi | `SECRET_WIFI_SSID` / `SECRET_WIFI_PASSWORD`; hotspot set to WPA2 / Maximize Compatibility |
| TLS / handshake error | our `mqtt_client_config.h` really replaced Infineon's |
| connection refused | `SECRET_MQTT_PASSWORD` |
| messages print, page doesn't change | Nada's laptop isn't running the main |
| Z.1.09 flips every 2 s | something else also sends for Z.1.09: only this board should. Tell Nada |

To watch the messages: https://www.hivemq.com/demos/websocket-client/, host `inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud`, port `8884`, SSL on, user `devices`, subscribe to `inhabitx/Z/#`.

## Sensor choice (N1)

_To be filled in: which sensor, why, and where it's mounted._
