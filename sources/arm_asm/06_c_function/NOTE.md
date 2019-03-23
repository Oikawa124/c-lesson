#第三回 バイナリやアセンブリから見るC言語とリンカ

## 01 アセンブリからC関数を呼ぶ

課題: アセンブリから文字列を渡して表示してみる
をおこなった。


## 02 分割コンパイルとリンク

### コンパイラとQEMUのセットアップ

<DONE>

Hello Worldと表示された。

### 分割コンパイルをしてみる。


(DONE)分割コンパイルのやり方


(DONE)nmでシンボルを確認する

#### 実行結果
```
00000000 t $a
00000000 r $d
0000001c t $d
00000000 T main
         U print_something
```

```
00000000 t $a
00000000 T func1
00000024 T print_something
         U puts
```


(DONE)オブジェクトファイルとは何か？


オブジェクトファイルの時点では、最終的なアドレスは決まってない。 このオブジェクトファイル内だけの相対アドレスとして、先頭が0番であるかのようにバイナリを生成します。


[オブジェクトファイル(wikipedia)](https://ja.wikipedia.org/wiki/%E3%82%AA%E3%83%96%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB)

>オブジェクトファイル (object file) またはオブジェクトコード (object code) とは、コンパイラがソースコードを処理した結果生成される、コード生成の結果であるバイナリコードを含む中間的なデータ表現のファイルである。中身は、機械語バイナリとそれに付随するシンボルテーブルやリロケーションテーブルといった付加情報であり、さらにデバッグ支援情報や近年はリンク時最適化等のための高度な情報が含まれる場合もある。オブジェクトファイル群をリンクすることによって最終的な実行ファイルやライブラリが作成される。オブジェクトファイルのほとんどは機械語である(コンピュータのCPUが直接実行できるコード)。オブジェクトファイルフォーマットはコンパイラやアセンブラが生成するオブジェクトファイルのファイルフォーマットである。


(DONE)2つのアセンブリファイルをリンクしてみる

arm-linux-gnueabi-gcc hello_printf.c main.c -static  
nm a.out  
大量のシンボルが出てくることを確認  


()解決されてないシンボルのバイナリを眺める


#### odjdump結果
```
Disassembly of section .text:

00000000 <main>:
   0:   e92d4800        push    {fp, lr}
   4:   e28db004        add     fp, sp, #4
   8:   e59f000c        ldr     r0, [pc, #12]   ; 1c <main+0x1c>
   c:   ebfffffe        bl      0 <print_something>
  10:   e3a03000        mov     r3, #0
  14:   e1a00003        mov     r0, r3
  18:   e8bd8800        pop     {fp, pc}
  1c:   00000000        .word   0x00000000
```

#### a.outのobjdump結果
```
gaku@DESKTOP-IB1OCGR:~$ arm-linux-gnueabi-objdump -S a.out

a.out:     file format elf32-littlearm


Disassembly of section .init:

000102bc <_init>:
   102bc:       e92d4008        push    {r3, lr}
   102c0:       eb000020        bl      10348 <call_weak_fn>
   102c4:       e8bd8008        pop     {r3, pc}

Disassembly of section .plt:

000102c8 <puts@plt-0x14>:
   102c8:       e52de004        push    {lr}            ; (str lr, [sp, #-4]!)
   102cc:       e59fe004        ldr     lr, [pc, #4]    ; 102d8 <_init+0x1c>
   102d0:       e08fe00e        add     lr, pc, lr
   102d4:       e5bef008        ldr     pc, [lr, #8]!
   102d8:       00010d28        .word   0x00010d28

000102dc <puts@plt>:
   102dc:       e28fc600        add     ip, pc, #0, 12
   102e0:       e28cca10        add     ip, ip, #16, 20 ; 0x10000
   102e4:       e5bcfd28        ldr     pc, [ip, #3368]!        ; 0xd28

000102e8 <__libc_start_main@plt>:
   102e8:       e28fc600        add     ip, pc, #0, 12
   102ec:       e28cca10        add     ip, ip, #16, 20 ; 0x10000
   102f0:       e5bcfd20        ldr     pc, [ip, #3360]!        ; 0xd20

000102f4 <__gmon_start__@plt>:
   102f4:       e28fc600        add     ip, pc, #0, 12
   102f8:       e28cca10        add     ip, ip, #16, 20 ; 0x10000
   102fc:       e5bcfd18        ldr     pc, [ip, #3352]!        ; 0xd18

00010300 <abort@plt>:
   10300:       e28fc600        add     ip, pc, #0, 12
   10304:       e28cca10        add     ip, ip, #16, 20 ; 0x10000
   10308:       e5bcfd10        ldr     pc, [ip, #3344]!        ; 0xd10

Disassembly of section .text:

0001030c <_start>:
   1030c:       e3a0b000        mov     fp, #0
   10310:       e3a0e000        mov     lr, #0
   10314:       e49d1004        pop     {r1}            ; (ldr r1, [sp], #4)
   10318:       e1a0200d        mov     r2, sp
   1031c:       e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   10320:       e52d0004        push    {r0}            ; (str r0, [sp, #-4]!)
   10324:       e59fc010        ldr     ip, [pc, #16]   ; 1033c <_start+0x30>
   10328:       e52dc004        push    {ip}            ; (str ip, [sp, #-4]!)
   1032c:       e59f000c        ldr     r0, [pc, #12]   ; 10340 <_start+0x34>
   10330:       e59f300c        ldr     r3, [pc, #12]   ; 10344 <_start+0x38>
   10334:       ebffffeb        bl      102e8 <__libc_start_main@plt>
   10338:       ebfffff0        bl      10300 <abort@plt>
   1033c:       000104fc        .word   0x000104fc
   10340:       0001047c        .word   0x0001047c
   10344:       0001049c        .word   0x0001049c

00010348 <call_weak_fn>:
   10348:       e59f3014        ldr     r3, [pc, #20]   ; 10364 <call_weak_fn+0x1c>
   1034c:       e59f2014        ldr     r2, [pc, #20]   ; 10368 <call_weak_fn+0x20>
   10350:       e08f3003        add     r3, pc, r3
   10354:       e7932002        ldr     r2, [r3, r2]
   10358:       e3520000        cmp     r2, #0
   1035c:       012fff1e        bxeq    lr
   10360:       eaffffe3        b       102f4 <__gmon_start__@plt>
   10364:       00010ca8        .word   0x00010ca8
   10368:       0000001c        .word   0x0000001c

0001036c <deregister_tm_clones>:
   1036c:       e59f301c        ldr     r3, [pc, #28]   ; 10390 <deregister_tm_clones+0x24>
   10370:       e59f001c        ldr     r0, [pc, #28]   ; 10394 <deregister_tm_clones+0x28>
   10374:       e0433000        sub     r3, r3, r0
   10378:       e3530006        cmp     r3, #6
   1037c:       912fff1e        bxls    lr
   10380:       e59f3010        ldr     r3, [pc, #16]   ; 10398 <deregister_tm_clones+0x2c>
   10384:       e3530000        cmp     r3, #0
   10388:       012fff1e        bxeq    lr
   1038c:       e12fff13        bx      r3
   10390:       0002102b        .word   0x0002102b
   10394:       00021028        .word   0x00021028
   10398:       00000000        .word   0x00000000

0001039c <register_tm_clones>:
   1039c:       e59f1024        ldr     r1, [pc, #36]   ; 103c8 <register_tm_clones+0x2c>
   103a0:       e59f0024        ldr     r0, [pc, #36]   ; 103cc <register_tm_clones+0x30>
   103a4:       e0411000        sub     r1, r1, r0
   103a8:       e1a01141        asr     r1, r1, #2
   103ac:       e0811fa1        add     r1, r1, r1, lsr #31
   103b0:       e1b010c1        asrs    r1, r1, #1
   103b4:       012fff1e        bxeq    lr
   103b8:       e59f3010        ldr     r3, [pc, #16]   ; 103d0 <register_tm_clones+0x34>
   103bc:       e3530000        cmp     r3, #0
   103c0:       012fff1e        bxeq    lr
   103c4:       e12fff13        bx      r3
   103c8:       00021028        .word   0x00021028
   103cc:       00021028        .word   0x00021028
   103d0:       00000000        .word   0x00000000

000103d4 <__do_global_dtors_aux>:
   103d4:       e92d4010        push    {r4, lr}
   103d8:       e59f4018        ldr     r4, [pc, #24]   ; 103f8 <__do_global_dtors_aux+0x24>
   103dc:       e5d43000        ldrb    r3, [r4]
   103e0:       e3530000        cmp     r3, #0
   103e4:       18bd8010        popne   {r4, pc}
   103e8:       ebffffdf        bl      1036c <deregister_tm_clones>
   103ec:       e3a03001        mov     r3, #1
   103f0:       e5c43000        strb    r3, [r4]
   103f4:       e8bd8010        pop     {r4, pc}
   103f8:       00021028        .word   0x00021028

000103fc <frame_dummy>:
   103fc:       e59f0028        ldr     r0, [pc, #40]   ; 1042c <frame_dummy+0x30>
   10400:       e5903000        ldr     r3, [r0]
   10404:       e3530000        cmp     r3, #0
   10408:       1a000000        bne     10410 <frame_dummy+0x14>
   1040c:       eaffffe2        b       1039c <register_tm_clones>
   10410:       e59f3018        ldr     r3, [pc, #24]   ; 10430 <frame_dummy+0x34>
   10414:       e3530000        cmp     r3, #0
   10418:       0afffffb        beq     1040c <frame_dummy+0x10>
   1041c:       e92d4010        push    {r4, lr}
   10420:       e12fff33        blx     r3
   10424:       e8bd4010        pop     {r4, lr}
   10428:       eaffffdb        b       1039c <register_tm_clones>
   1042c:       00020f14        .word   0x00020f14
   10430:       00000000        .word   0x00000000

00010434 <func1>:
   10434:       e92d4800        push    {fp, lr}
   10438:       e28db004        add     fp, sp, #4
   1043c:       e24dd008        sub     sp, sp, #8
   10440:       e50b0008        str     r0, [fp, #-8]
   10444:       e51b0008        ldr     r0, [fp, #-8]
   10448:       ebffffa3        bl      102dc <puts@plt>
   1044c:       e1a00000        nop                     ; (mov r0, r0)
   10450:       e24bd004        sub     sp, fp, #4
   10454:       e8bd8800        pop     {fp, pc}

00010458 <print_something>:
   10458:       e92d4800        push    {fp, lr}
   1045c:       e28db004        add     fp, sp, #4
   10460:       e24dd008        sub     sp, sp, #8
   10464:       e50b0008        str     r0, [fp, #-8]
   10468:       e51b0008        ldr     r0, [fp, #-8]
   1046c:       ebfffff0        bl      10434 <func1>
   10470:       e1a00000        nop                     ; (mov r0, r0)
   10474:       e24bd004        sub     sp, fp, #4
   10478:       e8bd8800        pop     {fp, pc}

0001047c <main>:
   1047c:       e92d4800        push    {fp, lr}
   10480:       e28db004        add     fp, sp, #4
   10484:       e59f000c        ldr     r0, [pc, #12]   ; 10498 <main+0x1c>
   10488:       ebfffff2        bl      10458 <print_something>
   1048c:       e3a03000        mov     r3, #0
   10490:       e1a00003        mov     r0, r3
   10494:       e8bd8800        pop     {fp, pc}
   10498:       0001050c        .word   0x0001050c

0001049c <__libc_csu_init>:
   1049c:       e92d47f0        push    {r4, r5, r6, r7, r8, r9, sl, lr}
   104a0:       e59f604c        ldr     r6, [pc, #76]   ; 104f4 <__libc_csu_init+0x58>
   104a4:       e59f504c        ldr     r5, [pc, #76]   ; 104f8 <__libc_csu_init+0x5c>
   104a8:       e08f6006        add     r6, pc, r6
   104ac:       e08f5005        add     r5, pc, r5
   104b0:       e0466005        sub     r6, r6, r5
   104b4:       e1a07000        mov     r7, r0
   104b8:       e1a08001        mov     r8, r1
   104bc:       e1a09002        mov     r9, r2
   104c0:       ebffff7d        bl      102bc <_init>
   104c4:       e1b06146        asrs    r6, r6, #2
   104c8:       08bd87f0        popeq   {r4, r5, r6, r7, r8, r9, sl, pc}
   104cc:       e3a04000        mov     r4, #0
   104d0:       e2844001        add     r4, r4, #1
   104d4:       e4953004        ldr     r3, [r5], #4
   104d8:       e1a02009        mov     r2, r9
   104dc:       e1a01008        mov     r1, r8
   104e0:       e1a00007        mov     r0, r7
   104e4:       e12fff33        blx     r3
   104e8:       e1540006        cmp     r4, r6
   104ec:       1afffff7        bne     104d0 <__libc_csu_init+0x34>
   104f0:       e8bd87f0        pop     {r4, r5, r6, r7, r8, r9, sl, pc}
   104f4:       00010a60        .word   0x00010a60
   104f8:       00010a58        .word   0x00010a58

000104fc <__libc_csu_fini>:
   104fc:       e12fff1e        bx      lr

Disassembly of section .fini:

00010500 <_fini>:
   10500:       e92d4008        push    {r3, lr}
   10504:       e8bd8008        pop     {r3, pc}
```


#### rodata
```
gaku@DESKTOP-IB1OCGR:~$ arm-linux-gnueabi-objdump -S -j .rodata a.out

a.out:     file format elf32-littlearm


Disassembly of section .rodata:

00010508 <_IO_stdin_used>:
   10508:       01 00 02 00 48 65 6c 6c 6f 20 57 6f 72 6c 64 00     ....Hello World.
gaku@DESKTOP-IB1OCGR:~$

```

## リンク入門