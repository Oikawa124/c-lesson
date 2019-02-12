#!/usr/bin/python3 -V
# -*- coding: utf-8 -*-
# Created by devel on 2019/02/12.
import subprocess
import os

filenames = ["test"]

def main():
    for file_name in filenames:
        compile_utils = "gcc -c cl_utils.c".split()
        compile_main = "gcc -c main.c".split()
        link = "gcc cl_utils.o main.o -o test".split()
        exec = f"test.exe ./test_input/{file_name}.ks".split()

        # コンパイル、実行
        subprocess.run(compile_utils, shell=True)
        subprocess.run(compile_main, shell=True)
        subprocess.run(link, shell=True)
        subprocess.run(exec, shell=True)

        # ファイル読み込み、バイナリ比較
        with open(f"./test.bin", "rb") as ac_f, open(f"./test_expect/{file_name}.bin", "rb") as ex_f:
            while True:
                actual = ac_f.read(4).hex()
                expect = ex_f.read(4).hex()
                if (len(actual) == 0) and (len(expect) == 0):
                    break

                if actual != expect:
                    print("error")
                    print(f"File Name: {file_name}")

                    # 16進数ダンプ
                    ac_iter = iter(list(actual.upper()))
                    print("actual: ", end="")
                    for a, b in zip(ac_iter, ac_iter):
                        print(f"{a}{b} ", end="")
                    print("")
                    ex_iter = iter(list(expect.upper()))
                    print("expect: ", end="")
                    for a, b in zip(ex_iter, ex_iter):
                        print(f"{a}{b} ", end="")

                    break

    # ファイル削除
    os.remove("./main.o")
    os.remove("./cl_utils.o")
    os.remove("./test.exe")
    #os.remove("./test.bin")



if __name__ == '__main__':
    main()
