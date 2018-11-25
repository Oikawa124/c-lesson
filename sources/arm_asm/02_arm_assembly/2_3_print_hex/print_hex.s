/*
 arm-none-eabi-as print_hex.s -o print_hex.o
 arm-none-eabi-ld print_hex.o -Ttext 0x00010000 -o print_hex.elf
 arm-none-eabi-objcopy print_hex.elf -O binary print_hex.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_hex.bin -serial mon:stdio
*/
.globl _start
_start:
    ldr r0,=0x101f1000
    ldr r1,=0xdeadbeaf
    b print_hex


/*
    print_hex:
    arg r0: address of memory mapped UART
    arg r1: address of target string.
    arg r3: counter for shifting by 4bits
    used internal register: r0, r1, r2, r3.
*/

print_hex:
    mov r3, #28
    mov r2, #0x30
    str r2, [r0]
    mov r2, #0x78
    str r2, [r0]

_loop:
    lsr r2, r1, r3
    and r2, r2, #0x0f
    cmp r2, #0x0a
    blt _under_ten
    add r2, r2, #0x07

_under_ten:
    add r2, r2, #0x30
    str r2, [r0]

    sub r3, r3, #4

    cmp r3, #0
    bge _loop
    b busy


busy:
    b busy

# output >> DEADBEAF
