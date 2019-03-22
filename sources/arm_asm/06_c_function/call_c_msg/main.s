.globl _start
_start:
    ldr r13,=0x07FFFFFF
    ldr r0,=msg

    // r0にmsgのアドレスを渡す

    // print_msg:のアセンブリから
    // str     r0, [sp, #4]
    // となっている。
    // r0のアドレスを [sp, #4]に渡す。

    // Hello Worldと表示するようにしてください。
    // 
    bl print_msg
loop:
    b loop
msg:
    .asciz "Hello World\n"
