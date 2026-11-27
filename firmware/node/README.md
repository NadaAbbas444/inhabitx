# Node firmware (E84 + presence sensor, one per room)

The node senses its room, decides **someone there or not** on the board, and sends only that to the cloud every 2 s. Everything else (bookings, ghost release, the web page) happens on the main.

## What to send

| | |
|---|---|
| Broker | `inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud`, port **8883**, MQTT 3.1.1 over **TLS** |
| Login | user `devices`, password from Nada (never commit it, see below) |
| Client id | `inhabitx-node-Z.1.09`: one per board, unique. Never `inhabitx-main-Z`, or the main is kicked off |
| Topic | `inhabitx/Z/1/Z.1.09/state` (`inhabitx/<building>/<floor>/<room>/state`) |
| Payload | `{"room":"Z.1.09","occupied":true,"ts":1764237600}` |
| How often | every **2 s**, also when nothing changed. QoS 0, **not retained** |

- **Z.1.09** is the live board's room; the fake nodes leave it out.
- `occupied` must be JSON `true` / `false`, not a string. `room` must match the room in the topic or the message is ignored.
- `ts` (Unix seconds) is for debugging only. The main uses its own clock, so if the board has no time yet, send `0`.
- Spaces in the JSON are fine. Anything else in it is ignored.
- If the main hears nothing from a room for **10 s**, the room shows **offline**. That's also how a dead node shows up, so keep sending.

## Deciding "occupied"

Raw sensor data never leaves the board.

```text
occupied = true   after ~3 s of presence
occupied = false  after ~2 min without presence   (demo: 20 s)
```

The long hold keeps a person who sits still (or is quiet) from "disappearing". With a microphone, the board computes a loudness or voice-activity level on the device and sends only `occupied`. **Never stream or store audio**: that's the privacy promise of the project ("no cameras, no recordings").

## TLS certificate

HiveMQ Cloud only accepts TLS. The board needs the root certificate the broker's certificate chains up to: **ISRG Root X1** (Let's Encrypt), valid until 2035. It's in this folder as `isrg-root-x1.pem`; paste it into the example's root-CA setting as a C string. Also set the broker host name for SNI if the example asks for it; HiveMQ Cloud needs it.

## Where to start

ModusToolbox™: create a project from Infineon's **Wi-Fi MQTT client** code example for the PSOC™ Edge E84, and add the sensor part from the matching sensor code example. In the MQTT example's config files, set:

| Setting | Value |
|---|---|
| Wi-Fi SSID / password | the phone hotspot (campus Wi-Fi needs a login page the board can't do) |
| broker address / port | the host above, `8883`, secure connection on |
| username / password | `devices` / from Nada |
| root CA | contents of `isrg-root-x1.pem` |
| client id | `inhabitx-node-Z.1.09` |
| publish topic / message | as above, every 2 s |

Keep the Wi-Fi and broker passwords in a header called `secrets.h` (in `.gitignore`) and include it from the config file, so they never end up in git.

## How to test, step by step

1. **Without the board** (works today): open the HiveMQ WebSocket client (https://www.hivemq.com/demos/websocket-client/), host above, port `8884`, SSL on, login `devices`. Publish the payload above to `inhabitx/Z/1/Z.1.09/state`. On the live page (https://nadaabbas444.github.io/inhabitx/) Z.1.09 changes within 2 s, as long as the main is running on Nada's laptop. Stop publishing: offline after 10 s.
2. **Board connects:** in the same WebSocket client, subscribe to `inhabitx/Z/#`. The board's messages appear every 2 s.
3. **Board drives the room:** with the main running, Z.1.09 on the page and the main's screen follows the board.
4. **Presence rules (N3):** empty room → `false`; one person sitting still for 1 min → `true` the whole time; one person moving → `true`; someone walking past at a distance → `false`.

If something doesn't work:

| Symptom | Likely cause |
|---|---|
| TLS handshake fails | root CA missing or wrong, SNI host name not set, or the board's clock is far off (some TLS stacks check dates) |
| connect refused | wrong user or password |
| connected, but nothing arrives | typo in the topic; `inhabitx/Z/1/Z.1.09/state` is case-sensitive |
| room flips between two states every 2 s | a second sender for the same room, e.g. `fake_nodes.py --room Z.1.09` still running |
| the main keeps losing its connection | the board uses the main's client id |

## Sensor choice (task N1)

_To be filled in._ The question that matters most: does it notice a **person sitting still**? Options: the sensors on the E84 kit, a microphone (only notices sound, so it needs the long hold time), PIR as the baseline. Note range, field of view and where it would be mounted.
