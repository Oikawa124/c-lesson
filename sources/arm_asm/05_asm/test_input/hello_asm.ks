ldr r0, [r15, #0x30]
mov r1,#0x68
str r1,[r0]
mov r1,#0x65
str r1,[r0]
mov r1,#0x6c
str r1,[r0]
mov r1,#0x6f
str r1,[r0]
mov r2,#0x0D
str r2,[r0]
mov r2,#0x0A
str r2,[r0]
loop:
    b loop