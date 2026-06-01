"""arch_preempt scenario: verify EDA task priority preemption.

N worker tasks are posted simultaneously at distinct priorities. Each busy-waits
a fixed duration (higher priority = shorter wait) and then prints a completion
marker ``arch_preempt:done id=N``.

If priority preemption is working, the scheduler dispatches the highest-priority
task first regardless of posting order. The host verifies that completion markers
arrive in priority order (id 0 → 1 → 2 → ...).
"""

import re

from vsf_bench import SerialInstrument, LogicAnalyzerInstrument, load_test_params


_ID_RE = re.compile(r"arch_preempt:done id=(\d+)")


def run(serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("arch_preempt", {})
    host_cfg = scenario.get("host", {}) or {}
    worker_count = int(host_cfg.get("worker_count", 2))
    suite_timeout_s = float(scenario.get("suite_timeout_s", 15.0))

    # Collect completion markers and verify they arrive in priority order.
    # Expected: id=0 first (highest prio), id=N-1 last (lowest prio).
    seen: list[int] = []
    for _ in range(worker_count):
        line = serial.expect("arch_preempt:done", timeout=suite_timeout_s)
        m = _ID_RE.search(line)
        if not m:
            raise AssertionError(
                f"arch_preempt: unexpected marker format: {line.strip()}")
        seen.append(int(m.group(1)))

    expected = list(range(worker_count))
    if seen != expected:
        raise AssertionError(
            f"arch_preempt: completion order {seen}, expected {expected} "
            f"(highest priority first)")

    print(f"[PASS] arch_preempt: completion order {seen} matches priority order")

    serial.expect_test_summary("arch_preempt", timeout=suite_timeout_s)
