"""CLI entry points for vsf-bench.

* `cli.run`     — vsf-bench (unified build/program/test pipeline)
* `cli.build`   — vsf-bench-build (standalone build)
* `cli.program`  — vsf-bench-program (standalone program)
* `cli.test`    — vsf-bench-test (standalone test, assumes firmware already running)

The standalone scripts and the unified pipeline share their phase logic
through `vsf_bench.pipeline` and `vsf_bench.suite` modules — there is one
implementation per concern, two entry surfaces.
"""
