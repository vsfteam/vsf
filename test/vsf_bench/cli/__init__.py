"""CLI entry points for vsf-bench.

* `cli.run`   — vsf-bench (unified build/flash/test pipeline)
* `cli.build` — vsf-bench-build (standalone build)
* `cli.flash` — vsf-bench-flash (standalone flash)
* `cli.test`  — vsf-bench-test (standalone test, assumes firmware already running)

The standalone scripts and the unified pipeline share their phase logic
through `vsf_bench.pipeline` and `vsf_bench.suite` modules — there is one
implementation per concern, two entry surfaces.
"""
