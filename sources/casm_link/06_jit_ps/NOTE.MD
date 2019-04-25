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

## ハードコードした処理
```
    /* emit binary in hard code */

    // "5"　.sファイルを作ってバイナリを考える

//    binary_buf[0] = 0xe3a02005;　// mov     r2, #5
//    binary_buf[1] = 0xe52d2004;　// push    {r2}
//    binary_buf[2] = 0xe49d2004;  pop    {r0}
//    binary_buf[3] = 0xe1a00002;
//    binary_buf[4] = 0xe1a0f00e;

    // "5 4 add" .sファイルを作ってバイナリを考える

//    binary_buf[0] = 0xe3a02005;  // mov     r2, #5
//    binary_buf[1] = 0xe52d2004;  // push    {r2}
//    binary_buf[2] = 0xe3a02004;  // mov     r2, #4
//    binary_buf[3] = 0xe52d2004;  // push    {r2}
//    binary_buf[4] = 0xe8bd000c;  // pop     {r2, r3}
//    binary_buf[5] = 0xe0822003;  // add     r2, r2, r3
//    binary_buf[6] = 0xe52d2004;  // push    {r2}
//    binary_buf[7] = 0xe49d0004;  // pop    {r0}
//    binary_buf[8] = 0xe1a0f00e;  // mov     r15, r14

    // "5 4 sub" .sファイルを作ってバイナリを考える

//    binary_buf[0] = 0xe3a02005;  // mov     r2, #5
//    binary_buf[1] = 0xe52d2004;  // push    {r2}
//    binary_buf[2] = 0xe3a02004;  // mov     r2, #4
//    binary_buf[3] = 0xe52d2004;  // push    {r2}
//    binary_buf[4] = 0xe8bd000c;  // pop     {r2, r3}
//    binary_buf[5] = 0xe2422003;  // sub     r2, r2, r3
//    binary_buf[6] = 0xe52d2004;  // push    {r2}
//    binary_buf[7] = 0xe49d0004;  // pop    {r0}
//    binary_buf[8] = 0xe1a0f00e;  // mov     r15, r14

    // "5 4 mul" .sファイルを作ってバイナリを考える

//    binary_buf[0] = 0xe3a02005;  // mov     r2, #5
//    binary_buf[1] = 0xe52d2004;  // push    {r2}
//    binary_buf[2] = 0xe3a02004;  // mov     r2, #4
//    binary_buf[3] = 0xe52d2004;  // push    {r2}
//    binary_buf[4] = 0xe8bd000c;  // pop     {r2, r3}
//    binary_buf[5] = 0xe0020392;  // mul     r2, r2, r3
//    binary_buf[6] = 0xe52d2004;  // push    {r2}
//    binary_buf[7] = 0xe49d0004;  // pop    {r0}
//    binary_buf[8] = 0xe1a0f00e;  // mov     r15, r14　
```

### 実行方法
```
arm-linux-gnueabi-gcc parser.c ps_jit.c && qemu-arm -L /usr/arm-linux-gnueabi ./a.out
```


### disasmの出力

```
/* ユニットテスト */

// char *input = "5";
0x00010000  mov r2, #0x5
0x00010004  push {r2}
0x00010008  pop {r0}
0x0001000c  mov r15, r14

// char *input = "5 4 add";
0x00010000  mov r2, #0x5
0x00010004  push {r2}
0x00010008  mov r2, #0x4
0x0001000c  push {r2}
0x00010010  pop {r2, r3}
0x00010014  add r2, r2, r3
0x00010018  push {r2}
0x0001001c  pop {r0}
0x00010020  mov r15, r14

// char *input = "5 4 sub";
0x00010000  mov r2, #0x5
0x00010004  push {r2}
0x00010008  mov r2, #0x4
0x0001000c  push {r2}
0x00010010  pop {r2, r3}
0x00010014  sub r2, r3, r2
0x00010018  push {r2}
0x0001001c  pop {r0}
0x00010020  mov r15, r14

// char *input = "5 4 mul";
0x00010000  mov r2, #0x5
0x00010004  push {r2}
0x00010008  mov r2, #0x4
0x0001000c  push {r2}
0x00010010  pop {r2, r3}
0x00010014  mul r2, r3, r2
0x00010018  push {r2}
0x0001001c  pop {r0}
0x00010020  mov r15, r14

// char *input = "r1"; 
// funcvar(1, 5)

0x00010000  push {r1}
0x00010004  pop {r0}
0x00010008  mov r15, r14

// char *input = "r0"; 
// funcvar(1, 5)
0x00010000  push {r0}
0x00010004  pop {r0}
0x00010008  mov r15, r14

// char *input = "5 4 add 1 add";
0x00010000  mov r2, #0x5
0x00010004  push {r2}
0x00010008  mov r2, #0x4
0x0001000c  push {r2}
0x00010010  pop {r2, r3}
0x00010014  add r2, r2, r3
0x00010018  push {r2}
0x0001001c  mov r2, #0x1
0x00010020  push {r2}
0x00010024  pop {r2, r3}
0x00010028  add r2, r2, r3
0x0001002c  push {r2}
0x00010030  pop {r0}
0x00010034  mov r15, r14

// char *input = "3 7 add 1 sub 5 mul";
0x00010000  mov r2, #0x3
0x00010004  push {r2}
0x00010008  mov r2, #0x7
0x0001000c  push {r2}
0x00010010  pop {r2, r3}
0x00010014  add r2, r2, r3
0x00010018  push {r2}
0x0001001c  mov r2, #0x1
0x00010020  push {r2}
0x00010024  pop {r2, r3}
0x00010028  sub r2, r3, r2
0x0001002c  push {r2}
0x00010030  mov r2, #0x5
0x00010034  push {r2}
0x00010038  pop {r2, r3}
0x0001003c  mul r2, r3, r2
0x00010040  push {r2}
0x00010044  pop {r0}
0x00010048  mov r15, r14


all test done

// funcvar = (int(*)(int, int))jit_script("3 7 add r1 sub 4 mul");

0x00010000  mov r2, #0x3
0x00010004  push {r2}
0x00010008  mov r2, #0x7
0x0001000c  push {r2}
0x00010010  pop {r2, r3}
0x00010014  add r2, r2, r3
0x00010018  push {r2}
0x0001001c  push {r1}
0x00010020  pop {r2, r3}
0x00010024  sub r2, r3, r2
0x00010028  push {r2}
0x0001002c  mov r2, #0x4
0x00010030  push {r2}
0x00010034  pop {r2, r3}
0x00010038  mul r2, r3, r2
0x0001003c  push {r2}
0x00010040  pop {r0}
0x00010044  mov r15, r14


```