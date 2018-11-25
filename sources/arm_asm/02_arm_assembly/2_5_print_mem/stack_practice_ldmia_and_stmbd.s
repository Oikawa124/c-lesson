.globl _start
_start:
  ldr r1,=0x101f1000

  mov r3, #0x34

  stmdb r13!, {r3}
  ldmia r13!, {r2}

  /* r2の文字列が表示されない、空白だけ表示される*/
  str r2, [r1]

  b busy

busy:
  b busy