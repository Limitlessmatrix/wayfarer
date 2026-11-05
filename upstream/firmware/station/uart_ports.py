from machine import Pin, UART, Timer
import rp2


@rp2.asm_pio(sideset_init=(rp2.PIO.OUT_HIGH,), out_init=(rp2.PIO.OUT_HIGH,), out_shiftdir=rp2.PIO.SHIFT_RIGHT)
def _pio_uart_tx():
    pull()
    set(x, 7).side(0) [7] # Start bit
    label("bitloop")
    out(pins, 1) [6]
    jmp(x_dec, "bitloop")
    nop() .side(1) [6] # Stop bit


@rp2.asm_pio(in_shiftdir=rp2.PIO.SHIFT_RIGHT, autopush=True, push_thresh=8)
def _pio_uart_rx():
    wait(0, pin, 0)
    set(x, 7) [11]  # Delay to middle of first data bit
    label("bitloop")
    in_(pins, 1) [6] # 8 cycles per loop 
    jmp(x_dec, "bitloop") # jmp takes 2 cycles
    wait(1, pin, 0) # Wait for stop bit

class UARTPortBase:
    def __init__(self, name, detect_pin):
        self.name = name
        self.detect_pin = Pin(detect_pin, Pin.IN, Pin.PULL_UP)
        self.connected = self.detect_pin.value() == 0
        self.rocket = None
        self.line_buffer = b""
        self.detect_pin.irq(trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING, handler=self._detect_changed)
        
    def _detect_changed(self, pin):
        #print(f"Detect pin changed for {self.name}: {pin.value()} {pin}")
        self.connected = pin.value() == 0

    def update_connection(self):
        self.connected = self.detect_pin.value() == 0
        return self.connected

    def is_connected(self):
        return self.connected

    def read(self):
        raise NotImplementedError

    def write(self, data):
        raise NotImplementedError
    
    def debug_status(self):
        print(f"Object ID: {id(self)}")
        print(f"Available attributes: {dir(self)}")
        print(f"Has rocket attribute? {hasattr(self, 'rocket')}")


class HardwareUARTPort(UARTPortBase):
    def __init__(self, name, uart_id, tx_pin, rx_pin, detect_pin, baud=9600):
        super().__init__(name, detect_pin)
        self.uart = UART(uart_id, baud, tx=Pin(tx_pin), rx=Pin(rx_pin), txbuf=1024, rxbuf=1024)
        self.uart.init(baud, bits=8, parity=None, stop=1)

    def read(self):
        if self.uart.any():
            self.line_buffer += self.uart.read()
        if b'\n' in self.line_buffer:
            idx = self.line_buffer.index(b'\n')
            line = self.line_buffer[:idx]
            self.line_buffer = self.line_buffer[idx + 1:]
            return line.rstrip(b'\r')
        return None

    def write(self, data):
        if data:
            #print (f"{self.name} TX: {data}")
            l = self.uart.write(data)
            if l is None:
                print(f"Warning: {self.name} UART write returned None, data may not have been sent.")

class PIOUARTPort(UARTPortBase):
    def __init__(self, name, sm_base, tx_pin, rx_pin, detect_pin, baud=9600):
        super().__init__(name, detect_pin)
        self.tx_pin_obj = Pin(tx_pin, Pin.OUT, value=1)
        self.rx_pin_obj = Pin(rx_pin, Pin.IN, Pin.PULL_UP)
        print("Initing", name)
        self.tx_sm = rp2.StateMachine(sm_base, _pio_uart_tx, freq=baud * 8, out_base=tx_pin, sideset_base=tx_pin)
        self.rx_sm = rp2.StateMachine(sm_base +1, _pio_uart_rx, freq=baud * 8, in_base=rx_pin, jmp_pin=rx_pin)
        self.tx_sm.active(1)
        self.rx_sm.active(1)
        # Set up timer to drain FIFO frequently
        self.timer = Timer()
        self.timer.init(period=1, mode=Timer.PERIODIC, callback=self._drain_fifo)

    def _drain_fifo(self, timer):
        """Drain the RX FIFO immediately, moving any data to line_buffer"""
        while self.rx_sm.rx_fifo() > 0:
            c = self.rx_sm.get() >> 24
            self.line_buffer += bytes([c])

    def read(self):
        if b'\n' in self.line_buffer:
            idx = self.line_buffer.index(b'\n')
            line = self.line_buffer[:idx]
            self.line_buffer = self.line_buffer[idx + 1:]
            return line.rstrip(b'\r')
        return None

    def write(self, data):
        if not data:
            return
        #print (f"{self.name} TX: {data}")
        for byte in data:
            self.tx_sm.put(byte)


def build_ports(port_configs):

    ports = []
    for config in port_configs:
        if config["type"] == "hw":
            ports.append(HardwareUARTPort(config["name"], config["uart_id"], config["tx"], config["rx"], config["detect"], config["baud"]))
        else:
            ports.append(PIOUARTPort(config["name"], config["sm_base"], config["tx"], config["rx"], config["detect"], config["baud"]))
    return ports
