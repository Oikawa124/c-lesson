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
	.file	"various_args.c"
	.globl	double_add              @ -- Begin function double_add
	.p2align	2
	.type	double_add,%function
	.code	32                      @ @double_add
double_add:
	.fnstart
@ %bb.0:
	push	{r4, r5, r11, lr}
	add	r11, sp, #8
	sub	sp, sp, #16
	str	r1, [sp, #12]
	str	r0, [sp, #8]
	str	r2, [sp, #4]
	ldr	r4, [sp, #8]
	ldr	r5, [sp, #12]
	ldr	r0, [sp, #4]
	bl	__floatsidf
	mov	r2, r0
	mov	r3, r1
	mov	r0, r4
	mov	r1, r5
	bl	__adddf3
	sub	sp, r11, #8
	pop	{r4, r5, r11, lr}
	mov	pc, lr
.Lfunc_end0:
	.size	double_add, .Lfunc_end0-double_add
	.fnend
                                        @ -- End function
	.globl	struct_arg              @ -- Begin function struct_arg
	.p2align	2
	.type	struct_arg,%function
	.code	32                      @ @struct_arg
struct_arg:
	.fnstart
@ %bb.0:
	sub	sp, sp, #16
	str	r3, [sp, #12]
	str	r2, [sp, #8]
	str	r1, [sp, #4]
	str	r0, [sp]
	mov	r0, sp
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	add	r0, r0, r1
	add	r0, r0, r2
	add	r0, r0, r3
	ldr	r1, [sp, #16]
	add	r0, r0, r1
	ldr	r1, [sp, #20]
	add	r0, r0, r1
	ldr	r1, [sp, #24]
	add	r0, r0, r1
	add	sp, sp, #16
	mov	pc, lr
.Lfunc_end1:
	.size	struct_arg, .Lfunc_end1-struct_arg
	.fnend
                                        @ -- End function
	.globl	struct_pointer_arg      @ -- Begin function struct_pointer_arg
	.p2align	2
	.type	struct_pointer_arg,%function
	.code	32                      @ @struct_pointer_arg
struct_pointer_arg:
	.fnstart
@ %bb.0:
	sub	sp, sp, #8
	str	r0, [sp]
	ldr	r0, [sp]
	ldr	r12, [r0]
	ldr	r2, [r0, #4]
	ldr	r3, [r0, #8]
	ldr	r1, [r0, #12]
	add	r2, r12, r2
	add	r2, r2, r3
	add	r1, r2, r1
	ldr	r2, [r0, #16]
	add	r1, r1, r2
	ldr	r2, [r0, #20]
	add	r1, r1, r2
	ldr	r0, [r0, #24]
	add	r0, r1, r0
	add	sp, sp, #8
	mov	pc, lr
.Lfunc_end2:
	.size	struct_pointer_arg, .Lfunc_end2-struct_pointer_arg
	.fnend
                                        @ -- End function
	.globl	main_double_add         @ -- Begin function main_double_add
	.p2align	2
	.type	main_double_add,%function
	.code	32                      @ @main_double_add
main_double_add:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	mov	r1, #266338304
	orr	r1, r1, #805306368
	mov	r0, #0
	mov	r2, #3
	bl	double_add
	str	r1, [sp, #4]
	str	r0, [sp]
	ldr	r2, [sp]
	ldr	r3, [sp, #4]
	ldr	r0, .LCPI3_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI3_0:
	.long	.L.str
.Lfunc_end3:
	.size	main_double_add, .Lfunc_end3-main_double_add
	.fnend
                                        @ -- End function
	.globl	main_struct_body        @ -- Begin function main_struct_body
	.p2align	2
	.type	main_struct_body,%function
	.code	32                      @ @main_struct_body
main_struct_body:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #48
	mov	r0, #7
	str	r0, [sp, #16]
	mov	r0, #6
	str	r0, [sp, #20]
	mov	r0, #5
	str	r0, [sp, #24]
	mov	r0, #4
	str	r0, [sp, #28]
	add	r0, sp, #16
	add	r0, r0, #16
	mov	r1, #3
	str	r1, [sp, #32]
	mov	r1, #2
	str	r1, [sp, #36]
	mov	r1, #1
	str	r1, [sp, #40]
	ldr	r1, [r0], #4
	mov	r2, sp
	str	r1, [r2], #4
	ldr	r1, [r0], #4
	str	r1, [r2], #4
	ldr	r1, [r0], #4
	str	r1, [r2], #4
	ldr	r0, [sp, #16]
	ldr	r1, [sp, #20]
	ldr	r2, [sp, #24]
	ldr	r3, [sp, #28]
	bl	struct_arg
	str	r0, [sp, #12]
	ldr	r1, [sp, #12]
	ldr	r0, .LCPI4_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI4_0:
	.long	.L.str.1
.Lfunc_end4:
	.size	main_struct_body, .Lfunc_end4-main_struct_body
	.fnend
                                        @ -- End function
	.globl	main_struct_pointer     @ -- Begin function main_struct_pointer
	.p2align	2
	.type	main_struct_pointer,%function
	.code	32                      @ @main_struct_pointer
main_struct_pointer:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #32
	mov	r0, #7
	str	r0, [sp, #4]
	mov	r0, #6
	str	r0, [sp, #8]
	mov	r0, #5
	str	r0, [sp, #12]
	mov	r0, #4
	str	r0, [sp, #16]
	mov	r0, #3
	str	r0, [sp, #20]
	mov	r0, #2
	str	r0, [sp, #24]
	mov	r0, #1
	str	r0, [sp, #28]
	add	r0, sp, #4
	bl	struct_pointer_arg
	str	r0, [sp]
	ldr	r1, [sp]
	ldr	r0, .LCPI5_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI5_0:
	.long	.L.str.1
.Lfunc_end5:
	.size	main_struct_pointer, .Lfunc_end5-main_struct_pointer
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
	bl	main_double_add
	bl	main_struct_body
	bl	main_struct_pointer
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end6:
	.size	main, .Lfunc_end6-main
	.fnend
                                        @ -- End function
	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"result is %f\n"
	.size	.L.str, 14

	.type	.L.str.1,%object        @ @.str.1
.L.str.1:
	.asciz	"starg res is %d\n"
	.size	.L.str.1, 17


	.ident	"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
	.section	".note.GNU-stack","",%progbits
