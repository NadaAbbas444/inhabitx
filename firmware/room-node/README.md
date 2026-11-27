# Room node firmware

Runs on one PSOC™ Edge E84 per room.

- **Cortex-M55 + Ethos-U55 NPU:** radar presence model (DEEPCRAFT™ Studio), noise level from the PDM mic
- **Cortex-M33:** room state machine (free → booked → ghost → occupied), e-ink/LED output, Wi-Fi + MQTT client

Publishes `campus/<building>/<floor>/<room>/state` and subscribes to `.../booking`.

Toolchain: ModusToolbox™.
