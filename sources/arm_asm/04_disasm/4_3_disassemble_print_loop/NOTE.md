## マスクする

&演算子を使う。  
x&0だと0になる。x&fだとxになる。(16進数の場合)


int offset = 0xE3A01068 & 0x0000ffff;
＞＞ 0x1068

[Ｃ言語編　第４９章　ビット演算](https://programming-place.net/ProgrammingPlacePlus/c/049.html)

##　ビットシフト

int _register = (0xE3A01068 >> 12);  
16進数なので、 4bit*3で三桁右にシフトすることができる。  
">> x"のxは二進数でどれくらいシフトするかを表す。


## 終末文字の扱い
```
char *cl_get_result(int num) {
    int null_count=0;
    int i=0;
    while (null_count != num-1) {
        if (buf[i] == '\0') {
            null_count++;
        }
        i++;
    }
    return &buf[i];
}

```
でどうして，配列のポインタを渡すだけでいいのだろうか．
```
char *word = "a\0b";
char word_array[] = {'a', '\0', 'b', '\0'};
```
上２つは違う
wordは "a"まで
word_array[] は{'a', '\0', 'b', '\0'}
までメモリに入る．

ので，char *xxx = &buf[i]
のように渡すと'\0'までの文字列が代入される．

メモリと出力の関係

    ｀｀｀char *word = "aa\0bb";

    for (int i = 0; i < 6; ++i) {
        printf("s: %s\n", &word[i]);
        printf("x_&: %x\n", &word[i]);

        printf("x: %x\n", word[i]);
        printf("#######\n");
    }｀｀｀
とすると、
```s: aa
x_&: 406132
x: 61
#######
s: a
x_&: 406133
x: 61
#######
s:
x_&: 406134
x: 0
"#######
s: bb
x_&: 406135
x: 62
#######
s: b
x_&: 406136
x: 62
#######
s:
x_&: 406137
x: 0
#######
```

## 2の補数
int a = 0xfffffd;  

24bit(4bit * 6)で表現するようにマスクする  
0b1を足す  
int b = (~a & 0xffffff) + 0b1;  
printf("%x", b);  


## cmake　PowerShellでの実行(ダメだった)

1. PS C:\Users\devel\CLionProjects\disasm> cd .\cmake-build-debug\
でbuildフォルダに移動
2. cmake ..
で1つ上の階層の実行(CMakeLists.txtがあるファルダ)
3. .exe
を実行する

上の方法ではうまくいかない・・・
普通にgccを使うことにする。

## objdumpのときどうして最後に変な命令がつくか


print_loop.sのobjdump
```
00000000 <.data>:
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

```6c6c6568```

はASCIIコードに対応させて、
6c : "l"
6c : "l"
65 : "e"
68 : "h"
になっている。

命令とは違うバイナリの値とarmの命令は、原理的に区別することができない。
ので変な命令が出てくる。

armにおいて、良く使う命令と文字はほとんどかぶらない。

ASCIIコードというのは、0x80より下の数値しか使われない。
0x80は8bit中一番上のbit。ここが1だったらasciiコードではない。
ARMの命令は0xe3とか0xe5など、eと何か、というバイトを一番上のバイトに含んでる事が多い（リトルインディアンでは最後に表示）
なので4文字asciiが続けば、0xe5とかは入らない事が保証されるので、自動的に「良く使う命令では無い」という事になる。

