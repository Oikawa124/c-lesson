    ldr r1,=0x101f1000
    mov r0, r15
    bl print_hex
    mov r0, #0x68
    bl print_hex
    b end
print_hex:
    mov r3, #0x28
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
    sub r3, r3, #0x4
    cmp r3, #0x0
    bge _loop
    mov r2, #0x0a
    str r2, [r1]
    mov r15, r14
end:
    b end