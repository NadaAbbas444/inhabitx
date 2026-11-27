#!/usr/bin/env python3
"""Fake slaves: publish room states to the broker like real slaves (README §4).

For rooms without hardware, so the master and the web page show a full floor.
Each room sends {"room": ..., "occupied": ..., "ts": ...} every 2 s on
inhabitx/<building>/<floor>/<room>/state. A room that stops sending turns
offline on the master after 10 s.

    python3 tools/fake_slaves.py                           # Z.1.08, Z.1.10, Z.1.11, Z.1.12
    python3 tools/fake_slaves.py --room Z.1.09 --occupied false
    python3 tools/fake_slaves.py --shuffle 3               # a random room changes every ~3 min

While it runs, type commands (room = full id or suffix, e.g. 10):
    o <room>   toggle someone there / empty
    x <room>   stop / start sending (room goes offline on the master)
    s          show the rooms
    q          quit

Login: the `devices` user from firmware/master/sim/broker.conf (same file as
the master simulator), or IX_MQTT_HOST / _PORT / _USER / _PASSWORD.
Needs paho-mqtt: pip install -r tools/requirements.txt
"""
import argparse
import json
import os
import random
import select
import sys
import threading
import time

try:
    import paho.mqtt.client as mqtt
except ImportError:
    sys.exit("paho-mqtt is missing: pip install -r tools/requirements.txt")

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_CONF = os.path.join(HERE, "..", "firmware", "master", "sim", "broker.conf")

# rooms without hardware and how they start (Z.1.09 is the live board's room)
DEFAULT_ROOMS = {"Z.1.08": True, "Z.1.10": False, "Z.1.11": True, "Z.1.12": False}


def load_conf(path):
    conf = {"port": "8883"}
    if os.path.exists(path):
        with open(path) as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith("#") and "=" in line:
                    k, v = line.split("=", 1)
                    if v.strip():
                        conf[k.strip()] = v.strip()
    for key in ("host", "port", "user", "password", "cafile"):
        v = os.environ.get("IX_MQTT_" + key.upper())
        if v:
            conf[key] = v
    missing = [k for k in ("host", "user", "password") if not conf.get(k)]
    if missing:
        sys.exit(f"no {', '.join(missing)} in {path} or IX_MQTT_* variables "
                 "(copy firmware/master/sim/broker.conf.example to broker.conf)")
    return conf


def floor_of(room):
    """'Z.1.09' -> '1'"""
    parts = room.split(".")
    if len(parts) != 3 or not parts[1].isdigit():
        sys.exit(f"room ids look like Z.1.09, not {room}")
    return parts[1]


def parse_bool(s):
    if s.lower() in ("true", "1", "yes", "on"):
        return True
    if s.lower() in ("false", "0", "no", "off"):
        return False
    raise argparse.ArgumentTypeError("use true or false")


def main():
    ap = argparse.ArgumentParser(description="Publish fake slave messages to the broker.")
    ap.add_argument("--room", action="append", help="room to fake (repeat for more); default: Z.1.08, Z.1.10, Z.1.11, Z.1.12")
    ap.add_argument("--occupied", type=parse_bool, help="start state for the --room rooms (default false)")
    ap.add_argument("--building", default="Z")
    ap.add_argument("--interval", type=float, default=2.0, help="seconds between messages (default 2)")
    ap.add_argument("--shuffle", type=float, metavar="MIN", help="toggle a random room about every MIN minutes")
    ap.add_argument("--conf", default=os.environ.get("IX_BROKER_CONF", DEFAULT_CONF))
    args = ap.parse_args()
    sys.stdout.reconfigure(line_buffering=True)

    if args.room:
        rooms = {r: bool(args.occupied) for r in args.room}
    else:
        rooms = dict(DEFAULT_ROOMS)
        if args.occupied is not None:
            rooms = {r: args.occupied for r in rooms}
    for r in rooms:
        floor_of(r)
    sending = {r: True for r in rooms}
    lock = threading.Lock()

    conf = load_conf(args.conf)
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=f"inhabitx-fake-slaves-{os.getpid()}")
    client.username_pw_set(conf["user"], conf["password"])
    client.tls_set(ca_certs=conf.get("cafile"))
    connected = threading.Event()
    quitting = threading.Event()

    def on_connect(c, userdata, flags, reason, props):
        if reason.is_failure:
            print(f"broker said no: {reason}")
            return
        connected.set()
        print(f"connected to {conf['host']} as {conf['user']}")

    def on_disconnect(c, userdata, flags, reason, props):
        connected.clear()
        if not quitting.is_set():
            print(f"disconnected ({reason}), reconnecting")

    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.reconnect_delay_set(1, 10)
    client.connect_async(conf["host"], int(conf["port"]), keepalive=30)
    client.loop_start()

    def show():
        with lock:
            for r in rooms:
                st = "occupied" if rooms[r] else "empty"
                print(f"  {r}  {st:8}  {'' if sending[r] else '(not sending)'}")

    def find(arg):
        arg = arg.strip()
        hits = [r for r in rooms if arg and r.endswith(arg)]
        return hits[0] if len(hits) == 1 else None

    def send_all():
        if not connected.is_set():
            return
        with lock:
            for r, occ in rooms.items():
                if sending[r]:
                    topic = f"inhabitx/{args.building}/{floor_of(r)}/{r}/state"
                    client.publish(topic, json.dumps({"room": r, "occupied": occ, "ts": int(time.time())}))

    print(f"faking {', '.join(rooms)} every {args.interval:g} s. Commands: o <room>, x <room>, s, q")
    show()
    next_send = 0.0
    next_shuffle = time.time() + args.shuffle * 60 * random.uniform(0.5, 1.5) if args.shuffle else None
    stdin_open = True
    try:
        while True:
            now = time.time()
            if now >= next_send:
                next_send = now + args.interval
                send_all()
            if next_shuffle and now >= next_shuffle:
                next_shuffle = now + args.shuffle * 60 * random.uniform(0.5, 1.5)
                with lock:
                    r = random.choice(list(rooms))
                    rooms[r] = not rooms[r]
                    print(f"  shuffle: {r} {'occupied' if rooms[r] else 'empty'}")
                next_send = 0.0
            if not stdin_open:
                time.sleep(0.2)
                continue
            ready, _, _ = select.select([sys.stdin], [], [], 0.2)
            if not ready:
                continue
            line = sys.stdin.readline()
            if not line:            # stdin closed (run in the background): keep sending
                stdin_open = False
                continue
            cmd, _, rest = line.strip().partition(" ")
            if cmd == "q":
                break
            if cmd == "s":
                show()
                continue
            if cmd in ("o", "x"):
                r = find(rest)
                if not r:
                    print("  unknown room")
                    continue
                with lock:
                    if cmd == "o":
                        rooms[r] = not rooms[r]
                        print(f"  {r} {'occupied' if rooms[r] else 'empty'}")
                    else:
                        sending[r] = not sending[r]
                        print(f"  {r} {'sending' if sending[r] else 'stopped (offline on the master in 10 s)'}")
                next_send = 0.0
                continue
            if cmd:
                print("  commands: o <room>, x <room>, s, q")
    except KeyboardInterrupt:
        pass
    quitting.set()
    client.disconnect()
    client.loop_stop()


if __name__ == "__main__":
    main()
