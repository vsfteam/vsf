#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
堆内存分析工具 - Heap Memory Map Visualizer

通用对比分析：对比任意两次 dump，显示 curr_only（新增）和 prev_only（释放）
锁定期间分析：通用对比的特例，额外保证 lock 期间不会真正释放内存，便于查找泄漏

生成 heap.dump 文件:
  在代码中调用 vsf_heap_dump() 函数，该函数会通过 vsf_trace 输出堆内存 dump 日志。
  将日志复制出来保存成文件（如 heap.dump.txt），不需要清理非 heap 的其他日志，工具会自动过滤。
  如果仅关注某段代码的内存申请或释放，使用方式如下：
    vsf_heap_dump(true);   // 在关注代码段开始前调用，锁定堆（锁定期间不会真正释放内存）
    // ... 你的代码 ...
    vsf_heap_dump(false);  // 在关注代码段结束后调用，解锁堆并追踪锁定期间的内存分配

使用方法:
  python visualize_heap_map.py heap.dump.txt                    # 基本用法
  python visualize_heap_map.py heap.dump.txt -s project.out     # 带符号解析 (GCC: .out/.elf, MDK: .axf)
  python visualize_heap_map.py heap.dump.txt -d 1 3             # 对比第1次和第3次 dump
"""

from __future__ import annotations
import argparse, bisect, math, os, re, sys
from dataclasses import dataclass, field
from functools import cached_property
from typing import Optional

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib.gridspec as gridspec
import matplotlib.colors as mc
import numpy as np

try:
    from elftools.elf.elffile import ELFFile
    from elftools.elf.sections import SymbolTableSection
except ImportError:
    print("错误: pyelftools 未安装，请运行: pip install pyelftools")
    sys.exit(1)


# ============================================================================
# 工具函数与颜色
# ============================================================================

# 常量
KB = 1024
MIN_SIZE_KB = 10  # 10KB 以上显示为整数 KB

def fmt_kb(b: int, p: int = 2) -> str:
    return f"{b/KB:.{p}f} KB"

def fmt_size(b: int) -> str:
    if b >= MIN_SIZE_KB * KB:
        return f"{b/KB:.0f}KB"
    elif b >= KB:
        return f"{b/KB:.1f}KB"
    else:
        return f"{b}B"

def hsl(h: float, s: float, l: float) -> list[int]:
    return [int(round(c * 255)) for c in mc.hsv_to_rgb([h / 360, s, l])]

class C:
    """颜色常量"""
    MCB, UNUSED, FREE = hsl(270, 0.4, 0.45), hsl(0, 0, 0.20), hsl(120, 0.3, 0.65)
    D_UNUSED, D_FREE, D_MCB = hsl(0, 0, 0.15), hsl(120, 0.2, 0.35), hsl(270, 0.3, 0.35)
    D_ALLOC_SAME = hsl(0, 0.3, 0.30)
    # 通用对比：新增(红)、释放(绿)
    D_NEW, D_OLD = hsl(0, 0.6, 0.75), hsl(120, 0.4, 0.80)
    # 锁定期间特例：泄漏(亮红)、unlock释放(亮绿) - 复用通用对比颜色
    D_LEAKED, D_FREED = D_NEW, D_OLD
    ALLOC = [hsl(30, 0.4, 0.50), hsl(30, 0.4, 0.60), hsl(30, 0.4, 0.70)]


# ============================================================================
# 数据模型
# ============================================================================

@dataclass
class Block:
    addr: int
    end: int
    free: bool
    size: int = 0
    mcb_sz: int = 0
    caller: int = 0
    idx: int = 0

    def __post_init__(self):
        self.size = self.size or (self.end - self.addr - self.mcb_sz)

    @property
    def buf_addr(self) -> int:
        return self.addr + self.mcb_sz


@dataclass
class Dump:
    index: int
    blocks: list[Block] = field(default_factory=list)
    total: int = 0
    used: int = 0
    free_sz: int = 0
    mcb_sz: int = 0
    mcb_align: int = 0
    fail_alloc: Optional[dict] = None
    locked: bool = False
    locked_idx: int = 0
    unlocked_idx: int = 0
    unlocked_freed: list[Block] = field(default_factory=list)

    # 缓存字段
    _heap_range: Optional[tuple[int, int]] = field(default=None, init=False, repr=False)
    _allocs: Optional[list[Block]] = field(default=None, init=False, repr=False)
    _frees: Optional[list[Block]] = field(default=None, init=False, repr=False)

    @property
    def heap_range(self) -> tuple[int, int]:
        if self._heap_range is None:
            if self.blocks:
                self._heap_range = (min(b.addr for b in self.blocks), max(b.end for b in self.blocks))
            else:
                self._heap_range = (0, self.total)
        return self._heap_range

    @property
    def allocs(self) -> list[Block]:
        if self._allocs is None:
            self._allocs = [b for b in self.blocks if not b.free]
        return self._allocs

    @property
    def frees(self) -> list[Block]:
        if self._frees is None:
            self._frees = [b for b in self.blocks if b.free]
        return self._frees

    def _invalidate_cache(self):
        """当 blocks 被修改时调用此方法清除缓存"""
        self._heap_range = None
        self._allocs = None
        self._frees = None


@dataclass
class Diff:
    """
    通用对比分析：对比任意两次 dump
    锁定期间分析：通用对比的特例，额外约束 prev.locked 且 curr.unlocked
    """
    prev: Dump
    curr: Dump

    def __post_init__(self):
        self._prev_addrs = {b.addr for b in self.prev.allocs}
        self._curr_addrs = {b.addr for b in self.curr.allocs}

    @property
    def is_lock_mode(self) -> bool:
        """是否为锁定期间分析模式：相邻两个 dump，第一个 locked，第二个 unlocked"""
        return (self.prev.locked and self.curr.unlocked_idx > 0 and
                self.curr.index == self.prev.index + 1)

    # ===== 通用对比分析 =====
    @property
    def curr_only(self) -> list[Block]:
        """curr 独有的已分配块（新增）"""
        return [b for b in self.curr.allocs if b.addr not in self._prev_addrs]

    @property
    def prev_only(self) -> list[Block]:
        """prev 独有的已分配块（释放）"""
        return [b for b in self.prev.allocs if b.addr not in self._curr_addrs]

    # ===== 锁定期间分析（通用对比的特例）=====
    @property
    def _freed_addrs(self) -> set[int]:
        return {b.addr for b in self.curr.unlocked_freed}

    @property
    def lock_new(self) -> list[Block]:
        """lock 期间新增的块（curr_only 中 idx >= locked_idx）"""
        if not self.is_lock_mode:
            return []
        return [b for b in self.curr_only if b.idx >= self.prev.locked_idx]

    @property
    def lock_freed(self) -> list[Block]:
        """lock 期间新增且在 unlock 时释放的块"""
        return list(self.curr.unlocked_freed) if self.is_lock_mode else []

    @property
    def leaked(self) -> list[Block]:
        """泄漏：lock 期间新增但未在 unlock 时释放"""
        if not self.is_lock_mode:
            return []
        return [b for b in self.lock_new if b.addr not in self._freed_addrs]

    def sum_size(self, blocks: list[Block]) -> int:
        return sum(b.size for b in blocks)


# ============================================================================
# 符号解析
# ============================================================================

class SymbolResolver:
    def __init__(self, path: Optional[str] = None):
        self.symbols: list[tuple[int, int, str]] = []
        if path:
            self._parse(path)

    def _parse(self, path: str):
        try:
            with open(path, 'rb') as f:
                elf = ELFFile(f)
                for sec in elf.iter_sections():
                    if isinstance(sec, SymbolTableSection):
                        for sym in sec.iter_symbols():
                            if sym['st_info']['type'] in ('STT_FUNC', 'STT_OBJECT') \
                                    and sym['st_shndx'] != 'SHN_UNDEF' and sym['st_value'] > 0 and sym.name:
                                self.symbols.append((sym['st_value'], sym['st_value'] + (sym['st_size'] or 4), sym.name))
            self.symbols = sorted(set(self.symbols), key=lambda x: x[0])
            print(f"从 ELF 解析成功: {len(self.symbols)} 个符号")
        except Exception as e:
            print(f"错误: ELF 解析失败: {e}")
            sys.exit(1)

    def resolve(self, addr: int) -> Optional[str]:
        i = bisect.bisect_right(self.symbols, (addr, float('inf'), ''))
        if i > 0:
            s, e, n = self.symbols[i - 1]
            if s <= addr < e:
                return n
        return None

    def fmt(self, addr: int) -> str:
        n = self.resolve(addr)
        return f"0x{addr:x} ({n})" if n else (f"0x{addr:x}" if addr else "N/A")

    def enhance_file(self, src: str, dst: str) -> int:
        try:
            with open(src, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            cnt = [0]
            def repl(m):
                n = self.resolve(int(m.group(2), 16))
                if n:
                    cnt[0] += 1
                    return f"{m.group(1)} ({n})"
                return m.group(1)
            content = re.sub(r'(caller=0x([0-9a-fA-F]+))', repl, content)
            with open(dst, 'w', encoding='utf-8') as f:
                f.write(content)
            return cnt[0]
        except Exception as e:
            print(f"警告: 增强 dump 失败: {e}")
            return 0


# ============================================================================
# 解析器
# ============================================================================

class Parser:
    RE = {
        'fail': re.compile(r'fail to allocate (\d+) bytes with (\d+) alignment', re.I),
        'total': re.compile(r'Total heap size: (\d+) bytes'),
        'used': re.compile(r'Used size: (\d+) bytes'),
        'free': re.compile(r'Free size: (\d+) bytes'),
        'mcb': re.compile(r'MCB size: (\d+) bytes(?:, align: (\d+) bytes)?'),
        'free_blk': re.compile(r'Free block: MCB=0x([0-9a-fA-F]+), buffer=0x([0-9a-fA-F]+), usable_size=(\d+) bytes'
                               r'(?:, caller=0x([0-9a-fA-F]+), idx=(\d+))?'),
        'alloc': re.compile(r'Allocated: MCB=0x([0-9a-fA-F]+), buffer_start=0x([0-9a-fA-F]+), '
                           r'max_size=(\d+) bytes, caller=0x([0-9a-fA-F]+), idx=(\d+), freed=(\d+)'),
        'locked': re.compile(r'heap locked on index (\d+)'),
        'unlocked': re.compile(r'heap unlocked on index (\d+)'),
    }

    def __init__(self, path: str):
        self.path, self.dumps = path, []

    def parse(self) -> list[Dump]:
        with open(self.path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        cur, sec, free_l, alloc_l, fail = None, None, [], [], None
        i = 0
        while i < len(lines):
            line = lines[i]
            if m := self.RE['fail'].search(line):
                fail = {'size': int(m.group(1)), 'alignment': int(m.group(2))}
            elif '=== Heap Statistics ===' in line:
                if cur:
                    self._finalize(cur, free_l, alloc_l)
                    self.dumps.append(cur)
                cur = Dump(len(self.dumps) + 1, fail_alloc=fail)
                fail, sec, free_l, alloc_l = None, 'stats', [], []
            elif '=== Free Blocks ===' in line:
                sec = 'free'
            elif '=== Allocated Blocks ===' in line:
                sec = 'alloc'
            elif cur:
                if sec == 'stats':
                    self._stats(line, cur)
                elif sec == 'free':
                    free_l.append(line)
                elif sec == 'alloc':
                    if m := self.RE['locked'].search(line):
                        cur.locked, cur.locked_idx = True, int(m.group(1))
                    elif m := self.RE['unlocked'].search(line):
                        cur.locked, cur.unlocked_idx = False, int(m.group(1))
                        i = self._unlocked(lines, i + 1, cur)
                        continue
                    else:
                        alloc_l.append(line)
            i += 1
        if cur:
            self._finalize(cur, free_l, alloc_l)
            self.dumps.append(cur)
        return self.dumps

    def _stats(self, line: str, d: Dump):
        if m := self.RE['total'].search(line):
            d.total = int(m.group(1))
        elif m := self.RE['used'].search(line):
            d.used = int(m.group(1))
        elif m := self.RE['free'].search(line):
            d.free_sz = int(m.group(1))
        elif m := self.RE['mcb'].search(line):
            d.mcb_sz, d.mcb_align = int(m.group(1)), int(m.group(2)) if m.group(2) else 0

    def _finalize(self, d: Dump, free_l: list, alloc_l: list):
        for line in free_l:
            if m := self.RE['free_blk'].search(line):
                mcb, buf, sz = int(m.group(1), 16), int(m.group(2), 16), int(m.group(3))
                d.blocks.append(Block(mcb, buf + sz, True, sz, d.mcb_sz or (buf - mcb)))
        for line in alloc_l:
            if m := self.RE['alloc'].search(line):
                mcb, buf, sz = int(m.group(1), 16), int(m.group(2), 16), int(m.group(3))
                d.blocks.append(Block(mcb, buf + sz, False, sz, d.mcb_sz or (buf - mcb),
                                      int(m.group(4), 16), int(m.group(5))))
        d.blocks.sort(key=lambda x: x.addr)

    def _unlocked(self, lines: list, start: int, d: Dump) -> int:
        i = start
        while i < len(lines):
            if '===' in lines[i]:
                return i - 1
            if m := self.RE['free_blk'].search(lines[i]):
                if m.group(4):
                    mcb, buf, sz = int(m.group(1), 16), int(m.group(2), 16), int(m.group(3))
                    d.unlocked_freed.append(Block(mcb, buf + sz, True, sz, d.mcb_sz or (buf - mcb),
                                                  int(m.group(4), 16), int(m.group(5))))
            i += 1
        return i

    @property
    def lock_pairs(self) -> list[tuple[Dump, Dump]]:
        return [(self.dumps[i], self.dumps[i + 1]) for i in range(len(self.dumps) - 1)
                if self.dumps[i].locked and self.dumps[i + 1].unlocked_idx > 0]


# ============================================================================
# 可视化
# ============================================================================

class Visualizer:
    def __init__(self, dump: Dump, out: str, w: int = 1920, h: int = 1080, prev: Optional[Dump] = None):
        self.dump, self.prev, self.out, self.w, self.h = dump, prev, out, w, h
        self.gran = 1

    def create(self, diff: Optional[Diff] = None):
        start, end = self.dump.heap_range
        sz = end - start or self.dump.total
        rows, cols, bpr = self._dim(sz)
        print(f"  {os.path.basename(self.out)}: {cols}x{rows}px, {self.gran}B/px, {fmt_size(bpr)}/row")

        img = np.full((rows, cols, 3), C.D_UNUSED if diff else C.UNUSED, dtype=np.uint8)
        masks = self._masks(diff, start, rows, cols) if diff else {}
        self._fill(img, start, rows, cols, diff, masks)
        self._render(img, start, end, rows, cols, bpr, diff)

    def _dim(self, sz: int) -> tuple[int, int, int]:
        cols, target = self.w, self.w * self.h
        gran = max(1, (sz + target - 1) // target)
        rows = ((sz + gran - 1) // gran + cols - 1) // cols
        if rows > self.h:
            gran = (sz + self.h * cols - 1) // (self.h * cols)
            rows = ((sz + gran - 1) // gran + cols - 1) // cols
        bpr = 1 << max(0, (sz // rows - 1).bit_length())
        self.gran = max(1, bpr // cols) or 1
        return (sz + bpr - 1) // bpr, cols, bpr

    def _masks(self, diff: Diff, start: int, rows: int, cols: int) -> dict:
        """构建可视化掩码 - 通用对比和锁定期间统一处理"""
        masks = {'new': set(), 'old': set(), 'leaked': set(), 'freed': set()}

        def add(m, b):
            for px in range((b.addr - start) // self.gran, (b.end - start + self.gran - 1) // self.gran):
                if 0 <= px < rows * cols:
                    m.add((px // cols, px % cols))

        # 通用对比：新增和释放
        for b in diff.curr_only:
            add(masks['new'], b)
        for b in diff.prev_only:
            add(masks['old'], b)

        # 锁定期间特例：在通用对比基础上，高亮泄漏和 unlock 释放
        if diff.is_lock_mode:
            for b in diff.leaked:
                add(masks['leaked'], b)
            for b in diff.lock_freed:
                add(masks['freed'], b)

        return masks

    def _fill(self, img: np.ndarray, start: int, rows: int, cols: int, diff: Optional[Diff], masks: dict):
        allocs = self.dump.allocs
        min_s, max_s = (min(b.size for b in allocs), max(b.size for b in allocs)) if allocs else (0, 1)

        for b in sorted(self.dump.blocks, key=lambda x: (not x.free, x.addr)):
            s_px, e_px = (b.addr - start) // self.gran, (b.end - start + self.gran - 1) // self.gran
            mcb_px = (b.buf_addr - start) // self.gran if b.mcb_sz else s_px

            base = None
            if not b.free and max_s > min_s:
                norm = (math.log10(max(b.size, 1)) - math.log10(max(min_s, 1))) / \
                       (math.log10(max(max_s, 1)) - math.log10(max(min_s, 1)))
                base = C.ALLOC[0] if norm < 0.33 else C.ALLOC[1] if norm < 0.67 else C.ALLOC[2]
            elif not b.free:
                base = C.ALLOC[1]

            for px in range(s_px, e_px):
                if not (0 <= px < rows * cols):
                    continue
                r, c = px // cols, px % cols
                if r >= rows:
                    continue
                if px < mcb_px:
                    img[r, c] = C.D_MCB if diff else C.MCB
                elif diff:
                    pos = (r, c)
                    # 锁定期间特例优先（泄漏和释放）
                    if pos in masks.get('leaked', set()):
                        img[r, c] = C.D_LEAKED
                    elif pos in masks.get('freed', set()):
                        img[r, c] = C.D_FREED
                    # 通用对比
                    elif pos in masks.get('new', set()):
                        img[r, c] = C.D_NEW
                    elif pos in masks.get('old', set()):
                        img[r, c] = C.D_OLD
                    elif b.free:
                        img[r, c] = C.D_FREE
                    else:
                        img[r, c] = C.D_ALLOC_SAME if base else C.D_UNUSED
                else:
                    img[r, c] = C.FREE if b.free else base

    def _render(self, img: np.ndarray, start: int, end: int, rows: int, cols: int, bpr: int, diff: Optional[Diff]):
        fig = plt.figure(figsize=(16, 12), dpi=100)
        gs = gridspec.GridSpec(2, 2, height_ratios=[9, 1.5], width_ratios=[1, 1], hspace=0.15, wspace=0.1)

        ax = fig.add_subplot(gs[0, :])
        ax.imshow(img, aspect='auto', interpolation='nearest')
        ax.set_title(self._title(bpr, diff), fontsize=14, pad=20)
        ax.set_xlabel(f'Address: 0x{start:x} - 0x{end:x} ({self.gran} B/px)', fontsize=12)
        ax.set_ylabel(f'Layout\n{fmt_kb(bpr, 1)}/row (2^{int(math.log2(bpr))})', fontsize=12)

        interval = max(1, rows // 15)
        ticks = list(range(0, rows, interval)) + ([rows - 1] if rows - 1 not in range(0, rows, interval) else [])
        ax.set_yticks(ticks)
        ax.set_yticklabels([f'0x{start + t * bpr:x}' if start + t * bpr <= end else '' for t in ticks], fontsize=9)
        ax.set_xticks([])

        for i, b in enumerate(sorted(self.dump.frees, key=lambda x: -x.size)[:10], 1):
            center = ((b.addr - start) // self.gran + (b.end - start) // self.gran) // 2
            r, c = center // cols, center % cols
            if 0 <= r < rows and 0 <= c < cols:
                ax.text(c, r, f"#{i}\n{fmt_size(b.size)}", ha='center', va='center', fontsize=8, color='black',
                        bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.5, linewidth=0), weight='bold')

        ax_t, ax_l = fig.add_subplot(gs[1, 0]), fig.add_subplot(gs[1, 1])
        ax_t.axis('off')
        ax_l.axis('off')
        self._info(ax_t, diff)
        self._legend(ax_l, diff)
        plt.savefig(self.out, dpi=150, bbox_inches='tight')
        plt.close()

    def _title(self, bpr: int, diff: Optional[Diff]) -> str:
        d, bpr_s = self.dump, fmt_kb(bpr, 1) if bpr >= 1024 else f"{bpr} B"
        if diff and self.prev:
            p, c = self.prev, d
            title = f'Heap Diff: #{p.index} -> #{c.index}\n'
            title += f'Used: {fmt_kb(p.used)} -> {fmt_kb(c.used)} [{c.used - p.used:+d}] | Free: {fmt_kb(p.free_sz)} -> {fmt_kb(c.free_sz)} [{c.free_sz - p.free_sz:+d}]\n'
            # 通用对比信息
            title += f'Curr only: {len(diff.curr_only)} ({fmt_size(diff.sum_size(diff.curr_only))}) | Prev only: {len(diff.prev_only)} ({fmt_size(diff.sum_size(diff.prev_only))})'
            # 锁定期间特例：额外显示泄漏信息
            if diff.is_lock_mode:
                title += f' | Leaked: {len(diff.leaked)} ({fmt_size(diff.sum_size(diff.leaked))})'
            title += f' | {bpr_s}/row'
            return title

        lock = f" [LOCKED={d.locked_idx}]" if d.locked else f" [UNLOCKED={d.unlocked_idx}]" if d.unlocked_idx else ""
        pct = d.used * 100 / d.total if d.total else 0
        title = f'Heap Map (#{d.index}){lock}\nTotal: {fmt_kb(d.total)} | Used: {fmt_kb(d.used)} ({pct:.1f}%) | Free: {fmt_kb(d.free_sz)}\n'
        if d.mcb_sz:
            title += f'MCB: {d.mcb_sz}B (align: {d.mcb_align}B) | '
        title += f'{bpr_s}/row'
        if d.fail_alloc:
            title += f'\n[WARNING] Fail: {d.fail_alloc["size"]} bytes'
        return title

    def _info(self, ax, diff: Optional[Diff]):
        if diff and self.prev:
            if diff.is_lock_mode:
                # Lock Mode: 只显示泄漏检测结果
                new_sz, freed_sz = diff.sum_size(diff.lock_new), diff.sum_size(diff.lock_freed)
                leaked_sz = diff.sum_size(diff.leaked)
                lines = [f"Lock idx: {self.prev.locked_idx} -> {self.dump.unlocked_idx}",
                         f"New: {new_sz} bytes ({len(diff.lock_new)} blocks)",
                         f"Freed: {freed_sz} bytes ({len(diff.lock_freed)} blocks)",
                         "",
                         f"LEAKED: {leaked_sz} bytes ({len(diff.leaked)} blocks)" if diff.leaked else "No Leak"]
            else:
                # General Mode: 通用对比
                lines = [f"Dump #{self.prev.index} vs #{self.dump.index}",
                         f"Used change: {self.dump.used - self.prev.used:+d} bytes",
                         f"Curr only: {len(diff.curr_only)} blocks, {diff.sum_size(diff.curr_only)} bytes",
                         f"Prev only: {len(diff.prev_only)} blocks, {diff.sum_size(diff.prev_only)} bytes"]

            ax.text(0.02, 0.95, 'Lock Mode' if diff.is_lock_mode else 'Statistics', fontsize=10, weight='bold', transform=ax.transAxes, va='top')
            ax.text(0.02, 0.80, '\n'.join(lines), fontsize=9, family='monospace', va='top', transform=ax.transAxes,
                    bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgray', edgecolor='black', alpha=0.9))
        else:
            top10 = sorted(self.dump.frees, key=lambda b: -b.size)[:10]
            if top10:
                ax.text(0.02, 0.95, 'Top 10 Free Blocks', fontsize=10, weight='bold', transform=ax.transAxes, va='top')
                lines = [f"#{i}: 0x{b.addr:x}, {b.size} bytes ({fmt_kb(b.size)})" for i, b in enumerate(top10, 1)]
                ax.text(0.02, 0.80, '\n'.join(lines), fontsize=9, family='monospace', va='top', transform=ax.transAxes,
                        bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgray', edgecolor='black', alpha=0.9))

    def _legend(self, ax, diff: Optional[Diff]):
        def p(c, l):
            return mpatches.Patch(facecolor=[x / 255 for x in c], label=l)

        items = []
        if self.dump.mcb_sz:
            items.append(p(C.D_MCB if diff else C.MCB, f'MCB ({self.dump.mcb_sz}B)'))
        if self.dump.allocs:
            items += [p(C.ALLOC[2], 'Alloc (large)'), p(C.ALLOC[1], 'Alloc (medium)'), p(C.ALLOC[0], 'Alloc (small)')]
        if diff:
            items.append(p(C.D_ALLOC_SAME, 'Alloc (unchanged)'))
        items += [p(C.D_UNUSED if diff else C.UNUSED, 'Unused'),
                  p(C.D_FREE if diff else C.FREE, 'Free' + (' (unchanged)' if diff else ''))]

        if diff:
            # 通用对比
            if diff.curr_only:
                items.append(p(C.D_NEW, f'Curr Only ({fmt_kb(diff.sum_size(diff.curr_only))})'))
            if diff.prev_only:
                items.append(p(C.D_OLD, f'Prev Only ({fmt_kb(diff.sum_size(diff.prev_only))})'))
            # 锁定期间特例
            if diff.is_lock_mode:
                if diff.leaked:
                    items.append(p(C.D_LEAKED, f'Leaked ({fmt_kb(diff.sum_size(diff.leaked))})'))
                if diff.lock_freed:
                    items.append(p(C.D_FREED, f'Lock Freed ({fmt_kb(diff.sum_size(diff.lock_freed))})'))

        if items:
            legend = ax.legend(handles=items, loc='center', fontsize=9, framealpha=0.9, frameon=True,
                               fancybox=True, shadow=True, facecolor='lightgray', edgecolor='black')
            legend.set_title('Legend', prop={'size': 10, 'weight': 'bold'})


# ============================================================================
# 报告生成
# ============================================================================

class Reporter:
    def __init__(self, dumps: list[Dump], pairs: list[dict], resolver: Optional[SymbolResolver] = None):
        self.dumps, self.pairs, self.resolver = dumps, pairs, resolver or SymbolResolver()

    def _fmt_block(self, b: Block, i: int) -> str:
        return f"      #{i}: 0x{b.addr:x} - 0x{b.end:x}, {b.size} bytes, caller={self.resolver.fmt(b.caller)}, idx={b.idx}"

    def _print_blocks(self, blocks: list[Block], sort_key=None):
        for i, b in enumerate(sorted(blocks, key=sort_key or (lambda x: x.addr)), 1):
            print(self._fmt_block(b, i))

    def write(self):
        lock_cnt = sum(1 for i in range(len(self.dumps) - 1)
                       if self.dumps[i].locked and self.dumps[i + 1].unlocked_idx > 0)
        print(f"\n解析完成: 找到 {len(self.dumps)} 次dump, {lock_cnt} 组 lock-unlock 对")

        # Dump 详细信息
        print(f"\n=== Dump 详细信息 ===")
        for d in self.dumps:
            pct = d.used * 100 / d.total if d.total else 0
            lock = f" [LOCKED={d.locked_idx}]" if d.locked else f" [UNLOCKED={d.unlocked_idx}]" if d.unlocked_idx else ""
            uf = d.unlocked_freed
            uf_info = f", Unlocked Freed: {len(uf)} blocks ({sum(b.size for b in uf)} bytes)" if uf else ""
            mcb = f", MCB: {d.mcb_sz}B" if d.mcb_sz else ""
            print(f"\nDump #{d.index}: {d.total}B, Used: {d.used}B ({pct:.1f}%), Free: {d.free_sz}B{mcb}, Blocks: {len(d.blocks)}{lock}{uf_info}")
            if d.fail_alloc:
                print(f"  -> Fail: {d.fail_alloc['size']} bytes")

        # Top 10 空闲块
        print(f"\n=== Top 10 最大空闲块 ===")
        for d in self.dumps:
            top10 = sorted(d.frees, key=lambda b: -b.size)[:10]
            if top10:
                print(f"\nDump #{d.index}")
                for i, b in enumerate(top10, 1):
                    print(f"  #{i}: 0x{b.addr:x} - 0x{b.end:x}, {b.size} bytes ({fmt_kb(b.size)})")

        # 对比分析
        if self.pairs:
            print(f"\n=== 对比分析 ===")
            for pair in self.pairs:
                prev, curr, diff = pair['prev'], pair['curr'], pair['diff']

                if diff.is_lock_mode:
                    # Lock Mode: 泄漏检测
                    print(f"\n[Lock Mode] Dump #{prev.index} -> Dump #{curr.index}")
                    print(f"  锁定时 idx: {prev.locked_idx}, 解锁时 idx: {curr.unlocked_idx}")
                    print(f"  锁定期间分配次数: {curr.unlocked_idx - prev.locked_idx}")

                    new_sz, freed_sz, leaked_sz = diff.sum_size(diff.lock_new), diff.sum_size(diff.lock_freed), diff.sum_size(diff.leaked)

                    # 新增内存
                    print(f"\n  [新增] {new_sz} bytes ({fmt_kb(new_sz)}), {len(diff.lock_new)} blocks")
                    if diff.lock_new:
                        self._print_blocks(diff.lock_new, sort_key=lambda x: x.idx)

                    # 已释放
                    print(f"\n  [已释放] {freed_sz} bytes ({fmt_kb(freed_sz)}), {len(diff.lock_freed)} blocks")
                    if diff.lock_freed:
                        self._print_blocks(diff.lock_freed, sort_key=lambda x: x.idx)

                    # 未释放（泄漏）
                    print(f"\n  [未释放] {leaked_sz} bytes ({fmt_kb(leaked_sz)}), {len(diff.leaked)} blocks")
                    if diff.leaked:
                        self._print_blocks(diff.leaked, sort_key=lambda x: x.idx)
                        print(f"\n  [警告] 可能存在内存泄漏")
                    else:
                        print(f"    无泄漏")
                else:
                    # General Mode: 通用对比
                    print(f"\n[General] Dump #{prev.index} -> Dump #{curr.index}")
                    used_chg, free_chg = curr.used - prev.used, curr.free_sz - prev.free_sz
                    print(f"  Used: {prev.used} -> {curr.used} [{used_chg:+d}]")
                    print(f"  Free: {prev.free_sz} -> {curr.free_sz} [{free_chg:+d}]")

                    if diff.curr_only:
                        print(f"\n  [Curr Only] {len(diff.curr_only)} blocks, {diff.sum_size(diff.curr_only)} bytes")
                        self._print_blocks(diff.curr_only)

                    if diff.prev_only:
                        print(f"\n  [Prev Only] {len(diff.prev_only)} blocks, {diff.sum_size(diff.prev_only)} bytes")
                        self._print_blocks(diff.prev_only)

                    if not diff.curr_only and not diff.prev_only:
                        print(f"\n  两个 dump 的已分配块完全相同")

        print(f"\n{'='*80}")
        print(f"分析完成！")


# ============================================================================
# 主程序
# ============================================================================

def main():
    ap = argparse.ArgumentParser(description='堆内存分析工具',
        epilog='示例:\n  %(prog)s heap.dump.txt -s project.out\n  %(prog)s heap.dump.txt -d 1 3',
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('map_file', help='堆 dump 文件')
    ap.add_argument('-s', '--symbols', dest='elf', help='ELF 文件')
    ap.add_argument('-o', '--output', help='输出前缀')
    ap.add_argument('-w', '--width', type=int, default=1920)
    ap.add_argument('--height', type=int, default=1080)
    ap.add_argument('-d', '--diff', nargs=2, type=int, metavar=('FROM', 'TO'))
    args = ap.parse_args()

    parser = Parser(args.map_file)
    dumps = parser.parse()
    if not dumps:
        print("错误: 未解析到 dump")
        sys.exit(1)

    resolver = None
    if args.elf and os.path.exists(args.elf):
        print(f"\n=== 解析符号 ===")
        resolver = SymbolResolver(args.elf)

    base = args.output.rstrip('.png') if args.output else os.path.splitext(os.path.basename(args.map_file))[0]
    if base.endswith('.dump'):
        base = base[:-5]

    if resolver and resolver.symbols:
        print(f"\n=== 生成增强 dump ===")
        enhanced = f"{base}_with_symbols.map"
        cnt = resolver.enhance_file(args.map_file, enhanced)
        if cnt > 0:
            print(f"  {enhanced}: {cnt} symbols")

    user_diff = None
    if args.diff:
        f, t = args.diff
        if not (1 <= f <= len(dumps) and 1 <= t <= len(dumps) and f != t):
            print(f"错误: diff 索引无效")
            sys.exit(1)
        user_diff = (dumps[f - 1], dumps[t - 1])
        print(f"\n用户指定对比: Dump #{f} -> #{t}")

    print(f"\n=== 生成图片 ===")
    pairs = []
    for d in dumps:
        Visualizer(d, f"{base}_dump{d.index}.png", args.width, args.height).create()

    for prev, curr in parser.lock_pairs:
        diff = Diff(prev, curr)
        pairs.append({'prev': prev, 'curr': curr, 'diff': diff})
        Visualizer(curr, f"{base}_diff{prev.index}_{curr.index}.png", args.width, args.height, prev).create(diff)

    if user_diff:
        prev, curr = user_diff
        diff = Diff(prev, curr)
        pairs.append({'prev': prev, 'curr': curr, 'diff': diff})
        Visualizer(curr, f"{base}_diff{prev.index}_{curr.index}.png", args.width, args.height, prev).create(diff)

    report = f"{base}_report.log"
    print(f"\n=== 生成报告 ===")
    print(f"  {report}")
    with open(report, 'w', encoding='utf-8') as f:
        stdout = sys.stdout
        sys.stdout = f
        Reporter(dumps, pairs, resolver).write()
        sys.stdout = stdout

    print(f"\n{'='*60}")
    print(f"完成! {len(dumps)} dumps + {len(pairs)} diffs")


if __name__ == '__main__':
    main()
