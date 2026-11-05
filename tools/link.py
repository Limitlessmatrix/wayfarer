"""WebSocket link between an emulator core and the relay. Thread safe queues on both sides."""
import asyncio
import json
import queue
import sys
import threading
import os

DEBUG = bool(os.environ.get("WAYFARER_LINK_DEBUG"))


class Link(threading.Thread):
    def __init__(self, url, code, name):
        super().__init__(daemon=True)
        self.url, self.code, self.name = url, code or "", name
        self.inbox = queue.Queue()     # lines from the relay, to push into the game UART
        self.outbox = queue.Queue()    # lines the game sent, to forward to the relay
        self.info = None
        self.error = None

    def run(self):
        try:
            asyncio.run(self._main())
        except Exception as e:  # noqa: BLE001
            self.error = e
            print(f"link error: {e}", file=sys.stderr)

    async def _main(self):
        import websockets
        async with websockets.connect(self.url) as ws:
            await ws.send(json.dumps({"join": self.code, "name": self.name}))

            async def reader():
                async for msg in ws:
                    m = json.loads(msg)
                    if "error" in m:
                        self.error = m["error"]
                        return
                    if "room" in m:
                        self.info = m
                        self.inbox.put(f"$MP ROOM {m['seed']} {m['slot']}")
                    elif "line" in m:
                        self.inbox.put(m["line"])

            async def writer():
                while True:
                    try:
                        line = self.outbox.get_nowait()
                    except queue.Empty:
                        await asyncio.sleep(0.01)
                        continue
                    await ws.send(json.dumps({"line": line}))

            await asyncio.gather(reader(), writer())


def pump(core, link, ctypes_mod, line_buf):
    """Move lines both ways between the core's UART bridge and the link."""
    if link is None:
        return
    while not link.inbox.empty():
        line = link.inbox.get()
        if DEBUG:
            print(f"[link<-] {line}", file=sys.stderr, flush=True)
        core.platform_2026_uart_push_str((line + "\n").encode())
    while core.platform_2026_uart_pop_line(line_buf, len(line_buf)):
        line = line_buf.value.decode(errors="replace")
        if DEBUG:
            print(f"[link->] {line}", file=sys.stderr, flush=True)
        if line.startswith("$MP"):
            link.outbox.put(line)
