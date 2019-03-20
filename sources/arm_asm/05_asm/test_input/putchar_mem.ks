  ldr r13,=0x08000000
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
putchar:
  stmdb r13!, {r1, r14}
  ldr r1,=0x101f1000
  str r0, [r1]
  ldmia r13!, {r1, r14}
  mov r15, r14
print:
  stmdb r13!, {r1, r2, r14}
  mov r2, r0
  ldrb r1,[r2]
_loop:
  mov r0, r1
  bl putchar
  add r2, r2, #0x1
  ldrb r1,[r2]
  cmp r1,#0x0
  bne _loop
  ldmia r13!, {r1, r2, r14}
  mov r15, r14
msg1:
  .raw "First text.\n"
msg2:
  .raw "Second text!\n"