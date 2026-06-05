"""GPIO — abstract capability for digital I/O pin control."""

from abc import ABC, abstractmethod


class GPIO(ABC):
    """Control a single GPIO pin (high/low).

    Adapters: FT232H, CH347, Raspberry Pi GPIO, etc.
    """

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, *args):
        self.close()

    def open(self) -> None:
        """Initialise the GPIO hardware."""

    def close(self) -> None:
        """Release the GPIO hardware."""

    @abstractmethod
    def set(self, high: bool) -> None:
        """Set the output level of the pin (True = high, False = low)."""
        ...
