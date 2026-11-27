# Booking service

Python · FastAPI · SQLite · Mosquitto (MQTT broker)

- `POST /bookings` with `{room, from, to, name, email}`. Rules: room free, at most 2 active bookings per email
- `GET /rooms` returns live state of every room (fed from MQTT)
- Releases ghost bookings when a room node reports `ghost` for more than 10 min
