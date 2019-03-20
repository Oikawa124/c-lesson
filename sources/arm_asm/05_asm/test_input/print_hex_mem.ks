  ldr r13,=0x08000000
  mov r0,r15
  bl print_hex
  mov r0, #0x68
  bl print_hex
end:
  b end
putchar:
  stmdb r13!, {r1, r14}
  ldr r1,=0x101f1000
  str r0, [r1]
  ldmia r13!, {r1, r14}
  mov r15, r14
print_hex:
  stmdb r13!, {r1, r2, r14}
  stmdb r13!, {r0}
  mov r0, #0x30
  bl putchar
  mov r0, #0x78
  bl putchar
  ldmia r13!, {r0}
  mov r2, #0x1c
  mov r1, r0
_loop:
  lsr r0, r1, r2
  and r0, r0, #0x0f
  cmp r0, #0x0a
  blt _under_ten
  add r0, r0, #0x07
_under_ten:
  add r0, r0, #0x30
  bl putchar
  sub r2, r2, #0x4
  cmp r2, #0x0
  bge _loop
  mov r0, #0x0a
  bl putchar
  ldmia r13!, {r1, r2, r14}
  mov r15, r14