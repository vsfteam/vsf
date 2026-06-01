"""usart_request_cancel suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

TX via fifo2req adapter; re-uses UART1.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("usart_request_cancel")
