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

0:   e1a01002        mov     r1, r2

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

CMAKE_C_STANDARD 99　がコンパイラのバージョン？


printf("%d", sizeof(emitter->array));

出力 > 4


printf("%d", sizeof(emitter->array[0]));

出力 > 4


バイナリエディタ
04 10 9F E5



##### コマンドプロンプトの場合


C:\Users\devel>gcc --version
realgcc.exe (Rev3, Built by MSYS2 project) 5.2.0
がバージョン？


printf("%d", sizeof(emitter->array));

出力 > 8

printf("%d", sizeof(emitter->array[0]));

出力 > 4

バイナリ
04 10 9F E5



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

### staticofについて

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


00000000 <.data>:
   0:   ea000001        b       0xc
   4:   e1a01002        mov     r1, r2
   8:   e1a01002        mov     r1, r2



## hello_asm.ksの実行

### objdump結果
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


### hello_asm.sのobjdump結果
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


## 文字列リテラルとstate machine







## hello_loop.ksをアセンブル出来るようにする















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


