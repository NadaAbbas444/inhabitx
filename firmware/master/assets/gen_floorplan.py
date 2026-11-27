#!/usr/bin/env python3
"""
Generate the floor-1 plan of the Zentralgebäude for the master screen AND the web page.

Outer walls: the real building outline from OpenStreetMap (© OpenStreetMap
contributors, ODbL). Interior (corridor, rooms, stairs, doors): a plausible
layout, NOT the official AAU plan. Replace ROOMS below with the real one when
it is available.

Writes:
  ui/floorplan.c      static plan as an RGB565 image + room rectangles (pixels)
  ../../web/floorplan.js  the same plan as SVG + room rectangles (metres)

  python3 gen_floorplan.py --size 1024x600
"""
import argparse
import math

from PIL import Image, ImageDraw

# Real outline (way 43707111), lat/lon
OUTLINE_LL = [[46.616200,14.265782],[46.616138,14.265792],[46.616137,14.265776],[46.616109,14.265410],[46.616108,14.265404],[46.616174,14.265393],[46.616172,14.265369],[46.616170,14.265346],[46.616148,14.265076],[46.616139,14.264961],[46.616137,14.264943],[46.616088,14.264951],[46.616066,14.264697],[46.616113,14.264688],[46.616103,14.264566],[46.616101,14.264543],[46.616100,14.264520],[46.616032,14.264532],[46.616031,14.264524],[46.616029,14.264503],[46.616001,14.264143],[46.616160,14.264117],[46.616178,14.264114],[46.616194,14.264111],[46.616271,14.264099],[46.616300,14.264473],[46.616349,14.264465],[46.616380,14.264460],[46.616381,14.264477],[46.616382,14.264487],[46.616384,14.264517],[46.616412,14.264880],[46.616413,14.264894],[46.616415,14.264913],[46.616417,14.264940],[46.616445,14.265301],[46.616447,14.265322],[46.616447,14.265330],[46.616450,14.265359],[46.616424,14.265363],[46.616379,14.265371],[46.616386,14.265450],[46.616408,14.265732],[46.616409,14.265747],[46.616397,14.265749],[46.616293,14.265766],[46.616200,14.265782]]

# ---- interior, in metres: x east, y south, origin = north-west corner ------
CORRIDOR = (1.0, 15.0, 126.0, 18.6)          # x0, y0, x1, y1
# kind: live = has a sensor (coloured by state), room = other room, wc, stairs, lounge
ROOMS = [
    # north side, west wing + middle
    ("Z.1.01", "room",    0.6,  9.2, 10.0, 15.0, "up"),
    ("Z.1.02", "room",   10.0,  9.2, 19.4, 15.0, "up"),
    ("Z.1.03", "room",   19.4,  9.2, 28.8, 15.0, "up"),
    ("Z.1.04", "room",   28.8,  0.4, 36.4, 15.0, "up"),
    ("Z.1.05", "room",   36.4,  0.4, 44.0, 15.0, "up"),
    # north side, east
    ("Z.1.06", "room",   44.0,  0.4, 54.0, 15.0, "up"),
    ("Z.1.07", "room",   54.0,  0.4, 64.0, 15.0, "up"),
    ("Z.1.08", "live",   64.0,  0.4, 80.0, 15.0, "up"),
    ("Z.1.09", "live",   80.0,  0.4, 88.0, 15.0, "up"),
    ("WC",     "wc",     88.0,  0.4, 92.0, 15.0, "up"),
    ("Z.1.13", "room",   92.0,  0.4, 97.8, 15.0, "up"),
    ("Z.1.14", "room",   97.8,  8.2, 107.0, 15.0, "up"),
    ("Z.1.15", "room",  107.0,  8.2, 116.0, 15.0, "up"),
    ("Z.1.18", "room",  116.0,  8.2, 126.6, 15.0, "up"),
    # south side, west wing + middle
    ("Z.1.20", "room",    0.6, 18.6, 10.0, 38.5, "down"),
    ("Z.1.21", "room",   10.0, 18.6, 19.0, 38.5, "down"),
    ("WC",     "wc",     19.0, 18.6, 23.0, 30.0, "down"),
    ("Stairs", "stairs", 23.0, 18.6, 29.9, 30.0, "down"),
    ("Z.1.22", "room",   29.9, 18.6, 42.9, 30.9, "down"),
    ("Z.1.23", "room",   19.0, 30.0, 29.9, 38.5, "up"),
    # south side
    ("Lounge", "lounge", 42.9, 18.6, 62.5, 36.2, "down"),
    ("Z.1.10", "live",   62.5, 18.6, 70.0, 30.6, "down"),
    ("Z.1.11", "live",   70.0, 18.6, 81.0, 30.6, "down"),
    ("Z.1.12", "live",   81.0, 18.6, 87.0, 30.6, "down"),
    ("Stairs", "stairs", 87.0, 18.6, 93.4, 30.6, "down"),
    ("Lift",   "wc",     93.4, 18.6, 97.2, 24.0, "down"),
    ("Z.1.16", "room",   97.2, 18.6, 107.0, 37.8, "down"),
    ("Z.1.17", "room",  107.0, 18.6, 116.0, 37.8, "down"),
    ("Z.1.19", "room",  116.0, 18.6, 126.6, 37.8, "down"),
]
HERE = (90.0, 16.8)                       # the master screen, by the stairs
VIEW = (52.0, -3.0, 54.0, 45.0)           # crop shown on the overview card: x, y, w, h
VIEW_FULL = (-1.5, -1.5, 130.0, 42.0)     # whole floor, for the full-screen view

# colours (dark theme, see ui_internal.h)
BG = (18, 26, 44)          # card
FLOOR = (22, 32, 56)
CORR = (27, 38, 64)
WALL_IN = (92, 112, 152)
WALL_OUT = (150, 170, 210)
WINDOW = (120, 180, 255)
STAIR = (60, 76, 110)
DOOR = (140, 158, 195)


def outline_m():
    lat0 = sum(p[0] for p in OUTLINE_LL) / len(OUTLINE_LL)
    lon0 = sum(p[1] for p in OUTLINE_LL) / len(OUTLINE_LL)
    kx, ky = math.cos(math.radians(lat0)) * 111320, 110540
    xy = [((lo - lon0) * kx, (la - lat0) * ky) for la, lo in OUTLINE_LL]
    a = math.radians(6.4958)                       # main axis of the building
    r = [(x * math.cos(-a) - y * math.sin(-a), x * math.sin(-a) + y * math.cos(-a)) for x, y in xy]
    minx, maxy = min(p[0] for p in r), max(p[1] for p in r)
    return [(round(x - minx, 2), round(maxy - y, 2)) for x, y in r]


def primitives():
    """Everything static, as ('poly'|'rect'|'line'|'arc', points, fill, stroke, width_m)."""
    P = []
    out = outline_m()
    P.append(("poly", out, FLOOR, None, 0))
    cx0, cy0, cx1, cy1 = CORRIDOR
    P.append(("rect", (cx0, cy0, cx1, cy1), CORR, None, 0))
    for rid, kind, x0, y0, x1, y1, side in ROOMS:
        if kind == "stairs":
            n = int((x1 - x0) / 0.45)
            for i in range(1, n):
                x = x0 + i * (x1 - x0) / n
                P.append(("line", [(x, y0 + 1.2), (x, y1 - 1.2)], None, STAIR, 0.12))
            P.append(("line", [((x0 + x1) / 2, y0 + 1.2), ((x0 + x1) / 2, y1 - 1.2)], None, STAIR, 0.25))
        if kind == "lounge":
            for gx in range(int(x0) + 3, int(x1) - 1, 5):        # a few tables
                for gy in (24.0, 30.0):
                    P.append(("rect", (gx, gy, gx + 2.2, gy + 1.2), None, STAIR, 0.12))
        P.append(("rect", (x0, y0, x1, y1), None, WALL_IN, 0.3))
        # door on the corridor side, with swing
        if kind not in ("lounge",):
            dx = x0 + 0.8 if kind != "stairs" else (x0 + x1) / 2 - 0.6
            yw = y1 if side == "up" else y0
            P.append(("rect", (dx, yw - 0.25, dx + 1.0, yw + 0.25), CORR, None, 0))
            if kind not in ("stairs",):
                sgn = -1 if side == "up" else 1
                P.append(("line", [(dx, yw), (dx, yw + sgn * 1.0)], None, DOOR, 0.18))
                P.append(("arc", (dx, yw, 1.0, 0 if sgn > 0 else 270, 90 if sgn > 0 else 360), None, DOOR, 0.14))
    P.append(("poly", out, None, WALL_OUT, 0.7))
    # windows along the long outer walls
    for (x1, y1), (x2, y2) in zip(out, out[1:]):
        L = math.hypot(x2 - x1, y2 - y1)
        if L < 6 or abs(y2 - y1) > 0.5:
            continue
        n = int(L // 3.2)
        for i in range(n):
            t0 = (i * 3.2 + 0.8) / L
            t1 = (i * 3.2 + 2.4) / L
            P.append(("line", [(x1 + (x2 - x1) * t0, y1 + (y2 - y1) * t0), (x1 + (x2 - x1) * t1, y1 + (y2 - y1) * t1)],
                      None, WINDOW, 0.35))
    return P


def raster(P, view, W, H, ss=4):
    vx, vy, vw, vh = view
    s = min(W / vw, H / vh)
    ox, oy = (W - vw * s) / 2, (H - vh * s) / 2

    def px(p):
        return ((ox + (p[0] - vx) * s) * ss, (oy + (p[1] - vy) * s) * ss)

    img = Image.new("RGB", (W * ss, H * ss), BG)
    d = ImageDraw.Draw(img)
    for kind, pts, fill, stroke, w in P:
        lw = max(1, int(w * s * ss))
        if kind == "poly":
            xy = [px(p) for p in pts]
            if fill: d.polygon(xy, fill=fill)
            if stroke: d.line(xy + [xy[0]], fill=stroke, width=lw, joint="curve")
        elif kind == "rect":
            (x0, y0), (x1, y1) = px(pts[:2]), px(pts[2:])
            d.rectangle([x0, y0, x1, y1], fill=fill, outline=stroke, width=lw if stroke else 0)
        elif kind == "line":
            d.line([px(p) for p in pts], fill=stroke, width=lw)
        elif kind == "arc":
            x, y, r, a0, a1 = pts
            (cx, cy), rr = px((x, y)), r * s * ss
            d.arc([cx - rr, cy - rr, cx + rr, cy + rr], a0, a1, fill=stroke, width=lw)
    return img.resize((W, H), Image.LANCZOS), s, ox, oy


def svg(P):
    def c(rgb): return "#%02x%02x%02x" % rgb
    out = []
    for kind, pts, fill, stroke, w in P:
        st = f' fill="{c(fill) if fill else "none"}"' + (f' stroke="{c(stroke)}" stroke-width="{w}"' if stroke else "")
        if kind == "poly":
            out.append(f'<polygon points="{" ".join(f"{x},{y}" for x, y in pts)}"{st} stroke-linejoin="round"/>')
        elif kind == "rect":
            x0, y0, x1, y1 = pts
            out.append(f'<rect x="{x0}" y="{y0}" width="{round(x1 - x0, 2)}" height="{round(y1 - y0, 2)}"{st}/>')
        elif kind == "line":
            out.append(f'<polyline points="{" ".join(f"{round(x, 2)},{round(y, 2)}" for x, y in pts)}"{st}/>')
        elif kind == "arc":
            x, y, r, a0, a1 = pts
            p0 = (x + r * math.cos(math.radians(a0)), y + r * math.sin(math.radians(a0)))
            p1 = (x + r * math.cos(math.radians(a1)), y + r * math.sin(math.radians(a1)))
            out.append(f'<path d="M{p0[0]:.2f},{p0[1]:.2f} A{r},{r} 0 0 1 {p1[0]:.2f},{p1[1]:.2f}"{st}/>')
    return "".join(out)


KINDS = {"live": "UI_PLAN_LIVE", "room": "UI_PLAN_ROOM", "wc": "UI_PLAN_LABEL",
         "stairs": "UI_PLAN_LABEL", "lounge": "UI_PLAN_LABEL"}


def emit_c(f, P, view, W, H, img_name, prefix):
    img, s, ox, oy = raster(P, view, W, H)
    vx, vy = view[0], view[1]
    tx = lambda x: int(round(ox + (x - vx) * s))
    ty = lambda y: int(round(oy + (y - vy) * s))
    data = bytearray()
    for r, g, b in img.getdata():
        v = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        data += bytes((v & 0xFF, v >> 8))
    f.write(f"static const uint8_t s_{img_name}_px[] = {{\n")
    for i in range(0, len(data), 24):
        f.write("    " + ",".join(f"0x{b:02x}" for b in data[i:i + 24]) + ",\n")
    f.write("};\n\n")
    f.write(f"const lv_image_dsc_t ui_img_{img_name} = {{\n"
            "    .header = { .magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_RGB565,\n"
            f"                .w = {W}, .h = {H}, .stride = {W * 2} }},\n"
            f"    .data_size = sizeof s_{img_name}_px,\n    .data = s_{img_name}_px,\n}};\n\n")
    f.write(f"const ui_plan_room_t {prefix}_rooms[] = {{\n")
    n = 0
    for rid, kind, x0, y0, x1, y1, side in ROOMS:
        X0, Y0, X1, Y1 = max(0, tx(x0)), max(0, ty(y0)), min(W, tx(x1)), min(H, ty(y1))
        if X1 - X0 < 8 or Y1 - Y0 < 8:
            continue
        f.write(f'    {{ "{rid}", {KINDS[kind]}, {X0}, {Y0}, {X1 - X0}, {Y1 - Y0} }},\n')
        n += 1
    f.write("};\n")
    f.write(f"const int {prefix}_room_count = {n};\n")
    f.write(f"const lv_point_t {prefix}_here = {{ {tx(HERE[0])}, {ty(HERE[1])} }};\n\n")
    return n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--size", default="1024x600")
    ap.add_argument("--c-out", default="../ui/floorplan.c")
    ap.add_argument("--js-out", default="../../../web/floorplan.js")
    a = ap.parse_args()
    sw, sh = (int(v) for v in a.size.split("x"))

    # same maths as ui_overview.c: right column card, plan area inside it
    page_w, page_h = sw - 208 - 40, sh - 92 - 20
    col_a = page_w * 61 // 100
    col_b = page_w - col_a - 16
    plan_card_h = page_h * 60 // 100
    W, H = col_b - 32, plan_card_h - 32 - 40

    P = primitives()
    with open(a.c_out, "w", encoding="utf-8") as f:
        f.write("/* Generated by assets/gen_floorplan.py. Do not edit.\n"
                " * Outer walls: OpenStreetMap (ODbL). Interior: approximate, not the official plan. */\n")
        f.write('#include "ui_internal.h"\n\n')
        f.write(f"#if UI_HOR_RES == {sw} && UI_VER_RES == {sh}\n\n")
        n1 = emit_c(f, P, VIEW, W, H, "floorplan", "ui_plan")
        # full-screen view: whole width minus margins, below a 72 px header
        FW = sw - 40
        FH = min(sh - 72 - 90, int(FW * VIEW_FULL[3] / VIEW_FULL[2]))
        n2 = emit_c(f, P, VIEW_FULL, FW, FH, "floorplan_full", "ui_plan_full")
        f.write("#endif\n")
    print(f"card {W}x{H}px ({n1} rooms), full {FW}x{FH}px ({n2} rooms)")

    with open(a.js_out, "w", encoding="utf-8") as f:
        f.write("/* Generated by firmware/master/assets/gen_floorplan.py. Do not edit.\n"
                " * Outer walls: OpenStreetMap (ODbL). Interior: approximate, not the official plan. */\n")
        rooms = ",".join(f'{{id:"{rid}",kind:"{kind}",x:{x0},y:{y0},w:{round(x1 - x0, 2)},h:{round(y1 - y0, 2)}}}'
                         for rid, kind, x0, y0, x1, y1, side in ROOMS)
        f.write(f"const FLOORPLAN = {{view:{list(VIEW)},full:{list(VIEW_FULL)},here:{list(HERE)},rooms:[{rooms}],\n"
                f"svg:'{svg(P)}'}};\n")


if __name__ == "__main__":
    main()
