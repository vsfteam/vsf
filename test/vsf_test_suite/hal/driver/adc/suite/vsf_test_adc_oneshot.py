"""adc_oneshot scenario: sample ADC channel 0 and verify 12-bit range.

The firmware asserts internally via VSF_TEST_ASSERT; this script waits
for the test framework summary line and asserts all cases passed.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("adc_oneshot")
