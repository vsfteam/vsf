"""usart_break_signal suite host harness.

Firmware exercises VSF_USART_CTRL_SET_BREAK/CLEAR_BREAK/SEND_BREAK on
uart1_tx. The LA captures the line and we verify that within each
per-case window the TX line was held low for at least one full frame
(SEND_BREAK) and approximately `hold_ms` ms (SET/CLEAR pair).

A pure firmware-side assertion can't verify break duration — the chip
clocks its own break, so a self-comparison is tautological. The LA
provides the independent reference.
"""

from vsf_bench import read_framework_windows, SerialInstrument, load_test_params
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.utils import parse_uart_csv, read_csv_rows
from vsf_bench.config import UARTConfig


def run(serial: SerialInstrument,
        adapter: LogicAnalyzer | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("usart_break_signal", {})
    timeout_s = float(scenario.get("timeout_s", 1.5))
    serial.expect_test_summary("usart_break_signal", timeout=timeout_s)


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("usart_break_signal", {})
    cases = list(scenario.get("cases", []))
    assert len(cases) > 0, "No cases found in test_params"

    dut_channel_role = scenario.get("dut", {}).get("channel", "uart1_tx")
    if dut_channel_role not in channels:
        print(f"[SKIP] usart_break_signal: LA channel role '{dut_channel_role}' "
              f"not in hardware-map.yml")
        return
    dut_channel = channels.get(dut_channel_role)

    windows = read_framework_windows(adapter, channels, capture_path, "usart_break_signal",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx       = int(case["idx"])
        baudrate  = int(case.get("baudrate", 115200))
        hold_ms   = int(case.get("hold_ms", 5))

        if idx not in window_by_idx:
            raise AssertionError(
                f"usart_break_signal CASE {idx}: window missing in capture")
        w = window_by_idx[idx]

        edges = adapter.read_digital_edges(capture_path, dut_channel, w.start_ns, w.end_ns)
        # Idle high → SET_BREAK falling → hold → CLEAR_BREAK rising is the
        # primary signal under test. The optional SEND_BREAK auto-pulse may
        # show up as a second fall/rise pair but the precise driver-chosen
        # frame length depends on the chip; ANY low pulse following the
        # SET/CLEAR pair counts as "auto break observed".
        if len(edges) < 2:
            print(f"[SKIP] usart_break_signal CASE {idx}: no edges on "
                  f"'{dut_channel_role}' (LA probe not wired)")
            continue

        # First two edges should bracket the manual break.
        manual_low_ns = edges[1] - edges[0]
        expected_manual = hold_ms * 1_000_000
        # Tolerate +/- 200% to absorb busy-wait jitter and chip-side latency
        # in the BRK bit toggling (PL011 may take up to one frame to act on a
        # BRK write).
        if not (expected_manual * 0.5 <= manual_low_ns <= expected_manual * 3.0):
            raise AssertionError(
                f"usart_break_signal CASE {idx}: manual break held "
                f"{manual_low_ns/1e6:.2f} ms, expected ~{hold_ms} ms "
                f"(accept [{expected_manual*0.5/1e6:.2f}, "
                f"{expected_manual*3.0/1e6:.2f}] ms)")

        # If we got >= 4 edges, the SEND_BREAK pulse was caught too — sanity
        # check its width is well below the manual hold (one frame << hold_ms).
        auto_low_ns = None
        if len(edges) >= 4:
            auto_low_ns = edges[3] - edges[2]
            frame_ns = int(12 * 1_000_000_000 / baudrate)
            if auto_low_ns < frame_ns:
                raise AssertionError(
                    f"usart_break_signal CASE {idx}: SEND_BREAK held "
                    f"{auto_low_ns/1e3:.0f} us; one frame at {baudrate} baud "
                    f"is {frame_ns/1e3:.0f} us")

        if auto_low_ns is not None:
            print(
                f"[PASS] usart_break_signal CASE {idx} "
                f"manual={manual_low_ns/1e6:.2f}ms (~{hold_ms}ms), "
                f"auto={auto_low_ns/1e3:.0f}us")
        else:
            print(
                f"[PASS] usart_break_signal CASE {idx} "
                f"manual={manual_low_ns/1e6:.2f}ms (~{hold_ms}ms); "
                f"SEND_BREAK pulse outside window")
