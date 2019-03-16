#!/usr/bin/python3 -V
# -*- coding: utf-8 -*-
# Created by devel on 2019/02/12.
import subprocess
import os

filenames = ["test", "hello_asm", "hello_msg",
             "print_loop", "putchar_bad",
              print_hex_bl, print_hex]

compile_utils = "gcc -c cl_utils.c".split()
compile_main = "gcc -c main.c".split()
compile_parse = "gcc -c parse.c".split()
compile_dictionary = "gcc -c dictionary.c".split()
compile_linked_list = "gcc -c linked_list.c".split()
compile_binary_tree = "gcc -c binary_tree.c".split()
compile_initialize = "gcc -c initialize.c".split()

link = "gcc cl_utils.o main.o parse.o dictionary.o linked_list.o binary_tree.o initialize.o -o test".split()


def main():
    # コンパイル
    subprocess.run(compile_utils, shell=True)
    subprocess.run(compile_main, shell=True)
    subprocess.run(compile_parse, shell=True)
    subprocess.run(compile_dictionary, shell=True)
    subprocess.run(compile_linked_list, shell=True)
    subprocess.run(compile_binary_tree, shell=True)
    subprocess.run(compile_initialize, shell=True)

    subprocess.run(link, shell=True)

    is_success = True
    for file_name in filenames:
        exec = f"test.exe ./test_input/{file_name}.ks".split()

        # 実行
        subprocess.run(exec, shell=True)

        # ファイル読み込み、バイナリ比較
        with open(f"./test.bin", "rb") as ac_f, open(f"./test_expect/{file_name}.bin", "rb") as ex_f:

            line_num = 0

            while True:
                actual = ac_f.read(4).hex()
                expect = ex_f.read(4).hex()
                if (len(actual) == 0) and (len(expect) == 0):
                    break

                if actual != expect:
                    print("error")
                    print(f"File Name: {file_name}")
                    print(f"line_num: 0x{line_num:08X}")

                    # 16進数ダンプ

                    # actual
                    act_str = actual.upper()
                    print("actual: ", end="")
                    for one in [act_str[i:(i+2)] for i in range(4)]:
                        print(one, " ", end="")
                    print("")

                    # expect
                    exp_str = expect.upper()
                    print("expect: ", end="")
                    for one in [exp_str[i:(i+2)] for i in range(4)]:
                        print(one, " ", end="")

                    is_success = False
                    break

                line_num += 0x4

        if not is_success:
            break

    # ファイル削除
    os.remove("./main.o")
    os.remove("./cl_utils.o")
    os.remove("./initialize.o")
    os.remove("./binary_tree.o")
    os.remove("./linked_list.o")
    os.remove("./parse.o")
    os.remove("./dictionary.o")

    os.remove("./test.exe")

    #todo ファイル削除を正規表現で書き直し

    if is_success:
        os.remove("./test.bin")



if __name__ == '__main__':
    main()
