"""vsf_bench — build, flash, and test orchestration for VSF firmware.

Public API surface. Internal modules may be renamed; import from here
for stability.
"""

from vsf_bench.capture_marker import read_framework_windows
from vsf_bench.hardware_map import load as load_board
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument, SuiteTimeoutError
from vsf_bench.pipeline import run_test_phase
from vsf_bench.test_params_loader import load_test_params

__all__ = [
    "LogicAnalyzerInstrument",
    "SerialInstrument",
    "SuiteTimeoutError",
    "read_framework_windows",
    "load_test_params",
    "load_board",
    "run_test_phase",
]
