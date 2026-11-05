#!/usr/bin/env python3
import os as _os, sys as _sys; _sys.path.insert(0, _os.path.dirname(_os.path.abspath(__file__)))
"""Desktop frontend for the SAINTCON 2026 badge game (Orion Trail / Wayfarer).

Runs the stock firmware compiled into build/libwayfarer.so and shows the round
360x360 screen, the 9 WS2812 LEDs, and persists the 1 KB EEPROM between runs.

Keys: arrows or WASD = d-pad, Enter/Space/E = centre press, Z = A, X = B,
      F1 = toggle 4x speed, F5 = save EEPROM now, F12 = screenshot, Esc = quit.
Touch pad style: the screen works like a phone. Tap rows, buttons, event options or map hexes,
drag the map to pan, tap the HUD for the ship menu, hold the screen to fast forward text, hold and
drag in the asteroid field to aim and fire, in the nebula to steer and thrust, in the wreck to walk.
Right click = B, middle = centre, wheel = up/down. F11 fullscreen, V view (fit/fill/zoom), M round
mask, C overlay controls (auto/on/off), P pause; focus loss pauses. Prefs and board id live beside
the EEPROM save.
Type console commands (HELP, IDENT, HANDLE name, UNLOCK code) on stdin.
"""
import argparse
import ctypes
import os
import queue
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
LIB = os.path.join(ROOT, "build", "libwayfarer.so")

BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_C, BTN_B, BTN_A = (1 << i for i in range(7))


def load_core():
    if not os.path.exists(LIB):
        sys.exit(f"missing {LIB}, run: make -C {ROOT}")
    c = ctypes.CDLL(LIB)
    c.emu_init.argtypes = [ctypes.c_uint32]
    c.emu_step.argtypes = [ctypes.c_uint32, ctypes.c_uint8]
    c.emu_framebuffer.restype = ctypes.POINTER(ctypes.c_uint16)
    c.emu_leds.restype = ctypes.POINTER(ctypes.c_uint8)
    c.emu_backlight.restype = ctypes.c_uint8
    c.emu_eeprom.restype = ctypes.POINTER(ctypes.c_uint8)
    c.emu_eeprom_dirty.restype = ctypes.c_int
    c.emu_set_speed.argtypes = [ctypes.c_uint32]
    c.emu_fb_width.restype = ctypes.c_int
    c.emu_fb_height.restype = ctypes.c_int
    c.emu_led_count.restype = ctypes.c_int
    c.emu_eeprom_size.restype = ctypes.c_int
    c.emu_framebuffer_rgb888.argtypes = [ctypes.c_void_p, ctypes.c_int]
    c.platform_2026_usb_push_str.argtypes = [ctypes.c_char_p]
    c.platform_2026_tap.argtypes = [ctypes.c_int, ctypes.c_int]
    c.platform_2026_drag.argtypes = [ctypes.c_int, ctypes.c_int]
    c.platform_2026_tap_pending.restype = ctypes.c_int
    c.platform_2026_uart_push_str.argtypes = [ctypes.c_char_p]
    c.platform_2026_uart_pop_line.argtypes = [ctypes.c_char_p, ctypes.c_size_t]
    c.platform_2026_uart_pop_line.restype = ctypes.c_int
    c.mp_peer_count.restype = ctypes.c_int
    c.platform_2026_set_log_enabled.argtypes = [ctypes.c_bool]
    return c


def stdin_reader(q):
    for line in sys.stdin:
        q.put(line.rstrip("\r\n"))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--eeprom", default=os.path.join(ROOT, "save", "eeprom.bin"),
                    help="1024 byte EEPROM image to load and persist (default save/eeprom.bin)")
    ap.add_argument("--import", dest="import_file", default=None,
                    help="seed the EEPROM from this file (read only, never written back), e.g. a badge backup")
    ap.add_argument("--fresh", action="store_true", help="ignore the EEPROM file, boot a factory profile")
    ap.add_argument("--seed", type=lambda s: int(s, 0), default=0, help="RNG seed (0 = time based)")
    ap.add_argument("--speed", type=int, default=1, help="virtual time multiplier")
    ap.add_argument("--scale", type=int, default=2, help="window scale factor")
    ap.add_argument("--log", action="store_true", help="print the badge USB console to stderr")
    ap.add_argument("--join", default=None, metavar="CODE",
                    help="multiplayer: join room CODE on the relay (use NEW to create one)")
    ap.add_argument("--server", default="wss://wayfarer-relay.nrp-nautilus.io", help="relay URL for --join (ws://127.0.0.1:8766 for a local relay)")
    ap.add_argument("--name", default=None, help="display name for the relay lobby (default: badge handle)")
    args = ap.parse_args()

    import pygame  # imported late so --help works without a display

    core = load_core()
    w, h = core.emu_fb_width(), core.emu_fb_height()
    nled = core.emu_led_count()
    eesize = core.emu_eeprom_size()

    src = args.import_file if args.import_file else (None if args.fresh else args.eeprom)
    if src and os.path.exists(src):
        data = open(src, "rb").read()
        if len(data) == eesize:
            ctypes.memmove(core.emu_eeprom(), data, eesize)
            print(f"loaded EEPROM from {src}, saving to {args.eeprom}", file=sys.stderr)
        else:
            print(f"ignoring {src}: {len(data)} bytes, expected {eesize}", file=sys.stderr)
    if args.import_file and os.path.abspath(args.import_file) == os.path.abspath(args.eeprom):
        sys.exit("--import file must differ from the --eeprom save file")

    core.platform_2026_set_log_enabled(args.log)
    seed = args.seed or (int(time.time()) & 0xFFFFFFFF) or 1
    core.platform_2026_set_board_id.argtypes = [ctypes.c_uint64]
    bid_path = os.path.join(os.path.dirname(args.eeprom) or ".", "board-id.bin")
    try:
        bid = int.from_bytes(open(bid_path, "rb").read(8), "little")
    except Exception:  # noqa: BLE001
        bid = ((int(time.time() * 1000) << 16) ^ os.getpid()) & 0xFFFFFFFFFFFFFFFF or 1
        os.makedirs(os.path.dirname(bid_path) or ".", exist_ok=True)
        open(bid_path, "wb").write(bid.to_bytes(8, "little"))
    core.platform_2026_set_board_id(bid)
    core.emu_init(seed)
    core.emu_set_speed(max(1, args.speed))

    def save_eeprom():
        os.makedirs(os.path.dirname(args.eeprom) or ".", exist_ok=True)
        with open(args.eeprom, "wb") as f:
            f.write(ctypes.string_at(core.emu_eeprom(), eesize))

    link = None
    if args.join is not None:
        from link import Link
        code = "" if args.join.upper() == "NEW" else args.join
        ee = ctypes.addressof(core.emu_eeprom().contents)
        handle = ctypes.string_at(ee + 0x10, 15).split(b"\0")[0].decode(errors="replace")
        link = Link(args.server, code, args.name or handle or "Captain")
        link.start()
        for _ in range(100):
            if link.info or link.error:
                break
            time.sleep(0.05)
        if link.info:
            print(f"joined room {link.info['room']} seed {link.info['seed']} "
                  f"({link.info['peers']} other ship(s)); tell friends: --join {link.info['room']}", file=sys.stderr)
        else:
            sys.exit(f"could not join {args.server}: {link.error}")
    from link import pump
    line_buf = ctypes.create_string_buffer(512)

    pygame.init()
    S = args.scale
    led_h = 26
    base_w, base_h = w * S, h * S + led_h
    flags = pygame.RESIZABLE
    win = pygame.display.set_mode((base_w, base_h), flags)
    pygame.display.set_caption("Wayfarer (Orion Trail) emulator" + (f"  room {link.info['room']}" if link else ""))
    font = pygame.font.SysFont(None, 22)
    clock = pygame.time.Clock()
    rgb = ctypes.create_string_buffer(w * h * 3)
    core.platform_2026_pointer.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    core.platform_2026_scene.restype = ctypes.c_int

    # presentation state, like a phone: fit by default, F11 fullscreen, V view, M mask, C controls, P pause
    view_modes = ["fit", "fill", "zoom"]
    prefs_path = os.path.join(os.path.dirname(args.eeprom) or ".", "display.json")
    prefs = {"view": "fit", "mask": True, "controls": "auto", "fullscreen": False}
    try:
        import json
        prefs.update(json.load(open(prefs_path)))
    except Exception:  # noqa: BLE001
        pass

    def save_prefs():
        try:
            import json
            os.makedirs(os.path.dirname(prefs_path) or ".", exist_ok=True)
            json.dump(prefs, open(prefs_path, "w"))
        except Exception:  # noqa: BLE001
            pass

    def apply_fullscreen():
        nonlocal win
        if prefs["fullscreen"]:
            win = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
        else:
            win = pygame.display.set_mode((base_w, base_h), flags)

    if prefs["fullscreen"]:
        apply_fullscreen()

    def game_rect():
        """Where the 360x360 image sits in the window for the current view mode."""
        ww, wh = win.get_size()
        wh_game = wh - (led_h if not prefs["fullscreen"] else 0)
        if prefs["view"] == "fill":
            return pygame.Rect(0, 0, ww, wh_game)
        if prefs["view"] == "zoom":
            side = max(ww, wh_game)
            return pygame.Rect((ww - side) // 2, (wh_game - side) // 2, side, side)
        side = min(ww, wh_game)
        return pygame.Rect((ww - side) // 2, (wh_game - side) // 2, side, side)

    def to_game(pos, rect):
        gx = (pos[0] - rect.x) * w / max(1, rect.width)
        gy = (pos[1] - rect.y) * h / max(1, rect.height)
        return int(gx), int(gy)

    overlay_bits = 0      # bits held through the on-screen overlay controls
    taps = []             # (bit, release_time)
    touch = {"down": None, "last": None, "dragging": False, "sent_at": None, "held": False, "frac": [0.0, 0.0]}
    paused = False

    def overlay_controls(rect):
        """Translucent d-pad bottom left, A/B bottom right, scaled to the game rect."""
        u = max(14, rect.width // 14)
        cx, cy = rect.x + u * 2 + u // 2, rect.bottom - u * 2 - u // 2
        ctr = {
            "up": (pygame.Rect(cx - u // 2, cy - u - u // 2, u, u), BTN_UP, "^"),
            "down": (pygame.Rect(cx - u // 2, cy + u // 2, u, u), BTN_DOWN, "v"),
            "left": (pygame.Rect(cx - u - u // 2, cy - u // 2, u, u), BTN_LEFT, "<"),
            "right": (pygame.Rect(cx + u // 2, cy - u // 2, u, u), BTN_RIGHT, ">"),
            "c": (pygame.Rect(cx - u // 2, cy - u // 2, u, u), BTN_C, "C"),
            "a": (pygame.Rect(rect.right - u * 2, rect.bottom - u * 3, int(u * 1.4), int(u * 1.4)), BTN_A, "A"),
            "b": (pygame.Rect(rect.right - u * 4, rect.bottom - u * 2, int(u * 1.2), int(u * 1.2)), BTN_B, "B"),
        }
        return ctr

    def controls_visible(scene):
        if prefs["controls"] == "on":
            return True
        if prefs["controls"] == "off":
            return False
        return scene in (1, 2, 3, 4, 5)   # flight and challenge scenes only

    def draw_overlay(ctr, active):
        layer = pygame.Surface(win.get_size(), pygame.SRCALPHA)
        for name, (r, bit, label) in ctr.items():
            on = active & bit
            col = (90, 200, 120, 170) if on else (120, 120, 130, 110)
            if name in ("a", "b"):
                pygame.draw.circle(layer, col, r.center, r.width // 2)
            else:
                pygame.draw.rect(layer, col, r, border_radius=6)
            t = font.render(label, True, (240, 240, 245))
            layer.blit(t, t.get_rect(center=r.center))
        win.blit(layer, (0, 0))

    keymap = {
        pygame.K_UP: BTN_UP, pygame.K_w: BTN_UP,
        pygame.K_DOWN: BTN_DOWN, pygame.K_s: BTN_DOWN,
        pygame.K_LEFT: BTN_LEFT, pygame.K_a: BTN_LEFT,
        pygame.K_RIGHT: BTN_RIGHT, pygame.K_d: BTN_RIGHT,
        pygame.K_RETURN: BTN_A, pygame.K_z: BTN_A,
        pygame.K_SPACE: BTN_C, pygame.K_e: BTN_C,
        pygame.K_x: BTN_B,
    }
    buttons = 0
    fast = False
    q = queue.Queue()
    threading.Thread(target=stdin_reader, args=(q,), daemon=True).start()

    running = True
    shot = 0
    ctr = {}
    while running:
        dt = clock.tick(60)
        rect = game_rect()
        scene = core.platform_2026_scene()
        show_ctr = controls_visible(scene)
        ctr = overlay_controls(rect) if show_ctr else {}
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                running = False
            elif ev.type == pygame.VIDEORESIZE and not prefs["fullscreen"]:
                win = pygame.display.set_mode(ev.size, flags)
            elif ev.type == pygame.WINDOWFOCUSLOST:
                paused = True
                core.platform_2026_pointer(0, 0, 0)
                overlay_bits = 0
            elif ev.type == pygame.WINDOWFOCUSGAINED:
                paused = False
            elif ev.type == pygame.MOUSEBUTTONDOWN:
                hit = None
                for name, (r, bit, label) in ctr.items():
                    if r.collidepoint(ev.pos):
                        hit = bit
                if ev.button in (4, 5):
                    taps.append((BTN_UP if ev.button == 4 else BTN_DOWN, time.time() + 0.08))
                elif hit:
                    overlay_bits |= hit
                elif rect.collidepoint(ev.pos):
                    if ev.button == 1:
                        touch["down"] = touch["last"] = ev.pos
                        touch["dragging"] = False
                        touch["frac"] = [0.0, 0.0]
                        gx, gy = to_game(ev.pos, rect)
                        core.platform_2026_pointer(gx, gy, 1)
                        touch["held"] = True
                    elif ev.button == 3:
                        taps.append((BTN_B, time.time() + 0.08))
                    elif ev.button == 2:
                        taps.append((BTN_C, time.time() + 0.08))
            elif ev.type == pygame.MOUSEBUTTONUP:
                overlay_bits = 0
                if ev.button == 1 and touch["down"] is not None:
                    if not touch["dragging"]:
                        gx, gy = to_game(touch["down"], rect)
                        core.platform_2026_tap(gx, gy)
                        touch["sent_at"] = time.time()
                    touch["down"] = None
                if touch["held"]:
                    core.platform_2026_pointer(0, 0, 0)
                    touch["held"] = False
            elif ev.type == pygame.MOUSEMOTION:
                if ev.buttons[0] and ctr:
                    hit = 0
                    for name, (r, bit, label) in ctr.items():
                        if r.collidepoint(ev.pos) and name not in ("a", "b"):
                            hit = bit
                    if hit and overlay_bits & (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_C):
                        overlay_bits = hit
                if ev.buttons[0] and touch["down"] is not None:
                    gx, gy = to_game(ev.pos, rect)
                    core.platform_2026_pointer(gx, gy, 1)
                    sx = w / max(1, rect.width)
                    dx, dy = (ev.pos[0] - touch["last"][0]) * sx, (ev.pos[1] - touch["last"][1]) * sx
                    if not touch["dragging"]:
                        tdx, tdy = ev.pos[0] - touch["down"][0], ev.pos[1] - touch["down"][1]
                        if (tdx * tdx + tdy * tdy) * sx * sx > 36:
                            touch["dragging"] = True
                            dx, dy = tdx * sx, tdy * sx
                    if touch["dragging"]:
                        # keep fractional movement so slow drags still move the map
                        touch["frac"][0] += dx
                        touch["frac"][1] += dy
                        ix, iy = int(touch["frac"][0]), int(touch["frac"][1])
                        if ix or iy:
                            core.platform_2026_drag(ix, iy)
                            touch["frac"][0] -= ix
                            touch["frac"][1] -= iy
                        touch["last"] = ev.pos
            elif ev.type in (pygame.KEYDOWN, pygame.KEYUP):
                down = ev.type == pygame.KEYDOWN
                if ev.key in keymap:
                    if down:
                        buttons |= keymap[ev.key]
                    else:
                        buttons &= ~keymap[ev.key]
                elif down and ev.key == pygame.K_ESCAPE:
                    if prefs["fullscreen"]:
                        prefs["fullscreen"] = False
                        apply_fullscreen()
                        save_prefs()
                    else:
                        running = False
                elif down and ev.key == pygame.K_F11:
                    prefs["fullscreen"] = not prefs["fullscreen"]
                    apply_fullscreen()
                    save_prefs()
                elif down and ev.key == pygame.K_v:
                    prefs["view"] = view_modes[(view_modes.index(prefs["view"]) + 1) % len(view_modes)]
                    save_prefs()
                elif down and ev.key == pygame.K_m:
                    prefs["mask"] = not prefs["mask"]
                    save_prefs()
                elif down and ev.key == pygame.K_c:
                    order = ["auto", "on", "off"]
                    prefs["controls"] = order[(order.index(prefs["controls"]) + 1) % 3]
                    save_prefs()
                elif down and ev.key == pygame.K_p:
                    paused = not paused
                elif down and ev.key == pygame.K_F1:
                    fast = not fast
                    core.emu_set_speed(4 if fast else max(1, args.speed))
                elif down and ev.key == pygame.K_F5:
                    save_eeprom()
                    print("EEPROM saved", file=sys.stderr)
                elif down and ev.key == pygame.K_F12:
                    shot += 1
                    path = os.path.join(ROOT, "save", f"shot{shot:03d}.png")
                    os.makedirs(os.path.dirname(path), exist_ok=True)
                    pygame.image.save(win, path)
                    print(f"saved {path}", file=sys.stderr)
        while not q.empty():
            line = q.get()
            core.platform_2026_usb_push_str((line + "\n").encode())

        now = time.time()
        # a tap no screen claimed within 150 ms becomes a plain A press (popups, title, shooter)
        if touch["sent_at"] is not None and now - touch["sent_at"] > 0.15:
            if core.platform_2026_tap_pending():
                core.platform_2026_tap_clear()
                taps.append((BTN_A, now + 0.08))
            touch["sent_at"] = None
        tap_bits = 0
        taps = [(b, t) for b, t in taps if t > now]
        for b, t in taps:
            tap_bits |= b
        all_bits = buttons | overlay_bits | tap_bits
        if not paused:
            pump(core, link, ctypes, line_buf)
            core.emu_step(min(dt, 50), all_bits)
            pump(core, link, ctypes, line_buf)

        core.emu_framebuffer_rgb888(rgb, 0)
        surf = pygame.image.frombuffer(rgb.raw, (w, h), "RGB")
        win.fill((12, 12, 16))
        frame = pygame.transform.smoothscale(surf, (rect.width, rect.height)) if rect.size != (w, h) else surf
        if prefs["mask"] and prefs["view"] != "fill":
            m = pygame.Surface((rect.width, rect.height), pygame.SRCALPHA)
            m.fill((12, 12, 16, 255))
            pygame.draw.circle(m, (0, 0, 0, 0), (rect.width // 2, rect.height // 2), min(rect.width, rect.height) // 2)
            frame = frame.copy()
            frame.blit(m, (0, 0))
        bl = core.emu_backlight()
        if bl < 255:
            frame = frame.copy()
            frame.fill((0, 0, 0, 255 - bl), special_flags=pygame.BLEND_RGBA_SUB)
        win.blit(frame, rect.topleft)

        if not prefs["fullscreen"]:
            leds = core.emu_leds()
            ww, wh = win.get_size()
            r = 7
            for i in range(nled):
                col = (leds[i * 3], leds[i * 3 + 1], leds[i * 3 + 2])
                cx = (i + 1) * ww // (nled + 1)
                cy = wh - led_h // 2
                pygame.draw.circle(win, (40, 40, 44), (cx, cy), r + 2)
                pygame.draw.circle(win, col, (cx, cy), r)
        if ctr:
            draw_overlay(ctr, all_bits)
        if paused:
            t = font.render("PAUSED  (P or click to resume)", True, (240, 240, 245))
            win.blit(t, t.get_rect(center=(win.get_width() // 2, win.get_height() // 2)))
        pygame.display.flip()

        if core.emu_eeprom_dirty():
            save_eeprom()

    save_eeprom()
    pygame.quit()


if __name__ == "__main__":
    main()
