InhabitX speech adapter
======================
Derived from Arduino-ESP32 3.3.12 libraries/ESP_SR/src/esp32-hal-sr.c:
https://github.com/espressif/arduino-esp32/blob/3.3.12/libraries/ESP_SR/src/esp32-hal-sr.c
Original SPDX copyright/license notices remain in VoiceEngine.c.

Changes: use distinct public symbols; copy recognition IDs/confidence and clean
MultiNet before delivering each match; clean on timeout; use a 10-second decoder
cycle for the application's 8-second window. Gate startup tasks until all are
created and allocate the feed buffer before starting them, so startup allocation
failure can clean up parked tasks. Forward confidence to the application.

VoiceTiming.h is shared by engine and policy. Stock installed files are untouched.
ESP_SR is still included to locate the SDK/library headers and copy srmodels.bin
during Arduino builds. The application calls this adapter, not stock sr_start.
Model files, microphone capture and MQTT contract are otherwise unchanged.
This adapter is fixed to the reviewed 3.3.12 interfaces; review it on core updates.
