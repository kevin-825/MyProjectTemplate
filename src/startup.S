	.extern main
	.section .init
	.globl _start
	.type _start,@function
_start:
	.cfi_startproc
	.cfi_undefined ra
.option push
.option norelax
	la  gp, __global_pointer$
.option pop
    /* ---------------------------------------------------------
     * 1. Set stack pointer
     * --------------------------------------------------------- */
    la      sp, _stack_top

    /* ---------------------------------------------------------
     * 2. Zero .bss (32-bit stores, RV32 & RV64 safe)
     * --------------------------------------------------------- */
    la      t0, _bss
    la      t1, _ebss
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:

    /* ---------------------------------------------------------
     * 3. Copy .data from ROM (LMA) to RAM (VMA)
     * --------------------------------------------------------- */
    la      t0, _data_lma     /* src */
    la      t1, _data         /* dst */
    la      t2, _edata        /* end */
3:
    bgeu    t1, t2, 4f
    lw      t3, 0(t0)
    sw      t3, 0(t1)
    addi    t0, t0, 4
    addi    t1, t1, 4
    j       3b
4:
#ifdef __riscv_fdiv
	// Enable FPU
	li          t0, (1 << 13)
	csrs        mstatus, t0
	fscsr       x0
#endif
#ifdef __riscv_vector
	// Enable VPU
	li          t0, (1 << 9)
	csrs        mstatus, t0
#endif

    /* ---------------------------------------------------------
     * 4. Setup PMP: guard region + null page
     * --------------------------------------------------------- */
    call    pmp_setup_guard
    call    pmp_block_null
    /* ---------------------------------------------------------
     * 5. Jump to main()
     * --------------------------------------------------------- */
    // argc, argv, envp is 0
	li  a0, 0
	li  a1, 0
	li  a2, 0
	jal main
1:
	wfi
	j 1b
    .cfi_endproc

/* ============================================================
 * PMP: Guard region between heap and stack
 * ============================================================ */
    .section .text.init
    .globl  pmp_setup_guard
pmp_setup_guard:
    la      t0, _guard_start
    la      t1, _guard_end

    srli    t0, t0, 2
    srli    t1, t1, 2

    csrw    pmpaddr0, t0
    csrw    pmpaddr1, t1

    li      t2, (1 << 3)      /* TOR, no R/W/X */
    csrw    pmpcfg0, t2

    ret


/* ============================================================
 * PMP: Block NULL page (0x0–0x1000)
 * ============================================================ */
    .section .text.init
    .globl  pmp_block_null
pmp_block_null:
    li      t0, 0
    csrw    pmpaddr2, t0

    li      t1, (0x1000 >> 2)
    csrw    pmpaddr3, t1

    li      t2, (1 << 3)      /* TOR, no R/W/X */
    csrw    pmpcfg1, t2

    ret

