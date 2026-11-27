#!/usr/bin/env python3
"""
Generate the Universität Klagenfurt campus map for the master screen.

Reads OpenStreetMap data (an .osm XML export of the campus area) and writes
ui/campus_map.c: a pre-drawn RGB565 background image plus the position of each
university building, so LVGL can put live labels and pins on top.

Map data © OpenStreetMap contributors, ODbL. The UI shows that credit.

  # get the data once (any OSM export of this bbox works)
  curl -o campus.osm "https://api.openstreetmap.org/api/0.6/map?bbox=14.2590,46.6130,14.2715,46.6195"
  python3 gen_campus_map.py campus.osm --size 1024x600 --out ../ui/campus_map.c
"""
import argparse
import math
import xml.etree.ElementTree as ET

from PIL import Image, ImageChops, ImageDraw, ImageFilter

# ---- what to show --------------------------------------------------------
# OSM way/relation name -> (code shown on the map, label, is live InhabitX building)
UNI_BUILDINGS = {
    "Zentralgebäude": ("Z", "Zentralgebäude", True),
    "Nordtrakt": ("N", "Nordtrakt", False),
    "Südtrakt": ("S", "Südtrakt", False),
    "Ost-Trakt": ("O", "Ost-Trakt", False),
    "Vorstufengebäude": ("V", "Vorstufe", False),
    "Universitätsbibliothek Klagenfurt": ("UB", "Bibliothek", False),
    "Mensa Gebäude": ("M", "Mensa", False),
    "USI": ("USI", "USI Sport", False),
}

# Colours (dark map, matches ui_theme)
C_BG = (10, 15, 28)
C_GREEN = (14, 30, 31)
C_WATER = (13, 33, 56)
C_ROAD = (27, 36, 56)
C_ROAD_MAJOR = (36, 47, 72)
C_BLD = (26, 34, 52)
C_BLD_EDGE = (38, 49, 74)
C_UNI = (34, 48, 80)
C_UNI_EDGE = (66, 88, 135)
C_LIVE = (47, 107, 255)
C_LIVE_EDGE = (120, 165, 255)
C_CAMPUS = (16, 23, 40)

SS = 3  # supersampling for smooth edges


def parse(path):
    root = ET.parse(path).getroot()
    nodes = {n.get("id"): (float(n.get("lat")), float(n.get("lon"))) for n in root.iter("node")}
    ways = {}
    for w in root.iter("way"):
        refs = [nd.get("ref") for nd in w.findall("nd")]
        pts = [nodes[r] for r in refs if r in nodes]
        tags = {t.get("k"): t.get("v") for t in w.findall("tag")}
        ways[w.get("id")] = (pts, tags, refs)
    rels = []
    for r in root.iter("relation"):
        tags = {t.get("k"): t.get("v") for t in r.findall("tag")}
        outers = [m.get("ref") for m in r.findall("member") if m.get("type") == "way" and m.get("role") in ("outer", "")]
        rels.append((tags, outers))
    return ways, rels


def join_rings(way_ids, ways):
    """Join relation member ways into closed rings."""
    segs = [list(ways[w][0]) for w in way_ids if w in ways and ways[w][0]]
    rings = []
    while segs:
        ring = segs.pop(0)
        changed = True
        while ring[0] != ring[-1] and changed:
            changed = False
            for i, s in enumerate(segs):
                if s[0] == ring[-1]:
                    ring += s[1:]
                elif s[-1] == ring[-1]:
                    ring += s[::-1][1:]
                elif s[-1] == ring[0]:
                    ring = s[:-1] + ring
                elif s[0] == ring[0]:
                    ring = s[::-1][:-1] + ring
                else:
                    continue
                segs.pop(i)
                changed = True
                break
        rings.append(ring)
    return rings


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("osm")
    ap.add_argument("--size", default="1024x600", help="screen size; map fills the campus page card")
    ap.add_argument("--out", default="../ui/campus_map.c")
    a = ap.parse_args()

    sw, sh = (int(v) for v in a.size.split("x"))
    # same layout maths as the UI: sidebar 208, page padding, header
    W = sw - 208 - 2 * 20
    H = sh - 92 - 20
    ways, rels = parse(a.osm)

    # projection centred on the university, metres -> pixels
    lat0, lon0 = 46.6168, 14.2655
    kx = math.cos(math.radians(lat0)) * 111320.0
    ky = 110540.0
    metres_w = 760.0                       # how much of the campus fits across
    scale = W / metres_w

    def px(p):
        lat, lon = p
        return ((lon - lon0) * kx * scale + W / 2) * SS, ((lat0 - lat) * ky * scale + H / 2) * SS

    img = Image.new("RGB", (W * SS, H * SS), C_BG)
    d = ImageDraw.Draw(img)

    def poly(pts, fill, edge=None, width=1):
        if len(pts) < 3:
            return
        xy = [px(p) for p in pts]
        d.polygon(xy, fill=fill)
        if edge:
            d.line(xy + [xy[0]], fill=edge, width=width * SS, joint="curve")

    # campus ground
    for wid, (pts, tags, refs) in ways.items():
        if tags.get("amenity") == "university" or tags.get("name") == "Universitätssportszentrum USI":
            poly(pts, C_CAMPUS)
    for tags, outers in rels:
        if tags.get("amenity") == "university":
            for ring in join_rings(outers, ways):
                poly(ring, C_CAMPUS)

    # green + water
    for wid, (pts, tags, refs) in ways.items():
        if tags.get("leisure") in ("park", "garden", "pitch") or tags.get("landuse") in ("grass", "meadow", "forest", "recreation_ground") or tags.get("natural") in ("wood", "scrub", "grassland"):
            if refs and refs[0] == refs[-1]:
                poly(pts, C_GREEN)
    for wid, (pts, tags, refs) in ways.items():
        if tags.get("natural") == "water" and refs and refs[0] == refs[-1]:
            poly(pts, C_WATER)
        elif tags.get("waterway") in ("canal", "river", "stream"):
            d.line([px(p) for p in pts], fill=C_WATER, width=int((9 if tags["waterway"] != "stream" else 4) * SS * scale * 1.4), joint="curve")

    # roads
    widths = {"primary": 10, "secondary": 9, "tertiary": 8, "residential": 6, "unclassified": 6,
              "service": 3.5, "footway": 2, "pedestrian": 4, "cycleway": 2, "path": 1.5, "living_street": 5}
    for wid, (pts, tags, refs) in ways.items():
        hw = tags.get("highway")
        if hw in widths and len(pts) > 1:
            w = max(1, int(widths[hw] * scale * SS * 1.3))
            col = C_ROAD_MAJOR if widths[hw] >= 6 else C_ROAD
            d.line([px(p) for p in pts], fill=col, width=w, joint="curve")

    # buildings
    labels = []
    live = []

    def add_building(name, rings, tags):
        if name in UNI_BUILDINGS:
            code, label, is_live = UNI_BUILDINGS[name]
            for r in rings:
                if is_live:
                    live.append(r)
                else:
                    poly(r, C_UNI, C_UNI_EDGE)
            allp = [p for r in rings for p in r]
            cx = sum(px(p)[0] for p in allp) / len(allp) / SS
            cy = sum(px(p)[1] for p in allp) / len(allp) / SS
            labels.append((code, label, int(cx), int(cy), is_live))
        else:
            for r in rings:
                poly(r, C_BLD, C_BLD_EDGE)

    for wid, (pts, tags, refs) in ways.items():
        if "building" in tags and tags.get("building") not in ("roof", "bridge") and refs and refs[0] == refs[-1]:
            add_building(tags.get("name"), [pts], tags)
    for tags, outers in rels:
        if "building" in tags:
            add_building(tags.get("name"), join_rings(outers, ways), tags)

    # the live building: soft glow + bright fill
    glow = Image.new("RGB", img.size, (0, 0, 0))
    gd = ImageDraw.Draw(glow)
    for r in live:
        gd.polygon([px(p) for p in r], fill=C_LIVE)
    glow = glow.filter(ImageFilter.GaussianBlur(16 * SS)).point(lambda v: int(v * 0.7))
    img = ImageChops.add(img, glow)
    d = ImageDraw.Draw(img)
    for r in live:
        poly(r, C_LIVE, C_LIVE_EDGE, 2)

    # Lakeside Park label from the B-buildings
    lake = [px(p) for pts, tags, refs in ways.values()
            if tags.get("building") == "commercial" and (tags.get("name") or "").startswith("B") for p in pts]
    if lake:
        labels.append(("", "Lakeside Park", int(sum(x for x, _ in lake) / len(lake) / SS),
                       int(sum(y for _, y in lake) / len(lake) / SS), False))

    labels = [l for l in labels if 40 <= l[2] <= W - 40 and 20 <= l[3] <= H - 20]
    img = img.resize((W, H), Image.LANCZOS)

    # RGB565 little endian
    data = bytearray()
    for r, g, b in img.getdata():
        v = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        data += bytes((v & 0xFF, v >> 8))

    with open(a.out, "w", encoding="utf-8") as f:
        f.write("/* Generated by assets/gen_campus_map.py. Do not edit.\n"
                " * Map data (c) OpenStreetMap contributors, ODbL. */\n")
        f.write('#include "ui_internal.h"\n\n')
        f.write(f"#if UI_HOR_RES == {sw} && UI_VER_RES == {sh}\n\n")
        f.write("static const uint8_t s_campus_px[] = {\n")
        for i in range(0, len(data), 24):
            f.write("    " + ",".join(f"0x{b:02x}" for b in data[i:i + 24]) + ",\n")
        f.write("};\n\n")
        f.write("const lv_image_dsc_t ui_img_campus = {\n"
                "    .header = { .magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_RGB565,\n"
                f"                .w = {W}, .h = {H}, .stride = {W * 2} }},\n"
                f"    .data_size = sizeof s_campus_px,\n"
                "    .data = s_campus_px,\n};\n\n")
        f.write("const ui_campus_label_t ui_campus_labels[] = {\n")
        for code, label, x, y, is_live in labels:
            f.write(f'    {{ "{code}", "{label}", {x}, {y}, {"true" if is_live else "false"} }},\n')
        f.write("};\n")
        f.write(f"const int ui_campus_label_count = {len(labels)};\n\n")
        f.write("#endif\n")
    print(f"wrote {a.out}: {W}x{H}, {len(labels)} labels")


if __name__ == "__main__":
    main()
