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