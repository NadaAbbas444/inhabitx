# InhabitX node on the board (room Z.1.09)

The board reads its sensor and sends *someone there or not* to the cloud every 2 s. The sending is done; **you only add your sensor code in `board/node_sensor.c`.**

This branch only holds the node. The whole project (main, web page, message contract) is on [`main`](https://github.com/NadaAbbas444/inhabitx/tree/main).

## How to use

1. **ModusToolbox → Project Creator:** your PSOC™ Edge E84 kit + Infineon's example **"Wi-Fi MQTT Client"** (`mtb-example-psoc-edge-wifi-mqtt-client`).
2. **Copy** everything from [`board/`](board/) into the project's `proj_cm33_ns/` folder and overwrite Infineon's `publisher_task.c`, `mqtt_client_config.h` and `wifi_config.h`.
3. **Passwords:** copy `secrets.h.example` to `secrets.h` (same folder) and fill in the hotspot name and password, plus the broker password from Nada. `secrets.h` is never committed.
4. **Your code** in `node_sensor.c`:
   - `node_sensor_init()`: set up the sensor, called once
   - `node_is_occupied()`: your readout + logic, `return true` if someone is there; called every 2 s, must return quickly
5. **Build + program.** The serial terminal shows every 2 s:
   `node: inhabitx/Z/1/Z.1.09/state -> {"room":"Z.1.09","occupied":true,"ts":0}`
6. **Check** https://nadaabbas444.github.io/inhabitx/: Z.1.09 follows your sensor within 2 s (Nada's laptop must be running). Board off → *Offline* after 10 s.

Logic to aim for: `true` after ~3 s of presence, `false` after ~20 s without. Only true/false leaves the board, never raw data or audio. If your sensor code runs on the CM55 core, tell Nada.

Don't run `main_sim` or `fake_nodes.py` yourself; a second copy makes rooms flip.

## If it doesn't work

| Problem | Check |
|---|---|
| no Wi-Fi | hotspot name/password in `secrets.h` (campus Wi-Fi won't work) |
| TLS / handshake error | our `mqtt_client_config.h` really replaced Infineon's |
| connection refused | broker password in `secrets.h` |
| messages print, page doesn't change | Nada's laptop isn't running the main |
| Z.1.09 flips every 2 s | someone else also sends for Z.1.09 |

To watch the messages: https://www.hivemq.com/demos/websocket-client/, host `inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud`, port `8884`, SSL on, user `devices`, subscribe to `inhabitx/Z/#`.

## Sensor choice (N1)

_To be filled in: which sensor, why, and where it's mounted._
