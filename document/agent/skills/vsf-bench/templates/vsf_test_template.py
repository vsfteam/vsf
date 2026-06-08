"""Template for new vsf-bench test scenario scripts.

Copy this file to vsf.demo/vsf/test/vsf_test/<peripheral>/suite/vsf_test_<suite>.py
and adapt the sections marked with [DELETE IF UNUSED].

Scripts are auto-discovered by vsf-bench. The orchestrator handles triggering;
scripts only validate output.
"""

from vsf_bench.serial import SerialInstrument
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer

SUITE_NAME = "<suite_name>"  # replace with actual suite name


# ---------------------------------------------------------------------------
# [DELETE IF UNUSED]  Pure-serial mode
# Use when the firmware self-reports results and the host only needs to
# confirm the test summary (no logic analyzer required).
# ---------------------------------------------------------------------------
def run(serial: SerialInstrument, la: LogicAnalyzer | None = None) -> None:
    serial.expect_test_summary(SUITE_NAME)


# ---------------------------------------------------------------------------
# [DELETE IF UNUSED]  Serial + Logic Analyzer mode
# Use when the host must stop the LA, decode captures, and assert on
# externally-observed signals (e.g. UART TX line, GPIO toggle).
#
# Uncomment and adapt the decode() signature below. The orchestrator calls
# run() first (triggers firmware + waits for completion), then decode()
# after stopping the LA and merging captures.
# ---------------------------------------------------------------------------
# def decode(
#     la: LogicAnalyzerInstrument,
#     decode_start_ns: int | None = None,
#     decode_end_ns: int | None = None,
# ) -> None:
#     # Example: read decoded CSV rows from a UART channel
#     # rows = la.read_csv_rows(la.output_dir / "uart_decoded.csv")
#     # payload = b"Hello VSF\r\n"
#     # got = bytes(b for t, b in rows)
#     # assert got == payload, f"expected {payload!r}, got {got!r}"
#
#     print(f"[PASS] {SCENE_NAME} decode ok")
