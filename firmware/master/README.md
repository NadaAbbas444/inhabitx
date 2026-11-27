# Master firmware (E84 + screen, one per building)

- Subscribes to `inhabitx/<building>/#` and keeps the latest state of every room
- Runs the logic: room state, bookings (name + email, max 2 per email), ghost release
- Draws the screen UI with LVGL (native UI, not a web page): floor map, free-now list, booking
- Publishes the building state to `inhabitx/<building>/rooms` (retained), with no names or emails

See README §4–§7 for the messages, the logic and the wireframes.
