"""vsf_bench — build, flash, and test orchestration for VSF firmware.

Public API surface. Internal modules may be renamed; import from here
for stability.
"""

from vsf_bench.test.marker import read_framework_windows
from vsf_bench.hardware_map import load as load_board
from vsf_bench.serial import SerialInstrument, SuiteTimeoutError
from vsf_bench.utils.core import parse_uart_csv, read_csv_rows
from vsf_bench.pipeline import run_test_phase
from vsf_bench.test.params_loader import load_test_params

__all__ = [
    "SerialInstrument",
    "SuiteTimeoutError",
    "parse_uart_csv",
    "read_csv_rows",
    "read_framework_windows",
    "load_test_params",
    "load_board",
    "run_test_phase",
]
