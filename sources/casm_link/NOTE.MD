# 04 コンパイラの吐くアセンブリをいろいろ見てみる

## 結果を返す場合とメモリレイアウト 回答

Q. [sp]には何をstrしたのか？C言語の言葉で答えよ（beginとかresとかの変数名を答えて下さい）

A.origin_spから上に、begin,end,res,iの順番で積み上げているので,　i(i=begin)の値をstrした。


Q. [sp], [sp, #4], [sp, #8], [sp, #12]はC言語の変数名ではそれぞれ何か？

A. i, res, end, begin

# いろいろ見てみよう


## 引数がいっぱいある場合


#### clang 3.8
```clang 3.8
        .text
        .syntax unified
        .eabi_attribute 67, "2.09"      @ Tag_conformance
        .eabi_attribute 6, 1    @ Tag_CPU_arch
        .eabi_attribute 8, 1    @ Tag_ARM_ISA_use
        .eabi_attribute 17, 1   @ Tag_ABI_PCS_GOT_use
        .eabi_attribute 20, 1   @ Tag_ABI_FP_denormal
        .eabi_attribute 21, 1   @ Tag_ABI_FP_exceptions
        .eabi_attribute 23, 3   @ Tag_ABI_FP_number_model
        .eabi_attribute 34, 1   @ Tag_CPU_unaligned_access
        .eabi_attribute 24, 1   @ Tag_ABI_align_needed
        .eabi_attribute 25, 1   @ Tag_ABI_align_preserved
        .eabi_attribute 38, 1   @ Tag_ABI_FP_16bit_format
        .eabi_attribute 14, 0   @ Tag_ABI_PCS_R9_use
        .file   "many_args.bc"
        .globl  many_sum
        .align  2
        .type   many_sum,%function
many_sum:                               @ @many_sum
        .fnstart
@ BB#0:
        sub     sp, sp, #52
        str     r1, [sp, #44]
        str     r0, [sp, #48]
        str     r2, [sp, #40]
        ldr     r0, [sp, #52]
        str     r3, [sp, #36]
        ldr     r1, [sp, #44]
        ldr     r2, [sp, #48]
        str     r0, [sp, #32]
        ldr     r0, [sp, #56]
        add     r1, r2, r1
        ldr     r2, [sp, #40]
        str     r0, [sp, #28]
        ldr     r0, [sp, #60]
        add     r1, r1, r2
        ldr     r2, [sp, #36]
        str     r0, [sp, #24]
        ldr     r0, [sp, #64]
        add     r1, r1, r2
        ldr     r2, [sp, #32]
        str     r0, [sp, #20]
        ldr     r0, [sp, #68]
        add     r1, r1, r2
        ldr     r2, [sp, #28]
        str     r0, [sp, #16]
        ldr     r0, [sp, #72]
        add     r1, r1, r2
        ldr     r2, [sp, #24]
        str     r0, [sp, #12]
        ldr     r0, [sp, #76]
        add     r1, r1, r2
        ldr     r2, [sp, #20]
        str     r0, [sp, #8]
        ldr     r0, [sp, #80]
        add     r1, r1, r2
        ldr     r2, [sp, #16]
        str     r0, [sp, #4]
        ldr     r0, [sp, #84]
        add     r1, r1, r2
        ldr     r2, [sp, #12]
        str     r0, [sp]
        add     r1, r1, r2
        ldr     r2, [sp, #8]
        add     r1, r1, r2
        ldr     r2, [sp, #4]
        add     r1, r1, r2
        add     r0, r1, r0
        lsl     r0, r0, #1
        add     sp, sp, #52
        mov     pc, lr
.Lfunc_end0:
        .size   many_sum, .Lfunc_end0-many_sum
        .fnend

        .globl  main
        .align  2
        .type   main,%function
main:                                   @ @main
        .fnstart
@ BB#0:
        push    {r4, r5, r11, lr}
        add     r11, sp, #8
        sub     sp, sp, #44
        mov     r0, #0
        mov     r2, #4
        mov     r3, #5
        mov     r1, #7
        mov     r4, #8
        mov     r12, #2
        mov     lr, #3
        mov     r5, #9
        str     r0, [r11, #-12]
        mov     r0, #1
        str     r0, [sp, #32]
        mov     r0, #6
        str     r4, [sp, #4]
        str     r1, [sp, #8]
        mov     r1, #12
        str     r0, [sp, #12]
        str     r3, [sp, #16]
        str     r2, [sp, #20]
        mov     r0, #13
        mov     r2, #11
        mov     r3, #10
        str     lr, [sp, #24]
        str     r12, [sp, #28]
        str     r5, [sp]
        bl      many_sum
        mov     r1, r0
        ldr     r0, .LCPI1_0
        str     r1, [r11, #-16]
        bl      printf
        mov     r0, #0
        sub     sp, r11, #8
        pop     {r4, r5, r11, lr}
        mov     pc, lr
        .align  2
@ BB#1:
.LCPI1_0:
        .long   .L.str
.Lfunc_end1:
        .size   main, .Lfunc_end1-main
        .fnend

        .type   .L.str,%object          @ @.str
        .section        .rodata.str1.1,"aMS",%progbits,1
.L.str:
        .asciz  "result is %d\n"
        .size   .L.str, 14


        .ident  "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
        .section        ".note.GNU-stack","",%progbits

```

#### clang 6.0
``` 
        .text
        .syntax unified
        .eabi_attribute 67, "2.09"      @ Tag_conformance
        .eabi_attribute 6, 1    @ Tag_CPU_arch
        .eabi_attribute 8, 1    @ Tag_ARM_ISA_use
        .eabi_attribute 34, 1   @ Tag_CPU_unaligned_access
        .eabi_attribute 17, 1   @ Tag_ABI_PCS_GOT_use
        .eabi_attribute 20, 1   @ Tag_ABI_FP_denormal
        .eabi_attribute 21, 1   @ Tag_ABI_FP_exceptions
        .eabi_attribute 23, 3   @ Tag_ABI_FP_number_model
        .eabi_attribute 24, 1   @ Tag_ABI_align_needed
        .eabi_attribute 25, 1   @ Tag_ABI_align_preserved
        .eabi_attribute 38, 1   @ Tag_ABI_FP_16bit_format
        .eabi_attribute 18, 4   @ Tag_ABI_PCS_wchar_t
        .eabi_attribute 14, 0   @ Tag_ABI_PCS_R9_use
        .file   "many_args.c"
        .globl  many_sum                @ -- Begin function many_sum
        .p2align        2
        .type   many_sum,%function
        .code   32                      @ @many_sum
many_sum:
        .fnstart
@ %bb.0:
        sub     sp, sp, #16
        ldr     r12, [sp, #48]
        ldr     r12, [sp, #44]
        ldr     r12, [sp, #40]
        ldr     r12, [sp, #36]
        ldr     r12, [sp, #32]
        ldr     r12, [sp, #28]
        ldr     r12, [sp, #24]
        ldr     r12, [sp, #20]
        ldr     r12, [sp, #16]
        str     r0, [sp, #12]
        str     r1, [sp, #8]
        str     r2, [sp, #4]
        str     r3, [sp]
        ldr     r0, [sp, #12]
        ldr     r1, [sp, #8]
        add     r0, r0, r1
        ldr     r1, [sp, #4]
        add     r0, r0, r1
        ldr     r1, [sp]
        add     r0, r0, r1
        ldr     r1, [sp, #16]
        add     r0, r0, r1
        ldr     r1, [sp, #20]
        add     r0, r0, r1
        ldr     r1, [sp, #24]
        add     r0, r0, r1
        ldr     r1, [sp, #28]
        add     r0, r0, r1
        ldr     r1, [sp, #32]
        add     r0, r0, r1
        ldr     r1, [sp, #36]
        add     r0, r0, r1
        ldr     r1, [sp, #40]
        add     r0, r0, r1
        ldr     r1, [sp, #44]
        add     r0, r0, r1
        ldr     r1, [sp, #48]
        add     r0, r0, r1
        lsl     r0, r0, #1
        add     sp, sp, #16
        mov     pc, lr
.Lfunc_end0:
        .size   many_sum, .Lfunc_end0-many_sum
        .fnend
                                        @ -- End function
        .globl  main                    @ -- Begin function main
        .p2align        2
        .type   main,%function
        .code   32                      @ @main
main:
        .fnstart
@ %bb.0:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #48
        mov     r0, #0
        str     r0, [r11, #-4]
        mov     r0, #1
        str     r0, [sp, #32]
        mov     r0, #2
        str     r0, [sp, #28]
        mov     r0, #3
        str     r0, [sp, #24]
        mov     r0, #4
        str     r0, [sp, #20]
        mov     r0, #5
        str     r0, [sp, #16]
        mov     r0, #6
        str     r0, [sp, #12]
        mov     r0, #7
        str     r0, [sp, #8]
        mov     r0, #8
        str     r0, [sp, #4]
        mov     r0, #9
        str     r0, [sp]
        mov     r0, #13
        mov     r1, #12
        mov     r2, #11
        mov     r3, #10
        bl      many_sum
        str     r0, [r11, #-8]
        ldr     r1, [r11, #-8]
        ldr     r0, .LCPI1_0
        bl      printf
        mov     r0, #0
        mov     sp, r11
        pop     {r11, lr}
        mov     pc, lr
        .p2align        2
@ %bb.1:
.LCPI1_0:
        .long   .L.str
.Lfunc_end1:
        .size   main, .Lfunc_end1-main
        .fnend
                                        @ -- End function
        .type   .L.str,%object          @ @.str
        .section        .rodata.str1.1,"aMS",%progbits,1
.L.str:
        .asciz  "result is %d\n"
        .size   .L.str, 14


        .ident  "clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"
        .section        ".note.GNU-stack","",%progbits

```

#### ubuntuにclang-6.0をインストール
```
wget -O – https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add –
sudo apt-add-repository “deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-6.0 main”
sudo apt-get update
sudo apt-get install -y clang-6.0
```
をして
```
sudo apt install llvm
```
とする。
[How to install latest clang (6.0) on Ubuntu 16.04 (xenial) / WSL](https://blog.kowalczyk.info/article/k/how-to-install-latest-clang-6.0-on-ubuntu-16.04-xenial-wsl.html)



## doubleを渡すとどうなるか？

0.5をdouble_addへ渡す場合は、

```
	mov	r1, #266338304
	orr	r1, r1, #805306368
```
として、r1と即値でorr(論理和)をとる。

0.5は、EEE754倍精度で、
```
3FE00000
```
で表せられる。
r1には、3FE00000が入っている。

```
double res = double_add(1.2345, 3);
```
の場合は、

```
main_double_add:
        .fnstart
@ %bb.0:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        ldr     r0, .LCPI3_0
        ldr     r1, .LCPI3_1
        mov     r2, #3
        bl      double_add
        str     r1, [sp, #4]
        str     r0, [sp]
        ldr     r2, [sp]
        ldr     r3, [sp, #4]
        ldr     r0, .LCPI3_2
        bl      printf
        mov     sp, r11
        pop     {r11, lr}
        mov     pc, lr
        .p2align        2
@ %bb.1:
.LCPI3_0:
        .long   309237645               @ 0x126e978d
.LCPI3_1:
        .long   1072939139              @ 0x3ff3c083
.LCPI3_2:
        .long   .L.
```
r0, r1にラベルのアドレスを代入するようになった。

floatの場合は、intと同じように一個のレジスタに即値で渡す。



## 構造体の実体を渡すとどうなるか

構造体の要素の値をレジスタとスタックに積んで、blする。
ポインタの場合は、構造体のアドレスをr0に代入する。


## ポインタのプラプラについて



## sizeofがどうなるか



## 文字列の配列とポインタの違い



# スタックウォークしてみよう