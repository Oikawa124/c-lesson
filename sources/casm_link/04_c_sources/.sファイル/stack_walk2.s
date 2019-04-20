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
	.file	"stack_walk2.c"
	.globl	print_address           @ -- Begin function print_address
	.p2align	2
	.type	print_address,%function
	.code	32                      @ @print_address
print_address:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r0, .LCPI0_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI0_0:
	.long	.L.str
.Lfunc_end0:
	.size	print_address, .Lfunc_end0-print_address
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	print_msg               @ -- Begin function print_msg
	.p2align	2
	.type	print_msg,%function
	.code	32                      @ @print_msg
print_msg:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r0, .LCPI1_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI1_0:
	.long	.L.str.1
.Lfunc_end1:
	.size	print_msg, .Lfunc_end1-print_msg
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	func3                   @ -- Begin function func3
	.p2align	2
	.type	func3,%function
	.code	32                      @ @func3
func3:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	ldr	r0, .LCPI2_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI2_0:
	.long	.L.str.2
.Lfunc_end2:
	.size	func3, .Lfunc_end2-func3
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	func2                   @ -- Begin function func2
	.p2align	2
	.type	func2,%function
	.code	32                      @ @func2
func2:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	ldr	r0, .LCPI3_0
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r0, .LCPI3_1
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI3_0:
	.long	.L.str.3
.LCPI3_1:
	.long	.L.str.4
.Lfunc_end3:
	.size	func2, .Lfunc_end3-func2
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	func1                   @ -- Begin function func1
	.p2align	2
	.type	func1,%function
	.code	32                      @ @func1
func1:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	ldr	r0, .LCPI4_0
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r0, .LCPI4_1
	bl	printf
	bl	func2
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI4_0:
	.long	.L.str.5
.LCPI4_1:
	.long	.L.str.6
.Lfunc_end4:
	.size	func1, .Lfunc_end4-func1
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
	ldr	r0, .LCPI5_0
	str	r0, [sp]
	ldr	r1, [sp]
	ldr	r0, .LCPI5_1
	bl	printf
	bl	func1
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI5_0:
	.long	.L.str.7
.LCPI5_1:
	.long	.L.str.8
.Lfunc_end5:
	.size	main, .Lfunc_end5-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"address: %x\n"
	.size	.L.str, 13

	.type	.L.str.1,%object        @ @.str.1
.L.str.1:
	.asciz	"We get (%s)\n"
	.size	.L.str.1, 13

	.type	.L.str.2,%object        @ @.str.2
.L.str.2:
	.asciz	"We are in func3\n"
	.size	.L.str.2, 17

	.type	.L.str.3,%object        @ @.str.3
.L.str.3:
	.asciz	"func2 message."
	.size	.L.str.3, 15

	.type	.L.str.4,%object        @ @.str.4
.L.str.4:
	.asciz	"We are in func2, %s\n"
	.size	.L.str.4, 21

	.type	.L.str.5,%object        @ @.str.5
.L.str.5:
	.asciz	"func1 msg"
	.size	.L.str.5, 10

	.type	.L.str.6,%object        @ @.str.6
.L.str.6:
	.asciz	"We are in func1, %s\n"
	.size	.L.str.6, 21

	.type	.L.str.7,%object        @ @.str.7
.L.str.7:
	.asciz	"We are in main."
	.size	.L.str.7, 16

	.type	.L.str.8,%object        @ @.str.8
.L.str.8:
	.asciz	"We are in main, %s\n"
	.size	.L.str.8, 20


	.ident	"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
	.section	".note.GNU-stack","",%progbits
