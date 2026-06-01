from vsf_bench import SerialInstrument

def run(serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("i2c_slave_fifo", timeout=15.0)
