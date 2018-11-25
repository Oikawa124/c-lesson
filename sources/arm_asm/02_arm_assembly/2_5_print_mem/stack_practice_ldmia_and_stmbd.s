.globl _start
_start:
  ldr r1,=0x101f1000

  mov r3, #0x34
  ldr r13,=0x08000000

  stmdb r13!, {r3}
  ldmia r13!, {r2}

  str r2, [r1]

  b busy

busy:
  b busy