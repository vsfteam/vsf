"""i2c_eeprom_rw scenario: read/write EEPROM via I2C.

Firmware performs I2C EEPROM read/write and verifies data internally.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("i2c_eeprom_rw", timeout=10.0)
