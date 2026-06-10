"""Build phase — compile firmware via registered builder."""

from __future__ import annotations

from pathlib import Path

from vsf_bench.builders.registry import get_builder_class
from vsf_bench.utils.tee_logger import get_logger as _get_logger


def build_phase(build_src) -> Path:
    build = getattr(build_src, "build", build_src)
    builder_cls = get_builder_class(build.tool)
    if builder_cls is None:
        raise RuntimeError(f"Unknown build tool: {build.tool}")
    _get_logger().event(f"Building ({build.source_dir}) via {build.tool}...")
    builder = builder_cls(build)
    build_dir = builder.build()
    _get_logger().event(f"Build complete: {build_dir}")
    return build_dir
