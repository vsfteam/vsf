"""adc_stream suite host harness."""

from vsf_bench import SerialInstrument


def run(serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("adc_stream", timeout=15.0)
