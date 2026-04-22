/*
 * Mini Kernel - 主入口
 * RISC-V 64-bit 最小内核
 * 使用 OpenSBI 作为固件
 */

#include "types.h"
#include "sbi.h"

// 使用 SBI 输出字符
static void putchar_uart(char c) {
    sbi_console_putchar((int)c);
}

// 输出字符串
static void puts_uart(char *s) {
    while (*s) {
        putchar_uart(*s);
        s++;
    }
}

// 主函数 - 内核入口
int main(void) {
    // 使用 SBI 输出
    putchar_uart('H');
    putchar_uart('e');
    putchar_uart('l');
    putchar_uart('l');
    putchar_uart('o');
    putchar_uart(' ');
    putchar_uart('K');
    putchar_uart('e');
    putchar_uart('r');
    putchar_uart('n');
    putchar_uart('e');
    putchar_uart('l');
    putchar_uart('!');
    putchar_uart('\n');

    puts_uart("=================================\n");
    puts_uart("   Mini Kernel - RISC-V 64-bit\n");
    puts_uart("   Using OpenSBI Firmware\n");
    puts_uart("=================================\n\n");

    puts_uart("Hello Kernel!\n");
    puts_uart("Kernel is running...\n");
    puts_uart("Loaded at 0x80200000\n\n");

    return 0;
}