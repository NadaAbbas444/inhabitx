# Slave firmware (E84 + radar, one per room)

Reads the radar, decides **someone there or not**, and publishes it to the MQTT broker.

- Topic: `inhabitx/<building>/<floor>/<room>/state`
- Payload: `{"room":"Z.1.09","occupied":true,"ts":1764237600}`, every 2 s
- `occupied` → true after ~3 s of presence, false after 2 min without (demo: 20 s)

Toolchain: ModusToolbox™. Start from the Infineon radar example and the Wi-Fi MQTT client example (TLS to HiveMQ Cloud).

## Sensor choice
_To be filled in (task S1)._
