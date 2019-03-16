  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
print:
  ldrb r3,[r0]
  ldr r1,=0x101f1000
loop:
  str r3,[r1]
  add r0, r0, #0x1
  ldrb r3,[r0]
  cmp r3,#0x0
  bne loop
  mov r15, r14
msg1:
.raw "First text.\n"
msg2:
.raw "Second text!\n"
