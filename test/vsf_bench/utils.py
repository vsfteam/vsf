"""Small utilities shared by test scripts and the harness."""

import time
from typing import Callable

from vsf_bench.instruments.serial_instrument import SuiteTimeoutError


def run_with_suite_timeout(
    fn: Callable, timeout_s: float, *args, **kwargs
):
    """Call `fn(*args, **kwargs)` and enforce a suite-level deadline.

    If `timeout_s` > 0 and `fn` has not returned within `timeout_s` seconds,
    raises `SuiteTimeoutError`.

    Returns whatever `fn` returns.
    """
    if timeout_s <= 0:
        return fn(*args, **kwargs)

    deadline = time.monotonic() + timeout_s
    result = fn(*args, **kwargs)
    if time.monotonic() > deadline:
        raise SuiteTimeoutError(
            f"Suite exceeded {timeout_s}s deadline (suite_timeout_s)"
        )
    return result
