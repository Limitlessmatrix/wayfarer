#!/usr/bin/env python3
"""Procedural pixel art for the frontier expansion -> upstream/firmware/include/img/frontier_img.h
Also writes PNG previews (4x) to save/sprites/. Black (0,0,0) is transparent in frame_draw_image,
so every opaque pixel uses a palette colour that is not pure black."""
import os, math, random
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_H = os.path.join(ROOT, "upstream/firmware/include/img/frontier_img.h")
PREV = os.path.join(ROOT, "save/sprites")
os.makedirs(PREV, exist_ok=True)

K = (10, 10, 12)        # near black, opaque
D = (58, 62, 72)        # dark steel
S = (108, 114, 126)     # steel
L = (158, 166, 180)     # light steel
H = (214, 220, 230)     # highlight
R = (146, 78, 40)       # rust
O = (242, 150, 48)      # ember
E = (232, 44, 44)       # red light
W = (96, 204, 224)      # window
B = (42, 36, 34)        # burnt
P = (200, 90, 230)      # relic purple
P2 = (240, 190, 255)
Y = (236, 176, 60)      # brass parts
C = (90, 200, 220)      # gas cyan
BR = (150, 110, 70)     # ore brown
CR = (184, 171, 138)    # crate tan
AL = (200, 204, 215)    # alloy

def rgb565(c):
    r, g, b = c
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def new(w, h):
    return Image.new("RGB", (w, h), (0, 0, 0))

def wreck(scrap=False):
    """48x40 derelict freighter: broken hull, bow with a dead window, stern with cold engines."""
    im = new(48, 40)
    d = ImageDraw.Draw(im)
    rnd = random.Random(7 if not scrap else 11)
    body = D if not scrap else B
    plate = S if not scrap else D
    edge = L if not scrap else S
    # bow section (upper right), a tilted capsule
    bow = [(26, 8), (44, 4), (46, 10), (40, 20), (28, 22), (24, 16)]
    d.polygon(bow, fill=body, outline=edge)
    d.line([(29, 11), (41, 7)], fill=plate)
    d.line([(30, 17), (38, 15)], fill=plate)
    d.rectangle([40, 7, 43, 10], fill=W if not scrap else D, outline=edge)
    # stern section (lower left), torn free
    stern = [(2, 24), (18, 20), (22, 26), (20, 34), (6, 36), (1, 30)]
    d.polygon(stern, fill=body, outline=edge)
    d.line([(5, 27), (17, 24)], fill=plate)
    d.line([(6, 32), (16, 30)], fill=plate)
    # engine bells
    d.rectangle([1, 26, 4, 29], fill=K, outline=edge)
    d.rectangle([2, 31, 5, 34], fill=K, outline=edge)
    if not scrap:
        d.point((2, 27), fill=O)
    # torn midsection: exposed ribs between the halves
    for i, (x0, y0) in enumerate([(19, 19), (21, 22), (23, 25), (25, 19)]):
        d.line([(x0, y0), (x0 + 3, y0 - 3)], fill=R if not scrap else B)
    d.line([(22, 20), (24, 24)], fill=R if not scrap else B)
    # jagged tear edges
    for (x, y) in [(23, 22), (24, 19), (21, 25), (26, 21)]:
        d.point((x, y), fill=edge)
    # red beacon on the bow, only on an intact wreck
    if not scrap:
        d.point((36, 5), fill=E)
        d.point((36, 4), fill=(120, 20, 20))
    # debris field
    for _ in range(14 if not scrap else 20):
        x, y = rnd.randint(0, 47), rnd.randint(0, 39)
        if im.getpixel((x, y)) == (0, 0, 0):
            im.putpixel((x, y), rnd.choice([D, S, B, R]) if not scrap else rnd.choice([B, D]))
    return im

def icon_crate():
    im = new(16, 16); d = ImageDraw.Draw(im)
    d.rectangle([2, 4, 13, 14], fill=CR, outline=(120, 100, 70))
    d.line([(2, 9), (13, 9)], fill=(120, 100, 70)); d.line([(7, 4), (7, 14)], fill=(120, 100, 70))
    d.rectangle([3, 2, 12, 4], fill=(150, 130, 95))
    return im

def icon_ore():
    im = new(16, 16); d = ImageDraw.Draw(im)
    d.polygon([(3, 12), (5, 5), (9, 3), (13, 7), (12, 13), (6, 14)], fill=BR, outline=(90, 60, 40))
    d.point((8, 7), fill=(210, 170, 120)); d.point((9, 9), fill=(210, 170, 120)); d.point((6, 10), fill=(110, 80, 50))
    return im

def icon_gas():
    im = new(16, 16); d = ImageDraw.Draw(im)
    d.rectangle([5, 3, 10, 14], fill=C, outline=(40, 120, 140))
    d.rectangle([6, 1, 9, 3], fill=S, outline=D)
    d.line([(7, 5), (7, 11)], fill=(180, 240, 250))
    return im

def icon_alloy():
    im = new(16, 16); d = ImageDraw.Draw(im)
    d.polygon([(2, 12), (5, 6), (14, 6), (11, 12)], fill=AL, outline=(120, 124, 136))
    d.line([(5, 6), (11, 6)], fill=H)
    d.line([(3, 11), (10, 11)], fill=(150, 154, 166))
    return im

def icon_parts():
    im = new(16, 16); d = ImageDraw.Draw(im)
    d.ellipse([3, 3, 12, 12], outline=Y)
    d.ellipse([6, 6, 9, 9], fill=K, outline=Y)
    for (x, y) in [(7, 1), (7, 14), (1, 7), (14, 7), (2, 2), (13, 13), (2, 13), (13, 2)]:
        d.point((x, y), fill=Y)
    return im

def icon_relic():
    im = new(16, 16); d = ImageDraw.Draw(im)
    d.polygon([(7, 1), (13, 7), (7, 14), (2, 7)], fill=P, outline=(120, 40, 150))
    d.line([(7, 3), (7, 11)], fill=P2); d.point((5, 7), fill=P2)
    return im

def icon_shipyard():
    """12x12 wrench badge drawn over outposts that can upgrade the ship."""
    im = new(12, 12); d = ImageDraw.Draw(im)
    d.ellipse([0, 0, 11, 11], fill=(30, 60, 50), outline=(80, 220, 120))
    d.line([(3, 8), (8, 3)], fill=H, width=2)
    d.rectangle([7, 2, 10, 5], fill=H)
    d.point((9, 3), fill=(30, 60, 50))
    return im

def icon_he3():
    """Helium-3 ice: pale green crystal shard."""
    im = new(16, 16); d = ImageDraw.Draw(im)
    g = (120, 230, 150); g2 = (200, 255, 215); g3 = (60, 150, 90)
    d.polygon([(7, 1), (12, 6), (9, 14), (4, 12), (3, 6)], fill=g, outline=g3)
    d.line([(7, 3), (6, 11)], fill=g2); d.point((9, 6), fill=g2)
    return im

def icon_dark():
    """Dark matter: violet swirl in a near black core."""
    im = new(16, 16); d = ImageDraw.Draw(im)
    v = (120, 40, 170); v2 = (190, 120, 240); core = (24, 10, 34)
    d.ellipse([2, 2, 13, 13], fill=core, outline=v)
    d.arc([3, 3, 12, 12], 200, 20, fill=v2)
    d.arc([5, 5, 10, 10], 20, 200, fill=v2)
    d.point((8, 8), fill=(255, 230, 255))
    return im

def icon_depot():
    """12x12 flame badge over outposts that are exotic fuel depots."""
    im = new(12, 12); d = ImageDraw.Draw(im)
    d.ellipse([0, 0, 11, 11], fill=(60, 30, 20), outline=(240, 150, 48))
    d.polygon([(6, 1), (9, 5), (8, 9), (6, 10), (4, 9), (3, 5)], fill=(242, 150, 48))
    d.polygon([(6, 5), (7, 7), (6, 9), (5, 7)], fill=(255, 240, 120))
    return im

SPRITES = {
    "icon_shipyard": icon_shipyard(),
    "icon_depot": icon_depot(),
    "icon_he3": icon_he3(),
    "icon_dark": icon_dark(),
    "wreck": wreck(False),
    "scrap": wreck(True),
    "icon_supplies": icon_crate(),
    "icon_ore": icon_ore(),
    "icon_gas": icon_gas(),
    "icon_alloy": icon_alloy(),
    "icon_parts": icon_parts(),
    "icon_relic": icon_relic(),
}

hdr = ["// Generated by tools/sprites.py, do not edit. RGB565, 0x0000 = transparent.", "#pragma once", "#include <stdint.h>", ""]
src = ["// Generated by tools/sprites.py, do not edit.", '#include "img/frontier_img.h"', ""]
for name, im in SPRITES.items():
    w, h = im.size
    im.resize((w * 4, h * 4), Image.NEAREST).save(os.path.join(PREV, name + ".png"))
    px = [rgb565(im.getpixel((x, y))) for y in range(h) for x in range(w)]
    hdr.append(f"#define {name.upper()}_W {w}")
    hdr.append(f"#define {name.upper()}_H {h}")
    hdr.append(f"extern const uint16_t {name}_image_data[{w * h}];")
    src.append(f"const uint16_t {name}_image_data[{w * h}] = {{")
    for i in range(0, len(px), w):
        src.append("    " + ", ".join(f"0x{v:04x}" for v in px[i:i + w]) + ",")
    src.append("};")
    src.append("")
hdr.append("extern const uint16_t* const good_icons[8];")
src.append("const uint16_t* const good_icons[8] = {icon_supplies_image_data, icon_ore_image_data, icon_gas_image_data, icon_alloy_image_data, icon_parts_image_data, icon_relic_image_data, icon_he3_image_data, icon_dark_image_data};")
OUT_C = os.path.join(ROOT, "upstream/firmware/frontier_img.c")
open(OUT_H, "w").write("\n".join(hdr) + "\n")
open(OUT_C, "w").write("\n".join(src) + "\n")
print("wrote", OUT_H, OUT_C)
