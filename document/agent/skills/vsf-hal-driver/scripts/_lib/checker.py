"""Shared infrastructure for VSF HAL driver checkers."""

from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

import tree_sitter_c as tsc
from tree_sitter import Language, Parser as TSParser, Node


# ---------------------------------------------------------------- tree-sitter init

_PARSER: TSParser | None = None


def _parser() -> TSParser:
    global _PARSER
    if _PARSER is None:
        _PARSER = TSParser(Language(tsc.language()))
    return _PARSER


# ---------------------------------------------------------------- exit codes

EXIT_PASS = 0
EXIT_ERROR = 1
EXIT_WARNING = 2


# ---------------------------------------------------------------- findings

@dataclass
class Finding:
    file: Path
    line: int
    rule_id: str
    message: str
    reference: str | None = None
    severity: str = "error"  # "error" or "warn"

    def render(self) -> str:
        ref = f"  (see REFERENCE.md: {self.reference})" if self.reference else ""
        sev_tag = "WARN" if self.severity == "warn" else "FAIL"
        return f"{self.file}:{self.line}: [{self.rule_id}] {sev_tag}: {self.message}{ref}"


# ---------------------------------------------------------------- scan line context

@dataclass
class ScanLine:
    lineno: int
    text: str
    in_imp_lv0: bool       # inside a #define ..._IMP_LV0 multi-line macro
    in_comment: bool       # inside a /* */ block comment OR #if 0 block
    in_string_literal: bool  # inside a "..." string literal
    suppress: set[str]     # rule ids suppressed for this line


_SUPPRESS_RE = re.compile(r"//\s*quality:\s*allow-([a-z][a-z0-9-]*)")


# ---------------------------------------------------------------- preprocess

def _collect_line_ranges(root: Node, node_type: str) -> set[int]:
    """Collect all line numbers (1-based) covered by nodes of *node_type*."""
    lines: set[int] = set()
    for node in _walk_all(root):
        if node.type == node_type:
            for ln in range(node.start_point[0] + 1, node.end_point[0] + 2):
                lines.add(ln)
    return lines


def preprocess(text: str) -> list[ScanLine]:
    """Tag each line with context derived from the tree-sitter CST."""
    tree = _parser().parse(text.encode(), encoding="utf8")
    root = tree.root_node
    lines = text.splitlines()

    comment_lines: set[int] = set()

    def _add_range(node: Node):
        for ln in range(node.start_point[0] + 1, node.end_point[0] + 2):
            comment_lines.add(ln)

    for node in _walk_all(root):
        if node.type == "comment":
            _add_range(node)
        elif node.type == "preproc_if":
            for child in node.children:
                if child.type in ("number_literal", "false"):
                    if child.text == b"0" or child.text == b"false":
                        _add_range(node)
                    break
                elif child.type in ("identifier", "true"):
                    break

    imp_lv0_lines: set[int] = set()
    for node in _walk_all(root):
        if node.type in ("preproc_def", "preproc_function_def"):
            if "_IMP_LV0" in node.text.decode():
                for ln in range(node.start_point[0] + 1, node.end_point[0] + 2):
                    imp_lv0_lines.add(ln)

    string_literal_lines = _collect_line_ranges(root, "string_literal")

    result: list[ScanLine] = []
    for idx, raw in enumerate(lines, start=1):
        suppress = set(_SUPPRESS_RE.findall(raw))
        result.append(ScanLine(
            lineno=idx,
            text=raw,
            in_imp_lv0=idx in imp_lv0_lines,
            in_comment=idx in comment_lines,
            in_string_literal=idx in string_literal_lines,
            suppress=suppress,
        ))
    return result


# ---------------------------------------------------------------- extract_functions

_VSF_MCONNECT_NAME_RE = re.compile(
    r'VSF_MCONNECT\s*\([^)]*,\s*(_\w+)\)', re.DOTALL
)


def extract_functions(text: str) -> list[dict]:
    """Extract top-level C function definitions via tree-sitter AST.

    Returns a list of dicts with keys:
        name, body, start_line, end_line, lines
    """
    tree = _parser().parse(text.encode(), encoding="utf8")
    root = tree.root_node
    lines = text.splitlines()
    funcs: list[dict] = []

    for node in _walk_all(root):
        if node.type != "function_definition":
            continue

        name = _extract_function_name(node)
        start_line = node.start_point[0] + 1
        end_line = node.end_point[0] + 1
        body_lines = lines[start_line - 1:end_line]

        funcs.append({
            "name": name,
            "body": "\n".join(body_lines),
            "start_line": start_line,
            "end_line": end_line,
            "lines": body_lines,
        })
    return funcs


def _extract_function_name(node: Node) -> str:
    sig_flat = node.text.decode().replace("\n", " ")

    m = _VSF_MCONNECT_NAME_RE.search(sig_flat)
    if m:
        return m.group(1)

    def _find_declarator_id(n: Node) -> str | None:
        if n.type == "function_declarator":
            for cc in n.children:
                if cc.type == "identifier":
                    return cc.text.decode()
                elif cc.type == "field_identifier":
                    return cc.text.decode()
                elif cc.type == "parenthesized_declarator":
                    for ccc in cc.children:
                        if ccc.type == "field_identifier":
                            return ccc.text.decode()
        for child in n.children:
            result = _find_declarator_id(child)
            if result:
                return result
        return None

    name = _find_declarator_id(node)
    if name:
        return name

    m = re.search(r'(\w+)\s*\(', sig_flat)
    return m.group(1) if m else sig_flat[:60].strip()


# ---------------------------------------------------------------- quality rule helpers


def emit(lines: Iterable[ScanLine], rule_id: str, message: str,
         predicate, reference: str | None = None) -> list[Finding]:
    """Yield a finding for every line where `predicate(line)` is truthy and the
    rule is not inline-suppressed."""
    out: list[Finding] = []
    for sl in lines:
        if sl.in_comment or sl.in_string_literal or rule_id in sl.suppress:
            continue
        if predicate(sl):
            out.append(Finding(Path(""), sl.lineno, rule_id, message, reference))
    return out


# ---------------------------------------------------------------- structured check helpers


class ResultAccumulator:
    """Collects OK/FAIL/WARN messages and tracks counts for structured checks."""

    def __init__(self):
        self.errors = 0
        self.warnings = 0

    def say(self, kind: str, msg: str):
        if kind == "OK":
            print(f"  OK: {msg}")
        elif kind == "FAIL":
            print(f"  FAIL: {msg}")
            self.errors += 1
        else:
            print(f"  WARN: {msg}")
            self.warnings += 1

    def finalize(self, label: str = "") -> int:
        print()
        if self.errors:
            print(f"FAIL: {self.errors} essential check(s) failed")
            return EXIT_ERROR
        elif self.warnings:
            print(f"PASS: all essential checks passed ({self.warnings} warnings)")
            return EXIT_WARNING
        else:
            print("PASS: all checks passed")
            return EXIT_PASS


# ---------------------------------------------------------------- YAML pattern rule engine


def load_pattern_rules(rules_yml: str | Path) -> list[dict]:
    """Load pattern-based quality rules from a YAML file."""
    import yaml  # deferred — only import when rules are actually loaded
    with open(rules_yml, encoding="utf-8") as f:
        data = yaml.safe_load(f)
    return data.get("rules", [])


def check_pattern_rules(lines: list[ScanLine], rules: list[dict],
                        filepath: Path) -> list[Finding]:
    """Apply pattern-based rules to preprocessed ScanLines.

    Each rule is a dict with:
        id, patterns (list of regex strings), skip_in_imp_lv0 (bool),
        message, reference (optional)
    """
    compiled: list[dict] = []
    for rule in rules:
        compiled.append({
            "id": rule["id"],
            "res": [re.compile(p) for p in rule["patterns"]],
            "skip_lv0": rule.get("skip_in_imp_lv0", True),
            "message": rule["message"],
            "reference": rule.get("reference"),
        })

    findings: list[Finding] = []
    for rule in compiled:
        for sl in lines:
            if sl.in_comment or sl.in_string_literal or rule["id"] in sl.suppress:
                continue
            if rule["skip_lv0"] and sl.in_imp_lv0:
                continue
            for r in rule["res"]:
                if r.search(sl.text):
                    findings.append(Finding(
                        filepath, sl.lineno, rule["id"],
                        rule["message"], rule["reference"],
                    ))
                    break
    return findings


def check_ast_pattern_rules(text: str, rules: list[dict],
                            filepath: Path) -> list[Finding]:
    """Apply pattern-based rules to specific AST node types via tree-sitter.

    Each rule may specify:
        node_type:   str or list of AST node type names to traverse
        patterns:    list of regex strings matched against node.text.decode()
        skip_in_imp_lv0: bool (default True)
        message, reference: standard Finding fields
    """
    tree = _parser().parse(text.encode(), encoding="utf8")
    root = tree.root_node

    # Collect line-level suppressions
    suppress: dict[int, set[str]] = {}
    for i, raw in enumerate(text.splitlines(), start=1):
        suppress[i] = set(_SUPPRESS_RE.findall(raw))

    # Collect IMP_LV0 line ranges
    imp_lv0_lines: set[int] = set()
    for node in _walk_all(root):
        if node.type in ("preproc_def", "preproc_function_def"):
            if "_IMP_LV0" in node.text.decode():
                for ln in range(node.start_point[0] + 1, node.end_point[0] + 2):
                    imp_lv0_lines.add(ln)

    findings: list[Finding] = []

    for rule in rules:
        node_types = rule.get("node_type") or rule.get("node_types")
        if not node_types:
            continue
        if isinstance(node_types, str):
            node_types = [node_types]

        compiled = [re.compile(p) for p in rule["patterns"]]
        skip_lv0 = rule.get("skip_in_imp_lv0", True)
        rule_id = rule["id"]

        for node in _walk_all(root):
            if node.type not in node_types:
                continue

            line_no = node.start_point[0] + 1
            if rule_id in suppress.get(line_no, set()):
                continue
            if skip_lv0 and line_no in imp_lv0_lines:
                continue

            node_text = node.text.decode()
            for r in compiled:
                if r.search(node_text):
                    findings.append(Finding(
                        filepath, line_no, rule_id,
                        rule["message"], rule.get("reference"),
                    ))
                    break

    return findings


# ---------------------------------------------------------------- tree helpers

def _walk_all(node: Node):
    """Recursively yield all descendant nodes (including self)."""
    yield node
    for child in node.children:
        yield from _walk_all(child)


# ---------------------------------------------------------------- zone replacement

def replace_zone(content: str, zone_name: str, replacement: str) -> str:
    """Replace the region delimited by
    // {zone_name}
    ...
    // {zone_name} end
    with *replacement*.
    If either marker is missing, returns *content* unchanged.
    """
    begin = f"// {zone_name}\n"
    end = f"// {zone_name} end\n"
    start_pos = content.find(begin)
    end_pos = content.find(end, start_pos)
    if start_pos == -1 or end_pos == -1:
        return content
    return content[:start_pos] + replacement.rstrip("\n") + "\n" + content[end_pos + len(end):]
