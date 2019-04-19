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
	.file	"sizeof.c"
	.globl	print_size              @ -- Begin function print_size
	.p2align	2
	.type	print_size,%function
	.code	32                      @ @print_size
print_size:
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
	.size	print_size, .Lfunc_end0-print_size
	.fnend
                                        @ -- End function
	.globl	array_size              @ -- Begin function array_size
	.p2align	2
	.type	array_size,%function
	.code	32                      @ @array_size
array_size:
	.fnstart
@ %bb.0:
	push	{r4, r5, r11, lr}
	add	r11, sp, #8
	sub	sp, sp, #16
	sub	sp, sp, #4096
	bic	sp, sp, #15
	mov	r0, #4096
	str	r0, [sp, #12]
	ldr	r0, [sp, #12]
	bl	print_size
	sub	sp, r11, #8
	pop	{r4, r5, r11, lr}
	mov	pc, lr
.Lfunc_end1:
	.size	array_size, .Lfunc_end1-array_size
	.fnend
                                        @ -- End function
	.globl	ptr_size                @ -- Begin function ptr_size
	.p2align	2
	.type	ptr_size,%function
	.code	32                      @ @ptr_size
ptr_size:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #16
	mov	r0, #1024
	mov	r1, #0
	bl	malloc
	str	r0, [sp, #8]
	mov	r0, #8
	str	r0, [sp, #4]
	ldr	r0, [sp, #4]
	bl	print_size
	ldr	r0, [sp, #8]
	bl	free
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end2:
	.size	ptr_size, .Lfunc_end2-ptr_size
	.fnend
                                        @ -- End function
	.globl	struct_size             @ -- Begin function struct_size
	.p2align	2
	.type	struct_size,%function
	.code	32                      @ @struct_size
struct_size:
	.fnstart
@ %bb.0:
	push	{r4, r10, r11, lr}
	add	r11, sp, #8
	sub	sp, sp, #32
	mov	r4, #24
	str	r4, [sp, #4]
	ldr	r0, [sp, #4]
	bl	print_size
	str	r4, [sp, #4]
	ldr	r0, [sp, #4]
	bl	print_size
	sub	sp, r11, #8
	pop	{r4, r10, r11, lr}
	mov	pc, lr
.Lfunc_end3:
	.size	struct_size, .Lfunc_end3-struct_size
	.fnend
                                        @ -- End function
	.globl	var_array               @ -- Begin function var_array
	.p2align	2
	.type	var_array,%function
	.code	32                      @ @var_array
var_array:
	.fnstart
@ %bb.0:
	push	{r4, r5, r6, r7, r11, lr}
	add	r11, sp, #16
	sub	sp, sp, #24
	bic	sp, sp, #15
	mov	r6, sp
	add	r4, r6, #4
	str	r0, [r4, #8]
	ldr	r0, [r4, #8]
	str	sp, [r4, #4]
	mov	r1, #7
	add	r1, r1, r0, lsl #2
	bic	r1, r1, #7
	sub	r1, sp, r1
	bic	sp, r1, #15
	lsl	r0, r0, #2
	str	r0, [r4]
	ldr	r0, [r4]
	bl	print_size
	ldr	sp, [r4, #4]
	sub	sp, r11, #16
	pop	{r4, r5, r6, r7, r11, lr}
	mov	pc, lr
.Lfunc_end4:
	.size	var_array, .Lfunc_end4-var_array
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
	bl	array_size
	bl	ptr_size
	bl	struct_size
	mov	r0, #123
	bl	var_array
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end5:
	.size	main, .Lfunc_end5-main
	.fnend
                                        @ -- End function
	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"size: %d\n"
	.size	.L.str, 10


	.ident	"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
	.section	".note.GNU-stack","",%progbits
