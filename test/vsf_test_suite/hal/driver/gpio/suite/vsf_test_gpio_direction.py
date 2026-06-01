"""gpio_direction suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Direction round-trip; no external wiring needed.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument


def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("gpio_direction")
