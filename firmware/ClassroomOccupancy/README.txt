FOLLOW-UP COMMAND DECODER ADAPTER
The installed Arduino ESP_SR adapter changes its mode flag after detection,
but does not explicitly clean the previous MultiNet recognition state. It
also creates a fixed 5760ms decoder cycle, shorter than the application
8-second command window. InhabitX now uses src/voice_engine/VoiceEngine.c,
a local adapter derived from Arduino-ESP32 3.3.12 with SPDX notices retained.
Installed Arduino libraries are not modified. It copies recognition results
and cleans the decoder in the detection task BEFORE notifying the application
for every match, including Hey Infineon. Timeouts also clean decoder state.
The decoder cycle is 10s; the application window remains 8s. Shared timing
constants prevent configuring a decoder cycle shorter than that window.

After rebuilding/uploading, test:
  Hey Infineon -> wait for Voice: listening -> room busy
  Hey Infineon -> wait for Voice: listening -> room empty
The original I am in the room / I am leaving the room phrases are still
registered with the same actions. Short phrases are additional alternatives.
Arrival and departure hold times remain 2 minutes / 10 minutes respectively.
Accepted room commands print their match confidence. A recognized command
outside the application window prints an explicit notice; lack of a match
still closes the window with a suggestion to try the short phrases.
The adapter validates every phrase registration and reports failures even when Core Debug Level is None. It gates startup tasks and feed-buffer allocation so a
startup allocation failure does not run a partially created pipeline.

Policy tests (including late follow-up, timeout cancellation and fresh wake)
pass on the host. The adapter's target build was attempted but the managed
runner still cannot launch the ESP32 compiler. Recognition accuracy and
this decoder change require a board retest; a known root cause on the
physical device has not been confirmed solely from the serial log.

VOICE STREAM RECOVERY AND RESET DIAGNOSTICS
The occupancy model and speech recognizer now track separate continuity.
An occupancy queue drop resets only the occupancy model. It no longer
invalidates speech PCM that was received intact. Physical I2S/DMA/read
faults change captureEpoch; actual speech block loss is detected by sequence.

Capture queue: 96 blocks (about 1.5s). Speech queue: 32 blocks (about 0.5s).
Both remain in PSRAM. A 100ms task scheduling delay alone no longer marks
speech faulty: the feeder waits and checks the next block's sequence/age.
Speech PCM older than 1s is still rejected rather than replaying old commands.

A real speech discontinuity cancels the current wake/command window and
quarantines commands. The feeder substitutes silence for at least 6s of
samples AND 6s of elapsed time, then waits for an actual MultiNet timeout.
Only then does recognition resume, with a brief settling interval and a
fresh Hey Infineon required. Expect recovery to take roughly 6-20s when
capture is flowing, depending on the decoder timeout. A stalled microphone
cannot complete recovery and cannot produce a command. Recovery generations
reject queued events from before the gap; existing manual occupancy TTLs
remain as declared. No permanent reboot-only voice latch remains.

Serial prints recovering/recovered once per transition. Reset: at boot
reports the ESP32 reset reason, so repeated startup can be investigated.
DMA overflow counters are zeroed while I2S is stopped after startup model
initialization; counts reported afterward represent runtime overflow.

Host tests cover contiguous/missing PCM, physical epoch changes, sequence
wrap, and recovery boundaries (silence samples + elapsed time + timeout).
Those tests pass. Recovery timing and concurrent processing still require
board testing; do not assume the unexplained restart in the prior log is fixed.

RUNTIME RAM FIX FOR INFERENCE-TASK ESP_ERR_NO_MEM
The speech models loaded successfully, but their allocations left too little
internal RAM to create the inference task's 16 KB stack. The firmware now:
- Allocates the model's 76800-byte workspace and 9992-byte state in PSRAM.
- Allocates the capture and speech audio queues in PSRAM.
- Reserves capture, inference and MQTT task stacks in INTERNAL RAM before
  speech initialization. Tasks wait on startup notifications until ready.
- Restarts I2S after speech initialization to discard old startup DMA audio.
- Prints internal free/largest-block and PSRAM totals before and after voice.

These changes relocate about 110 KiB of memory out of internal RAM. No task
stack or DMA buffer is placed in PSRAM. OPI PSRAM is now required; failure
to allocate model workspace stops with UNKNOWN before starting recognition.
The original model.c is preserved byte-for-byte as src/model/model.c.reference
(not a compiled source extension). The adapted model.c changes only writable
workspace allocation: weights, feature extraction and inference math match.
model.h is unchanged. Re-exporting the model requires reapplying this adapter
and rerunning the comparison; never copy only the pointer declarations.

Host comparison: original static buffers versus heap-backed buffers, silence
and deterministic nonzero signals over 3 model resets, 12 predictions total;
all output scores match bit-for-bit. The test is tests/model_memory_test.c,
compiled once with the original reference and once with the adapted model
and CLASSROOM_MODEL_HEAP_TEST defined. It also checks idempotent allocation.
This verifies inference parity, not ESP32 timing or available heap on board.
The managed compiler-launch limitation still prevents a fresh target build
here. Rebuild with Build-VoiceFirmware.ps1, require Build passed, then upload
that new build. Runtime coexistence of both models still needs board testing.

LOCAL BUILD COMMAND (AVOIDS A STALE ARDUINO IDE MENU SELECTION)
Installing a partition menu option does not select it in an open IDE.
If the error still says Maximum is 3145728 bytes, the build is still using
stock ESP SR 16M. The last local build.options.json confirmed this mismatch.
From this sketch folder, run:
  powershell -NoProfile -ExecutionPolicy Bypass -File .\Build-VoiceFirmware.ps1
This builds with the stock ESP-SR model-upload settings plus an explicit
upload.maximum_size=4194304 and this sketch's 4 MB partitions.csv. No board
package edits or IDE menu selections are needed for this build command.
Use -CheckOnly to validate resolved properties without compiling. An optional
-ConfigFile points to an Arduino CLI config; the workspace arduino-cli.yaml
is used automatically when present. Build output is ../build-voice-local.
The script compiles only; it does not flash the device. For upload through
Arduino IDE, select the installed InhabitX Voice 16M option before uploading.
Local CheckOnly passed: 4 MB maximum, 16 MB flash, OPI PSRAM, model at 0xC10000.
The existing application .bin is 3208608 bytes and fits the 4194304-byte slot.
Fresh compilation under the managed runner still cannot start the compiler
(Windows Illegal System DLL Relocation); no fresh build success is claimed.

ClassroomOccupancy - Waveshare ESP32-S3-AUDIO-Board
================================================

Open ClassroomOccupancy.ino in Arduino IDE. Keep all headers and the src
directory beside it. The supplied model weights and inference calculations are bundled unchanged.
The ES7210 C driver is bundled too; no additional Arduino libraries required.

INHABITX SETUP
1. Copy secrets.h.example to secrets.h beside the sketch. Fill in the Wi-Fi
   SSID/password and the HiveMQ devices password supplied by Nada.
   Without credentials the firmware runs local sensing and logs an offline notice.
2. Use a 2.4 GHz WPA2 hotspot with outbound MQTT/TLS port 8883 and NTP available.
3. In InhabitXConfig.h set NODE_BUILDING, NODE_FLOOR and NODE_ROOM for this board.
   Default is Z / 1 / Z.1.09. Do not run another node for that same room;
   the PSOC node must be stopped when this ESP32 replaces it.
4. Verify/upload with the settings below. On Serial Monitor expect:
   MQTT: connected | room Z.1.09
   Room: OCCUPIED | activity 95% | mic -30 dBFS
5. Keep the main application running on Nada's laptop. Check room Z.1.09 at
   https://nadaabbas444.github.io/inhabitx/ . Removing power should show Offline
   after about 10 seconds. Test hotspot loss/restoration and broker reconnection.

NetworkClientSecure and WiFi are supplied by Arduino-ESP32. PubSubClient 2.8
is bundled under src/mqtt with its MIT license, so no Library Manager install
is required. BrokerCertificate.h contains the project's ISRG Root X1 CA.
TLS verifies the broker hostname and certificate. NTP sets the system clock
before connecting; if NTP is blocked, publishing waits and logs a notice.
Time sync starts after Wi-Fi obtains an IP and restarts on reconnect, with
60s retries across three public NTP servers. A no-reply warning prints once.
If it persists, try another internet-connected hotspot or allow UDP port 123;
NTP_SERVER_1/2/3 in InhabitXConfig.h can point to a reachable local time server.
No certificate verification bypass is used. Passwords are never printed.
secrets.h and built binaries contain credentials; keep them out of shared files.

INHABITX MESSAGE AND HEALTH POLICY
MQTT 3.1.1 over TLS, host inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud:8883.
Username devices; client id inhabitx-node-Z.1.09 (derived from NODE_ROOM).
Topic inhabitx/<building>/<floor>/<room>/state; QoS 0; retain=false; every 2s.
Payload is only {"room":"Z.1.09","occupied":true/false,"ts":0}.
ts=0 is allowed: the main uses its own receipt clock. No audio, model scores
or diagnostic levels are published. No subscriptions or last will are needed.

OCCUPIED maps to true; UNOCCUPIED_INFERRED maps to false. UNKNOWN, a prediction
older than 2s, or an invalidated capture epoch suppresses publishing. The
main then shows Offline after its 10s timeout rather than a false empty claim.
A one-item mailbox keeps only the newest decision. No offline message backlog
is sent on reconnection. Wi-Fi and broker retries run in a lower priority task
on core 0; both broker-only and Wi-Fi disconnects are retried automatically.
The model and its existing filter are unchanged: 3 activity predictions to
confirm occupancy, 60s hold after last activity, background timing as below.
These differ from InhabitX's suggested ~3s confirmation / 2min hold; tune
OccupancyFilter.h after validation if those exact application timings are needed.
Network scheduling and added TLS RAM still require hardware validation; monitor
dropped blocks, overflows and queue depth with Wi-Fi active.

INSTALL THE 4 MB VOICE PARTITION (WINDOWS, ONCE PER CORE INSTALL)
The voice build exceeds the stock 3 MB app slot. Download this branch first,
close Arduino IDE, open PowerShell in the ClassroomOccupancy sketch folder:
  powershell -NoProfile -ExecutionPolicy Bypass -File .\Install-VoicePartition.ps1
Restart Arduino IDE and choose the new InhabitX Voice 16M partition option.
The installer defaults to Arduino-ESP32 3.3.12 under LOCALAPPDATA/Arduino15.
Pass -PlatformPath for another installation. The installer preserves existing
board entries, adds a separate menu option, and installs its partition CSV.

partitions.csv uses two 4 MB app slots and 4 MB SPIFFS, keeping the speech
model at 0xC10000 (3.9 MB) and coredump at 0xFF0000. Do not move the model
without updating the upload address. build_opt.h preserves -O3 and defines
ESP_SR's compatible-partition macro for the custom board menu option.
A local partitions.csv alone is not sufficient: Arduino IDE must also use
upload.maximum_size=4194304, supplied by the new menu option. Leave the
stock ESP SR 16M option available for other projects. Rerun after core updates.

ARDUINO SETTINGS
Boards Manager: esp32 by Espressif Systems, version 3.3.12 (installed locally).
Board: ESP32S3 Dev Module
CPU Frequency: 240 MHz
Flash Size: 16MB
PSRAM: OPI PSRAM
USB Mode: Hardware CDC and JTAG
USB CDC On Boot: Enabled
Partition Scheme: InhabitX Voice 16M (4MB APP/4MB SPIFFS/3.9MB MODEL).
Use normal USB upload, then Serial Monitor at 115200 baud.
Do not select the Arduino Nano ESP32 board or an ESP32-C3 board.
Voice requires the ESP-SR model partition and bundled English model image.
Selecting InhabitX Voice 16M makes Arduino copy/flash srmodels.bin automatically.
build_opt.h supplies -O3 for the generated inference code.

HARDWARE
No external microphone wiring is needed. Capture uses the onboard analog
microphone through ES7210, not analogRead and not a direct PDM microphone.
ESP32 is the clock master; the ADC is a slave.
I2C SDA GPIO11; I2C SCL GPIO10; ADC seven-bit address 0x40.
I2S MCLK GPIO12; BCLK GPIO13; WS/LRCK GPIO14; ADC data -> ESP GPIO15.
GPIO16 is speaker data and is not configured by this sketch.
Sample rate 16000 Hz; 16-bit signed PCM; four-slot Philips TDM.
MCLK = 256 * Fs = 4.096 MHz; BCLK = 4 * 16 * Fs = 1.024 MHz.
The sketch selects the first slot (MIC1) as mono input. It prints all four
slot RMS levels to help verify the microphone path. Do not average the four
slots: they can include an echo-reference channel or an unused input.
ES7210 microphone bias 2.87V; initial analog gain 24dB; digital gain 0dB.
Tune MIC_GAIN after observing audio levels; reduce gain if clipping occurs.
The speaker/DAC, display, camera, SD and GPIO expander are not initialized.
Only connect the board via USB for the first test. Battery operation follows
the board's normal wiring and battery switch; USB CDC will then be absent.

MODEL CONTRACT
Exact supplied model ID: e48a1b9d-548a-41c8-860a-86ac08e95c34
Input: one float sample at a time, normalized signed PCM (pcm16 / 32768.0f).
All feature extraction is already included. Do not add another FFT/Mel stage.
The export actually contains Hamming smoothing; preserve it even though an
earlier manual discussion mentioned Hann. Training/export define the pipeline.
Context: 1 second, 32 features at 50 Hz; predictions nominally every 0.5 sec.
Outputs: [0] unlabeled, [1] background, [2] human_activity.
Generated memory report: 76,800 bytes buffers + 9,992 bytes state;
34,308 bytes readonly, plus driver, queues, stacks and Arduino runtime memory.
No TensorFlow Lite Micro library or retraining required for this ANSI C export.
ModelBridge.h handles C linkage and the C99 restrict keyword in the header.

BEHAVIOR
UNKNOWN while warming up or when input is invalid/stale.
OCCUPIED after 3 consecutive activity predictions at or above 0.80.
UNOCCUPIED_INFERRED after 20 sec continuous confident background at startup.
After occupancy: at least 60 sec since the most recent confident activity,
plus at least 10 sec continuous confident background, before inferring empty.
Thresholds and times are in OccupancyFilter.h, not trained model parameters.
Ambiguous/unlabeled predictions do not establish that a room is empty.
Invalid/clipped/constant input, dropped blocks, or ADC failures must not be
interpreted as evidence of an empty room. Overflow resets inference context.

SERIAL OUTPUT
Default: a short Room status line on each state change and every 10 seconds.
It shows OCCUPIED / EMPTY (inferred) / UNKNOWN, activity score and mic level.
Connection notices and errors remain visible. Audio warning totals appear
only when counters change. Successful MQTT publishes are quiet; sending is still 2s.
SerialConfig.h controls SERIAL_STATUS_EVERY_MS and SERIAL_VERBOSE (default false).
Set SERIAL_VERBOSE=true to restore prediction JSON and MQTT payload logging.
Verbose JSON includes state, human_activity/background/unlabeled scores, selected mic
RMS in dBFS, all four slot RMS levels, clipping fraction, worst processing
block time, queue depth, dropped blocks, DMA overflows and read errors.
The processing time includes generated neural-network inference during that block.
Capture runs separately on core 0; the single stateful model runs on core 1.
The occupancy queue holds about 1.5 sec and speech about 0.5 sec. This absorbs brief bursts, not sustained slow inference.

VOICE COMMANDS (EXPERIMENTAL, OFFLINE)
Say "Hey Infineon", pause for the "Voice: listening" serial notice, then within
8 seconds say one of:
  "I am in the room"       -> user-confirmed occupied; override lasts 2 minutes.
  "I am leaving the room" -> user-declared empty; override lasts 10 minutes.
After a command, say the activation phrase again for another command. An
arrival command cancels a departure override immediately. After expiry,
the continuously running automatic occupancy filter resumes control.
VoicePolicy.h holds these durations. VoiceRecognizer.h has VOICE_ENABLED.

Recognition uses the Arduino-ESP32 ESP_SR library with its stock English
MultiNet model. "Hey Infineon" is registered as a custom command to gate
the two room commands. This is not a purpose-trained WakeNet wake-word
model; continuous command recognition has different CPU/power/false-trigger
characteristics, and brand pronunciation needs testing. No cloud service
or audio upload is used. Recognition is fixed-phrase, not open dictation.

Select InhabitX Voice 16M and OPI PSRAM before uploading. No extra wiring is needed.
Only captureTask reads I2S; it fans out the selected mono PCM to a separate
speech queue. ESP_SR handles the audio front end and command model. Model
and speech inference run concurrently and need load/memory testing on board.
If the speech queue/audio stream has a gap, voice commands are disabled
until reboot and automatic detection continues; this avoids trusting a
partially lost command. A missing speech model/partition disables voice
with a serial explanation. DEEPCRAFT weights/math are unchanged; its workspace now allocates in PSRAM.

Voice-confirmed publishes retain the normal room/occupied/ts fields and add:
  "source":"voice","message":"Room occupied confirmed by user"
or:
  "source":"voice","message":"Room marked empty by user"
These fields are sent on the same /state topic, not a new subscription.
The current main ignores extra fields: it updates occupancy, but displaying
the confirmation text on the dashboard requires a separate main/UI change.
"MQTT: sent user-confirmed OCCUPIED" / "sent user-declared EMPTY" means
the node's MQTT publish call succeeded, not an application-level acknowledgement.
Unknown/stale/faulted automatic audio still pauses publishing, including
during a voice override; no old audio result is refreshed just to keep online.

TESTING VOICE
1. Verify/upload with InhabitX Voice 16M; expect "Voice: ready".
2. Say a room command without "Hey Infineon": no manual override.
3. Say "Hey Infineon", pause, then "I am in the room": occupied confirmation.
4. Repeat wake + leaving: empty override; sound must not undo it for 10 minutes.
5. Repeat wake + arrival before expiry: occupied immediately.
6. Wake, wait over 8 seconds, then command: no override.
7. Check CPU load, no growing capture warnings, accent/brand pronunciation,
   background conversation and played audio. Audio from a recording can
   trigger recognition; the system does not authenticate a speaker.
Host policy/JSON tests pass. ESP32 compilation is still blocked in this
environment by Windows Illegal System DLL Relocation before compiler launch;
speech recognition quality and simultaneous inference are not hardware-verified.
Reference: https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/speech_command_recognition/README.html

FIRST TEST
1. Open the sketch with the settings above, Verify, then upload.
2. Initialization must find ES7210 at 0x40. If not, confirm board identity,
   pins and power. A failure prints FATAL and keeps occupancy UNKNOWN.
3. Speak near MIC1. slot_rms[0] and mic_rms_dbfs should respond, not remain
   zero or constant. The selected slot should not show sustained clipping.
4. Compare silence, ordinary speech, footsteps and classroom background.
   Inspect scores before changing thresholds. Do not expect every clap to
   pass the three-prediction confirmation rule.
5. Confirm dropped, dma_overflows and read_errors remain zero. Startup
   overflow counters can increase during initial codec configuration; ongoing
   growth is a fault. Watch queued_blocks for a sustained rising backlog.
6. Validate score parity with known training-format WAV audio before field
   evaluation. Gain and feature amplitude must match deployment conditions.

LIMITATION
This estimates occupancy from audible human activity. Silent occupants can
be missed and speech played by a phone or from outside can trigger it.
UNOCCUPIED_INFERRED means a timeout with confident background, not verified
absence of people. Field-test and retrain on real recordings from this board.
Audio remains local; the sketch does not save or transmit microphone audio.

VERIFICATION STATUS
Source/header/API review completed against the installed Arduino-ESP32
3.3.12 headers and supplied generated model. The model header and original export reference
remain byte-identical to the originals; model.c now adapts workspace allocation.
A target build was attempted but Windows prevented the installed compiler
from launching: Illegal System DLL Relocation. No compiler result or board
execution has been obtained in this session. Use Arduino IDE Verify first.
No device was flashed or serial port opened by the assistant.
InhabitX integration: target compile was attempted again and hit the same
Windows compiler-launch error before a compile result could be obtained.
Host-side contract tests cover JSON boolean encoding, UNKNOWN suppression,
truncation, stale results, stream reset invalidation and millis rollover.
See verification.json for the recorded result. No live broker or board test
was performed; credentials have not been supplied.

INTEGRATION PROVENANCE
InhabitX branch feat-run-node-on-board, commit 86c5d450ebd3da5d2ee5f3c520ce49fe2a947287:
https://github.com/NadaAbbas444/inhabitx/tree/86c5d450ebd3da5d2ee5f3c520ce49fe2a947287
Broker CA/message contract from main, commit 2f8881bcaa92f51d4098d813f7405b8b8b1b4b24.
MQTT implementation (vendored, unmodified):
https://github.com/knolleary/pubsubclient/tree/v2.8
The Infineon hardware/RTOS source was not copied: this is an ESP32 port of
the MQTT contract, retaining the existing Waveshare microphone and model.

REFERENCES AND PROVENANCE
Waveshare board pin table (explicit SDA11/SCL10, MCLK12/BCLK13/WS14/DIN15):
https://www.waveshare.com/wiki/ESP32-S3-AUDIO-Board
Waveshare schematic v1.1 (U8 ES7210, MIC1/MIC2, shared audio clocks, 0x40):
https://files.waveshare.com/wiki/ESP32-S3-AUDIO-Board/ESP32-S3-AUDIO-Board_1.1.pdf
Waveshare Arduino example documentation (lists es7210 driver dependency):
https://docs.waveshare.com/ESP32-S3-AUDIO-Board/Arduino
Waveshare ESP-IDF example documentation (audio peripheral initialization):
https://docs.waveshare.com/ESP32-S3-AUDIO-Board/ESP-IDF
Official demo archive (could not be transferred in this session):
https://files.waveshare.com/wiki/ESP32-S3-AUDIO-Board/ESP32-S3-AUDIO-Board-Demo.zip
Espressif ES7210 upstream driver API/source:
https://github.com/espressif/esp-bsp/tree/master/components/es7210
ESP-IDF I2S documentation:
https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32s3/api-reference/peripherals/i2s.html

Driver files copied from the user's existing Documents/Arduino/libraries/es7210
installation. They retain Espressif SPDX Apache-2.0 notices. Their reset,
clock coefficient, mic bias and TDM setup were inspected. This project uses
the TDM mode supported by that driver rather than copying the complete
Waveshare factory speech recognition/display application. The full factory
demo source was not retrieved or built, so example documentation checks
should not be mistaken for a source-level audit of that entire archive.
Generated model files retain their original DEEPCRAFT notices and terms.

PARTITION FIX VERIFICATION
User build screenshot: application 3208467 bytes, stock maximum 3145728.
New maximum: 4194304 per app slot. Original model address is unchanged.
CSV generation/round-trip checked using Arduino-ESP32 3.3.12's partition tool
with --flash-size 16MB. The installer has been reviewed but not executed on
the Windows machine in this turn. No new target compilation or hardware test.
