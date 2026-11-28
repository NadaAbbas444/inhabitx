# InhabitX classroom occupancy on PSoC Edge E84

This project replaces Infineon's baby-cry model with the same DEEPCRAFT classroom model used in the Waveshare ESP32 firmware. The generated model files are unchanged. Audio acquisition and application policy are native C for ModusToolbox.

| Setting | Value |
| --- | --- |
| Board | KIT_PSE84_EVAL_EPC2, application BSP APP_KIT_PSE84_EVAL_EPC2 |
| Model | conv2d-small-balanced-0, ID e48a1b9d-548a-41c8-860a-86ac08e95c34 |
| Input | 16 kHz, mono, signed 16-bit PCM normalized by 32768.0 |
| Output order | unlabeled, background, human_activity |
| Execution | CM55, floating-point generated C, CPU inference |
| Configuration | Debug, GCC_ARM 14.2.1; C code optimized with -O2 |
| Audio | BSP PDM channel 3, signed 16-bit, existing 16 kHz clock/decimation configuration |
| Serial | KitProg3 UART, 115200 baud, 8N1 |

## Behavior

- Starts as `UNKNOWN` while the model and occupancy filter warm up.
- Requires three consecutive human-activity predictions at or above 0.80 before reporting `OCCUPIED`.
- Requires background probability at or above 0.75 for 20 seconds at startup before reporting `UNOCCUPIED_INFERRED`.
- After activity, retains occupancy for at least 60 seconds and requires 10 seconds of continuous background before inferring unoccupied.
- Ambiguous predictions cannot establish an empty room. A silent person can still be present: this is an acoustic inference, not a people counter.
- Queue drops, PDM FIFO errors, stalled or stuck input, clipping and model errors reset the state to `UNKNOWN` and restart model/filter history.

`shared/audio_queue.h` provides a bounded ISR-to-foreground queue. Capture never overwrites unread samples. All model calls occur in the foreground. Discontinuous or stale blocks are discarded; the model and filter restart together. SysTick supplies a local 1 ms health/timeout clock. Do not install another SysTick handler without moving this clock into the application's scheduler.

The model uses about 86.8 KB for generated state/buffers; the PCM queue adds about 16.6 KB. Neither figure includes the BSP, stacks, libraries or other application data. Actual target memory use and inference timing must be checked in the target build and on hardware.

## Output and integration boundary

The UART prints room state, human/background scores, microphone level, dropped-block count and FIFO-error count once per second. For valid known states it also emits the existing InhabitX node message format:

```json
{"room":"Z.1.09","occupied":true,"ts":0}
```

`UNKNOWN` suppresses these occupancy JSON messages. A UART gateway must separately detect link loss. Set `INHABITX_ROOM` at compile time to change the room name, using a string safe for JSON.

This audio application does not initialize Wi-Fi, TLS or MQTT. The repository's existing `board/` networking scaffold is separate; it has not been linked into this bare-metal example. Voice activation/commands are also not part of this E84 model port. UART reporting can be consumed by a host or gateway; it does not automatically reach the broker.

## Build

Install ModusToolbox 3.9, Arm GNU Toolchain 14.2.1, and Edge Protect Security Suite through ModusToolbox Setup. Use a checkout path without spaces. In modus-shell:

```bash
cd mtw/Inhabix
make getlibs
make -j8 build
```

No board is needed to build. The application combines CM33 secure, CM33 non-secure and CM55 images using `configs/boot_with_extended_boot.json`. Its output is `build/app_combined.hex`. The normal build regenerates libraries omitted from the repository from the included dependency declarations and version locks.

The model also exists in `proj_cm33_ns` so the two core directories no longer contain different classifiers. CM55 is the selected and checked target. CM33 execution is an optional configuration requiring its own target validation; retain the default `ML_DEEPCRAFT_CPU=cm55` for this port.

## Program and check on the board

Follow the kit setup in `Inhabix/README.upstream.md` (BOOT SW HIGH/ON and J20/J21 NC for the E84 EVAL), connect the KitProg3 USB port, and run `make program` from the application directory after a successful build. Open its UART at 115200 baud.

Check that the startup banner identifies `InhabitX classroom occupancy`, 16000 Hz mono and model `e48a1b9d`. Then check human-activity detection, the 60-second hold, startup/background timing, microphone level and error counters. The PDM gain starts at 5 dB with no software boost; calibrate it using classroom recordings. The ESP32 ES7210's 24 dB setting does not transfer directly to the E84 microphone.

## Tests and artifacts

`tests/` contains executable tests for occupancy boundaries, queue ownership/rollover, generated-model streaming/reset determinism, and the real pipeline with simulated PDM/clock/model interfaces. These simulations exercise fault handling; they do not measure the real microphone, ISR timing or model accuracy.

`verification.json` records what was checked and whether an updated target build exists. Historical baby-cry demonstration binaries are kept under `artifacts/baseline-audio-demo/` and are not classroom firmware. Do not flash those to test the occupancy port.

Upstream documentation and licenses are retained in the application. The generated model retains its DEEPCRAFT notices and terms.
