"""timer_async suite host harness."""

from vsf_bench import SerialInstrument


def run(serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("timer_async", timeout=10.0)
