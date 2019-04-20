# インラインアセンブリ入門

## 課題: sum_range_inline.cを完成させよ

sum_range_inline.cのsum_range_inlineの実装を埋めて、
テストをパスするようにしてください。 気まぐれでendの所も足すようにしたので注意してsum_rangeの方を見てみてください。

- 完了



# JIT入門

## 少し調べてみること
- JIT
- mmap
- 関数ポインタにキャスト

## 課題: sum_till_jit.cを完成させよ

### objdump結果
```
0001042c <sum_till>:
   1042c:       e24dd00c        sub     sp, sp, #12
   10430:       e58d0008        str     r0, [sp, #8]
   10434:       e3a00000        mov     r0, #0
   10438:       e58d0004        str     r0, [sp, #4]
   1043c:       e58d0000        str     r0, [sp]
   10440:       eaffffff        b       10444 <sum_till+0x18>
   10444:       e59d0000        ldr     r0, [sp]
   10448:       e59d1008        ldr     r1, [sp, #8]
   1044c:       e1500001        cmp     r0, r1
   10450:       aa000009        bge     1047c <sum_till+0x50>
   10454:       eaffffff        b       10458 <sum_till+0x2c>
   10458:       e59d0000        ldr     r0, [sp]
   1045c:       e59d1004        ldr     r1, [sp, #4]
   10460:       e0810000        add     r0, r1, r0
   10464:       e58d0004        str     r0, [sp, #4]
   10468:       eaffffff        b       1046c <sum_till+0x40>
   1046c:       e59d0000        ldr     r0, [sp]
   10470:       e2800001        add     r0, r0, #1
   10474:       e58d0000        str     r0, [sp]
   10478:       eafffff1        b       10444 <sum_till+0x18>
   1047c:       e59d0004        ldr     r0, [sp, #4]
   10480:       e28dd00c        add     sp, sp, #12
   10484:       e1a0f00e        mov     pc, lr 
```

.ksファイルのobjdump
```
   0:   e3a01000        mov     r1, #0
   4:   e3a02000        mov     r2, #0
   8:   e3500001        cmp     r0, #1
   c:   1a000002        bne     0x1c
  10:   e2822001        add     r2, r2, #1
  14:   e2811001        add     r1, r1, #1
  18:   eafffffa        b       0x8
  1c:   e1a00002        mov     r0, r2
```

- 完了。
第2回で作った簡易アセンブラを使って、bgeやbのオフセットを調べた。
.ksファイルとsum_till_jit.oをobjdumpして、課題に必要なバイナリを集めた。