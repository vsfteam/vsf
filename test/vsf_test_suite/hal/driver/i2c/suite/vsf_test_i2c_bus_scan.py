"""i2c_bus_scan scenario: scan I2C bus for devices.

Firmware scans the I2C bus and reports found device addresses.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("i2c_bus_scan", timeout=10.0)
