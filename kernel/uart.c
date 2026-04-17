/*
 * UART 驱动 - 串口通信
 * QEMU virt 平台的 NS16550 兼容串口
 * 基地址: 0x10000000
 */

#include "types.h"

// UART 寄存器偏移地址（相对于基地址）
#define UART_BASE      0x10000000  // QEMU virt UART 基地址
#define UART_RHR       (UART_BASE + 0x00)  // 接收保持寄存器 (Read)
#define UART_THR       (UART_BASE + 0x00)  // 发送保持寄存器 (Write)
#define UART_IER       (UART_BASE + 0x01)  // 中断使能寄存器
#define UART_FCR       (UART_BASE + 0x02)  // FIFO 控制寄存器
#define UART_ISR       (UART_BASE + 0x02)  // 中断状态寄存器
#define UART_LCR       (UART_BASE + 0x03)  // Line 控制寄存器
#define UART_MCR       (UART_BASE + 0x04)  // Modem 控制寄存器
#define UART_LSR       (UART_BASE + 0x05)  // Line 状态寄存器
#define UART_MSR       (UART_BASE + 0x06)  // Modem 状态寄存器
#define UART_DLL       (UART_BASE + 0x00)  // Divisor LSB (DLAB=1)
#define UART_DLM       (UART_BASE + 0x01)  // Divisor MSB (DLAB=1)

// Line Status Register 位定义
#define LSR_RX_READY   0x01    // 接收数据就绪
#define LSR_TX_EMPTY   0x20    // 发送保持寄存器空

// 内存屏障 - 确保 MMIO 操作顺序正确
#define mb()           asm volatile("fence" ::: "memory")

/*
 * 读取 UART 寄存器
 * vol: 寄存器地址
 */
static inline uint8_t uart_read_reg(volatile uint8_t *addr) {
    mb();
    return *addr;
}

/*
 * 写入 UART 寄存器
 * val: 要写入的值
 */
static inline void uart_write_reg(volatile uint8_t *addr, uint8_t val) {
    *addr = val;
    mb();
}

/*
 * 初始化 UART
 * 配置为 115200-8-N-1（无流控）
 */
void uart_init(void) {
    // 1. 使能 DLAB（访问除数锁存器）
    // 设置 LCR 的 bit 7 为 1，以访问 DLL/DLM
    uart_write_reg((volatile uint8_t *)UART_LCR, 0x80);

    // 2. 设置波特率除数
    // 假设输入时钟为 1843200 Hz (常见值)
    // 除数 = 1843200 / (16 * 115200) = 1
    // DLL = 1, DLM = 0
    uart_write_reg((volatile uint8_t *)UART_DLL, 1);
    uart_write_reg((volatile uint8_t *)UART_DLM, 0);

    // 3. 设置数据格式：8位数据、1位停止位、无校验
    // LCR = 0x03: 8位数据 (bit 0-1 = 11), 1位停止位 (bit 2 = 0), 无校验 (bit 3-4 = 00)
    uart_write_reg((volatile uint8_t *)UART_LCR, 0x03);

    // 4. 使能 FIFO（可选，增强性能）
    // FCR = 0x01: 使能 FIFO
    uart_write_reg((volatile uint8_t *)UART_FCR, 0x01);

    // 5. 使能接收和发送（IER）
    // IER = 0x03: 使能接收就绪中断 (bit 0) 和发送完成中断 (bit 1)
    // 暂时不使能中断，保持轮询模式
    uart_write_reg((volatile uint8_t *)UART_IER, 0x00);
}

/*
 * 发送一个字符
 */
void uart_putc(char c) {
    // 等待发送缓冲区空
    // 检查 LSR 的 TX_EMPTY 位 (bit 5)
    while (!(uart_read_reg((volatile uint8_t *)UART_LSR) & LSR_TX_EMPTY));

    // 写入字符到发送保持寄存器
    uart_write_reg((volatile uint8_t *)UART_THR, (uint8_t)c);
}

/*
 * 接收一个字符
 * 返回: 接收的字符，如果无数据则返回 -1
 */
int uart_getc(void) {
    // 检查是否有数据就绪
    // 检查 LSR 的 RX_READY 位 (bit 0)
    if (uart_read_reg((volatile uint8_t *)UART_LSR) & LSR_RX_READY) {
        return (int)uart_read_reg((volatile uint8_t *)UART_RHR);
    }
    return -1;
}

/*
 * 发送字符串
 */
void puts(char *s) {
    while (*s) {
        // 处理换行符：输出 \r\n
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s);
        s++;
    }
}