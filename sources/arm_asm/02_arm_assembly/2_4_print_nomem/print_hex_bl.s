.globl _start
_start:
    ldr r1,=0x101f1000
    mov r0, r15
    bl print_hex

    mov r0, #0x68
    bl print_hex
    b end

/*
    print_hex:
    arg r1: address of memory mapped UART.
    arg r0: address of target string.
    arg r3: counter for shifting by 4bits.
    arg r14: link register.
    arg r15: program counter.
    used internal register: r0, r1, r2, r3.

*/


print_hex:
    mov r3, #28
    mov r2, #0x30
    str r2, [r1]
    mov r2, #0x78
    str r2, [r1]

_loop:
    lsr r2, r0, r3
    and r2, r2, #0x0f
    cmp r2, #0x0a
    blt _under_ten
    add r2, r2, #0x07

_under_ten:
    add r2, r2, #0x30
    str r2, [r1]

    sub r3, r3, #4

    cmp r3, #0
    bge _loop

    /* #0x0a is line feed */
    mov r2, #0x0a
    str r2, [r1]
    mov r15, r14

end:
    b end
