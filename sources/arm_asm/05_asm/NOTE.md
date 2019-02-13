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

アライメント？が関係しているか？

そのため、  

fwrite(emitter->array, sizeof(unsigned int), emitter->pos, fp);

に変更した。  

intが4byteか8byteか処理系で違うということ。　　


#### uint32_tを使う

固定幅の型であるuint32_tを使う

[Fixed width integer types ](https://en.cppreference.com/w/c/types/integer)











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


//