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
	.file	"string.c"
	.globl	print_string_array      @ -- Begin function print_string_array
	.p2align	2
	.type	print_string_array,%function
	.code	32                      @ @print_string_array
print_string_array:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	ldr	r0, .LCPI0_0
	ldr	r1, .LCPI0_1
	bl	printf
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI0_0:
	.long	.L.str.1
.LCPI0_1:
	.long	string_array
.Lfunc_end0:
	.size	print_string_array, .Lfunc_end0-print_string_array
	.fnend
                                        @ -- End function
	.globl	print_string_ptr        @ -- Begin function print_string_ptr
	.p2align	2
	.type	print_string_ptr,%function
	.code	32                      @ @print_string_ptr
print_string_ptr:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	ldr	r0, .LCPI1_0
	ldr	r1, [r0]
	ldr	r0, .LCPI1_1
	bl	printf
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI1_0:
	.long	string_ptr
.LCPI1_1:
	.long	.L.str.1
.Lfunc_end1:
	.size	print_string_ptr, .Lfunc_end1-print_string_ptr
	.fnend
                                        @ -- End function
	.globl	print_sizeof            @ -- Begin function print_sizeof
	.p2align	2
	.type	print_sizeof,%function
	.code	32                      @ @print_sizeof
print_sizeof:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	ldr	r0, .LCPI2_0
	mov	r2, #14
	mov	r3, #0
	bl	printf
	ldr	r0, .LCPI2_1
	mov	r2, #8
	mov	r3, #0
	bl	printf
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI2_0:
	.long	.L.str.2
.LCPI2_1:
	.long	.L.str.3
.Lfunc_end2:
	.size	print_sizeof, .Lfunc_end2-print_sizeof
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
	bl	print_string_array
	bl	print_string_ptr
	bl	print_sizeof
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end3:
	.size	main, .Lfunc_end3-main
	.fnend
                                        @ -- End function
	.type	string_array,%object    @ @string_array
	.data
	.globl	string_array
string_array:
	.asciz	"This is array"
	.size	string_array, 14

	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"This is pointer"
	.size	.L.str, 16

	.type	string_ptr,%object      @ @string_ptr
	.data
	.globl	string_ptr
	.p2align	3
string_ptr:
	.long	.L.str
	.size	string_ptr, 4

	.type	.L.str.1,%object        @ @.str.1
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str.1:
	.asciz	"%s\n"
	.size	.L.str.1, 4

	.type	.L.str.2,%object        @ @.str.2
.L.str.2:
	.asciz	"str array sizeof: %d\n"
	.size	.L.str.2, 22

	.type	.L.str.3,%object        @ @.str.3
.L.str.3:
	.asciz	"str ptr sizeof: %d\n"
	.size	.L.str.3, 20


	.ident	"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
	.section	".note.GNU-stack","",%progbits
