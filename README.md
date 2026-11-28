# InhabitX ESP32-S3 occupancy node

Arduino firmware for the **Waveshare ESP32-S3-AUDIO-Board**: onboard ES7210 microphone capture, DEEPCRAFT inference, occupancy filtering, and InhabitX MQTT reporting.

The branch is named `feat-run-node-on-XIAO`, but this firmware currently targets the Waveshare audio board. A Seeed XIAO board needs its own microphone driver and pin configuration.

## Build locally without relying on the IDE menu

Installing a partition option does not change a sketch's saved selection. If the menu still selects stock **ESP SR 16M** or the error still reports **3145728 bytes**, the old 3 MB limit remains active. Either fully restart Arduino IDE and select **InhabitX Voice 16M**, or use this local build command from `firmware/ClassroomOccupancy`:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Build-VoiceFirmware.ps1
```

The script explicitly sets **upload.maximum_size=4194304**, uses the sketch's 4 MB partition table, and preserves stock ESP-SR model-upload settings. No board installation or menu selection is required for this command. `-CheckOnly` checks the resolved configuration without compiling. It compiles only and does not flash the board. IDE uploads still require selecting **InhabitX Voice 16M**.

Local verification: script syntax and `-CheckOnly` pass; the installed CLI resolves 4 MB maximum, 16 MB flash and model upload at 0xC10000. The existing 3,208,608-byte application binary fits. A fresh compile was attempted but the managed runner still cannot launch the ESP32 compiler (Illegal System DLL Relocation); no fresh-build success is claimed.

## Fixing the voice firmware size limit (Windows)

The combined voice and occupancy build exceeds the stock ESP SR 16M 3 MB application limit. This sketch includes a custom `partitions.csv` with **two 4 MB app slots**, 4 MB SPIFFS, and the original speech-model partition at **0xC10000**. The IDE size limit and speech-model upload address must match the layout.

1. Download this branch and close Arduino IDE.
2. Open PowerShell in `firmware/ClassroomOccupancy` and run:

   ```powershell
   powershell -NoProfile -ExecutionPolicy Bypass -File .\Install-VoicePartition.ps1
   ```

3. Restart Arduino IDE. With **ESP32S3 Dev Module** selected, choose **Tools → Partition Scheme → InhabitX Voice 16M (4MB APP/4MB SPIFFS/3.9MB MODEL)**. Keep **Flash Size = 16MB** and **PSRAM = OPI PSRAM**. Verify and upload again.

The script registers a separate menu option and preserves other board definitions. It installs into Arduino-ESP32 **3.3.12** by default; pass `-PlatformPath` for another installation. Rerun after updating the ESP32 core. Keep the sketch's `partitions.csv` and `build_opt.h` alongside the .ino. The latter preserves -O3 and marks this stock-compatible speech partition for ESP_SR's compile-time check.

The custom table was generated and validated with Espressif's partition tool for 16 MB flash. The reported 3,208,467-byte application fits the 4,194,304-byte slots. This is layout validation, not a new target build or hardware test.

## Setup

1. Open [ClassroomOccupancy.ino](firmware/ClassroomOccupancy/ClassroomOccupancy.ino) in Arduino IDE.
2. Install **esp32 by Espressif Systems 3.3.12**. Select **ESP32S3 Dev Module**, 16 MB flash, OPI PSRAM, USB CDC enabled, and **InhabitX Voice 16M (4MB APP/4MB SPIFFS/3.9MB MODEL)** partition.
3. Copy [secrets.h.example](firmware/ClassroomOccupancy/secrets.h.example) to `secrets.h` beside the sketch. Fill in your 2.4 GHz Wi-Fi credentials and HiveMQ `devices` password locally.
4. Set the room in [InhabitXConfig.h](firmware/ClassroomOccupancy/InhabitXConfig.h); default: **Z.1.09**. Stop any other node publishing for that room.
5. Verify and upload. Keep the main application running for the [room dashboard](https://nadaabbas444.github.io/inhabitx/) to update.

All required MQTT/model/codec sources are included. See [full setup and hardware notes](firmware/ClassroomOccupancy/README.txt).

## Reporting

- Publishes `{"room":"Z.1.09","occupied":true,"ts":0}` every 2 seconds to `inhabitx/Z/1/Z.1.09/state` over verified TLS; QoS 0, not retained.
- Unknown, stale, or invalidated predictions pause reporting; the main marks the room offline after 10 seconds without messages.
- Audio and inference scores stay on the device. Wi-Fi/TLS/MQTT run separately from capture and inference.
- Serial shows state changes and one short status line every 10 seconds. Detailed diagnostics can be enabled in [SerialConfig.h](firmware/ClassroomOccupancy/SerialConfig.h).
- Time sync starts after Wi-Fi connects and retries every 60 seconds. If sync fails, check internet access and outbound NTP (UDP 123), or configure a reachable local time server.

The existing occupancy filter confirms activity across 3 predictions and holds occupancy for at least 60 seconds after the last confident activity. This is an estimate from audible human activity; quiet people may be missed.

## Validation

Host tests cover JSON encoding and stale/invalid snapshot handling; see [test source](firmware/tests/inhabitx_contract_test.cpp). Generated model hashes remain unchanged. The local ESP32 target build was blocked before compiler launch by Windows' Illegal System DLL Relocation error. User serial output showed inference running and waiting for NTP; the latest time-sync changes and live MQTT delivery remain unverified on hardware.

The original Infineon PSOC Edge integration remains in [board/](board/) for reference; its sensor function is a placeholder. The ESP32 firmware is in `firmware/ClassroomOccupancy/`.

## Offline voice control (experimental)

Say **Hey Infineon**, pause for `Voice: listening`, then within 8 seconds say:

| Command | Effect |
|---|---|
| I am in the room | Publishes occupied with `source: voice` and `Room occupied confirmed by user`; holds occupied for 2 minutes. |
| I am leaving the room | Publishes empty with a user-declaration message; overrides automatic detection for **10 minutes**. |

Say the activation phrase again before each command. Arrival cancels an active leaving override immediately. Automatic detection resumes after the override expires. Audio remains on the board.

Select **InhabitX Voice 16M** and **OPI PSRAM** before uploading; the bundled Arduino-ESP32 speech models are flashed automatically. A missing model disables voice with a serial notice. `VoicePolicy.h` controls the durations.

This implementation uses continuous English MultiNet command recognition to gate room commands behind the custom activation phrase. It is **not a custom-trained WakeNet model**. Recognition accuracy for “Infineon”, false triggers, and running speech alongside the occupancy model need hardware validation. A speech audio gap disables voice until reboot while automatic detection continues.

The main receives the normal occupancy boolean plus confirmation text; its current UI ignores the extra text fields. Host voice-policy and message tests pass. The target build remains blocked by the local Windows compiler-launch issue; voice recognition has not been tested on a board. See the [voice instructions and test checklist](firmware/ClassroomOccupancy/README.txt).
