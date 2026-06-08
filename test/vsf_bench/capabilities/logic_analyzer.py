"""LogicAnalyzer — abstract capability for a logic analyzer device."""

from abc import ABC, abstractmethod
from pathlib import Path

from vsf_bench.config import UARTConfig


class LogicAnalyzer(ABC):
    """A hardware logic analyzer: capture + offline protocol decode.

    Adapters: DSView (dsview-cli), Saleae Logic, sigrok/PulseView, etc.

    Lifecycle::

        la = DSViewAdapter(cli_path, device, samplerate, channels)
        capture_path = Path("capture.dsl")
        la.start(capture_path, duration_s=30)
        la.wait_until_started()
        # ... firmware runs ...
        la.stop()
        la.wait()
        # ... decode offline ...
        csv = la.decode_uart(capture_path, "CH8", UARTConfig(baudrate=2000000))
    """

    # -------------------------------------------------------------- capture

    @abstractmethod
    def start(self, capture_path: Path, duration_s: float) -> None:
        """Start a live capture.  Must be idempotent (no-op if running)."""
        ...

    @abstractmethod
    def wait_until_started(self, timeout: float = 5.0) -> None:
        """Block until the hardware signals sampling has begun."""
        ...

    @abstractmethod
    def stop(self) -> None:
        """Gracefully stop an ongoing capture.  Must be idempotent."""
        ...

    @abstractmethod
    def wait(self, timeout: float = 300.0) -> None:
        """Block until the capture process exits successfully."""
        ...

    # -------------------------------------------------------------- decode

    @abstractmethod
    def decode_uart(
        self,
        capture_path: Path,
        channel: str,
        config: UARTConfig,
        start_ns: int | None = None,
        end_ns: int | None = None,
    ) -> Path:
        """Decode a time window of a channel as UART, returning CSV path."""
        ...

    @abstractmethod
    def read_digital_edges(
        self,
        capture_path: Path,
        channel: str,
        start_ns: int | None = None,
        end_ns: int | None = None,
    ) -> list[int]:
        """Return absolute edge timestamps (ns) on a digital channel."""
        ...
