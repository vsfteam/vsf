"""flash_erase_program_read scenario: erase, program, and read back flash.

Firmware performs erase → verify 0xFF → write pattern → verify pattern
internally. This script waits for the test summary.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("flash_erase_program_read", timeout=10.0)
