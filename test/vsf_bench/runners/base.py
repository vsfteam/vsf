"""FlashRunner — abstract base class for all flash runner backends.

Custom runners (post-MVP) must inherit from this class and implement flash().
"""

from abc import ABC, abstractmethod
from pathlib import Path

from vsf_bench.config import RunnerConfig


class FlashRunner(ABC):
    """Abstract base for flash runners.

    Subclasses must:
    - Set REQUIRED_PARAMS: list of param keys that must exist in hardware-map.yml
    - Implement flash(build_dir): flash firmware to the target board
    - Optionally override validate_params(params) for type-specific checks
    """

    REQUIRED_PARAMS: list[str] = []

    def __init__(self, config: RunnerConfig):
        self._config = config

    @abstractmethod
    def flash(self, build_dir: Path) -> None:
        """Flash firmware to the target board."""
        ...

    @classmethod
    def validate_params(cls, params: dict) -> list[str]:
        """Extra validation beyond REQUIRED_PARAMS. Returns list of error strings."""
        return []
