/*
 * Mini Kernel - 主入口
 * RISC-V 64-bit 最小内核
 * 使用 OpenSBI 作为固件
 */

#include "types.h"
#include "sbi.h"

// 外部符号（来自链接脚本和汇编）
extern void uart_init(void);
extern void uart_putc(char c);
extern void puts(char *s);

// 使用内联汇编直接写 UART
static void putchar_uart(char c) {
    if (c == '\n') {
        putchar_uart('\r');
    }
    // 直接写入 UART 寄存器 0x10000000
    volatile uint8_t *uart = (volatile uint8_t *)0x10000000;
    // 等待发送缓冲区空 (LSR TX_EMPTY = bit 5)
    while (!(uart[5] & 0x20));
    // 写入字符
    uart[0] = (uint8_t)c;
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
    // 直接 UART 输出
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

    // // 调用 SBI shutdown 确认内核运行
    // puts_uart("Calling SBI shutdown...\n");
    // sbi_shutdown();

    return 0;
}