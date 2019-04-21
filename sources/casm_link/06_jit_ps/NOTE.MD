# 超簡易PostScriptもどきをJITしよう

最後の課題として、VMのJITに相当する物を実装してみましょう。

## 実行方法
```
arm-linux-gnueabi-gcc parser.c eval.c ps_jit.c && qemu-arm -L /usr/arm-linux-gnueabi ./a.out
```

