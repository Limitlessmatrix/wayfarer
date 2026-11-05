import time
import json
import gc
import os
import sys
import rp2
from machine import Pin, Timer, WDT, unique_id, reset
from utime import sleep
import neopixel
import network
import asyncio
import komihash
import hashlib
import binascii

try:
    import aiohttp
except ImportError:
    aiohttp = None

try:
    from station_lights import StationLightManager
except ImportError:
    from station_lights import StationLightManager

urlbase = "https://z88608jzej.execute-api.us-east-1.amazonaws.com/default/Saintcon26StationUpdate/"
StationName = "Frontier"

rp2.country('US') 
pin = Pin("LED", Pin.OUT)

try:
    from station.uart_ports import build_ports
except ImportError:
    from uart_ports import build_ports

# ======= Configuration =======
# Adjust these pin mappings to your connector layout.
UART_PORTS = [
    {"name": "UART0", "type": "hw",  "uart_id": 0, "tx": 0,  "rx": 1,  "detect": 2,  "baud": 9600},
    {"name": "UART1", "type": "hw",  "uart_id": 1, "tx": 4,  "rx": 5,  "detect": 6,  "baud": 9600},
    {"name": "UART2", "type": "pio", "sm_base": 0, "tx": 8,  "rx": 9,  "detect": 10, "baud": 9600},
    {"name": "UART3", "type": "pio", "sm_base": 2, "tx": 12, "rx": 13,  "detect": 14, "baud": 9600},
    {"name": "UART4", "type": "pio", "sm_base": 6, "tx": 16, "rx": 17,  "detect": 18, "baud": 9600},
]

WS2812_PIN = 20
WS2812_COUNT = 150
LED_UPDATE_MS = 100
POLL_MS = 50
LED_RENDER_HZ = 25

# Initialize NeoPixel object on your chosen pin
wspin = Pin(WS2812_PIN, Pin.OUT)


wlan = network.WLAN(network.STA_IF)
try:
    with open('/wifi.json', 'r') as f:
        wifi_config = json.load(f)
    ssid = wifi_config.get('ssid')
    password = wifi_config.get('password')
except Exception:
    ssid = None
    password = None
wifi_configured = False

async def ensure_network(timeout=30):
    """Ensure the WLAN is connected. Try to (re)connect for up to `timeout` seconds.
    Returns True if connected, False otherwise.
    """
    global wlan, ssid, password, wifi_configured
    try:
        if not wifi_configured:
            if not ssid or not password:
                print('Wi-Fi credentials missing; add ssid/password to /wifi.json')
                return False
            try:
                wlan.active(True)
                if not wlan.active():
                    print('WLAN failed to become active; resetting device to attempt recovery')
                    Pin(23, Pin.OUT).value(0) 
                    time.sleep_ms(250)
                    reset()  # Reset the device to attempt recovery
                wlan.config(pm=network.WLAN.PM_NONE)
                wifi_configured = True
            except Exception as exc:
                print('WLAN initialization failed:', exc)
                return False

        # Quick path: some ports expose isconnected()
        if hasattr(wlan, 'isconnected'):
            try:
                if wlan.isconnected():
                    return True
            except Exception:
                pass

        print("Network disconnected — attempting reconnect")
        try:
            wlan.connect(ssid, password)
        except Exception as exc:
            print('WLAN connect failed:', exc)
            return False

        for _ in range(timeout):
            try:
                if hasattr(wlan, 'isconnected') and wlan.isconnected():
                    print('Reconnected, IP:', wlan.ifconfig()[0])
                    return True
            except Exception:
                pass
            await asyncio.sleep(1)

        print('Failed to reconnect after', timeout, 'seconds')
        return False
    except Exception as e:
        print('ensure_network error', e)
        return False


rocket_db = {}
vote_db = {}
update_db = {}
modified = False
vote_data = None
motd = "The station is quiet for a moment as the crew goes about its work. Machinery hums, cargo moves, and the stars shine beyond the windows. Another day on the frontier is underway."
post_lock = None
stationId = 0

highroller = {'name':b'Plum', 'class':b'A'[0],'score':0}


with open('/station.json', 'r') as f:
    names = {1:"Horizon", 2:"Meridian", 3:"Nexus"}
    station_data = json.load(f)
    stationId = station_data.get('station_number', 0)
    StationName = names.get(stationId, 'Frontier')


def log_exception(context, exc):
    print("[ERROR] {}".format(context))
    try:
        sys.print_exception(exc)
    except Exception:
        print(str(exc))

def verify_hash(line, rocketId):
    signkey = int.from_bytes(rocketId, 'little')
    hash = komihash.komihash(line[:-4], signkey)
    b64hash = binascii.b2a_base64(hash.to_bytes(8, 'little'))
    if b64hash[:4] != line[-4:]:
        print("Signature verify failed!", hex(signkey), b64hash[:4], line[-4:])
        raise ValueError


def get_device_id():
    return binascii.hexlify(hashlib.sha1(unique_id()).digest()).decode()


def safe_parse_line(port, line):
    if not line:
        return

    try:
        if line.startswith(b"$ID "):
            parse_rocket(line, port)
        elif line.startswith(b'$GAME '):
            parse_game(line, port)
        elif line.startswith(b'$VOTE '):
            parse_vote(line, port)
        elif line.startswith(b'$EOL '):
            parse_eol(line, port)
    except Exception as exc:
        log_exception("parse_line({})".format(port.name if port else 'unknown'), exc)

try:
    if 'rockets.json' in os.listdir('/data'):
        with open('/data/rockets.json', "r") as f:
            # JSON keys are always saved as strings, convert them back to ints
            rocket_db = json.load(f)
        for k,v in rocket_db.items():
            if v['S'] > highroller['score']:
                highroller["score"] = v['S']
        print("loaded %d records from rockets.json" % (len(rocket_db)))
    else:
        rocket_db = {}
except Exception as e:
    print("Error loading rocket DB, starting fresh:", e)
    rocket_db = {}

try:
    with open('/data/leader.txt', 'r') as f:
        highroller["name"] = f.read()
        print ("Highroller loaded", highroller["name"])
except Exception as e:
    print("Error loading leader name", e)

try:
    if 'votes.json' in os.listdir('/data'):
        with open('/data/votes.json', "r") as f:
            vote_db = json.load(f)
        print("loaded %d records from votes.json" % (len(vote_db)))
    else:
        vote_db = {}
except Exception as e:
    print("Error loading vote DB, starting fresh:", e)
    vote_db = {}

try:
    if 'vote_data.json' in os.listdir('/data'):
        with open('/data/vote_data.json', "r") as f:
            vote_data = json.load(f)
        motd = vote_data.get('message', motd)
        print("loaded vote_data.json")
except Exception as e:
    print("Error loading vote data, starting without cached vote data:", e)
    vote_data = None


class Rocket:
    def __init__(self, idstr):
        if idstr[0:4] != b'$ID ':
            raise ValueError(b"Invalid Rocket ID str: " + idstr) 
        self.role = idstr[4]
        self.id = idstr[5:10]
        self.name = idstr[10:24]
        self.unlocks = int(idstr[24:32], 16)
        self.str = idstr[4:]
        verify_hash(idstr, self.id)

    def get_id_str(self):
        idstr = b"%c%5s%14s " % (self.role, self.id, self.name)
        return idstr


def format_bytes(data):
    if not data:
        return ""
    return "".join(chr(b) if 32 <= b < 127 else "." for b in data)


def parse_rocket(line, port):
    try:
        port.rocket = Rocket(line)
        if vote_data is not None and port.rocket.id not in vote_db:
            vcount = summerize_votes()
            #port.write(b'$DIALOG ' + vote_data['prompt'] + b'\r\n')
            line = '$OPTIONS '
            opts = ['%02d%03d%s'%(x['icon'], vcount.get(i, 0), x['name']) for i, x in enumerate(vote_data['options'])]
            vo = '$OPTIONS ' + '|'.join(opts) + '\r\n'
            port.write(vo)
        val = {'S':0, 'M':0, 'P':0}
        if port.rocket.id in rocket_db:
            val = rocket_db[port.rocket.id]
        else:
            rocket_db[port.rocket.id] = val
        print("Rocket parsed:", chr(port.rocket.role), port.rocket.id, port.rocket.name, hex(port.rocket.unlocks), val)
    except Exception as e:
        log_exception("parse_rocket", e)
        print("Error parsing ID: ")
        print(line)

def parse_game(line, port):
    global modified
    if not port or port.rocket is None:
        return

    try:
        #$GAME %5s%5s %5d %2d 
        if len(line) < 26:
            return
        verify_hash(line, port.rocket.id)
        rocket_id = line[6:11]
        if len(line) < 26:
            return
        game_id = line[11:16]
        score = int(line[17:22], 10)
        
        if rocket_id in rocket_db:
            rocket_db[rocket_id]['S'] += score
            rocket_db[rocket_id]['M'] += 1
        else:
            rocket_db[rocket_id] = {'S':score, 'M':1, 'P':0}

        tscore = rocket_db[rocket_id]['S']
        if tscore > highroller['score']:
            highroller['name'] = port.rocket.name
            highroller['score'] = tscore
            highroller['class'] = port.rocket.role

        crewdata = line[26:-5]
        crew = chunks = [crewdata[i:i + 5] for i in range(0, len(crewdata), 5)]
        for c in crew:
            if c == rocket_id:
                continue
            if c in rocket_db:
                rocket_db[c]['S'] += score // 5
                rocket_db[c]['P'] += 1
            else:
                rocket_db[c] = {'S':score // 5, 'M':0, 'P':1}

        if rocket_id not in update_db:
            update_db[rocket_id] = {'games':{}}
        rocket_entry = update_db[rocket_id]
        rocket_entry['name'] = port.rocket.name
        rocket_entry['unlocks'] = port.rocket.unlocks
        rocket_entry['games'][game_id] = {'score': score, 'crew': crew}
        print("Game parsed:", rocket_id, game_id, score, crew)

        modified = True
        port.write(line[0:16]+ b'\r\n')
    except Exception as e:
        log_exception("parse_game", e)
        print("Error parsing Game: ")
        print(line)

def parse_vote(line, port):
    global modified
    try:
        if len(line) < 14:
            return
        verify_hash(line, port.rocket.id)
        rocket_id = line[6:11]
        vid = int(line[12:14], 10)
        vote_db[rocket_id] = vid
        modified = True
        print("Vote parsed:", rocket_id, vid)
    except Exception as e:
        log_exception("parse_vote", e)
        print("Error parsing Vote: ")
        print(line)

def parse_eol(line, port):
    try:
        if not port or port.rocket is None:
            return
        data = rocket_db.get(port.rocket.id)
        if data is None:
            data = {'S':0, 'M':0, 'P':0}
        port.write(b'$EOL %7d %3d %3d\r\n' % (data['S'], data['M'], data['P']))
    except Exception as e:
        log_exception("parse_eol", e)
        print("Error parsing EOL: ")
        print(line)

def update_leds(ports, lights):
    if wlan.isconnected():
        lights.set_pattern(0, 'on', (0,30,0))
    else:
        lights.set_pattern(0, 'on', (30,0,0))
    
def get_station_line():
    return b"$STATION %d|%c%d|%s|%s\r\n" % (
        stationId, 
        highroller['class'], 
        highroller['score'],
        highroller['name'], 
        motd
    )

async def send_post():
    global update_db, post_lock, vote_data, vote_db, motd

    if update_db is None:
        return

    if aiohttp is None:
        print('Skipping POST: aiohttp module not available')
        return

    if post_lock is None:
        try:
            post_lock = asyncio.Lock()
        except Exception as e:
            print('POST lock unavailable:', e)
            post_lock = None

    if post_lock is not None and post_lock.locked():
        print('POST already in progress; skipping duplicate request')
        return

    if not await ensure_network():
        print('Skipping POST: no network')
        return

    vcounts = summerize_votes()
    update_temp = {'games': update_db}
    if vote_data is not None:
        vote_results = {
            v['name']: count
            for v, count in zip(vote_data.get('options', []), vcounts.values())
        }
        update_temp['votes'] = {
            'version': vote_data['version'],
            'results': vote_results
        }
    headers = {'Content-Type': 'application/json', 'X-API-Key': get_device_id()}
    url = urlbase + str(StationName).replace(' ', '')
    request_timeout_s = 10
    gc.collect()
    post_body = json.dumps(update_temp)
    del update_temp
    del vcounts
    gc.collect()
    post_phase = ['session']
    post_heap = 'unavailable'
    try:
        post_heap = (gc.mem_alloc(), gc.mem_free())
    except AttributeError:
        pass

    async def _do_post(post_url, post_data, post_headers):
        global update_db, vote_data, vote_db, motd

        post_phase[0] = 'session allocation'
        async with aiohttp.ClientSession() as session:
            post_phase[0] = 'connect/TLS or request allocation'
            async with session.post(
                post_url,
                data=post_data,
                headers=post_headers,
                ssl=True,
            ) as response:
                post_phase[0] = 'response body allocation'
                body = await response.text()
                status = response.status
                print('POST response status:', status)

                if status == 200:
                    try:
                        rjson = json.loads(body)
                    except Exception as e:
                        print('POST JSON decode failed:', e)
                        return

                    if 'vote' in rjson:
                        incoming_vote_data = rjson['vote']
                        cached_vote_data = vote_data
                        version_changed = False
                        if cached_vote_data is not None:
                            version_changed = (
                                incoming_vote_data.get('version') != cached_vote_data.get('version')
                            )
                            if version_changed:
                                print("Vote data updated:", vote_data)
                                print("Received updated vote info %s != %s" % (
                                    incoming_vote_data.get('version'),
                                    cached_vote_data.get('version')
                                ))
                                vote_db = {}
                                with open('/data/votes.json', "w") as f:
                                    json.dump(vote_db, f)
                                print("Cleared votes.json for new vote version")
                        vote_data = incoming_vote_data
                        motd = vote_data.get('message', motd)
                        with open('/data/vote_data.json', "w") as f:
                            json.dump(vote_data, f)

                    update_db = {}
                    del rjson
                    del body
                    return

                print('POST failed with status', status, 'body:', body[:200])

    try:
        print('Sending POST length', len(post_body))
        if post_lock is not None:
            async with post_lock:
                await asyncio.wait_for(_do_post(url, post_body, headers), timeout=request_timeout_s)
        else:
            await asyncio.wait_for(_do_post(url, post_body, headers), timeout=request_timeout_s)
    except asyncio.TimeoutError:
        print('POST timed out after', request_timeout_s, 'seconds; keeping queued data for retry')
    except Exception as e:
        if isinstance(e, OSError) and e.args and e.args[0] == 12:
            print('POST hit ENOMEM during', post_phase[0],
                  '- likely network/TLS buffer allocation; error:', repr(e),
                  'args:', e.args)
        else:
            print('Error sending update during', post_phase[0], repr(e))
    finally:
        gc.collect()

def summerize_votes():
    value_counts = {}
    for value in vote_db.values():
        value_counts[value] = value_counts.get(value, 0) + 1
    return value_counts

def sync_data():
    global modified, update_db
    print("Saving data", modified)
    try:
        if modified == True:
            with open('/data/rockets.json', "w") as f:
                json.dump(rocket_db, f)
                print("Saved rockets.json")
            with open('/data/votes.json', "w") as f:
                json.dump(vote_db, f)
                print("Saved votes.json")
            if vote_data is not None:
                with open('/data/vote_data.json', "w") as f:
                    json.dump(vote_data, f)
                print("Saved vote_data.json")
            savedname = ''
            try:
                with open('/data/leader.txt', 'r') as f:
                    savedname = f.read()
            except Exception as e:
                print("error reading leader", e)

            if savedname != highroller['name']:
                with open('/data/leader.txt', 'w') as f:
                    f.write(highroller['name'])
                    print("Saved leader.txt")

        if True: #update_db:
            asyncio.create_task(send_post())
    except Exception as e:
        log_exception("sync_data", e)

    modified = False

async def main():
    print("Starting docking station")
    ports = build_ports(UART_PORTS)
    pixels = neopixel.NeoPixel(wspin, WS2812_COUNT)
    pixels.timing = (300, 800, 800, 300)
    lights = StationLightManager(pixels, brightness_limit=0.5)
    lights.clear()
    lights.set_all("off")
    lights.set_station(stationId)
    async def light_render_loop():
        while True:
            lights.render()
            await asyncio.sleep_ms(max(1, int(1000 / LED_RENDER_HZ)))
    asyncio.create_task(light_render_loop())
    lights.render()
    watchdog = WDT(timeout=8000)

    last_crew_update = time.ticks_ms()

    last_web_sync = 0
    asyncio.create_task(send_post())

    while True:
        try:
            active_count = 0
            crew = b'$CREW '
            for port in ports:
                try:
                    if port.connected:
                        active_count += 1
                    if port.rocket:
                        crew += port.rocket.get_id_str()
                    while True:
                        line = port.read()
                        if line is None:
                            break
                        #print("[{}] RX: {}".format(port.name, format_bytes(line)))
                        safe_parse_line(port, line)
                except Exception as exc:
                    log_exception("port loop ({})".format(getattr(port, 'name', 'unknown')), exc)

            update_leds(ports, lights)

            crew += b'\r\n'
            now = time.ticks_ms()
            if (now - last_crew_update) > 1000:
                for port in ports:
                    try:
                        if port.connected:
                            if port.rocket == None:
                                print("Port {} has no rocket, sending station line".format(port.name))
                                port.write(get_station_line())
                            else:
                                port.write(crew)
                        else:
                            if port.rocket:
                                port.rocket = None
                    except Exception as exc:
                        log_exception("port heartbeat ({})".format(getattr(port, 'name', 'unknown')), exc)
                last_crew_update = now

            if (now - last_web_sync) > (1000 * 60):
                sync_data()
                last_web_sync = now

            await asyncio.sleep_ms(POLL_MS)
            watchdog.feed()
            pin.value(not pin.value())
        except KeyboardInterrupt:
            raise
        except Exception as exc:
            log_exception("main_loop", exc)
            await asyncio.sleep_ms(1000)


if __name__ == "__main__":
    time.sleep(3)  # Wait a moment for the system to stabilize
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("Shutdown requested")
    except Exception as exc:
        log_exception("main_entry", exc)
        while True:
            try:
                time.sleep(1)
            except KeyboardInterrupt:
                raise
