#!/usr/bin/env python3
"""Wayfarer multiplayer relay.

Rooms are identified by a short code. Every text line a client sends is forwarded to the other
members of its room. The relay owns the room's map seed so everyone flies the same galaxy.

    tools/relay.py --port 8766

Client protocol (JSON text frames):
    -> {"join": "ABCD", "name": "Photon Welkin"}     join or create a room ("" = new random code)
    <- {"room": "ABCD", "seed": 123456, "slot": 2, "peers": 1}
    -> {"line": "$MP POS ..."}                        forwarded to the room as {"line": ...}
    <- {"line": "$MP BYE <id>"}                       sent by the relay when a member disconnects
"""
import argparse
import asyncio
import json
import random
import string
import time

import websockets

MAX_PER_ROOM = 8
rooms = {}   # code -> {"seed": int, "clients": {ws: {"name": str, "id": str}}, "created": float}


def new_code():
    while True:
        code = "".join(random.choice("ABCDEFGHJKLMNPQRSTUVWXYZ23456789") for _ in range(4))
        if code not in rooms:
            return code


async def send(ws, obj):
    try:
        await ws.send(json.dumps(obj))
        return True
    except Exception:
        return False


async def broadcast(room, sender, line):
    dead = []
    for c in list(room["clients"]):
        if c is sender:
            continue
        if not await send(c, {"line": line}):
            dead.append(c)
    for c in dead:
        room["clients"].pop(c, None)


async def handler(ws):
    room = None
    me = None
    try:
        async for msg in ws:
            try:
                m = json.loads(msg)
            except ValueError:
                continue
            if "join" in m:
                code = (m.get("join") or "").strip().upper() or new_code()
                room = rooms.setdefault(code, {"seed": random.getrandbits(31) or 1, "clients": {}, "created": time.time()})
                if len(room["clients"]) >= MAX_PER_ROOM:
                    await send(ws, {"error": f"room {code} is full"})
                    room = None
                    continue
                me = {"name": str(m.get("name", "?"))[:14], "id": None}
                room["clients"][ws] = me
                await send(ws, {"room": code, "seed": room["seed"], "slot": len(room["clients"]),
                                "peers": len(room["clients"]) - 1})
                print(f"[{code}] join {me['name']} ({len(room['clients'])} in room, seed {room['seed']})", flush=True)
            elif "line" in m and room is not None:
                line = str(m["line"])[:400]
                parts = line.split()
                if len(parts) >= 3 and parts[0] == "$MP" and parts[1] == "HELLO":
                    me["id"] = parts[2]
                await broadcast(room, ws, line)
    except websockets.ConnectionClosed:
        pass
    finally:
        if room is not None:
            room["clients"].pop(ws, None)
            if me and me["id"]:
                await broadcast(room, ws, f"$MP BYE {me['id']}")
            codes = [k for k, v in rooms.items() if v is room]
            print(f"[{codes[0] if codes else '?'}] leave {me['name'] if me else '?'}", flush=True)
            if not room["clients"]:
                for k in codes:
                    rooms.pop(k, None)


async def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--host", default="0.0.0.0")
    ap.add_argument("--port", type=int, default=8766)
    args = ap.parse_args()
    async with websockets.serve(handler, args.host, args.port, ping_interval=20, ping_timeout=20, max_size=4096):
        print(f"wayfarer relay listening on ws://{args.host}:{args.port}", flush=True)
        await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())
