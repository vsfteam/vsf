"""i2c_eeprom_page scenario: read/write EEPROM across page boundaries via I2C.

Firmware performs I2C EEPROM page-boundary write and verifies data internally.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("i2c_eeprom_page", timeout=10.0)
