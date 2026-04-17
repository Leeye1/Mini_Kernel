# UART 驱动详解

## 什么是 UART？

UART (Universal Asynchronous Receiver/Transmitter) 是一种串行通信协议，用于在设备和 CPU 之间传输数据。

**特点**：
- 异步通信（不需要时钟线）
- 全双工（同时收发）
- 经典 PC 接口

## 硬件基础

### 信号线
- **TX** (Transmit)：发送数据
- **RX** (Receive)：接收数据
- **GND**：地线

### 通信参数
- **波特率**：如 115200 bps
- **数据位**：通常 8 位
- **停止位**：1 位
- **校验位**：无

## QEMU Virt 平台的 UART

QEMU virt 使用 NS16550 兼容的 UART 控制器：

```
基地址：0x10000000
```

### 寄存器映射

| 偏移 | 寄存器 | 说明 |
|------|--------|------|
| 0x00 | RHR/THR | 接收/发送保持寄存器 |
| 0x01 | IER | 中断使能寄存器 |
| 0x02 | FCR/ISR | FIFO控制/中断状态 |
| 0x03 | LCR | Line 控制寄存器 |
| 0x05 | LSR | Line 状态寄存器 |

### 关键寄存器

**Line Status Register (LSR)** - 0x05
- bit 0: RX_READY - 接收数据就绪
- bit 5: TX_EMPTY - 发送缓冲区空

**Line Control Register (LCR)** - 0x03
- bit 7: DLAB - 除数锁存器访问位
- bit 0-1: 数据位长度（00=5, 01=6, 10=7, 11=8）

## 驱动实现

### 初始化

```c
void uart_init(void) {
    // 1. 使能 DLAB，访问除数锁存器
    uart_write_reg(UART_LCR, 0x80);
    
    // 2. 设置波特率除数
    // 假设时钟 1843200 Hz，除数 = 1 (115200 波特)
    uart_write_reg(UART_DLL, 1);
    uart_write_reg(UART_DLM, 0);
    
    // 3. 设置数据格式：8-N-1
    uart_write_reg(UART_LCR, 0x03);
    
    // 4. 使能 FIFO
    uart_write_reg(UART_FCR, 0x01);
}
```

### 发送字符

```c
void uart_putc(char c) {
    // 等待发送缓冲区空
    while (!(uart_read_reg(UART_LSR) & LSR_TX_EMPTY));
    
    // 写入数据
    uart_write_reg(UART_THR, c);
}
```

### 接收字符

```c
int uart_getc(void) {
    if (uart_read_reg(UART_LSR) & LSR_RX_READY) {
        return uart_read_reg(UART_RHR);
    }
    return -1;  // 无数据
}
```

## 内存屏障

代码中的 `mb()` 是什么？

```c
#define mb() asm volatile("fence" ::: "memory")
```

这是内存屏障（Memory Barrier）：
- 确保 MMIO（内存映射 I/O）操作的顺序
- 防止编译器优化导致读写顺序错误

## 下一步

UART 驱动已经完成，接下来是实现：
- 简单的 printf 函数（阶段 1 扩展）
- 中断驱动的 UART（阶段 2）
- 完整的控制台驱动