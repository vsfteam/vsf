// gp-aware setjmp/longjmp for RISC-V
//  standard newlib RISC-V setjmp saves ra/sp/s0-s11 but NOT gp(x3)/tp(x4),
//  while VSF linux uses x3(gp) as the thread register(GOT base of applets),
//  and vsf_thread context switches rely on setjmp/longjmp, so gp/tp MUST be
//  saved/restored across context switches.
//
//  FPU context must survive thread switches too. fcsr and all f0-f31 are
//  saved on hard-float builds. fsw/flw are used for ilp32f, fsd/fld for
//  ilp32d; no-FPU builds keep the 17-word layout. The jmp_buf typedef in
//  simple_libc/setjmp/setjmp.h must be built with matching flags.
//
// jmp_buf layout used here(rv32, 50 words with FPU):
//   0: ra   4: sp   8: s0   12: s1  16: s2  20: s3
//  24: s4  28: s5  32: s6  36: s7  40: s8  44: s9
//  48: s10 52: s11 56: gp  60: tp
//  64..: f0..f31(4B each on F, 8B each on D), fcsr in the word right after,
//  then mstatus in the last word(196 with FPU, 64 without)
//
//  mstatus preserves the interrupt-enable state across context switches.

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
#if defined(__riscv_flen) && __riscv_flen >= 32
	addi	t0, a0, 64
#if __riscv_flen >= 64
	.irp	r, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	fsd	f\r, 0(t0)
	addi	t0, t0, 8
	.endr
#else
	.irp	r, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	fsw	f\r, 0(t0)
	addi	t0, t0, 4
	.endr
#endif
	csrrs	t1, fcsr, x0
	sw	t1, 0(t0)
	/* t0 now points past fcsr: mstatus tail word */
	csrr	t1, mstatus
	sw	t1, 4(t0)
#else
	csrr	t1, mstatus
	sw	t1, 64(a0)
#endif
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
#if defined(__riscv_flen) && __riscv_flen >= 32
	addi	t0, a0, 64
#if __riscv_flen >= 64
	.irp	r, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	fld	f\r, 0(t0)
	addi	t0, t0, 8
	.endr
#else
	.irp	r, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	flw	f\r, 0(t0)
	addi	t0, t0, 4
	.endr
#endif
	lw	t1, 0(t0)
	csrrw	x0, fcsr, t1
	/* mstatus tail word */
	lw	t1, 4(t0)
	csrw	mstatus, t1
#else
	lw	t1, 64(a0)
	csrw	mstatus, t1
#endif
	mv	a0, a1
	bnez	a1, 1f
	li	a0, 1
1:
	ret
