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
	.file	"pointer_array.c"
	.globl	print_one               @ -- Begin function print_one
	.p2align	2
	.type	print_one,%function
	.code	32                      @ @print_one
print_one:
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
	.size	print_one, .Lfunc_end0-print_one
	.fnend
                                        @ -- End function
	.globl	print_one_struct        @ -- Begin function print_one_struct
	.p2align	2
	.type	print_one_struct,%function
	.code	32                      @ @print_one_struct
print_one_struct:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #16
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	ldr	r1, [r0]
	ldr	r2, [r0, #4]
	ldr	r3, [r0, #8]
	ldr	r0, [r0, #12]
	str	r0, [sp]
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
	.size	print_one_struct, .Lfunc_end1-print_one_struct
	.fnend
                                        @ -- End function
	.globl	iterate_array           @ -- Begin function iterate_array
	.p2align	2
	.type	iterate_array,%function
	.code	32                      @ @iterate_array
iterate_array:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp]
	ldr	r0, [sp]
	ldr	r0, [r0]
	bl	print_one
	ldr	r0, [sp]
	ldr	r0, [r0, #4]
	bl	print_one
	ldr	r0, [sp]
	ldr	r0, [r0, #8]
	bl	print_one
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end2:
	.size	iterate_array, .Lfunc_end2-iterate_array
	.fnend
                                        @ -- End function
	.globl	iterate_array_ptr       @ -- Begin function iterate_array_ptr
	.p2align	2
	.type	iterate_array_ptr,%function
	.code	32                      @ @iterate_array_ptr
iterate_array_ptr:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp]
	ldr	r0, [sp]
	add	r1, r0, #4
	str	r1, [sp]
	ldr	r0, [r0]
	bl	print_one
	ldr	r0, [sp]
	add	r1, r0, #4
	str	r1, [sp]
	ldr	r0, [r0]
	bl	print_one
	ldr	r0, [sp]
	add	r1, r0, #4
	str	r1, [sp]
	ldr	r0, [r0]
	bl	print_one
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end3:
	.size	iterate_array_ptr, .Lfunc_end3-iterate_array_ptr
	.fnend
                                        @ -- End function
	.globl	iterate_struct_array_ptr @ -- Begin function iterate_struct_array_ptr
	.p2align	2
	.type	iterate_struct_array_ptr,%function
	.code	32                      @ @iterate_struct_array_ptr
iterate_struct_array_ptr:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp]
	ldr	r0, [sp]
	add	r1, r0, #16
	str	r1, [sp]
	bl	print_one_struct
	ldr	r0, [sp]
	add	r1, r0, #16
	str	r1, [sp]
	bl	print_one_struct
	ldr	r0, [sp]
	add	r1, r0, #16
	str	r1, [sp]
	bl	print_one_struct
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end4:
	.size	iterate_struct_array_ptr, .Lfunc_end4-iterate_struct_array_ptr
	.fnend
                                        @ -- End function
	.globl	write_pointer           @ -- Begin function write_pointer
	.p2align	2
	.type	write_pointer,%function
	.code	32                      @ @write_pointer
write_pointer:
	.fnstart
@ %bb.0:
	sub	sp, sp, #8
	str	r0, [sp]
	ldr	r0, [sp]
	add	r1, r0, #4
	str	r1, [sp]
	mov	r1, #123
	str	r1, [r0]
	ldr	r0, [sp]
	add	r1, r0, #4
	str	r1, [sp]
	mov	r1, #456
	str	r1, [r0]
	add	sp, sp, #8
	mov	pc, lr
.Lfunc_end5:
	.size	write_pointer, .Lfunc_end5-write_pointer
	.fnend
                                        @ -- End function
	.globl	setup_arrays            @ -- Begin function setup_arrays
	.p2align	2
	.type	setup_arrays,%function
	.code	32                      @ @setup_arrays
setup_arrays:
	.fnstart
@ %bb.0:
	sub	sp, sp, #4
	mov	r0, #0
	str	r0, [sp]
	b	.LBB6_1
.LBB6_1:                                @ =>This Inner Loop Header: Depth=1
	ldr	r0, [sp]
	cmp	r0, #1024
	bge	.LBB6_4
	b	.LBB6_2
.LBB6_2:                                @   in Loop: Header=BB6_1 Depth=1
	ldr	r0, [sp]
	rsb	r1, r0, #1024
	ldr	r2, .LCPI6_0
	str	r1, [r2, r0, lsl #2]
	ldr	r0, [sp]
	ldr	r1, .LCPI6_1
	str	r0, [r1, r0, lsl #4]
	ldr	r0, [sp]
	lsl	r2, r0, #1
	add	r0, r1, r0, lsl #4
	str	r2, [r0, #4]
	ldr	r0, [sp]
	add	r2, r0, r0, lsl #1
	add	r0, r1, r0, lsl #4
	str	r2, [r0, #8]
	ldr	r0, [sp]
	lsl	r2, r0, #2
	add	r0, r1, r0, lsl #4
	str	r2, [r0, #12]
	b	.LBB6_3
.LBB6_3:                                @   in Loop: Header=BB6_1 Depth=1
	ldr	r0, [sp]
	add	r0, r0, #1
	str	r0, [sp]
	b	.LBB6_1
.LBB6_4:
	add	sp, sp, #4
	mov	pc, lr
	.p2align	2
@ %bb.5:
.LCPI6_0:
	.long	intarray
.LCPI6_1:
	.long	structArray
.Lfunc_end6:
	.size	setup_arrays, .Lfunc_end6-setup_arrays
	.fnend
                                        @ -- End function
	.globl	main                    @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                      @ @main
main:
	.fnstart
@ %bb.0:
	push	{r4, r10, r11, lr}
	add	r11, sp, #8
	sub	sp, sp, #8
	mov	r0, #0
	str	r0, [sp, #4]
	bl	setup_arrays
	ldr	r4, .LCPI7_0
	mov	r0, r4
	bl	write_pointer
	mov	r0, r4
	bl	iterate_array
	mov	r0, r4
	bl	iterate_array_ptr
	ldr	r0, .LCPI7_1
	bl	iterate_struct_array_ptr
	mov	r0, #0
	sub	sp, r11, #8
	pop	{r4, r10, r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI7_0:
	.long	intarray
.LCPI7_1:
	.long	structArray
.Lfunc_end7:
	.size	main, .Lfunc_end7-main
	.fnend
                                        @ -- End function
	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"%d\n"
	.size	.L.str, 4

	.type	.L.str.1,%object        @ @.str.1
.L.str.1:
	.asciz	"%d, %d, %d, %d\n"
	.size	.L.str.1, 16

	.type	intarray,%object        @ @intarray
	.comm	intarray,4096,16
	.type	structArray,%object     @ @structArray
	.comm	structArray,16384,16

	.ident	"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
	.section	".note.GNU-stack","",%progbits
