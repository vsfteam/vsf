// gp-aware setjmp/longjmp for RISC-V
//  standard newlib RISC-V setjmp saves ra/sp/s0-s11 but NOT gp(x3)/tp(x4),
//  while VSF linux uses x3(gp) as the thread register(GOT base of applets),
//  and vsf_thread context switches rely on setjmp/longjmp, so gp/tp MUST be
//  saved/restored across context switches.
//
// jmp_buf layout used here(rv32, 16 words):
//   0: ra   4: sp   8: s0   12: s1  16: s2  20: s3
//   24: s4  28: s5  32: s6  36: s7  40: s8  44: s9
//   48: s10 52: s11 56: gp  60: tp
//
// NOTE: FPU registers are NOT saved for now, firmware is built without FPU
//  codegen; audit this when enabling rv32imafdc/ilp32d.

	.section	.text.vsf_arch_rv_setjmp, "ax", @progbits
	.global	vsf_arch_rv_setjmp
	.align	2
vsf_arch_rv_setjmp:
	sw	ra, 0(a0)
	sw	sp, 4(a0)
	sw	s0, 8(a0)
	sw	s1, 12(a0)
	sw	s2, 16(a0)
	sw	s3, 20(a0)
	sw	s4, 24(a0)
	sw	s5, 28(a0)
	sw	s6, 32(a0)
	sw	s7, 36(a0)
	sw	s8, 40(a0)
	sw	s9, 44(a0)
	sw	s10, 48(a0)
	sw	s11, 52(a0)
	sw	gp, 56(a0)
	sw	tp, 60(a0)
	li	a0, 0
	ret

	.section	.text.vsf_arch_rv_longjmp, "ax", @progbits
	.global	vsf_arch_rv_longjmp
	.align	2
vsf_arch_rv_longjmp:
	lw	ra, 0(a0)
	lw	sp, 4(a0)
	lw	s0, 8(a0)
	lw	s1, 12(a0)
	lw	s2, 16(a0)
	lw	s3, 20(a0)
	lw	s4, 24(a0)
	lw	s5, 28(a0)
	lw	s6, 32(a0)
	lw	s7, 36(a0)
	lw	s8, 40(a0)
	lw	s9, 44(a0)
	lw	s10, 48(a0)
	lw	s11, 52(a0)
	lw	gp, 56(a0)
	lw	tp, 60(a0)
	mv	a0, a1
	bnez	a1, 1f
	li	a0, 1
1:
	ret
