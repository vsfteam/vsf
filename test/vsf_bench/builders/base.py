"""BuildRunner abstract base class.

A BuildRunner turns source code into a firmware artifact. It is the
counterpart to FlashRunner (which uploads artifacts to hardware). The
separation lets a tool like IAR occupy *both* roles: IARBuilder compiles,
and IARFlashRunner uploads.
"""

from abc import ABC, abstractmethod
from pathlib import Path


class BuildRunner(ABC):
    """Abstract base for build tools (CMake, IAR, Ninja, etc.)."""

    @abstractmethod
    def build(self) -> Path:
        """Run the build and return the build directory.

        Raises on build error.
        """
        ...
