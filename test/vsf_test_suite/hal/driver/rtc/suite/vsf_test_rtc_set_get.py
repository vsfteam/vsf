"""rtc_set_get suite host harness."""

from vsf_bench import SerialInstrument


def run(serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("rtc_set_get", timeout=10.0)
