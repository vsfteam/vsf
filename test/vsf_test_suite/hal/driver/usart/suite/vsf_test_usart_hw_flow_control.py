"""usart_hw_flow_control suite host harness.

Firmware uses PL011's internal loopback (LBE) so UARTRTS feeds UARTCTS
inside the chip. Each case configures one of the three flow control
modes (RTS, CTS, RTS+CTS), enables CTS IRQ, then toggles UARTCR.RTS to
generate CTS edges. The firmware asserts cts_count >= 1; no host UART
is needed.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument



def run(serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("usart_hw_flow_control")
