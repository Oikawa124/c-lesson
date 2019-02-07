## 05簡易アンブラを作ろう

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

### グローバル変数の.cファイルで共有する方法




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
    
    



