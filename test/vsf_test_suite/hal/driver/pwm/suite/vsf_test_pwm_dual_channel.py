"""pwm_dual_channel suite host harness."""

from vsf_bench import SerialInstrument


def run(serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("pwm_dual_channel", timeout=10.0)
