# Tools

## `fake_nodes.py`

Publishes node messages for rooms without hardware, so the main and the web page can be tested and the demo shows a full floor. Simulated signals are allowed by the challenge.

```bash
pip install -r tools/requirements.txt
python3 tools/fake_nodes.py                              # Z.1.08, Z.1.10, Z.1.11, Z.1.12
python3 tools/fake_nodes.py --room Z.1.09 --occupied false
python3 tools/fake_nodes.py --shuffle 3                  # a random room changes about every 3 min
```

Each room sends `{"room":"Z.1.10","occupied":false,"ts":…}` every 2 s on `inhabitx/Z/1/Z.1.10/state` (README §3). It logs in as `devices` with the same `firmware/main/sim/broker.conf` as the main simulator (or `IX_MQTT_*` variables, or `--conf FILE`).

While it runs, type:

| Command | Effect |
|---|---|
| `o 10` | toggle someone there / empty in Z.1.10 |
| `x 12` | stop / start sending for Z.1.12 (offline on the main after 10 s) |
| `s` | show the rooms |
| `q` | quit |

Z.1.09 is left out by default because that's the live board's room; fake it with `--room Z.1.09` until the real node runs. Run the main simulator without `--fake` alongside, so the built-in fake nodes don't overwrite these.
