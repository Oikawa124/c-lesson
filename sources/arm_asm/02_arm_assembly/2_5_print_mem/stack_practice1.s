.globl _start
_start:
  ldr r1,=0x101f1000

  mov r5, #0x31
  ldr r13,=0x08000000
  sub r13, r13, #1
  str r5, [r13]
  ldr r5, [r13]
  add r13, r13, #1
  str r5, [r1]

  b busy


busy:
  b busy



