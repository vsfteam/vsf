"""usart_tx_fifo_irq suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

TX side only — re-uses UART1 wiring already in hardware-map.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("usart_tx_fifo_irq")
