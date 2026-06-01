"""LogicAnalyzerInstrument — DSView CLI wrapper for UART capture and decode."""

import csv
import re
import subprocess
import threading
from dataclasses import dataclass
from pathlib import Path


@dataclass
class MarkerEvent:
    case_idx: int
    time_ns: int


class LogicAnalyzerInstrument:
    """Wraps dsview-cli for logic capture and offline UART protocol decode.

    Lifecycle (managed by pipeline.run_test_phase):
        la.start(duration_s)   → launches dsview-cli capture in background
        la.wait()              → blocks until capture finishes (idempotent)

    Decode (called by test script after la.wait()):
        la.decode_markers(...)  → find CASE:N markers in the marker channel CSV
        la.decode_uart(...)     → decode a time window of a DUT channel
        la.parse_uart_csv(...)  → convert decoded CSV to bytes
    """

    def __init__(
        self,
        cli_path: str | Path,
        device: str,
        samplerate: str,
        channels: dict[str, str],
        capture_path: Path,
    ):
        self._cli = Path(cli_path)
        self._device = device
        self._samplerate = samplerate
        # role → CH label, e.g. {"uart0_tx": "CH1", "uart1_rx": "CH2"}
        self._channels = channels
        self._capture_path = capture_path

        self._proc: subprocess.Popen | None = None
        self._thread: threading.Thread | None = None
        self._done = threading.Event()
        self._started = threading.Event()
        self._exit_code: int | None = None
        self._stop_requested = False

    @property
    def output_dir(self) -> Path:
        """Directory containing the capture file; suitable for decode outputs."""
        return self._capture_path.parent

    def channel(self, role: str) -> str:
        """Resolve a logical role to a CH label (e.g. 'uart1_rx' → 'CH2')."""
        if role not in self._channels:
            raise KeyError(f"No channel for role '{role}'. Known: {list(self._channels)}")
        return self._channels[role]

    def start(self, duration_s: float) -> None:
        """Start a live capture to self._capture_path.

        If a capture is already running this call is a no-op.
        """
        if self._proc is not None:
            return

        self._capture_path.parent.mkdir(parents=True, exist_ok=True)
        self._done.clear()
        self._started.clear()
        self._stop_requested = False

        ch_sel = ",".join(
            f"{int(label[2:])}={label}"
            for label in sorted(set(self._channels.values()), key=lambda x: int(x[2:]))
        )

        cmd = [
            str(self._cli),
            "-d", self._device,
            "-c", f"samplerate={self._samplerate}",
            "--time", f"{int(duration_s)}s",
            "-C", ch_sel,
            "-O", "dsl",
            "-o", str(self._capture_path),
        ]

        print(f"[LA] capture start: {' '.join(cmd)}")
        self._proc = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, encoding="utf-8", errors="replace",
            bufsize=1,
        )

        def _drain_stdout(stream):
            """Consume stdout line-by-line so we can see the sampling-started
            tag in real time. Lines are accumulated and printed at the end so
            the existing post-run log shape is preserved.
            """
            assert self._proc is not None
            lines: list[str] = []
            for line in iter(stream.readline, ""):
                lines.append(line)
                if "[CAPTURE] sampling-started" in line and not self._started.is_set():
                    self._started.set()
            return "".join(lines)

        def _wait():
            assert self._proc is not None
            # Read stdout in this thread so we can detect the start tag as it
            # arrives; stderr only matters at the end so .read() is fine.
            stdout = _drain_stdout(self._proc.stdout)
            stderr = self._proc.stderr.read() if self._proc.stderr else ""
            self._proc.wait()
            self._exit_code = self._proc.returncode
            if stdout.strip():
                print(f"[LA] stdout: {stdout.strip()}")
            if stderr.strip():
                print(f"[LA] stderr: {stderr.strip()}")
            # Unblock any waiter even if dsview-cli never emitted the tag
            # (older binaries, immediate failure, …).
            self._started.set()
            self._done.set()

        self._thread = threading.Thread(target=_wait, daemon=True)
        self._thread.start()

    def wait_until_started(self, timeout: float = 5.0) -> None:
        """Block until dsview-cli prints `[CAPTURE] sampling-started`.

        Replaces the fixed 3s sleep that used to follow `start()`.
        Typical wait on the DSLogic is ~1s. If the binary is older and
        never emits the tag, the helper still unblocks when the capture
        process exits (`_wait` sets the flag on cleanup) — at which point
        the next test step will fail loudly on its own, which is the right
        failure mode.
        """
        if self._proc is None:
            raise RuntimeError("LA capture not running; call start() first")
        if not self._started.wait(timeout=timeout):
            raise TimeoutError(
                f"LA did not signal sampling-started within {timeout}s"
            )

    def stop(self) -> None:
        """Gracefully stop an ongoing capture. Idempotent."""
        if self._proc is None or self._done.is_set():
            return
        self._stop_requested = True
        try:
            self._proc.terminate()
        except ProcessLookupError:
            pass
        self._done.wait(timeout=10.0)

    def wait(self, timeout: float = 300.0) -> None:
        """Block until the capture process exits. Idempotent."""
        if self._done.is_set() or self._proc is None:
            return
        if not self._done.wait(timeout=timeout):
            raise TimeoutError(f"LA capture did not finish within {timeout}s")
        if self._exit_code != 0:
            raise RuntimeError(f"dsview-cli capture exited with code {self._exit_code}")
        print(f"[LA] capture done: {self._capture_path}")

    # ------------------------------------------------------------------ decode

    def decode_markers(
        self,
        channel: str,
        baudrate: int,
        pattern: str,
        output_dir: Path | None = None,
        output_csv: Path | None = None,
        start_ns: int | None = None,
        end_ns: int | None = None,
    ) -> list[MarkerEvent]:
        """Decode the marker channel offline and extract case marker events.

        Args:
            channel: CH label of the marker USART channel (e.g. 'CH1').
            baudrate: baud rate of that channel (e.g. 115200).
            pattern: regex with one capture group for case index (e.g. r'CASE:(\\d+)').
            output_dir: directory for intermediate CSV; defaults to capture_path dir.
            output_csv: explicit output CSV path; overrides output_dir if given.
            start_ns / end_ns: optional decode-window bounds (passed to dsview-cli).

        Returns:
            List of MarkerEvent sorted by time_ns.
        """
        if output_csv is None:
            out_dir = output_dir or self._capture_path.parent
            output_csv = out_dir / f"markers_{channel}.csv"
        self._offline_decode(channel, baudrate, start_ns, end_ns, output_csv)

        rows = self._read_csv_rows(output_csv)
        text = ""
        timestamps: list[int] = []
        for time_ns, byte_val in rows:
            text += chr(byte_val)
            timestamps.append(time_ns)

        events: list[MarkerEvent] = []
        for m in re.finditer(pattern, text):
            # case_idx is captured by group(1) when the pattern targets a
            # per-case marker like "CASE:(\d+)"; suite-level markers like
            # "<suite>:END" carry no capture group and get case_idx=-1.
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

    def decode_uart(
        self,
        channel: str,
        baudrate: int,
        start_ns: int | None,
        end_ns: int | None,
        output_csv: Path,
        parity_type: str = "none",
        num_data_bits: int = 8,
        num_stop_bits: float = 1.0,
    ) -> Path:
        """Decode a time window of a DUT channel offline.

        Args:
            channel: CH label (e.g. 'CH2').
            baudrate: baud rate to decode at.
            start_ns: window start in nanoseconds since capture start (None = from beginning).
            end_ns: window end in nanoseconds (None = to end).
            output_csv: path for the decoded CSV output.
            parity_type: UART parity type (none | odd | even | zero | one).
            num_data_bits: number of data bits per frame (default 8).
            num_stop_bits: number of stop bits (default 1.0).

        Returns:
            output_csv path.
        """
        self._offline_decode(
            channel, baudrate, start_ns, end_ns, output_csv,
            parity_type=parity_type,
            num_data_bits=num_data_bits,
            num_stop_bits=num_stop_bits,
        )
        return output_csv

    def parse_uart_csv(self, csv_path: Path) -> bytes:
        """Parse a decoded UART CSV and return the byte sequence.

        CSV format from dsview-cli:
          Header: Id,Time[ns],<decoder_name>
          Data bytes (printable ASCII 0x21-0x7E): stored as the character.
          Other bytes: stored as [HH] (uppercase hex).
        """
        rows = self._read_csv_rows(csv_path)
        return bytes(b for _, b in rows)

    def read_csv_rows(self, csv_path: Path) -> list[tuple[int, int]]:
        """Public accessor for decoded UART CSV rows: list of (time_ns, byte_value)."""
        return self._read_csv_rows(csv_path)

    def read_digital_edges(
        self,
        channel: str,
        start_ns: int | None = None,
        end_ns: int | None = None,
    ) -> list[int]:
        """Return absolute edge timestamps (ns) on a digital channel.

        Internally invokes dsview-cli with the sigrok ``timing`` protocol
        decoder, which emits one CSV row per edge with that edge's absolute
        timestamp. The decoder reports BOTH rising and falling edges; callers
        that care about period (rather than edge count) can pair consecutive
        timestamps directly without worrying about which polarity they got.

        Args:
            channel: channel label (e.g. ``"CH4"``).
            start_ns: window start in nanoseconds since capture start
                (``None`` = from beginning).
            end_ns: window end in nanoseconds (``None`` = to end).

        Returns:
            Sorted list of edge timestamps in nanoseconds.
        """
        output_csv = self.output_dir / f"edges_{channel}.csv"
        output_csv.parent.mkdir(parents=True, exist_ok=True)
        cmd = [
            str(self._cli),
            "-i", str(self._capture_path),
            "-P", f"timing:data={channel}",
            "--decode-output", str(output_csv),
        ]
        if start_ns is not None:
            cmd += ["--decode-start", self._ns_to_time_str(start_ns)]
        if end_ns is not None:
            cmd += ["--decode-end", self._ns_to_time_str(end_ns)]

        print(f"[LA] read_digital_edges: {' '.join(cmd)}")
        result = subprocess.run(
            cmd, capture_output=True, text=True,
            encoding="utf-8", errors="replace",
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"dsview-cli timing decode failed (exit {result.returncode})\n"
                f"stderr: {result.stderr}"
            )

        edges: list[int] = []
        with open(output_csv, newline="", encoding="utf-8") as f:
            reader = csv.reader(f)
            next(reader, None)  # skip header
            for row in reader:
                if len(row) < 2:
                    continue
                try:
                    edges.append(int(float(row[1])))
                except (ValueError, IndexError):
                    continue
        return edges

    def batch_decode_uart(
        self,
        specs: list,
    ) -> None:
        """Decode the capture at multiple UART configurations in one dsview-cli call.

        Each spec is (channel, baudrate, start_ns, end_ns, output_csv,
        parity_type, num_data_bits, num_stop_bits). dsview-cli applies a single
        time window to every decoder, so all specs must share the same
        (start_ns, end_ns); the window comes from the first spec.
        """
        if not specs:
            return
        first_start, first_end = specs[0][2], specs[0][3]
        for s in specs:
            if s[2] != first_start or s[3] != first_end:
                raise ValueError(
                    "batch_decode_uart requires all specs to share the same time window; "
                    "split into multiple batches if they differ"
                )

        cmd = [str(self._cli), "-i", str(self._capture_path)]
        for channel, baud, _start, _end, out_csv, parity, data_bits, stop_bits in specs:
            out_csv.parent.mkdir(parents=True, exist_ok=True)
            protocol = (
                f"uart:rx={channel}:baudrate={baud}"
                f":parity_type={parity}"
                f":num_data_bits={data_bits}"
                f":num_stop_bits={stop_bits}"
            )
            cmd += ["-P", protocol, "--decode-output", str(out_csv)]
        if first_start is not None:
            cmd += ["--decode-start", self._ns_to_time_str(first_start)]
        if first_end is not None:
            cmd += ["--decode-end", self._ns_to_time_str(first_end)]

        print(f"[LA] batch decode ({len(specs)} decoders)")
        result = subprocess.run(
            cmd, capture_output=True, text=True,
            encoding="utf-8", errors="replace",
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"dsview-cli batch decode failed (exit {result.returncode})\n"
                f"stderr: {result.stderr}"
            )

    # ---------------------------------------------------------------- internal

    def _ns_to_time_str(self, ns: int) -> str:
        ms = ns // 1_000_000
        return f"{ms}ms"

    def _offline_decode(
        self,
        channel: str,
        baudrate: int,
        start_ns: int | None,
        end_ns: int | None,
        output_csv: Path,
        parity_type: str = "none",
        num_data_bits: int = 8,
        num_stop_bits: float = 1.0,
    ) -> None:
        output_csv.parent.mkdir(parents=True, exist_ok=True)
        protocol = (
            f"uart:rx={channel}:baudrate={baudrate}"
            f":parity_type={parity_type}"
            f":num_data_bits={num_data_bits}"
            f":num_stop_bits={num_stop_bits}"
        )
        cmd = [
            str(self._cli),
            "-i", str(self._capture_path),
            "-P", protocol,
            "--decode-output", str(output_csv),
        ]
        if start_ns is not None:
            cmd += ["--decode-start", self._ns_to_time_str(start_ns)]
        if end_ns is not None:
            cmd += ["--decode-end", self._ns_to_time_str(end_ns)]

        print(f"[LA] decode: {' '.join(cmd)}")
        result = subprocess.run(
            cmd, capture_output=True, text=True,
            encoding="utf-8", errors="replace",
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"dsview-cli decode failed (exit {result.returncode})\n"
                f"stderr: {result.stderr}"
            )

    def _read_csv_rows(self, csv_path: Path) -> list[tuple[int, int]]:
        """Read decoded UART CSV; return list of (time_ns, byte_value)."""
        if not csv_path.exists():
            raise FileNotFoundError(f"Decoded CSV not found: {csv_path}")

        result = []
        with open(csv_path, newline="", encoding="utf-8") as f:
            reader = csv.reader(f)
            next(reader)  # skip header
            for row in reader:
                if len(row) < 3:
                    continue
                time_ns = int(float(row[1]))
                text = row[2].strip()
                if not text:
                    continue
                byte_val = self._parse_csv_cell(text)
                if byte_val is not None:
                    result.append((time_ns, byte_val))
        return result

    @staticmethod
    def _parse_csv_cell(text: str) -> int | None:
        """Parse one CSV cell to a byte value.

        Printable ASCII (stored as single char): return ord(char).
        Non-printable (stored as [HH]): return int(HH, 16).
        Anything else (annotations like 'start'/'stop'): return None.
        """
        if len(text) == 1 and 33 <= ord(text) <= 126:
            return ord(text)
        if len(text) == 4 and text[0] == "[" and text[3] == "]":
            try:
                return int(text[1:3], 16)
            except ValueError:
                return None
        return None
