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


## 謎
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
