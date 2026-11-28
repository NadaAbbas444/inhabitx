#pragma once
#include <stdint.h>

// Normal mode: state changes and one short status line every 10 seconds.
// Enable only when diagnosing microphone/model input or MQTT payloads.
constexpr bool SERIAL_VERBOSE = false;
constexpr uint32_t SERIAL_STATUS_EVERY_MS = 10000;
