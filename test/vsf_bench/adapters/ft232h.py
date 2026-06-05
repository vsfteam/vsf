"""FT232HAdapter — GPIO via FT232H MPSSE mode (pyftdi).

Also supports I2C/SPI via the same chip (future).
"""

from vsf_bench.capabilities.gpio import GPIO

try:
    from pyftdi.ftdi import Ftdi
except ImportError:
    Ftdi = None


class FT232HAdapter(GPIO):
    """Control an FT232H pin as GPIO output.

    Parameters:
        serial: FT232H USB serial number (e.g. "FT96OF9L").
        pin: pin number (0-7).
        port: "AD" for ADBUS, "AC" for ACBUS (default "AD").
        active_low: True if the logical "active" state is low (default True).
    """

    # MPSSE command bytes for each port
    _CMD = {"AD": 0x80, "AC": 0x82}

    def __init__(self, serial: str, pin: int = 0, port: str = "AD",
                 active_low: bool = True):
        if Ftdi is None:
            raise RuntimeError("pyftdi not installed. Run: pip install pyftdi")
        if port not in self._CMD:
            raise ValueError(f"port must be 'AD' or 'AC', got {port!r}")
        self._serial = serial
        self._pin = pin
        self._port = port
        self._cmd = self._CMD[port]
        self._active_low = active_low
        self._ftdi = None
        self._value = 0xFF

    def open(self) -> None:
        self._ftdi = Ftdi()
        self._ftdi.open_mpsse_from_url(
            f"ftdi://ftdi:232h:{self._serial}/1",
            direction=0xFF,  # all output
        )
        self._value = 0xFF

    def close(self) -> None:
        if self._ftdi:
            try:
                self.set(False)
                self._ftdi.write_data(bytes([self._cmd, 0xFF, 0xFF]))
                self._ftdi.close()
            except Exception:
                pass
            self._ftdi = None

    def set(self, active: bool) -> None:
        """Set pin to active (taking active_low into account)."""
        high = not active if self._active_low else active
        if high:
            self._value |= (1 << self._pin)
        else:
            self._value &= ~(1 << self._pin)
        self._ftdi.write_data(bytes([self._cmd, self._value, 0xFF]))
