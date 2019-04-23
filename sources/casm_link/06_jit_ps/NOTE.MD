# 超簡易PostScriptもどきをJITしよう

最後の課題として、VMのJITに相当する物を実装してみましょう。

## 実行方法
```
arm-linux-gnueabi-gcc parser.c eval.c ps_jit.c && qemu-arm -L /usr/arm-linux-gnueabi ./a.out
```

## 簡単な例から実装していく

### "5"だけ入力

```
    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script("5");

    int actual = funcvar(0, 0); // 引数は使われない
```

.sファイルを作りobdumpした。
```
Disassembly of section .data:

00000000 <.data>:
   0:   e3a02005        mov     r2, #5
   4:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   8:   e49d2004        pop     {r2}            ; (ldr r2, [sp], #4)
   c:   e1a00002        mov     r0, r2
  10:   e1a0f00e        mov     pc, lr
```


### "5 4 add"

```
    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script("5 4 add");

    int actual = funcvar(0, 0); // 引数は使われない

```

#### 疑似コード

```
push(5);
push(4);
int a = pop();
int b = pop();
push(a+b);
int res = pop();
return res;
```

```
mov r2, #5
push {r2}
mov r3, #4
push {r3}

pop {r2, r3}
add r2, r2, r3

push {r2}

mov r0, r2
mov r15, r14 

```

#### objdump
```
00000000 <.data>:
   0:   e3a02005        mov     r2, #5
   4:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   8:   e3a03004        mov     r3, #4
   c:   e52d3004        push    {r3}            ; (str r3, [sp, #-4]!)
  10:   e8bd000c        pop     {r2, r3}
  14:   e0822003        add     r2, r2, r3
  18:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  1c:   e1a00002        mov     r0, r2
  20:   e1a0f00e        mov     pc, lr
```

もっといいやり方がある？

```
mov r2, #5
push {r2}
mov r2, #4
push {r2}

pop {r2, r3}
add r2, r2, r3

push {r2}

mov r0, r2
mov r15, r14 
```

#### objdump結果

```
   0:   e3a02005        mov     r2, #5
   4:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   8:   e3a02004        mov     r2, #4
   c:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  10:   e8bd000c        pop     {r2, r3}
  14:   e0822003        add     r2, r2, r3
  18:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  1c:   e1a00002        mov     r0, r2
  20:   e1a0f00e        mov     pc, lr
```

テストが通った。がpopするレジスタはr0でいいか？

```
   0:   e3a02005        mov     r2, #5
   4:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   8:   e3a02004        mov     r2, #4
   c:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  10:   e8bd000c        pop     {r2, r3}
  14:   e0822003        add     r2, r2, r3
  18:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  1c:   e49d0004        pop     {r0}            ; (ldr r0, [sp], #4)
  20:   e1a0f00e        mov     pc, lr
```

これでいく。

### "5 4 sub"

####objdump
```
   0:   e3a02005        mov     r2, #5
   4:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   8:   e3a02004        mov     r2, #4
   c:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  10:   e8bd000c        pop     {r2, r3}
  14:   e2422003        sub     r2, r2, r3
  18:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  1c:   e49d0004        pop     {r0}            ; (ldr r0, [sp], #4)
  20:   e1a0f00e        mov     pc, lr
```

### "5 4 mul"
```
   0:   e3a02005        mov     r2, #5
   4:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
   8:   e3a02004        mov     r2, #4
   c:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  10:   e8bd000c        pop     {r2, r3}
  14:   e0020392        mul     r2, r2, r3
  18:   e52d2004        push    {r2}            ; (str r2, [sp, #-4]!)
  1c:   e49d0004        pop     {r0}            ; (ldr r0, [sp], #4)
  20:   e1a0f00e        mov     pc, lr
```

### "5 5 div"
```

```

・・・除算はあとで考えよう

### レジスタ

```
   0:   e52d0004        push    {r0}            ; (str r0, [sp, #-4]!)
   4:   e52d1004        push    {r1}            ; (str r1, [sp, #-4]!)
```
