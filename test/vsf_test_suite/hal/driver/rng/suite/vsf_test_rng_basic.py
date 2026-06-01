from vsf_bench import SerialInstrument

def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("rng_basic", timeout=5.0)
