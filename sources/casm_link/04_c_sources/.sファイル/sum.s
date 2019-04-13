        .text
        .syntax unified
        .eabi_attribute 67, "2.09"      @ Tag_conformance
        .eabi_attribute 6, 1    @ Tag_CPU_arch
        .eabi_attribute 8, 1    @ Tag_ARM_ISA_use
        .eabi_attribute 17, 1   @ Tag_ABI_PCS_GOT_use
        .eabi_attribute 20, 1   @ Tag_ABI_FP_denormal
        .eabi_attribute 21, 1   @ Tag_ABI_FP_exceptions
        .eabi_attribute 23, 3   @ Tag_ABI_FP_number_model
        .eabi_attribute 34, 1   @ Tag_CPU_unaligned_access
        .eabi_attribuibute 25te 24, 1   @ Tag_ABI_align_needed
                                  .eabi_attr, 1   @ Tag_ABI_align_preserved
        .eabi_attribute 38, 1   @ Tag_ABI_FP_16bit_format
        .eabi_attribute 14, 0   @ Tag_ABI_PCS_R9_use
        .file   "sum.bc"
        .globl  sum
        .align  2
        .type   sum,%function
sum:                                    @ @sum
        .fnstart
@ BB#0:
        sub     sp, sp, #16
        str     r0, [sp, #12]
        mov     r0, #0
        str     r1, [sp, #8]
        str     r0, [sp, #4]
        ldr     r0, [sp, #12]
        b       .LBB0_2
.LBB0_1:                                @   in Loop: Header=BB0_2 Depth=1
        ldm     sp, {r0, r1}
        add     r0, r1, r0
        str     r0, [sp, #4]
        ldr     r0, [sp]
        add     r0, r0, #1
.LBB0_2:                                @ =>This Inner Loop Header: Depth=1
        str     r0, [sp]
        ldr     r0, [sp, #8]
        ldr     r1, [sp]
        cmp     r1, r0
        ble     .LBB0_1
@ BB#3:
        ldr     r0, [sp, #4]
        add     sp, sp, #16
        mov     pc, lr
.Lfunc_end0:
        .size   sum, .Lfunc_end0-sum
        .fnend

        .globl  main
        .align  2
        .type   main,%function
main:                                   @ @main
        .fnstart
@ BB#0:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        mov     r1, #12
        str     r0, [sp, #4]
        mov     r0, #5
        bl      sum
        mov     r1, r0
        ldr     r0, .LCPI1_0
        str     r1, [sp]
        bl      printf
        mov     r0, #0
        mov     sp, r11
        pop     {r11, lr}
        mov     pc, lr
        .align  2
@ BB#1:
.LCPI1_0:
        .long   .L.str
.Lfunc_end1:
        .size   main, .Lfunc_end1-main
        .fnend

        .type   .L.str,%object          @ @.str
        .section        .rodata.str1.1,"aMS",%progbits,1
.L.str:
        .asciz  "result is %d\n"
        .size   .L.str, 14


        .ident  "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
        .section        ".note.GNU-stack","",%progbits
