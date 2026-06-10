"""vsf_bench — build, flash, and test orchestration for VSF firmware.

Public API surface. Internal modules may be renamed; import from here
for stability.
"""

from vsf_bench.vsf_test.marker import read_framework_windows
from vsf_bench.config.map import load as load_board
from vsf_bench.utils.serial import SerialInstrument, SuiteTimeoutError
from vsf_bench.utils.core import parse_uart_csv, read_csv_rows
from vsf_bench.test_runner import run_test_phase
from vsf_bench.vsf_test.params_loader import load_test_params

from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer

# Backward-compat: LogicAnalyzerInstrument removed (→ capabilities/adapters).
# Test scripts still import it for type annotations on unused `la` parameter.
LogicAnalyzerInstrument = object  # type: ignore[name-assigned]

__all__ = [
    "SerialInstrument",
    "SuiteTimeoutError",
    "parse_uart_csv",
    "read_csv_rows",
    "read_framework_windows",
    "load_test_params",
    "load_board",
    "run_test_phase",
    "LogicAnalyzer",
    "LogicAnalyzerInstrument",
]
