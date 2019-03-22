	.cpu arm7tdmi
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"print_msg.c"
	.text
	.align	2
	.global	print_msg
	.syntax unified
	.arm
	.fpu softvfp
	.type	print_msg, %function
print_msg:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #8
	str	r0, [sp, #4]
	b	.L2
.L3:
	ldr	r3, [sp, #4]
	add	r2, r3, #1
	str	r2, [sp, #4]
	ldr	r2, .L4
	ldrb	r3, [r3]	@ zero_extendqisi2
	strb	r3, [r2]
.L2:
	ldr	r3, [sp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L3
	nop
	add	sp, sp, #8
	@ sp needed
	bx	lr
.L5:
	.align	2
.L4:
	.word	270471168
	.size	print_msg, .-print_msg
	.ident	"GCC: (GNU Tools for Arm Embedded Processors 7-2018-q3-update) 7.3.1 20180622 (release) [ARM/embedded-7-branch revision 261907]"
