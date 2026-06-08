"""PluginRunner — dynamically load a flash runner from an external module.

This is the key name-isolation mechanism: vsf-bench code never imports or
mentions ``bh1098_flash`` or ``BH1098UartFlashRunner``.  Instead,
``hardware-map.yml`` (in the private repository) specifies ``module`` and
``class`` names, and PluginRunner loads them via ``importlib`` at runtime.
"""

import importlib
from pathlib import Path

from vsf_bench.config.models import RunnerConfig
from vsf_bench.runners.base import FlashRunner


class PluginRunner(FlashRunner):
    """Dynamically loads a FlashRunner subclass from an external Python module.

    Required params (set in hardware-map.yml):
      * ``module``: dotted Python module path (e.g. ``"scripts.bh1098_flash"``)
      * ``class``: class name inside that module
    """

    REQUIRED_PARAMS = ["module", "class"]

    def __init__(self, config: RunnerConfig):
        super().__init__(config)
        self._module_name = config.params["module"]
        self._class_name = config.params["class"]
        self._delegate = None

    def _load_delegate(self) -> FlashRunner:
        if self._delegate is None:
            mod = self._resolve_module(self._module_name)
            cls = getattr(mod, self._class_name, None)
            if cls is None:
                raise RuntimeError(
                    f"PluginRunner: module '{self._module_name}' has no "
                    f"class '{self._class_name}'"
                )
            self._delegate = cls(self._config)
        return self._delegate

    @staticmethod
    def _resolve_module(module_name: str):
        """Import *module_name*, auto-discovering the private repo root.

        Tries ``importlib.import_module`` first.  On ImportError, walks
        upward from cwd looking for a directory that contains the first
        component of *module_name* (e.g. ``scripts/``), adds it to
        ``sys.path``, and retries.
        """
        import sys as _sys

        try:
            return importlib.import_module(module_name)
        except ImportError:
            pass

        # Search upward for the private repo root
        top_pkg = module_name.split(".")[0]
        search = Path.cwd()
        while True:
            candidate = search / top_pkg
            if candidate.is_dir() or (search / f"{top_pkg}.py").exists():
                repo_root = str(search)
                if repo_root not in _sys.path:
                    _sys.path.insert(0, repo_root)
                return importlib.import_module(module_name)
            parent = search.parent
            if parent == search:
                break
            search = parent

        raise RuntimeError(
            f"PluginRunner: cannot import '{module_name}'. "
            f"Tried cwd upward search — not found. "
            f"Verify the private repo is accessible from cwd."
        )

    def prepare(self) -> None:
        self._load_delegate().prepare()

    def close(self) -> None:
        if self._delegate is not None:
            self._delegate.close()

    def flash(self, build_dir: Path) -> None:
        self._load_delegate().flash(build_dir)
