  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
putchar:
  ldr r1,=0x101f1000
  str r0, [r1]
  mov r15, r14
print:
  ldrb r3,[r0]
  mov r2, r0
  mov r4, r14
_loop:
  mov r0, r3
  bl putchar
  add r2, r2, #0x1
  ldrb r3,[r2]
  cmp r3,#0x0
  bne _loop
  mov r15, r4
msg1:
.raw "First text.\n"
msg2:
.raw "Second text!\n"