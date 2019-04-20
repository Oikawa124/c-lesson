	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 1	@ Tag_CPU_arch
	.eabi_attribute	8, 1	@ Tag_ARM_ISA_use
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	17, 1	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	18, 4	@ Tag_ABI_PCS_wchar_t
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"sum_range_inline.c"
	.globl	sum_range               @ -- Begin function sum_range
	.p2align	2
	.type	sum_range,%function
	.code	32                      @ @sum_range
sum_range:
	.fnstart
@ %bb.0:
	sub	sp, sp, #16
	str	r0, [sp, #12]
	str	r1, [sp, #8]
	mov	r0, #0
	str	r0, [sp, #4]
	ldr	r0, [sp, #12]
	str	r0, [sp]
	b	.LBB0_1
.LBB0_1:                                @ =>This Inner Loop Header: Depth=1
	ldr	r0, [sp]
	ldr	r1, [sp, #8]
	cmp	r0, r1
	bgt	.LBB0_4
	b	.LBB0_2
.LBB0_2:                                @   in Loop: Header=BB0_1 Depth=1
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	add	r0, r1, r0
	str	r0, [sp, #4]
	b	.LBB0_3
.LBB0_3:                                @   in Loop: Header=BB0_1 Depth=1
	ldr	r0, [sp]
	add	r0, r0, #1
	str	r0, [sp]
	b	.LBB0_1
.LBB0_4:
	ldr	r0, [sp, #4]
	add	sp, sp, #16
	mov	pc, lr
.Lfunc_end0:
	.size	sum_range, .Lfunc_end0-sum_range
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	sum_range_inline        @ -- Begin function sum_range_inline
	.p2align	2
	.type	sum_range_inline,%function
	.code	32                      @ @sum_range_inline
sum_range_inline:
	.fnstart
@ %bb.0:
	sub	sp, sp, #12
	str	r0, [sp, #8]
	str	r1, [sp, #4]
	@APP
	mov	r2, #0
	@NO_APP
	@APP
loop:
	@NO_APP
	@APP
	cmp	r0, r1
	@NO_APP
	@APP
	bgt	end
	@NO_APP
	@APP
	add	r2, r2, r0
	@NO_APP
	@APP
	add	r0, r0, #1
	@NO_APP
	@APP
	b	loop
	@NO_APP
	@APP
end:
	@NO_APP
	@APP
	mov	r0, r2
	@NO_APP
	str	r0, [sp]
	ldr	r0, [sp]
	add	sp, sp, #12
	mov	pc, lr
.Lfunc_end1:
	.size	sum_range_inline, .Lfunc_end1-sum_range_inline
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	assert_true             @ -- Begin function assert_true
	.p2align	2
	.type	assert_true,%function
	.code	32                      @ @assert_true
assert_true:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r0, [sp, #4]
	cmp	r0, #0
	bne	.LBB2_2
	b	.LBB2_1
.LBB2_1:
	ldr	r0, .LCPI2_0
	bl	printf
	b	.LBB2_2
.LBB2_2:
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.3:
.LCPI2_0:
	.long	.L.str
.Lfunc_end2:
	.size	assert_true, .Lfunc_end2-assert_true
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	main                    @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                      @ @main
main:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	mov	r0, #0
	str	r0, [sp, #4]
	mov	r0, #3
	mov	r1, #10
	bl	sum_range
	str	r0, [sp]
	ldr	r0, [sp]
	cmp	r0, #52
	mov	r0, #0
	moveq	r0, #1
	bl	assert_true
	mov	r0, #3
	mov	r1, #10
	bl	sum_range_inline
	str	r0, [sp]
	ldr	r1, [sp]
	ldr	r0, .LCPI3_0
	bl	printf
	ldr	r0, [sp]
	cmp	r0, #52
	mov	r0, #0
	moveq	r0, #1
	bl	assert_true
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI3_0:
	.long	.L.str.1
.Lfunc_end3:
	.size	main, .Lfunc_end3-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"assert fail\n"
	.size	.L.str, 13

	.type	.L.str.1,%object        @ @.str.1
.L.str.1:
	.asciz	"%d"
	.size	.L.str.1, 3


	.ident	"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
	.section	".note.GNU-stack","",%progbits
