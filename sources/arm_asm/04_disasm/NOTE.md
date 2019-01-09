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

    