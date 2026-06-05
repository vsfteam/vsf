"""ProgramCapability — abstract interface for any adapter that can program firmware."""

from abc import ABC, abstractmethod
from pathlib import Path

from vsf_bench.config import RunnerConfig


class ProgramCapability(ABC):
    """A hardware capability to program firmware to a target device.

    Subclasses implement the programming protocol (UART HCI, DFU, SWD, etc.).
    The caller (program_phase) follows this lifecycle::

        adapter = DFUAdapter(config)
        with adapter:                    # → prepare(), open resources
            power_cycle(board)
            adapter.flash(build_dir)     # → handshake / wait / download
        # → close(), release resources
    """

    REQUIRED_PARAMS: list[str] = []

    def __init__(self, config: RunnerConfig):
        self._config = config

    def __enter__(self):
        self.prepare()
        return self

    def __exit__(self, *args):
        self.close()

    def prepare(self) -> None:
        """Optional hook before power-on (open serial, press DFU key, etc.)."""

    def close(self) -> None:
        """Optional hook to release resources (close serial, release GPIO)."""

    @abstractmethod
    def flash(self, build_dir: Path) -> None:
        """Program firmware to the target device."""
        ...

    @classmethod
    def validate_params(cls, params: dict) -> list[str]:
        return []
