.globl _start
_start:
  ldr r1,=0x101f1000

  mov r5, #0x31
  mov r4, #0x34

  ldr r13,=0x08000000

  sub r13, r13, #1
  str r5, [r13]

  sub r13, r13, #1
  str r4, [r13]

  ldr r4, [r13]
  add r13, r13, #1

  ldr r5, [r13]
  add r13, r13, #1

  /* r4だけ表示される r5はスペース?なのか、文字がでない
     先にpopしたほうだけ表示される。(r4とr5を逆にしても
     r4だけ表示)*/

  str r4, [r1]
  str r5, [r1]


  b busy

busy:
  b busy

# output >> "4 "
# ""は表示された文字列。4の左がスペース?のように空いている。