"""DSViewAdapter — LogicAnalyzer implementation via dsview-cli subprocess."""

import csv
import subprocess
import threading
from pathlib import Path

from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.config.models import UARTConfig


class DSViewAdapter(LogicAnalyzer):
    """Logic analyzer backed by a DSLogic device and the dsview-cli binary.

    Parameters:
        cli_path: path to ``dsview-cli`` executable.
        device: device name (e.g. ``"DSLogic"``).
        samplerate: sample rate string (e.g. ``"10M"``).
        channels: ``{role: CH_label}`` mapping for channel selection at
                  capture time.
    """

    def __init__(
        self,
        cli_path: str | Path,
        device: str,
        samplerate: str,
        channels: dict[str, str],
    ):
        self._cli = Path(cli_path)
        self._device = device
        self._samplerate = samplerate
        self._channels = channels

        self._proc: subprocess.Popen | None = None
        self._thread: threading.Thread | None = None
        self._done = threading.Event()
        self._started = threading.Event()
        self._exit_code: int | None = None

    # ------------------------------------------------------------------ capture

    def start(self, capture_path: Path, duration_s: float) -> None:
        if self._proc is not None:
            return

        capture_path.parent.mkdir(parents=True, exist_ok=True)
        self._done.clear()
        self._started.clear()

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
            "-o", str(capture_path),
        ]

        print(f"[LA] capture start: {' '.join(cmd)}")
        self._proc = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, encoding="utf-8", errors="replace", bufsize=1,
        )

        def _drain_stdout(stream):
            assert self._proc is not None
            lines: list[str] = []
            for line in iter(stream.readline, ""):
                lines.append(line)
                if "[CAPTURE] sampling-started" in line and not self._started.is_set():
                    self._started.set()
            return "".join(lines)

        def _wait():
            assert self._proc is not None
            stdout = _drain_stdout(self._proc.stdout)
            stderr = self._proc.stderr.read() if self._proc.stderr else ""
            self._proc.wait()
            self._exit_code = self._proc.returncode
            if stdout.strip():
                print(f"[LA] stdout: {stdout.strip()}")
            if stderr.strip():
                print(f"[LA] stderr: {stderr.strip()}")
            self._started.set()
            self._done.set()

        self._thread = threading.Thread(target=_wait, daemon=True)
        self._thread.start()

    def wait_until_started(self, timeout: float = 5.0) -> None:
        if self._proc is None:
            raise RuntimeError("LA capture not running; call start() first")
        if not self._started.wait(timeout=timeout):
            raise TimeoutError(f"LA did not signal sampling-started within {timeout}s")

    def stop(self) -> None:
        if self._proc is None or self._done.is_set():
            return
        try:
            self._proc.terminate()
        except ProcessLookupError:
            pass
        self._done.wait(timeout=10.0)

    def wait(self, timeout: float = 300.0) -> None:
        if self._done.is_set() or self._proc is None:
            return
        if not self._done.wait(timeout=timeout):
            raise TimeoutError(f"LA capture did not finish within {timeout}s")
        if self._exit_code != 0:
            raise RuntimeError(f"dsview-cli capture exited with code {self._exit_code}")
        print(f"[LA] capture done: {self._proc}")  # capture_path logged by caller

    # ------------------------------------------------------------------ decode

    def decode_uart(
        self,
        capture_path: Path,
        channel: str,
        config: UARTConfig,
        start_ns: int | None = None,
        end_ns: int | None = None,
    ) -> Path:
        tag = f"{channel}_{config.baudrate}"
        if start_ns is not None or end_ns is not None:
            s = start_ns or 0
            e = end_ns or 0
            tag += f"_{s}_{e}"
        output_csv = capture_path.parent / f"uart_decode_{tag}.csv"
        output_csv.parent.mkdir(parents=True, exist_ok=True)

        protocol = (
            f"uart:rx={channel}:baudrate={config.baudrate}"
            f":parity_type={config.parity_type}"
            f":num_data_bits={config.num_data_bits}"
            f":num_stop_bits={config.num_stop_bits}"
        )
        cmd = [
            str(self._cli),
            "-i", str(capture_path),
            "-P", protocol,
            "--decode-output", str(output_csv),
        ]
        if start_ns is not None:
            cmd += ["--decode-start", _ns_to_time_str(start_ns)]
        if end_ns is not None:
            cmd += ["--decode-end", _ns_to_time_str(end_ns)]

        print(f"[LA] decode: {' '.join(cmd)}")
        result = subprocess.run(cmd, capture_output=True, text=True,
                                encoding="utf-8", errors="replace")
        if result.returncode != 0:
            raise RuntimeError(
                f"dsview-cli decode failed (exit {result.returncode})\n"
                f"stderr: {result.stderr}"
            )
        return output_csv

    def read_digital_edges(
        self,
        capture_path: Path,
        channel: str,
        start_ns: int | None = None,
        end_ns: int | None = None,
    ) -> list[int]:
        output_csv = capture_path.parent / f"edges_{channel}.csv"
        output_csv.parent.mkdir(parents=True, exist_ok=True)
        cmd = [
            str(self._cli),
            "-i", str(capture_path),
            "-P", f"timing:data={channel}",
            "--decode-output", str(output_csv),
        ]
        if start_ns is not None:
            cmd += ["--decode-start", _ns_to_time_str(start_ns)]
        if end_ns is not None:
            cmd += ["--decode-end", _ns_to_time_str(end_ns)]

        print(f"[LA] read_digital_edges: {' '.join(cmd)}")
        result = subprocess.run(cmd, capture_output=True, text=True,
                                encoding="utf-8", errors="replace")
        if result.returncode != 0:
            raise RuntimeError(
                f"dsview-cli timing decode failed (exit {result.returncode})\n"
                f"stderr: {result.stderr}"
            )

        edges: list[int] = []
        with open(output_csv, newline="", encoding="utf-8") as f:
            reader = csv.reader(f)
            next(reader, None)
            for row in reader:
                if len(row) < 2:
                    continue
                try:
                    edges.append(int(float(row[1])))
                except (ValueError, IndexError):
                    continue
        return edges


def _ns_to_time_str(ns: int) -> str:
    ms = ns // 1_000_000
    return f"{ms}ms"
