"""Case Window slicer for scenario decoders.

Owns the Capture Marker protocol: which marker types fire, which is start
vs end of each case, how the per-case `[start_ns, end_ns)` window is built.

`read_framework_windows()` handles both TX and RX scenarios:
  * TX: windows bounded by `CASE:N` → next `CASE:N+1` (or `END` for last)
  * RX: when READY markers are present, windows bounded by `CASE:N:ready`
    → `CASE:N:done`; otherwise falls back to CASE:N → next CASE/END.
"""

import re
from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import (
    LogicAnalyzerInstrument,
    MarkerEvent,
)


@dataclass(frozen=True)
class CaseWindow:
    case_idx: int
    start_ns: int
    end_ns: int


# Cache decoded marker channel data so multiple suites (and multiple
# marker-pattern searches per suite) share a single dsview-cli call.
# Key: (capture_path, channel, baudrate, start_ns, end_ns)
_marker_cache: dict = {}


def _find_markers(
    text: str,
    timestamps: list[int],
    pattern: str,
) -> list[MarkerEvent]:
    """Search decoded marker text for a regex pattern and return events."""
    events: list[MarkerEvent] = []
    for m in re.finditer(pattern, text):
        if m.groups():
            try:
                case_idx = int(m.group(1))
            except (ValueError, IndexError):
                case_idx = -1
        else:
            case_idx = -1
        time_ns = timestamps[m.start()]
        events.append(MarkerEvent(case_idx=case_idx, time_ns=time_ns))
    events.sort(key=lambda e: e.time_ns)
    return events


def _read_marker_channel(
    la: LogicAnalyzerInstrument,
    channel: str,
    marker_baud: int,
    decode_start_ns: int | None,
    decode_end_ns: int | None,
) -> tuple[str, list[int]]:
    """Decode the marker channel once and cache the result.

    Returns (decoded_text, timestamps) where decoded_text is the ASCII
    concatenation of all decoded bytes and timestamps is the parallel list
    of absolute nanosecond timestamps.
    """
    key = (str(la._capture_path), channel, marker_baud, decode_start_ns, decode_end_ns)
    if key in _marker_cache:
        return _marker_cache[key]

    out_dir = la.output_dir
    csv_path = out_dir / f"markers_{channel}_all.csv"
    la.decode_uart(channel, marker_baud, decode_start_ns, decode_end_ns, csv_path)
    rows = la.read_csv_rows(csv_path)

    text = ""
    timestamps: list[int] = []
    for time_ns, byte_val in rows:
        text += chr(byte_val)
        timestamps.append(time_ns)

    _marker_cache[key] = (text, timestamps)
    return text, timestamps


def read_framework_windows(
    la: LogicAnalyzerInstrument,
    suite_name: str,
    decode_start_ns: int | None = None,
    decode_end_ns: int | None = None,
    marker_baud: int = 115200,
    marker_channel: str = "uart0_tx",  # configurable per-board via logic_analyzer config
) -> list[CaseWindow]:
    """Per-case windows for both TX and RX scenarios.

    If READY markers are found for this suite, windows are bounded by
    READY → DONE. Otherwise, windows are bounded by CASE:N → CASE:N+1
    (or END for the last case).

    *marker_channel* can be configured per-board via hardware-map.yml
    ``logic_analyzer.marker_channel``.  Decoded once per unique capture
    window and cached.
    """
    ch = la.channel(marker_channel)
    text, timestamps = _read_marker_channel(
        la, ch, marker_baud, decode_start_ns, decode_end_ns
    )

    starts = _find_markers(
        text, timestamps, rf"{suite_name}:case:(\d+)(?![\d:])"
    )
    readys = _find_markers(
        text, timestamps, rf"{suite_name}:case:(\d+):ready"
    )
    dones = _find_markers(
        text, timestamps, rf"{suite_name}:case:(\d+):done"
    )
    ends = _find_markers(
        text, timestamps, rf"{suite_name}:end"
    )

    if not starts:
        raise RuntimeError(f"{suite_name}: no CASE start markers found in LA decode")
    starts_sorted = sorted(starts, key=lambda e: e.time_ns)
    end_ns = ends[-1].time_ns if ends else None

    # If any READY markers were found for this suite, use READY→DONE windows.
    use_ready = len(readys) > 0
    ready_by_case = {ev.case_idx: ev.time_ns for ev in readys}
    done_by_case = {ev.case_idx: ev.time_ns for ev in dones}

    windows: list[CaseWindow] = []
    for i, ev in enumerate(starts_sorted):
        case_idx = ev.case_idx
        if use_ready:
            if case_idx not in ready_by_case:
                raise RuntimeError(
                    f"{suite_name}: case {case_idx} missing READY marker"
                )
            if case_idx not in done_by_case:
                raise RuntimeError(
                    f"{suite_name}: case {case_idx} has READY but no DONE marker"
                )
            s = ready_by_case[case_idx]
            e = done_by_case[case_idx]
        else:
            s = ev.time_ns
            if i + 1 < len(starts_sorted):
                e = starts_sorted[i + 1].time_ns
            elif end_ns is not None:
                e = end_ns
            else:
                raise RuntimeError(
                    f"{suite_name}: last case has no upper bound "
                    f"(missing `{suite_name}:end` marker in LA decode)"
                )
        windows.append(CaseWindow(case_idx=case_idx, start_ns=s, end_ns=e))
    return windows
