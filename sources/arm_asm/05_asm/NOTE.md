# 05簡易アンブラを作ろう

リロケーション無しで簡単なアセンブリを作る。
0x00010000決め打ち。
文字列埋め込みは最後限定で。 ラベルはサポートする。

ldrで=で大きな数字を読み込んだりはサポートしないが、
ラベルで同じ事が出来るようにはする。

## 一番単純なmovをサポートする

mov r1, r2
をアセンブルする。

### mov r1, r2のobjdump結果
```
0:   e1a01002        mov     r1, r2
```
int expect = 0xE1A01002;


### 1byte
1byte = 8bit

0001 1010 == 1A <- 1byte

2 桁で 1 Byte

### 1word

今回は，32bitで1word


### Unit Test用
int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }


### whileと　do while

カウンタの進み方が違う．
今回の実装では，whileのほうが素直にカウンタを更新できるような気がする．


### .sは16進数を使う

.s
```
mov r1, #0x6c
``` 

即値は16進数を使っている。

.bin
```
e3a0106c 
```
.binは16進数で即値を表している

## .raw命令

.raw 0xFFFFFFFF
の時に、
hex_numは”-1”と解釈されてしまう。  
~~そのため、
PARSE_FAILを"-2"に変更~~
```
int single_atoi_hex(char *str, int *out_num) ;
```
として、パースが成功したら、1を返すようにした。


## ldr, str 

objdump結果例

ldr  
```
0 :   e59f0030        ldr     r0, [pc, #48]   ; 0x38
0 :   e59f002c        ldr     r0, [pc, #44]   ; 0x34
4 :   e59f102c        ldr     r1, [pc, #44]   ; 0x38  
24:   e59f1064        ldr     r1, [pc, #100]  ; 0x90
4 :   e51f0030        ldr     r0, [pc, #-48]
4 :   e59f0000        ldr     r0, [pc]        ; 0xc
```
簡易逆アセンブラでは、"pc"は"r15"として表示する。


ldr     r0, [pc, #48]  
では、オフセットは、48  
16進数表示で、 0x30  
ので  
ldr     r0, [pc, #0x30]
とする。


str  
```
8 :   e5801000        str     r1, [r0]
10:   e5801000        str     r1, [r0]
30:   e5802000        str     r2, [r0]     
   
```

## cl_ulits.cの"cl"は何の略か。

c-lessonの略。  


## regression test

### 実装する前のメモ
Pythonを使ってバイナリを比較する。  
Python実行は、Powershellを使う。  


```
main.cのコンパイルは、Pythonの実行はPowershellを使う。
バイナリ比較だけをPyhtonで。
```
いや、さすがにこれならば、全部Powershellでやったほうがよい。


Pythonからコマンドを呼び出すようにする。



```
ex. > python .\hello.py
```
のように実行する。  

必要そうなこと  
- ファイルの読み込み方
- バイナリの読み込み方
- ディレクトリからファイルの見つけ方

### 実装した実行方法

powershellで
```
py -3 .\regression_test.py
```
と打つ。


### なぜ、clionで実行したときと、PSのgccで実行したときに、emmit.arrayの大きさが違ったか？

cmdのgccで実行すると、サイズが2倍になった。

最初は、  
fwrite(emitter->array, sizeof(emitter.array), emitter->pos, fp);
としていた。  
Clionで実行する分では問題がなかった。  
しかし、cmdでgccを実行すると、バイナリの最後の方に、00000000が追加されていた。  

sizeof(emitter.array)
をすると、  
Clionで実行では、大きさは、"4"だった。  
cmdで実行すると、大きさは、"8"となった。  

~~アライメント？が関係しているか？~~

そのため、  

fwrite(emitter->array, sizeof(unsigned int), emitter->pos, fp);

に変更した。  

~~intが4byteか8byteか処理系で違うということ。~~　　


#### uint32_tを使う

固定幅の型であるuint32_tを使う

[Fixed width integer types ](https://en.cppreference.com/w/c/types/integer)



###　現状のコード

emitter部分寄せ集め


#### ヘッダー
```
/*結果をいれる配列*/
struct Emitter {
    unsigned int *array;
    int pos;
};

void initialize_result_arr(struct Emitter *emitter);
void emit_word(struct Emitter *emitter, unsigned int oneword);
```

#### cl_utils.c
```
/* アセンブラ結果の出力関係*/
static uint32_t g_asm_result[1000];

void initialize_result_arr(struct Emitter *emitter){
    emitter->array = g_asm_result;
    emitter->pos = 0;
}

void emit_word(struct Emitter* emitter, uint32_t oneword){
    emitter->array[emitter->pos] = oneword;
    emitter->pos++;
}

```
#### main.c

```
void write_binary_file(struct Emitter *emitter){
    FILE *fp;

    fp = fopen("test.bin", "wb");

    fwrite(emitter->array, sizeof(uint32_t), emitter->pos, fp);

    fclose(fp);
}

```

現状はこうなっています。  

unsigned int -> uint32_t
に変更ではまずいか？

変更して、emitter.arrayの大きさを調べる。

test.ks
```
ldr r1, [r15, #0x04]
```


##### CLionの場合
```
CMAKE_C_STANDARD 99　がコンパイラのバージョン？


printf("%d", sizeof(emitter->array));

出力 > 4


printf("%d", sizeof(emitter->array[0]));

出力 > 4


バイナリエディタ
04 10 9F E5
```


##### コマンドプロンプトの場合


C:\Users\devel>gcc --version
realgcc.exe (Rev3, Built by MSYS2 project) 5.2.0
がバージョン？
```

printf("%d", sizeof(emitter->array));

出力 > 8

printf("%d", sizeof(emitter->array[0]));

出力 > 4

バイナリ
04 10 9F E5

```

バイナリエディタで見た分には、バイナリは同じ。
しかし、emitter.arrayの大きさが違う・・・
emitter.array[0]の大きさは同じ・・・
-> arrayの一要素の大きさは同じ？ （編集済み） 

int: サイズ、32bit（とも限らない）
uint32_t: unsigned long intのエイリアス、サイズ: 32bit

### ポインタの大きさ
CLion
```
uint32_t data;

printf("%d", sizeof(data));
出力 > 4

uint32_t *data;

printf("%d", sizeof(data));
出力 > 4

printf("%p", sizeof(data));
出力 > 00000004


```

cmd
```
uint32_t data;

printf("%d", sizeof(data));
出力 > 4

uint32_t *data;

printf("%d", sizeof(data));
出力 > 8

printf("%p", sizeof(data));
出力 > 0000000000000008

```

ポインタにすると大きさが違う。
Clionが32bitで、cmdが64bitだからか。




emitter.arrayは、  
```
struct Emitter {
    uint32_t *array;
    int pos;
};
```
より、  
uint32_t型のポインタ

32bitか64bitで大きさが変わる。

uint32_t自体の大きさは、
32bit、64bitで同じ？


### fwrite
```
size_t fwrite(const void *buf, size_t size, size_t n, FILE *fp); 
```
【引数】
const void *buf　：　書き込みデータのポインタ
size_t size　：　書き込みデータのバイト長さ
size_t n　：　書き込みデータの数
FILE *fp　：　FILEポインタ

より、上手くいかなかったコードでは、  
書き込みデータのバイトの長さに、  
sizeof(emitter.array)  
を渡していた。  
そのため、clion(32bit)とcmd(64bit)で実行したときに、違うバイナリになった。

cmdでは、書き込みデータのバイトの長さが2倍になっていたため、  
バイナリの最後のほうに、00 00 00 00 が書き込まれていた。


### 確認

fwrite(emitter->array, sizeof(emitter->array), emitter->pos, fp);
として実行。

バイナリが変わった。


### 結論

ポインタ(int型)の大きさが違う。
Clionが32bitで、cmdが64bitだから。



## ファイル分割

main.cが大きくなったため、ファイルを分割する。  
まずは、パースする関数は、parse.cにする。



## binary tree

アセンブリだとシンボルが良く出てくるので、シンボルを実装する事にします。　　
ニモニックのツリーとラベルのツリーは別にして、ニモニックは1から、ラベルは10000から　　

leftはstrcmpしてマイナスの時に行く、+だったらrightに行く。　　


追加する時にはnameはmallocしてstrcpyなりmemcpy  

探すだけの関数と探してなかったらその時ついでに追加する関数の2つ  



参考  
[strcmp](http://www9.plala.or.jp/sgwr-t/lib/strcmp.html)

### sizeofについて

sizeofは静的に決まるサイズしか返さない。
mallocしたものをくわせても、そのサイズは教えてくれず、そのポインタを入れるのに必要なサイズしか教えてくれない。

アスタリスクをつけたら先頭の要素のサイズしか教えてくれない。
むしろ配列だけが特別で、それ以外は全部型のサイズ。
sizeof(node->name)は、sizeof(char*)
とまったく同じ。  

"char *"はポインタであるため、サイズは4になる。　　


入力は、char *name
```
printf("before node.name %d\n", sizeof(node->name)); //4
printf("before mem %p\n", &node->name); // 00721578
node->name = malloc(sizeof(*name));
printf("after node.name %d\n", sizeof(node->name)); // 4
printf("after mem  %p\n", &node->name); // 00721578

printf("char name %d\n", sizeof(*name)); // 1
printf("mov %d\n", sizeof("mov")); // 4 ('\0'を含むため)

printf("before value mem %p\n", &node->value); // 00AA157C
node->value = (int)malloc(sizeof(int));
printf("after value mem %p\n", node->value); // 00AA157C
```


## ラベルのサポート

```
b end
end:
  b end    
```
のobjdumpは、  

```
Disassembly of section .data:

00000000 <.data>:
   0:   eaffffff        b       0x4
   4:   eafffffe        b       0x4
```


### 方針メモ

bのemmiter.array[i]の位置を覚えておく  
emmiter.array[i] には、oneword = 0を入れておく。  

線形リストには、emmitter.array[i]のi、bの命令があるメモリアドレス、  
ラベルのシンボルを覚えておく。

ラベルのシンボルがわかれば、そのラベルのメモリの位置が分かる。

```
char *input1 = "b loop";
char *input2 = "mov r1, r2";
char *input3 = "mov r1, r2";
char *input4 = "loop:";

```


#### objdump結果

```
00000000 <.data>:
   0:   ea000001        b       0xc
   4:   e1a01002        mov     r1, r2
   8:   e1a01002        mov     r1, r2
```


## hello_asm.ksの実行

### objdump結果
```
Disassembly of section .data:

00000000 <.data>:
   0:   e59f0030        ldr     r0, [pc, #48]   ; 0x38
   4:   e3a01068        mov     r1, #104        ; 0x68
   8:   e5801000        str     r1, [r0]
   c:   e3a01065        mov     r1, #101        ; 0x65
  10:   e5801000        str     r1, [r0]
  14:   e3a0106c        mov     r1, #108        ; 0x6c
  18:   e5801000        str     r1, [r0]
  1c:   e3a0106f        mov     r1, #111        ; 0x6f
  20:   e5801000        str     r1, [r0]
  24:   e3a0200d        mov     r2, #13
  28:   e5802000        str     r2, [r0]
  2c:   e3a0200a        mov     r2, #10
  30:   e5802000        str     r2, [r0]
  34:   eafffffe        b       0x34
```

### hello_asm.sのobjdump結果
```
   0:   e59f0030        ldr     r0, [pc, #48]   ; 0x38
   4:   e3a01068        mov     r1, #104        ; 0x68
   8:   e5801000        str     r1, [r0]
   c:   e3a01065        mov     r1, #101        ; 0x65
  10:   e5801000        str     r1, [r0]
  14:   e3a0106c        mov     r1, #108        ; 0x6c
  18:   e5801000        str     r1, [r0]
  1c:   e3a0106f        mov     r1, #111        ; 0x6f
  20:   e5801000        str     r1, [r0]
  24:   e3a0200d        mov     r2, #13
  28:   e5802000        str     r2, [r0]
  2c:   e3a0200a        mov     r2, #10
  30:   e5802000        str     r2, [r0]
  34:   eafffffe        b       0x34
```

## 文字列リテラルとstate machine







## hello_loop.ksをアセンブル出来るようにする


### 入力
```
b loop
loop: .asciz "hello"   
```

### objdump結果
```
    0:   eaffffff        b       0x4
    4:   6c6c6568        cfstr64vs       mvdx6, [ip], #-416      ; 0xfffffe60
    8:   0000006f        andeq   r0, r0, pc, rrx
```


### 入力
.asciz "test"  

### objdump結果
```
00000000 <.data>:
   0:   eaffffff        b       0x4
   4:   74736574        ldrbtvc r6, [r3], #-1396        ; 0xfffffa8c
   8:   00000000        andeq   r0, r0, r0
```

### 入力
"Hello World\n"

### objdump結果
```
   0:   eaffffff        b       0x4
   4:   6c6c6548        cfstr64vs       mvdx6, [ip], #-288      ; 0xfffffee0
   8:   6f57206f        svcvs   0x0057206f
   c:   0a646c72        beq     0x191b1dc
  10:   00000000        andeq   r0, r0, r0
```


### 入力
"escape1 \" end"

### objdump結果
```
   0:   eaffffff        b       0x4
   4:   61637365        cmnvs   r3, r5, ror #6
   8:   20316570        eorscs  r6, r1, r0, ror r5
   c:   6e652022        cdpvs   0, 6, cr2, cr5, cr2, {1}
  10:   00000064        andeq   r0, r0, r4, rrx
```


### 入力
"escape2 \\ end"

### objdump結果
``` 
   0:   eaffffff        b       0x4
   4:   61637365        cmnvs   r3, r5, ror #6
   8:   20326570        eorscs  r6, r2, r0, ror r5
   c:   6e65205c        mcrvs   0, 3, r2, cr5, cr12, {2}
  10:   00000064        andeq   r0, r0, r4, rrx
```
### 入力
"escape3 \\"

### objdump結果
```
   0:   eaffffff        b       0x4
   4:   61637365        cmnvs   r3, r5, ror #6
   8:   20336570        eorscs  r6, r3, r0, ror r5
   c:   0000005c        andeq   r0, r0, ip, asr r0
```

### 入力
"escape4 \\\" end"

### objdump結果
```
   0:   eaffffff        b       0x4
   4:   61637365        cmnvs   r3, r5, ror #6
   8:   20346570        eorscs  r6, r4, r0, ror r5
   c:   6520225c        strvs   r2, [r0, #-604]!        ; 0xfffffda4
  10:   0000646e        andeq   r6, r0, lr, ror #8
```


## ldrのラベルをサポート

### 入力
```
ldr r1, =msg
msg:
    .asciz  "hello"
```                    

### objdump結果
```
   0:   e59f1004        ldr     r1, [pc, #4]    ; 0xc
   4:   6c6c6568        cfstr64vs       mvdx6, [ip], #-416      ; 0xfffffe60
   8:   0000006f        andeq   r0, r0, pc, rrx
   c:   00010004        andeq   r0, r1, r4
```


## print_loopの実装


###入力

```
    ldr r0,=0x101f1000
    ldr r1,=message
    ldrb r3,[r1]    
loop:
    str r3,[r0]
    add r1, r1, #1
    ldrb r3,[r1]
    cmp r3,#0
    bne loop
end:
    b end

message:
    .asciz  "hello, world\n"

```

###objdumpファイル
```
   0:   e59f002c        ldr     r0, [pc, #44]   ; 0x34
   4:   e59f102c        ldr     r1, [pc, #44]   ; 0x38
   8:   e5d13000        ldrb    r3, [r1]
   c:   e5803000        str     r3, [r0]
  10:   e2811001        add     r1, r1, #1
  14:   e5d13000        ldrb    r3, [r1]
  18:   e3530000        cmp     r3, #0
  1c:   1afffffa        bne     0xc
  20:   eafffffe        b       0x20
  24:   6c6c6568        cfstr64vs       mvdx6, [ip], #-416      ; 0xfffffe60
  28:   77202c6f        strvc   r2, [r0, -pc, ror #24]!
  2c:   646c726f        strbtvs r7, [ip], #-623 ; 0xfffffd91
  30:   0000000a        andeq   r0, r0, sl
  34:   101f1000        andsne  r1, pc, r0
  38:   00010024        andeq   r0, r1, r4, lsr #32
```

### .ks入力
```
ldr r0,=0x101f1000
ldr r1,=message
ldrb r3,[r1]
loop:
    str r3,[r0]
    add r1, r1, #0x1
    ldrb r3,[r1]
    cmp r3,#0x0
    bne loop
end:
    b end
message:
    .raw  "hello, world\n"
```

###objdumpファイル
```
   0:   e59f002c        ldr     r0, [pc, #44]   ; 0x34
   4:   e59f102c        ldr     r1, [pc, #44]   ; 0x38
   8:   e5d13000        ldrb    r3, [r1]
   c:   e5803000        str     r3, [r0]
  10:   e2811001        add     r1, r1, #1
  14:   e5d13000        ldrb    r3, [r1]
  18:   e3530000        cmp     r3, #0
  1c:   1afffffa        bne     0xc
  20:   eafffffe        b       0x20
  24:   6c6c6568        cfstr64vs       mvdx6, [ip], #-416      ; 0xfffffe60
  28:   77202c6f        strvc   r2, [r0, -pc, ror #24]!
  2c:   646c726f        strbtvs r7, [ip], #-623 ; 0xfffffd91
  30:   0000000a        andeq   r0, r0, sl
  34:   101f1000        andsne  r1, pc, r0
  38:   00010024        andeq   r0, r1, r4, lsr #32
```



### bne

### 入力
```
loop:
bne loop
```
### objdumpファイル
```
   0:   1afffffe        bne     0x0
```




## 改行文字が命令の間にはいっている場合を修正
```
ex
mov r1, r2

mov r3, r4
```
上記の例では、改行文字が２つになっている？


## 残りの命令の実装


### putchar_bad.s入力
```
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end

/*
  putchar:
    arg r0: target character which is printed.
    used internal register: r1.
*/
putchar:
  ldr r1,=0x101f1000
  str r0, [r1]
  mov r15, r14


/*
  print:
    arg r0: Address of target string, must end by \0.
    used internal register: r0, r1, r3.
*/

print:
  ldrb r3,[r0]
  mov r2, r0
  mov r4, r14

_loop:
  /*
  str r3, [r1]
  */
  mov r0, r3
  bl putchar

  add r2, r2, #1
  ldrb r3,[r2]
  cmp r3,#0
  bne _loop
  mov r15, r4

msg1: .asciz "First text.\n"
msg2: .asciz "Second text!\n"

```

### objdump結果

```
00000000 <.data>:
   0:   e59f005c        ldr     r0, [pc, #92]   ; 0x64
   4:   eb000005        bl      0x20
   8:   e59f0058        ldr     r0, [pc, #88]   ; 0x68
   c:   eb000003        bl      0x20
  10:   eafffffe        b       0x10
  14:   e59f1050        ldr     r1, [pc, #80]   ; 0x6c
  18:   e5810000        str     r0, [r1]
  1c:   e1a0f00e        mov     pc, lr
  20:   e5d03000        ldrb    r3, [r0]
  24:   e1a02000        mov     r2, r0
  28:   e1a0400e        mov     r4, lr
  2c:   e1a00003        mov     r0, r3
  30:   ebfffff7        bl      0x14
  34:   e2822001        add     r2, r2, #1
  38:   e5d23000        ldrb    r3, [r2]
  3c:   e3530000        cmp     r3, #0
  40:   1afffff9        bne     0x2c
  44:   e1a0f004        mov     pc, r4
  48:   73726946        cmnvc   r2, #1146880    ; 0x118000
  4c:   65742074        ldrbvs  r2, [r4, #-116]!        ; 0xffffff8c
  50:   0a2e7478        beq     0xb9d238
  54:   63655300        cmnvs   r5, #0, 6
  58:   20646e6f        rsbcs   r6, r4, pc, ror #28
  5c:   74786574        ldrbtvc r6, [r8], #-1396        ; 0xfffffa8c
  60:   00000a21        andeq   r0, r0, r1, lsr #20
  64:   00010048        andeq   r0, r1, r8, asr #32
  68:   00010055        andeq   r0, r1, r5, asr r0
  6c:   101f1000        andsne  r1, pc, r0

```




### putchar_bad.ks 入力

```
putchar:
  ldr r1,=0x101f1000
  str r0, [r1]
  mov r15, r14
print:
  ldrb r3,[r0]
  mov r2, r0
  mov r4, r14
_loop:
  mov r0, r3
  bl putchar
  add r2, r2, #0x1
  ldrb r3,[r2]
  cmp r3,#0x0
  bne _loop
  mov r15, r4
msg1:
.raw "First text.\n"
msg2:
.raw "Second text!\n"

```

####objdump結果

```
00000000 <.data>:
   0:   e59f0060        ldr     r0, [pc, #96]   ; 0x68
   4:   eb000005        bl      0x20
   8:   e59f005c        ldr     r0, [pc, #92]   ; 0x6c
   c:   eb000003        bl      0x20
  10:   eafffffe        b       0x10
  14:   e59f1054        ldr     r1, [pc, #84]   ; 0x70
  18:   e5810000        str     r0, [r1]
  1c:   e1a0f00e        mov     pc, lr
  20:   e5d03000        ldrb    r3, [r0]
  24:   e1a02000        mov     r2, r0
  28:   e1a0400e        mov     r4, lr
  2c:   e1a00003        mov     r0, r3
  30:   ebfffff7        bl      0x14
  34:   e2822001        add     r2, r2, #1
  38:   e5d23000        ldrb    r3, [r2]
  3c:   e3530000        cmp     r3, #0
  40:   1afffff9        bne     0x2c
  44:   e1a0f004        mov     pc, r4
  48:   73726946        cmnvc   r2, #1146880    ; 0x118000
  4c:   65742074        ldrbvs  r2, [r4, #-116]!        ; 0xffffff8c
  50:   0a2e7478        beq     0xb9d238
  54:   00000000        andeq   r0, r0, r0
  58:   6f636553        svcvs   0x00636553
  5c:   7420646e        strtvc  r6, [r0], #-1134        ; 0xfffffb92
  60:   21747865        cmncs   r4, r5, ror #16
  64:   0000000a        andeq   r0, r0, sl
  68:   00010048        andeq   r0, r1, r8, asr #32
  6c:   00010058        andeq   r0, r1, r8, asr r0
  70:   101f1000        andsne  r1, pc, r0
```


### 今回の実装のnull文字の扱い

今回の実装では、文字列のバイナリが8byteの間で収まった場合、
上の54のように、4byteでnull文字を表す。



## 残りの命令

### print_nomem.ks 入力
```
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
print:
  ldrb r3,[r0]
  ldr r1,=0x101f1000
loop:
  str r3,[r1]
  add r0, r0, #0x1
  ldrb r3,[r0]
  cmp r3,#0x0
  bne loop
  mov r15, r14
msg1:
.raw "First text.\n"
msg2:
.raw "Second text!\n"
```

### objdump 結果

```
   0:   e59f004c        ldr     r0, [pc, #76]   ; 0x54
   4:   eb000002        bl      0x14
   8:   e59f0048        ldr     r0, [pc, #72]   ; 0x58
   c:   eb000000        bl      0x14
  10:   eafffffe        b       0x10
  14:   e5d03000        ldrb    r3, [r0]
  18:   e59f103c        ldr     r1, [pc, #60]   ; 0x5c
  1c:   e5813000        str     r3, [r1]
  20:   e2800001        add     r0, r0, #1
  24:   e5d03000        ldrb    r3, [r0]
  28:   e3530000        cmp     r3, #0
  2c:   1afffffa        bne     0x1c
  30:   e1a0f00e        mov     pc, lr
  34:   73726946        cmnvc   r2, #1146880    ; 0x118000
  38:   65742074        ldrbvs  r2, [r4, #-116]!        ; 0xffffff8c
  3c:   0a2e7478        beq     0xb9d224
  40:   00000000        andeq   r0, r0, r0
  44:   6f636553        svcvs   0x00636553
  48:   7420646e        strtvc  r6, [r0], #-1134        ; 0xfffffb92
  4c:   21747865        cmncs   r4, r5, ror #16
  50:   0000000a        andeq   r0, r0, sl
  54:   00010034        andeq   r0, r1, r4, lsr r0
  58:   00010044        andeq   r0, r1, r4, asr #32
  5c:   101f1000        andsne  r1, pc, r0
```

### print_hex_bl.ks 入力
```
    ldr r1,=0x101f1000
    mov r0, r15
    bl print_hex
    mov r0, #0x68
    bl print_hex
    b end
print_hex:
    mov r3, #0x28
    mov r2, #0x30
    str r2, [r1]
    mov r2, #0x78
    str r2, [r1]
_loop:
    lsr r2, r0, r3
    and r2, r2, #0x0f
    cmp r2, #0x0a
    blt _under_ten
    add r2, r2, #0x07
_under_ten:
    add r2, r2, #0x30
    str r2, [r1]
    sub r3, r3, #0x4
    cmp r3, #0x0
    bge _loop
    mov r2, #0x0a
    str r2, [r1]
    mov r15, r14
end:
    b end
```

#### 実装必要な命令

lsr 実装した。
and 実装した。
sub 実装した。

bge 実装した。
blt 実装した。


### objdump 結果

```
   0:   e59f105c        ldr     r1, [pc, #92]   ; 0x64
   4:   e1a0000f        mov     r0, pc
   8:   eb000002        bl      0x18
   c:   e3a00068        mov     r0, #104        ; 0x68
  10:   eb000000        bl      0x18
  14:   ea000011        b       0x60
  18:   e3a03028        mov     r3, #40 ; 0x28
  1c:   e3a02030        mov     r2, #48 ; 0x30
  20:   e5812000        str     r2, [r1]
  24:   e3a02078        mov     r2, #120        ; 0x78
  28:   e5812000        str     r2, [r1]
  2c:   e1a02330        lsr     r2, r0, r3
  30:   e202200f        and     r2, r2, #15
  34:   e352000a        cmp     r2, #10
  38:   ba000000        blt     0x40
  3c:   e2822007        add     r2, r2, #7
  40:   e2822030        add     r2, r2, #48     ; 0x30
  44:   e5812000        str     r2, [r1]
  48:   e2433004        sub     r3, r3, #4
  4c:   e3530000        cmp     r3, #0
  50:   aafffff5        bge     0x2c
  54:   e3a0200a        mov     r2, #10
  58:   e5812000        str     r2, [r1]
  5c:   e1a0f00e        mov     pc, lr
  60:   eafffffe        b       0x60
  64:   101f1000        andsne  r1, pc, r0
```

### print_hex.ks 入力

```
    ldr r0,=0x101f1000
    ldr r1,=0xdeadbeaf
    b print_hex
print_hex:
    mov r3, #0x1c
    mov r2, #0x30
    str r2, [r0]
    mov r2, #0x78
    str r2, [r0]
_loop:
    lsr r2, r1, r3
    and r2, r2, #0x0f
    cmp r2, #0x0a
    blt _under_ten
    add r2, r2, #0x07
_under_ten:
    add r2, r2, #0x30
    str r2, [r0]
    sub r3, r3, #0x4
    cmp r3, #0x0
    bge _loop
    b busy
busy:
    b busy

```


### objdump結果
```
00000000 <.data>:
   0:   e59f0048        ldr     r0, [pc, #72]   ; 0x50
   4:   e59f1048        ldr     r1, [pc, #72]   ; 0x54
   8:   eaffffff        b       0xc
   c:   e3a0301c        mov     r3, #28
  10:   e3a02030        mov     r2, #48 ; 0x30
  14:   e5802000        str     r2, [r0]
  18:   e3a02078        mov     r2, #120        ; 0x78
  1c:   e5802000        str     r2, [r0]
  20:   e1a02331        lsr     r2, r1, r3
  24:   e202200f        and     r2, r2, #15
  28:   e352000a        cmp     r2, #10
  2c:   ba000000        blt     0x34
  30:   e2822007        add     r2, r2, #7
  34:   e2822030        add     r2, r2, #48     ; 0x30
  38:   e5802000        str     r2, [r0]
  3c:   e2433004        sub     r3, r3, #4
  40:   e3530000        cmp     r3, #0
  44:   aafffff5        bge     0x20
  48:   eaffffff        b       0x4c
  4c:   eafffffe        b       0x4c
  50:   101f1000        andsne  r1, pc, r0
  54:   deadbeaf        cdple   14, 10, cr11, cr13, cr15, {5}
```


### putchar_memの実装

残りの命令

stmdb
ldmia


#### 入力
```
  ldr r13,=0x08000000
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
putchar:
  stmdb r13!, {r1, r14}
  ldr r1,=0x101f1000
  str r0, [r1]
  ldmia r13!, {r1, r14}
  mov r15, r14
print:
  stmdb r13!, {r1, r2, r14}
  mov r2, r0
  ldrb r1,[r2]
_loop:
  mov r0, r1
  bl putchar
  add r2, r2, #0x1
  ldrb r1,[r2]
  cmp r1,#0x0
  bne _loop
  ldmia r13!, {r1, r2, r14}
  mov r15, r14
msg1: 
  .raw "First text.\n"
msg2:
  .raw "Second text!\n"
```



### stmdb r13!, {r0}


#### 最初の方針
r0をスタックにつむ。  

stmdbは二モニック  
r13はレジスタ。  
!はビックリマーク  
,をパース  
  
{をパース  
レジスタを}までパース？  

とりあえずスタックに一個のレジスタ積む場合を実装してみる。  

```
int asm_stmdb_op(char *str, int start, struct Emitter *emitter);  
```

### 仕様を調べる

#### 入力

stmdb r13!, {r0}

#### objdump結果
```
   0:   e92d0001        stmfd   sp!, {r0}
```


#### 入力

stmdb r13!, {r0, r1}

#### objdump結果
```
   0:   e92d0003        push    {r0, r1}
```


#### 入力

stmdb r13!, {r0-r10}

#### objdump結果
```
0:   e92d07ff  push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl}
```


#### 入力

stmdb r13!, {r0, r1, r2, r3, r4, r5}

#### objdump結果
```
   0:   e92d003f        push    {r0, r1, r2, r3, r4, r5}
```


#### 入力

stmdb r13!, {r0, r1-r5}

#### objdump結果
```
   0:   e92d003f        push    {r0, r1, r2, r3, r4, r5}
```


#### 入力

stmdb r13!, {r0, r1-r5}

#### objdump結果
```
   0:   e92d003f        push    {r0, r1, r2, r3, r4, r5}
```


#### 入力

stmdb r13!, {r1-r5, r0}

#### objdump結果
```
   0:   e92d003f        push    {r0, r1, r2, r3, r4, r5}
```
 

#### 警告メッセージ
```
gaku@DESKTOP-IB1OCGR:~$ arm-none-eabi-as hello.s -o hello.o;
hello.s: Assembler messages:
hello.s:4: Warning: register range not in ascending order
```
レジスタは昇順で書かないと警告がでる。  


#### 入力

stmdb r13!, {r0-r15}

#### objdump結果
```
  0:   e92dffff        push    {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
```

#### 警告メッセージ
```
gaku@DESKTOP-IB1OCGR:~$ arm-none-eabi-as hello.s -o hello.o;
hello.s: Assembler messages:
hello.s:4: Warning: if writeback register is in list, it must be the lowest reg in the list
```

r13(write back register)がスタックに積まれている。  




### メモ
[,]と[-]で処理を分けるステートマシンにする？  
それとも、省略をサポートしないか。




















# 詰まったところ、変な実装

## forループ

下の書き方はわかりにくい。

### 前
```
   // 数字取得
    for (; isdigit(ch); ch = str[pos]) {
        reg_num = reg_num * 10 + (ch - '0');
        pos++;
    }

```
for文は強力すぎるので、あんまりイディオム的じゃない書き方では使わない方が良いと思う。
で、このforの最初の括弧の中の、三つ目の式、つまりch= str[pos]は、あんまりfor文では見ない書き方と思う。

### 改善例

```
  // 数字取得
    for (; isdigit(str[pos]); pos++) {
        reg_num = reg_num * 10 + (str[pos] - '0');
    }
    
    // 数字取得
    while(isdigit(ch)) {
        reg_num = reg_num * 10 + (ch - '0');
        ch = str[++pos]
    }
    
    
    
```
または、
```
    // 数字取得
    while(isdigit(str[pos])){
        reg_num = reg_num * 10 + (str[pos] - '0');
        pos++;
    }
```
    


## regression test 変なイテレータ、zipを使った

###前
```python
ac_iter = iter(list(actual.upper()))
print("actual: ", end="")
for a, b in zip(ac_iter, ac_iter):
    print(f"{a}{b} ", end="")
```

これは、変なイテレータやzipの使いかた。  

二文字ずつ表示させるなら。  

###改善例
```python
act_str = actual.upper()
print("actual: ", end="")
for one in [act_str[i:(i+2)] for i in range(4)]:
    print(one, " ", end="")
```

こんな感じに  


