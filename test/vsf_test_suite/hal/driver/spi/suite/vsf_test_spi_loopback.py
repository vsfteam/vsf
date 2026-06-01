"""spi_loopback suite host harness.

Firmware initializes SPI0, sends a fixed pattern with MOSI-MISO
looped back, and asserts rx matches tx. Host script just checks
the test summary.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument



def run(serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("spi_loopback")


def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    """Decode SPI signals from LA capture.

    Saves the LA capture for manual inspection. No automated assertion
    since wiring (single-SPI loopback vs dual-SPI cross-connect) is
    board-specific.
    """
    pass
