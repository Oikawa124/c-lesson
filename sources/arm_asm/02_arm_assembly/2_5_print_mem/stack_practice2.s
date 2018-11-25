.globl _start

/*
    arg r1: address of memory mapped UART.
    arg r13: stack pointer.

    used internal register: r1, r4, r5, r13.
*/

_start:
  ldr r1,=0x101f1000

  mov r5, #0x31
  mov r4, #0x34

  ldr r13,=0x08000000

  sub r13, r13, #4
  str r5, [r13]

  sub r13, r13, #4
  str r4, [r13]

  ldr r4, [r13]
  add r13, r13, #4

  ldr r5, [r13]
  add r13, r13, #4

  str r4, [r1]
  str r5, [r1]


  b busy

busy:
  b busy

# output >> "41"