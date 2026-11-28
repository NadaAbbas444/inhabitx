# InhabitX node on the board (room Z.1.09)

The E84 listens to the room with its microphone and sends *someone there or not* to the cloud every 2 s. The main (another device, e.g. `main_sim` on the laptop) and the web page show it as room Z.1.09.

```text
  E84 board (this branch)
  ┌───────────────────────────────────────────────┐
  │ CM55: microphone → classroom model → filter   │  mtw/Inhabix (proj_cm55)
  │        │ room state, once a second (mailbox)  │  mtw/Inhabix/shared/node_link.h
  │ CM33: Wi-Fi + MQTT → sends it every 2 s       │  Infineon's Wi-Fi core + board/
  └───────────────────────────────────────────────┘
                 │  inhabitx/Z/1/Z.1.09/state  {"room":"Z.1.09","occupied":true,"ts":0}
                 ▼
     HiveMQ  →  main (laptop)  →  https://nadaabbas444.github.io/inhabitx/
```

While the microphone has no reliable answer (the first ~20 s, after an audio gap or fault: `UNKNOWN`), the node sends **nothing**, so the main shows the room *Offline* instead of wrongly empty.

This branch only holds the node. The whole project (main, web page, message contract) is on [`main`](https://github.com/NadaAbbas444/inhabitx/tree/main).

## In the Infineon Live Lab: what to run

Each slot can be a fresh lab PC: plan about 25 minutes (downloads + first build).

**On the laptop, before the slot** (on `main`):
- start the main: `./firmware/main/sim/build/main_sim`. Z.1.09 shows **offline**.
- nothing else may send for Z.1.09: no fake node for it, and nobody else running this node.

**On the lab PC:**
1. Open the lab in **one browser tab only** (a second login ends the session), **Plug in**, open the Development Environment, start **Visual Studio Code**, then **Terminal → New Terminal**.
2. **Run**, as three short lines (long lines get garbled over the remote link):
   ```powershell
   cd ~\Documents
   iwr https://raw.githubusercontent.com/NadaAbbas444/inhabitx/feat-run-node-on-board/lab_node.ps1 -OutFile lab_node.ps1
   powershell -ExecutionPolicy Bypass -File lab_node.ps1
   ```
3. **Type the broker password** when it asks (hidden). The lab Wi-Fi (`PSOC-E84-AP`) is read from the lab's own `wifi_connect.bat`.
4. **Wait.** It downloads this branch and Infineon's Wi-Fi example, puts the node together (`ixnode`), downloads the libraries, builds and programs the board. If it stops with an error, run the last line again: finished steps are skipped.
5. **When it says "press RESET"**: Input Panel (left) → Board Controls → **RESET**. It reads the board for up to 3 minutes and ends with:
   ```text
     OK      wifi
     OK      broker
     OK      audio
     OK      room_known
     OK      sending
   ```
   A `MISSING` line comes with a hint. Look again later without building: add `-Check` to the last line.

## How to test it

| Do | Laptop main / web page |
|---|---|
| board starts, room still `UNKNOWN` (~20 s) | Z.1.09 stays **offline** |
| ~20 s quiet | Z.1.09 **free** (`UNOCCUPIED_INFERRED`): the node is online |
| Microphone panel (lab page, left): unmute, talk for ~10 s | Z.1.09 **occupied** within a few seconds |
| stop talking | stays occupied at least 60 s, then **free** after 10 s of quiet (~60–70 s) |
| RESET or the slot ends | **offline** after ~10 s |

The serial output shows both cores: `Room: OCCUPIED | activity 92% | …` once a second from the microphone side, `node: audio says OCCUPIED` and `node: inhabitx/Z/1/Z.1.09/state -> {…}` from the Wi-Fi side.

**If the microphone build fails:** `powershell -ExecutionPolicy Bypass -File lab_node.ps1 -NoMic` builds Infineon's plain Wi-Fi example with the node files instead: always *empty*, but it proves Wi-Fi, broker and *node online* on the laptop.

## Before the slot ends

Remove everything that holds the password (`secrets.h` and the built firmware):

```powershell
cd ~\Documents; Remove-Item ixnode, ixn, ifx-wifi, lab_node.ps1 -Recurse -Force
```

Don't sign in to anything on the lab PC. If in doubt, tell Nada, who can change the broker password.

## Files

| Path | What |
|---|---|
| `lab_node.ps1` | the one command above |
| `mtw/Inhabix/` | the audio app (microphone, DEEPCRAFT classroom model, occupancy filter): see [its README](mtw/README.md). On its own it prints the room state on the serial port; with `lab_node.ps1` it becomes the node's audio core |
| `mtw/Inhabix/shared/node_link.h` | the mailbox: the audio core writes the room state, the Wi-Fi core reads it |
| `board/` | the node's Wi-Fi core files, on top of Infineon's `mtb-example-psoc-edge-wifi-mqtt-client` (`proj_cm33_ns`): what to send and where (`node_message.*`, `publisher_task.c`, `mqtt_client_config.h`, `wifi_config.h`), and `node_sensor.c`, which reads the mailbox |

By hand (own board): copy `mtw/Inhabix` to a new folder, replace its `proj_cm33_ns` with the Wi-Fi example's, copy `board/*` and `mtw/Inhabix/shared/node_link.h` into it, write `secrets.h` from `secrets.h.example`, then `make getlibs` and `make program`.

## If it doesn't work

| Problem | Check |
|---|---|
| `MISSING wifi` | Wi-Fi name/password in `ixnode\proj_cm33_ns\secrets.h` (capital letters count) |
| `MISSING broker` | broker password: delete `secrets.h` and run again to retype it |
| `MISSING audio` | the microphone core didn't start: send the serial lines; meanwhile `-NoMic` tests the rest |
| `node: no news from the audio core` | same: the audio core isn't writing the mailbox |
| stays `UNKNOWN` | normal in the first ~20 s of quiet; talk into the Microphone panel |
| sending, but the page doesn't change | the laptop main isn't running |
| Z.1.09 flips every 2 s | something else also sends for Z.1.09: only this board should |

To watch the messages: https://www.hivemq.com/demos/websocket-client/, host `inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud`, port `8884`, SSL on, user `devices`, subscribe to `inhabitx/Z/#`.

Not tested on the board yet: the combined build (audio core + Infineon's Wi-Fi core), the mailbox between the two cores, and both cores sharing the serial port. Checked on a PC: the audio app's three tests, the mailbox with the audio pipeline test, and every case of `node_sensor.c`.

## Sensor choice (N1)

Microphone (the E84 kit's PDM microphone, 16 kHz mono) with a DEEPCRAFT classroom model (`conv2d-small-balanced-0`) and an occupancy filter: occupied after three confident human-activity results, held for at least 60 s, empty only after sustained background. It's an acoustic inference: a silent person can still be in the room. Only true/false leaves the board, never audio.
