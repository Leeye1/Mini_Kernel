//
/*
 * Mini Kernel - 主入口
 * RISC-V 64-bit 最小内核
 * 使用 OpenSBI 作为固件
 * 
 * 启动流程：
 *   1. start.S 设置栈指针 sp
 *   2. jal main 跳转到这里
 *   3. 通过 SBI console 输出字符
 *   4. 返回后 start.S 调用 sbi_shutdown 关机
 */

#include "types.h"
#include "sbi.h"

/*
 * putchar_uart — 通过 SBI 输出一个字符
 * 
 * 换行符 \n 需要转换为 \r\n（回车+换行），
 * 因为终端通常期望 CRLF 换行
 */
static void putchar_uart(char c) {
    if (c == '\n') {
        sbi_console_putchar('\r');  /* 先回车 */
    }
    sbi_console_putchar((int)c);     /* 再输出字符 */
}

/*
 * puts_uart — 通过 SBI 输出一个字符串
 */
static void puts_uart(const char *s) {
    while (*s) {
        putchar_uart(*s);
        s++;
    }
}

/*
* shutdown — 通过 SBI 关机
*/
static void shutdown(void) {
    sbi_shutdown();
}

/*
 * main — 内核主入口
 * 
 * 由 start.S 通过 jal main 调用
 * 返回后 start.S 会调用 sbi_shutdown 关机
 */
int main(void) {
    /* 逐字符输出欢迎信息 */
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

    /* 使用字符串输出 */
    puts_uart("=================================\n");
    puts_uart("   Mini Kernel - RISC-V 64-bit\n");
    puts_uart("   Using OpenSBI Firmware\n");
    puts_uart("=================================\n\n");

    puts_uart("Hello Kernel!\n");
    puts_uart("Kernel is running...\n");
    puts_uart("Loaded at 0x80200000\n\n");

    shutdown();  /* 调用 SBI 关机 */
    return 0;
}
