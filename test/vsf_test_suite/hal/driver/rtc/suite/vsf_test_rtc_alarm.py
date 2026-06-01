"""rtc_alarm suite host harness."""

from vsf_bench import SerialInstrument


def run(serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("rtc_alarm", timeout=10.0)
