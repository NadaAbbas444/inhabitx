# Web page

A single `index.html` using `mqtt.js`. It connects to the cloud broker with the **read-only** `web` login over secure WebSockets, subscribes to `inhabitx/<building>/rooms` and shows the live room list. Anyone with the link can open it.

View-only during the hackathon; online booking comes later via `inhabitx/<building>/bookings/new`.
Hosted on Netlify (free), updated on every push to `main`.
