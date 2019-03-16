    ldr r0,=0x101f1000
    ldr r1,=0xdeadbeaf
    b print_hex
print_hex:
    mov r3, #0x1c
    mov r2, #0x30
    str r2, [r0]
    mov r2, #0x78
    str r2, [r0]
_loop:
    lsr r2, r1, r3
    and r2, r2, #0x0f
    cmp r2, #0x0a
    blt _under_ten
    add r2, r2, #0x07
_under_ten:
    add r2, r2, #0x30
    str r2, [r0]
    sub r3, r3, #0x4
    cmp r3, #0x0
    bge _loop
    b busy
busy:
    b busy