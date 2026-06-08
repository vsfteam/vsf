"""spi_async suite host harness.

Firmware initializes SPI0 in master mode with MOSI-MISO loopback jumper,
tests async request_transfer (full-duplex, TX-only, RX-only) and cancel.
Host script checks the test summary.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument


def run(serial: SerialInstrument,
        adapter: LogicAnalyzer | None = None) -> None:
    serial.expect_test_summary("spi_async")


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    pass
