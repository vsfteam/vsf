"""dma_scatter_gather scenario: Verify DMA scatter-gather M2M transfers.

Tests three scatter-gather patterns using RP2040 DMA with ISR chaining:
1. Two-segment M2M
2. Scatter read (non-contiguous src -> contiguous dst)
3. Gather write (contiguous src -> non-contiguous dst)
"""

from vsf_bench import SerialInstrument, load_test_params


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("dma_scatter_gather", {})
    timeout_s = float(scenario.get("timeout_s", 10.0))

    serial.expect_test_summary("dma_scatter_gather", timeout=timeout_s)
