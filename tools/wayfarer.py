#!/usr/bin/env python3
import os as _os, sys as _sys; _sys.path.insert(0, _os.path.dirname(_os.path.abspath(__file__)))
"""Desktop frontend for the SAINTCON 2026 badge game (Orion Trail / Wayfarer).

Runs the stock firmware compiled into build/libwayfarer.so and shows the round
360x360 screen, the 9 WS2812 LEDs, and persists the 1 KB EEPROM between runs.

Keys: arrows or WASD = d-pad, Enter/Space/E = centre press, Z = A, X = B,
      F1 = toggle 4x speed, F5 = save EEPROM now, F12 = screenshot, Esc = quit.
Touch pad style: the round screen works like a phone. Tap a menu row, a market button, an event
option or a hex on the map to act on it, drag the map to pan, tap the HUD for the ship menu.
Right click = B (back), middle click = centre, wheel = up/down. The side panel still works.
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
    core.platform_2026_set_board_id((int(time.time() * 1000) << 16) ^ os.getpid())
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
    led_h = 40 * S // 2 + 20
    panel_w = 130 * S
    win = pygame.display.set_mode((w * S + panel_w, h * S + led_h))
    font = pygame.font.SysFont(None, 11 * S)

    # on-screen controls: name -> (rect, bit, label)
    px0 = w * S + 10 * S
    cx, cy = px0 + 55 * S, 95 * S
    k = 22 * S
    controls = {
        "up":    (pygame.Rect(cx - k // 2, cy - k - k // 2, k, k), BTN_UP, "^"),
        "down":  (pygame.Rect(cx - k // 2, cy + k // 2, k, k), BTN_DOWN, "v"),
        "left":  (pygame.Rect(cx - k - k // 2, cy - k // 2, k, k), BTN_LEFT, "<"),
        "right": (pygame.Rect(cx + k // 2, cy - k // 2, k, k), BTN_RIGHT, ">"),
        "c":     (pygame.Rect(cx - k // 2, cy - k // 2, k, k), BTN_C, "C"),
        "a":     (pygame.Rect(px0 + 70 * S, 190 * S, 36 * S, 36 * S), BTN_A, "A"),
        "b":     (pygame.Rect(px0 + 18 * S, 215 * S, 36 * S, 36 * S), BTN_B, "B"),
    }
    screen_rect = pygame.Rect(0, 0, w * S, h * S)
    mouse_bits = 0
    taps = []   # (bit, release_time)
    touch = {"down": None, "last": None, "dragging": False, "sent_at": None}

    def draw_panel(active_bits):
        pygame.draw.rect(win, (28, 28, 34), (w * S, 0, panel_w, h * S + led_h))
        for name, (r, bit, label) in controls.items():
            on = active_bits & bit
            col = (90, 200, 120) if on else ((70, 70, 80) if name not in ("a", "b") else (80, 70, 60))
            if name in ("a", "b"):
                pygame.draw.circle(win, col, r.center, r.width // 2)
                pygame.draw.circle(win, (140, 140, 150), r.center, r.width // 2, 2)
            else:
                pygame.draw.rect(win, col, r, border_radius=4 * S)
                pygame.draw.rect(win, (140, 140, 150), r, 2, border_radius=4 * S)
            t = font.render(label, True, (235, 235, 240))
            win.blit(t, t.get_rect(center=r.center))
        hints = ["click or drag", "screen: L=A R=B M=C", "wheel = up/down",
                 "F1 fast  F5 save", "F12 shot  Esc quit"]
        for i, hline in enumerate(hints):
            t = font.render(hline, True, (150, 150, 160))
            win.blit(t, (px0, (265 + i * 13) * S))
    pygame.display.set_caption("Wayfarer (Orion Trail) emulator" + (f"  room {link.info['room']}" if link else ""))
    clock = pygame.time.Clock()
    rgb = ctypes.create_string_buffer(w * h * 3)
    mask = pygame.Surface((w * S, h * S), pygame.SRCALPHA)
    mask.fill((0, 0, 0, 255))
    pygame.draw.circle(mask, (0, 0, 0, 0), (w * S // 2, h * S // 2), w * S // 2)
    dim = pygame.Surface((w * S, h * S), pygame.SRCALPHA)

    keymap = {
        pygame.K_UP: BTN_UP, pygame.K_w: BTN_UP,
        pygame.K_DOWN: BTN_DOWN, pygame.K_s: BTN_DOWN,
        pygame.K_LEFT: BTN_LEFT, pygame.K_a: BTN_LEFT,
        pygame.K_RIGHT: BTN_RIGHT, pygame.K_d: BTN_RIGHT,
        pygame.K_RETURN: BTN_C, pygame.K_SPACE: BTN_C, pygame.K_e: BTN_C, pygame.K_c: BTN_C,
        pygame.K_z: BTN_A, pygame.K_x: BTN_B,
    }
    buttons = 0
    fast = False
    q = queue.Queue()
    if not sys.stdin.isatty() or True:
        threading.Thread(target=stdin_reader, args=(q,), daemon=True).start()

    running = True
    shot = 0
    while running:
        dt = clock.tick(60)
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                running = False
            elif ev.type == pygame.MOUSEBUTTONDOWN:
                if ev.button in (4, 5):
                    taps.append((BTN_UP if ev.button == 4 else BTN_DOWN, time.time() + 0.08))
                elif screen_rect.collidepoint(ev.pos):
                    if ev.button == 1:
                        touch["down"] = touch["last"] = ev.pos
                        touch["dragging"] = False
                    else:
                        mouse_bits |= {3: BTN_B, 2: BTN_C}.get(ev.button, 0)
                else:
                    for name, (r, bit, label) in controls.items():
                        if r.collidepoint(ev.pos):
                            mouse_bits |= bit
            elif ev.type == pygame.MOUSEBUTTONUP:
                mouse_bits = 0
                if ev.button == 1 and touch["down"] is not None:
                    if not touch["dragging"]:
                        core.platform_2026_tap(touch["down"][0] // S, touch["down"][1] // S)
                        touch["sent_at"] = time.time()
                    touch["down"] = None
            elif ev.type == pygame.MOUSEMOTION and ev.buttons[0] and touch["down"] is not None:
                dx, dy = ev.pos[0] - touch["last"][0], ev.pos[1] - touch["last"][1]
                if not touch["dragging"]:
                    tdx, tdy = ev.pos[0] - touch["down"][0], ev.pos[1] - touch["down"][1]
                    if tdx * tdx + tdy * tdy > (6 * S) ** 2:
                        touch["dragging"] = True
                        dx, dy = tdx, tdy
                if touch["dragging"] and (dx or dy):
                    core.platform_2026_drag(dx // S, dy // S)
                    touch["last"] = ev.pos
            elif ev.type == pygame.MOUSEMOTION and ev.buttons[0] and not screen_rect.collidepoint(ev.pos):
                # drag across the pad: follow the pointer so the d-pad feels like a stick
                hit = 0
                for name, (r, bit, label) in controls.items():
                    if r.collidepoint(ev.pos) and name not in ("a", "b"):
                        hit = bit
                if hit:
                    mouse_bits = hit
            elif ev.type in (pygame.KEYDOWN, pygame.KEYUP):
                down = ev.type == pygame.KEYDOWN
                if ev.key in keymap:
                    if down:
                        buttons |= keymap[ev.key]
                    else:
                        buttons &= ~keymap[ev.key]
                elif down and ev.key == pygame.K_ESCAPE:
                    running = False
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
        # a tap no screen claimed within 150 ms becomes a plain A press (popups, title, minigames)
        if touch["sent_at"] is not None and now - touch["sent_at"] > 0.15:
            if core.platform_2026_tap_pending():
                core.platform_2026_tap_clear()
                taps.append((BTN_A, now + 0.08))
            touch["sent_at"] = None
        tap_bits = 0
        taps = [(b, t) for b, t in taps if t > now]
        for b, t in taps:
            tap_bits |= b
        all_bits = buttons | mouse_bits | tap_bits
        pump(core, link, ctypes, line_buf)
        core.emu_step(min(dt, 50), all_bits)
        pump(core, link, ctypes, line_buf)

        core.emu_framebuffer_rgb888(rgb, 0)
        surf = pygame.image.frombuffer(rgb.raw, (w, h), "RGB")
        if S != 1:
            surf = pygame.transform.scale(surf, (w * S, h * S))
        win.fill((20, 20, 24))
        win.blit(surf, (0, 0))
        bl = core.emu_backlight()
        if bl < 255:
            dim.fill((0, 0, 0, 255 - bl))
            win.blit(dim, (0, 0))
        win.blit(mask, (0, 0))

        leds = core.emu_leds()
        r = 8 * S // 2 + 4
        for i in range(nled):
            col = (leds[i * 3], leds[i * 3 + 1], leds[i * 3 + 2])
            cx = (i + 1) * (w * S) // (nled + 1)
            cy = h * S + led_h // 2
            pygame.draw.circle(win, (40, 40, 44), (cx, cy), r + 2)
            pygame.draw.circle(win, col, (cx, cy), r)
        draw_panel(all_bits)
        pygame.display.flip()

        if core.emu_eeprom_dirty():
            save_eeprom()

    save_eeprom()
    pygame.quit()


if __name__ == "__main__":
    main()
