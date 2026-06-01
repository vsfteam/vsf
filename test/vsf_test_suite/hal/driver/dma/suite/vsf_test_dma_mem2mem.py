"""dma_mem2mem scenario: DMA memory-to-memory transfer verification.

Firmware allocates src/dst buffers, acquires a DMA channel, configures
memory-to-memory mode, starts transfer, polls for completion, and
asserts data integrity.

No host-side serial interaction required — this is an internal test.
"""

from vsf_bench import SerialInstrument, load_test_params




def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("dma_mem2mem", {})
    timeout_s = float(scenario.get("timeout_s", 10.0))

    serial.expect_test_summary("dma_mem2mem", timeout=timeout_s)
