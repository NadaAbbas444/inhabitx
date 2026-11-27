#!/usr/bin/env python3
"""
Generate the floor-1 plan of the Zentraltrakt for the master screen AND the web page.

SOURCE: traced by eye from the public AAU campus plan (https://campusplan.aau.at,
floor 1), read off a screenshot at 5.8 px per metre (map scale line: 10 m = 98 CSS px).
Room codes, names and capacities as shown there. Accuracy roughly +-0.5 m; small
rooms and doors are simplified. North is up.

Writes:
  ui/floorplan.c            plan images (RGB565) + room rectangles in pixels
  ../../../web/floorplan.js the same plan as SVG + rooms in metres

  python3 gen_floorplan.py --size 1024x600
"""
import argparse

from PIL import Image, ImageDraw

PX_PER_M = 5.8           # screenshot scale used for tracing
ORIGIN = (100, 315)      # screenshot pixel that becomes (0 m, 0 m)

# ---- traced rectangles, in screenshot pixels: (x0, y0, x1, y1) -------------
# kind: live = has an InhabitX sensor, room, hall (open area/corridor),
#       stairs, wc, aula (open hall of the L building, context only)
TRACE = [
    # corridors and halls (open)
    ("Z.1.D",  "hall",   268, 320, 292, 445),
    ("Z.1.A",  "hall",   150, 465, 225, 502),
    ("Z.1.B",  "hall",   225, 465, 300, 502),
    ("Z.1.F",  "hall",   292, 452, 430, 540),
    ("Z.1.J",  "hall",   430, 400, 612, 500),
    ("Z.1.L",  "hall",   650, 318, 668, 452),
    ("Z.1.N",  "hall",   612, 428, 700, 455),
    ("Z.1.0",  "hall",   700, 418, 795, 505),
    ("Aula",   "aula",   312, 540, 655, 612),
    # west block: offices around a ring corridor
    ("Z.1.03", "room",   112, 437, 150, 455),
    ("Z.1.04", "room",   112, 455, 150, 478),
    ("Z.1.05", "room",   112, 478, 150, 500),
    ("Z.1.05a", "room",  112, 500, 150, 520),
    ("Z.1.24", "room",   112, 520, 150, 535),
    ("Z.1.08a", "room",  112, 535, 150, 555),
    ("Z.1.23a", "room",  112, 555, 150, 575),
    ("Z.1.23b", "room",  112, 575, 150, 600),
    ("Z.1.02", "room",   150, 437, 172, 465),
    ("Z.1.06a", "room",  172, 437, 193, 465),
    ("Z.1.06b", "room",  193, 437, 213, 465),
    ("Z.1.07a", "room",  213, 437, 234, 465),
    ("Z.1.07b", "room",  234, 437, 255, 465),
    ("Stairs", "stairs", 255, 425, 282, 465),
    ("Z.1.01", "room",   178, 470, 223, 488),
    ("Z.1.08", "live",   162, 502, 225, 600),
    ("Z.1.09", "live",   225, 502, 293, 594),
    # lecture halls north of the hall
    ("Z.1.10", "live",   290, 347, 368, 458),
    ("Z.1.11", "live",   368, 365, 418, 452),
    ("Z.1.11a", "room",  368, 345, 390, 365),
    ("Z.1.11b", "room",  390, 345, 412, 365),
    ("Stairs", "stairs", 420, 362, 440, 400),
    ("WC",     "wc",     470, 328, 520, 395),
    # east: HS 1, Oman-Saal, offices
    ("Z.1.15", "room",   522, 345, 565, 432),
    ("Z.1.15a", "room",  540, 325, 565, 345),
    ("Z.1.15b", "room",  518, 325, 540, 345),
    ("Z.1.17", "room",   565, 330, 650, 428),
    ("Z.1.17a", "room",  570, 318, 640, 330),
    ("Z.1.29", "room",   668, 370, 748, 415),
    ("Z.1.30", "room",   748, 372, 772, 400),
    ("Z.1.2b1", "room",  612, 460, 630, 478),
    ("Stairs", "stairs", 655, 460, 680, 500),
    ("WC",     "wc",     680, 455, 720, 490),
    ("Z.1.34", "room",   680, 490, 700, 512),
    ("Z.1.35b", "room",  680, 512, 705, 535),
    ("Z.1.35c", "room",  705, 512, 728, 535),
    ("Z.1.36", "room",   728, 512, 750, 535),
    ("Z.1.37", "room",   750, 512, 770, 535),
    ("Z.1.38", "room",   770, 512, 792, 535),
]
NAMES = {"Z.1.10": "HS 4", "Z.1.11": "HS 3", "Z.1.17": "HS 1", "Z.1.29": "Oman-Saal"}
HERE_PX = (300, 485)                       # master screen: where the west block meets the hall
VIEW_PX = (105, 335, 440, 612)             # overview card: west block + HS 3/4
FULL_PX = (105, 312, 800, 615)             # whole traced area


def m(px, py):
    return round((px - ORIGIN[0]) / PX_PER_M, 2), round((py - ORIGIN[1]) / PX_PER_M, 2)


def view_m(v):
    (x, y) = m(v[0], v[1])
    return (x, y, round((v[2] - v[0]) / PX_PER_M, 2), round((v[3] - v[1]) / PX_PER_M, 2))


ROOMS = []
for rid, kind, x0, y0, x1, y1 in TRACE:
    (a, b), (c, d) = m(x0, y0), m(x1, y1)
    ROOMS.append((rid, kind, a, b, c, d))
HERE = m(*HERE_PX)
VIEW = view_m(VIEW_PX)
VIEW_FULL = view_m(FULL_PX)

# colours (dark theme, see ui_internal.h)
BG = (18, 26, 44)
HALL = (27, 38, 64)
AULA = (22, 31, 52)
ROOM = (22, 32, 56)
WALL = (110, 130, 172)
STAIR = (70, 88, 125)


def primitives():
    """('rect'|'line', coords, fill, stroke, width_m) in metres."""
    P = []
    for rid, kind, x0, y0, x1, y1 in ROOMS:
        if kind in ("hall", "aula"):
            P.append(("rect", (x0, y0, x1, y1), HALL if kind == "hall" else AULA, None, 0))
    for rid, kind, x0, y0, x1, y1 in ROOMS:
        if kind in ("hall", "aula"):
            continue
        P.append(("rect", (x0, y0, x1, y1), ROOM, WALL, 0.3))
        if kind == "stairs":
            vertical = (y1 - y0) > (x1 - x0)
            n = int(((y1 - y0) if vertical else (x1 - x0)) / 0.45)
            for i in range(1, n):
                if vertical:
                    y = y0 + i * (y1 - y0) / n
                    P.append(("line", [(x0 + .4, y), (x1 - .4, y)], None, STAIR, 0.1))
                else:
                    x = x0 + i * (x1 - x0) / n
                    P.append(("line", [(x, y0 + .4), (x, y1 - .4)], None, STAIR, 0.1))
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
        if kind == "rect":
            (x0, y0), (x1, y1) = px(pts[:2]), px(pts[2:])
            d.rectangle([x0, y0, x1, y1], fill=fill, outline=stroke, width=lw if stroke else 0)
        elif kind == "line":
            d.line([px(p) for p in pts], fill=stroke, width=lw)
    return img.resize((W, H), Image.LANCZOS), s, ox, oy


def svg(P):
    def c(rgb): return "#%02x%02x%02x" % rgb
    out = []
    for kind, pts, fill, stroke, w in P:
        st = f' fill="{c(fill) if fill else "none"}"' + (f' stroke="{c(stroke)}" stroke-width="{w}"' if stroke else "")
        if kind == "rect":
            x0, y0, x1, y1 = pts
            out.append(f'<rect x="{x0}" y="{y0}" width="{round(x1 - x0, 2)}" height="{round(y1 - y0, 2)}"{st}/>')
        elif kind == "line":
            out.append(f'<polyline points="{" ".join(f"{round(x, 2)},{round(y, 2)}" for x, y in pts)}"{st}/>')
    return "".join(out)


KINDS = {"live": "UI_PLAN_LIVE", "room": "UI_PLAN_ROOM", "wc": "UI_PLAN_LABEL",
         "stairs": "UI_PLAN_LABEL", "hall": "UI_PLAN_HALL", "aula": "UI_PLAN_LABEL"}


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
    for rid, kind, x0, y0, x1, y1 in ROOMS:
        X0, Y0, X1, Y1 = max(0, tx(x0)), max(0, ty(y0)), min(W, tx(x1)), min(H, ty(y1))
        if X1 - X0 < 6 or Y1 - Y0 < 6:
            continue
        f.write(f'    {{ "{rid}", "{NAMES.get(rid, rid)}", {KINDS[kind]}, {X0}, {Y0}, {X1 - X0}, {Y1 - Y0} }},\n')
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
                " * Traced from the public AAU campus plan (campusplan.aau.at), floor 1. */\n")
        f.write('#include "ui_internal.h"\n\n')
        f.write(f"#if UI_HOR_RES == {sw} && UI_VER_RES == {sh}\n\n")
        n1 = emit_c(f, P, VIEW, W, H, "floorplan", "ui_plan")
        FW = sw - 40
        FH = min(sh - 72 - 80, int(FW * VIEW_FULL[3] / VIEW_FULL[2]))
        n2 = emit_c(f, P, VIEW_FULL, FW, FH, "floorplan_full", "ui_plan_full")
        f.write("#endif\n")

    with open(a.js_out, "w", encoding="utf-8") as f:
        f.write("/* Generated by firmware/master/assets/gen_floorplan.py. Do not edit.\n"
                " * Traced from the public AAU campus plan (campusplan.aau.at), floor 1. */\n")
        rooms = ",".join(f'{{id:"{rid}",name:"{NAMES.get(rid, rid)}",kind:"{kind}",x:{x0},y:{y0},'
                         f'w:{round(x1 - x0, 2)},h:{round(y1 - y0, 2)}}}' for rid, kind, x0, y0, x1, y1 in ROOMS)
        f.write(f"const FLOORPLAN = {{view:{list(VIEW)},full:{list(VIEW_FULL)},here:{list(HERE)},rooms:[{rooms}],\n"
                f"svg:'{svg(P)}'}};\n")
    print(f"card {W}x{H}px ({n1} rooms), full {FW}x{FH}px ({n2} rooms); traced area "
          f"{VIEW_FULL[2]} x {VIEW_FULL[3]} m")


if __name__ == "__main__":
    main()
