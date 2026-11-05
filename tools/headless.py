#!/usr/bin/env python3
"""Headless driver for tests: runs the core with a timed input script, optional relay link.

Script lines: wait <ms> | press <btn> [n] | hold <btn> <ms> | tap <x> <y> | drag <dx> <dy> | usb <text> | uart <text> | shot <name> | peers
Buttons: up down left right c a b (combine with +).
"""
import argparse
import ctypes
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
import wayfarer  # noqa: E402
from link import Link, pump  # noqa: E402

BTN = {"up": 1, "down": 2, "left": 4, "right": 8, "c": 16, "center": 16, "b": 32, "a": 64}


def mask(tok):
    m = 0
    for p in tok.split("+"):
        m |= BTN[p.lower()]
    return m


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--script", required=True)
    ap.add_argument("--out", default="save/headless")
    ap.add_argument("--seed", type=lambda s: int(s, 0), default=0x20265343)
    ap.add_argument("--join", default=None)
    ap.add_argument("--server", default="ws://127.0.0.1:8766")
    ap.add_argument("--name", default="Tester")
    ap.add_argument("--log", action="store_true")
    ap.add_argument("--realtime", action="store_true", help="pace steps to wall clock (needed with a relay)")
    args = ap.parse_args()

    core = wayfarer.load_core()
    core.platform_2026_set_log_enabled(args.log)
    core.emu_init(args.seed)
    link = None
    if args.join is not None:
        link = Link(args.server, "" if args.join.upper() == "NEW" else args.join, args.name)
        link.start()
        for _ in range(100):
            if link.info or link.error:
                break
            time.sleep(0.05)
        if not link.info:
            sys.exit(f"join failed: {link.error}")
        print(f"[{args.name}] joined {link.info}", flush=True)
    line_buf = ctypes.create_string_buffer(512)
    w, h = core.emu_fb_width(), core.emu_fb_height()
    rgb = ctypes.create_string_buffer(w * h * 3)
    os.makedirs(args.out, exist_ok=True)

    def step(ms, buttons):
        remaining = ms
        while remaining > 0:
            d = min(16, remaining)
            pump(core, link, ctypes, line_buf)
            core.emu_step(d, buttons)
            pump(core, link, ctypes, line_buf)
            remaining -= d
            if args.realtime:
                time.sleep(d / 1000.0)

    for raw in open(args.script):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        cmd = parts[0].lower()
        if cmd in ("wait", "run"):
            step(int(parts[1]), 0)
        elif cmd == "press":
            n = int(parts[2]) if len(parts) > 2 else 1
            for _ in range(n):
                step(64, mask(parts[1]))
                step(64, 0)
        elif cmd == "hold":
            step(int(parts[2]), mask(parts[1]))
            step(32, 0)
        elif cmd == "tap":
            core.platform_2026_tap.argtypes = [ctypes.c_int, ctypes.c_int]
            core.platform_2026_tap(int(parts[1]), int(parts[2]))
            step(160, 0)
            if core.platform_2026_tap_pending():
                core.platform_2026_tap_clear()
                step(64, BTN["a"])
                step(64, 0)
        elif cmd == "drag":
            core.platform_2026_drag.argtypes = [ctypes.c_int, ctypes.c_int]
            core.platform_2026_drag(int(parts[1]), int(parts[2]))
        elif cmd == "uart":
            core.platform_2026_uart_push_str((" ".join(parts[1:]) + "\n").encode())
        elif cmd == "usb":
            core.platform_2026_usb_push_str((" ".join(parts[1:]) + "\n").encode())
        elif cmd == "shot":
            import pygame
            core.emu_framebuffer_rgb888(rgb, 0)
            surf = pygame.image.frombuffer(rgb.raw, (w, h), "RGB").copy()
            # the badge panel is round: black out everything outside the circle so clipping shows
            rmask = pygame.Surface((w, h), pygame.SRCALPHA)
            rmask.fill((0, 0, 0, 255))
            pygame.draw.circle(rmask, (0, 0, 0, 0), (w // 2, h // 2), w // 2)
            surf.blit(rmask, (0, 0))
            path = os.path.join(args.out, parts[1] + ".png")
            pygame.image.save(surf, path)
            print(f"[{args.name}] shot {path}", flush=True)
        elif cmd == "peers":
            print(f"[{args.name}] peers={core.mp_peer_count()}", flush=True)
        else:
            sys.exit(f"bad script line: {line}")
    print(f"[{args.name}] done, peers={core.mp_peer_count()}", flush=True)
    sys.stdout.flush()
    os._exit(0)


if __name__ == "__main__":
    main()
