#include <stdio.h>

int sum_range(int beg, int end) {
    int sum=0;
    for(int i = beg; i <= end; i++) {
        sum+=i;
    }
    return sum;
}

int sum_range_inline(int beg, int end) {
    int res;
    /*
    TODO: Fix code below to pass test.
    */

    // r0 : beg (loop counter)
    // r1 : end
    // r2 : tmp result

    asm("mov r2, #0");
    asm("loop:");
    asm("cmp r0, r1");
    asm("bgt end");
    asm("add r2, r2, r0"); // sum+=i
    asm("add r0, r0, #1"); // i++
    asm("b loop");
    asm("end:");

    asm("mov %0, r2" :"=r"(res));

    return res;
}


/*
test code
*/


void assert_true(int boolflag) {
    if(!boolflag) {
        printf("assert fail\n");
    }
}


int main() {
    int res;

    res = sum_range(3, 10);
    assert_true(res == 52);

    // TODO: fix sum_range_inline to pass test.
    res = sum_range_inline(3, 10);
    printf("%d", res);

    assert_true(res == 52);

    return 0;
}

