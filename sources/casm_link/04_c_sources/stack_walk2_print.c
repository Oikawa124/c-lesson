#include <stdio.h>

void print_address(int address) {
    printf("address: %x\n", address);
}

void print_msg(char *str) {
    printf("We get (%s)\n", str);
}

void func3 () {
    // TODO: get main_msg from here as target someway.
    char *target;

    printf("We are in func3\n");


    print_address(((int) &target) + 4); //func2のr11をstrしたアドレス

    print_address(*(int *) ((int) &target + 4)); // func2のr11の中身

    print_address(*(int *) (*(int *) ((int) &target + 4))); // func2のr11の中身の中身 (func1でpushしたr11)

    print_address(*(int *) (*(int *) (*(int *) ((int) &target + 4)))); // mainでpushしたr11

    print_address(*(int *) (*(int *) (*(int *) ((int) &target + 4))) - 8); // .LCP_15_0のアドレス

    print_address(*(int *) (*(int *) (*(int *) (*(int *) ((int) &target + 4))) - 8)); // .LCP15_0のアドレスの中身

    print_msg((char *) (*(int *)(*(int *)(*(int *)(*(int *)((int)&target + 4))) - 8)));
}

void func2() {
    char *msg = "func2 message.";
    printf("We are in func2, %s\n", msg);
    func3();
}

void func1() {
    char *msg = "func1 msg";
    printf("We are in func1, %s\n", msg);
    func2();
}


int main() {
    char *main_msg = "We are in main.";
    printf("We are in main, %s\n", main_msg);
    func1();
    return 0;
}

