# Master (one per building)

Runs on the **laptop** for now (see README §3, "What runs where right now"), on an E84 + screen later.

- Subscribes to `inhabitx/<building>/#` and keeps the latest state of every room
- Runs the logic: room state, bookings (name + email, max 2 per email), ghost release, lost & found
- Answers booking and found-item requests from the web page on `replies/<req>`
- Draws the screen UI with LVGL (native UI, not a web page)
- Publishes the building state and found items (retained), with no names or emails

| Folder | What | Status |
|---|---|---|
| `logic/` | plain C, no LVGL, no MQTT | done, tests pass |
| `ui/` | LVGL screen UI | done in the simulator |
| `sim/` | runs the master on a Linux laptop | done, MQTT still to add (M1, M4, M6) |
| `assets/` | generators for floor plan, campus map, fonts | done |

How to build and run: [sim/README.md](sim/README.md). Messages, logic and booking rules: README §4–§6.
