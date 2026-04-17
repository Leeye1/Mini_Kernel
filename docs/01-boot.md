# 启动流程详解

## RISC-V 启动流程概述

RISC-V 系统的启动流程遵循严格的硬件规范：

```
硬件复位
    ↓
OpenSBI 固件 (0x80000000)
    ↓
加载内核到 0x80200000
    ↓
跳转执行内核代码 (S-mode)
```

## QEMU virt 平台启动

在 QEMU virt 平台上：
1. OpenSBI 作为固件，加载到 0x80000000
2. 内核被加载到 0x80200000（避开 OpenSBI）
3. 跳转到内核入口点

### OpenSBI 信息解读

```
Platform Name             : riscv-virtio,qemu
Platform HART Count       : 1               // 1 个 CPU 核心
Platform Timer Device     : aclint-mtimer @ 10000000Hz  // 10MHz 时钟
Platform Console Device   : uart8250         // 串口

Firmware Base             : 0x80000000
Firmware Size             : 322 KB

Domain0 Next Address      : 0x0000000080200000  // 内核加载地址
Domain0 Next Mode         : S-mode              // 内核运行在 S 模式
```

## 链接脚本 (linker.ld)

链接脚本定义了内核在内存中的布局：

```ld
. = 0x80200000;    // 内核基地址（避开 OpenSBI）
.text : { ... }    // 代码段
.rodata : { ... }  // 只读数据
.data : { ... }    // 已初始化数据
.bss : { ... }     // 未初始化数据
.stack : { ... }   // 栈空间
```

关键点：
- **0x80000000** 是 OpenSBI 地址
- **0x80200000** 是内核加载地址
- **0x80410000** 是栈顶地址（在 OpenSBI 区域内）

## 启动汇编 (start.S)

```asm
_start:
    la sp, _stack_top    # 设置栈指针
    jal main             # 跳转到 main 函数
_loop:
    wfi                  # 等待中断
    j _loop              # 无限循环
```

关键点：
- `la sp, _stack_top`：加载栈顶地址到 sp
- `jal main`：跳转到 main 并保存返回地址到 ra
- `wfi`：Wait For Interrupt，低功耗等待

## 主函数 (main.c)

```c
int main(void) {
    putchar_uart('H');   // 直接写 UART
    puts_uart("Hello\n");
    sbi_shutdown();      // 调用 SBI 关机
}
```

内核启动后：
1. 使用直接 UART 方式输出字符（绕过 SBI console）
2. 显示欢迎信息
3. 调用 SBI shutdown

## 为什么用直接 UART 而非 SBI console？

SBI console_putchar() 需要通过 ecall 指令调用 OpenSBI，但在 **nographic** 模式下，SBI console 输出可能与 QEMU 的输出冲突或被缓冲。

直接写入 0x10000000 (UART 基地址) 更可靠。

## 下一步

阶段 2 将实现：
- 虚拟内存管理（页表）
- 物理内存管理
- 中断处理框架