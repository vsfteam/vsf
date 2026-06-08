"""VsfTestShellProtocol — 固件 vsf-test shell 串口协议封装。

从 pipeline.py 提取纯 shell 对话逻辑（字符串级别的命令/响应），
不包含测试编排、日志、LA 等 pipeline 职责。

Usage::

    from vsf_bench.vsf_test.shell import VsfTestShellProtocol
    shell = VsfTestShellProtocol(ser)
    suites = shell.query_firmware_suites()
    shell.wait_for_shell_ready()
"""

import time

from vsf_bench.utils.serial import SerialInstrument


class VsfTestShellProtocol:
    """vs-test firmware shell 协议 — 命令发送、响应解析、REPL 管理。

    所有方法通过 *ser* (SerialInstrument) 和固件对话。
    调用方持有 SerialInstrument 的生命周期。
    """

    def __init__(self, ser: SerialInstrument):
        self._ser = ser

    # ── 固件查询 ──────────────────────────────────────────

    def query_firmware_suites(self) -> set[str]:
        """发送 ``vsf-test list-suites``，解析返回的 suite 名列表。"""
        self._ser.send("vsf-test list-suites\r\n")
        time.sleep(0.3)
        output = self._ser.read_all(timeout=2.0)
        suites: set[str] = set()
        for line in output.splitlines():
            line = line.strip()
            if line and line[0].isdigit():
                parts = line.split(None, 1)
                if len(parts) == 2:
                    suites.add(parts[1])
        return suites

    # ── REPL 管理 ─────────────────────────────────────────

    def drain_repl(self) -> None:
        """丢弃串口缓冲区中的残留数据。"""
        self._ser.read_all(timeout=0.1)

    def wait_for_shell_ready(self, timeout: float = 2.0) -> bool:
        """等待 ``> `` 提示符出现，最多 3 次尝试。返回是否成功。"""
        self.drain_repl()
        self._ser.send("\r\n")
        for _attempt in range(3):
            try:
                self._ser.expect("> ", timeout=timeout)
                return True
            except TimeoutError:
                self.drain_repl()
                self._ser.send("\r\n")
        return False

    # ── trace level ───────────────────────────────────────

    def set_trace_level(self, level: str) -> bool:
        """发送 ``vsf-test trace-level <level>`` 并等待确认。"""
        self._ser.send(f"vsf-test trace-level {level}\r\n")
        try:
            self._ser.expect("trace-level set:", timeout=1.0)
            return True
        except TimeoutError:
            # ack 可能混在下一个 prompt 里
            self._ser.read_all(timeout=0.3)
            return True  # 保守视为成功

    # ── suite 命令构建 ────────────────────────────────────

    @staticmethod
    def build_run_cmd(suite: str, case: str | None = None) -> str:
        """构建 ``vsf-test run-suite`` 或 ``vsf-test run-case`` 命令。"""
        if case:
            return f"vsf-test run-case {suite} {case}\r\n"
        return f"vsf-test run-suite {suite}\r\n"

    # ── suite ack ─────────────────────────────────────────

    SUITE_ACK_PATTERN = (
        r"\[vsf-test\](?: \[\d+\.\d+ s\])? suite ack:"
        r"|"
        r"\[vsf-test\](?: \[\d+\.\d+ s\])? suite not found:"
        r"|"
        r"\[vsf-test\](?: \[\d+\.\d+ s\])? case not found:"
    )

    def expect_suite_ack(self, timeout: float = 1.0) -> str | None:
        """等待 suite ack / not-found 响应。超时返回 None。"""
        try:
            return self._ser.expect(self.SUITE_ACK_PATTERN, timeout=timeout)
        except TimeoutError:
            return None

    # ── shuffle seed (no-op, 向后兼容) ────────────────────

    def send_shuffle_seed(self, suite_name: str, seed: int) -> bool:
        """Shuffle seed 通知（当前固件不支持，host 端处理）。"""
        # 向后兼容 CLI --random --seed 参数
        return True
