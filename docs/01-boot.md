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
- **0x80400000** 是栈底地址（在 OpenSBI 区域内）
- **0x80420000** 是栈顶地址

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
    putchar_uart('H');   // 通过 SBI console 输出字符
    puts_uart("Hello\n");
    sbi_shutdown();      // 调用 SBI 关机
}
```

内核启动后：
1. 通过 SBI console 服务输出字符（`sbi_console_putchar()`）
2. 显示欢迎信息
3. 调用 SBI shutdown

## SBI 调用约定（重要！）

SBI v0.2+ 的寄存器约定：
- `a0-a5`：参数
- `a6`：Function ID (FID)
- `a7`：Extension ID (EID)
- `ecall`：触发调用

**注意**：OpenSBI v1.3 运行时 SBI 版本为 1.0，已移除旧的 Console 扩展（EID=0x4）。必须使用 Legacy Console 扩展（EID=0x1, FID=0x0）才能正常输出字符。

## 下一步

阶段 2 将实现：
- 虚拟内存管理（页表）
- 物理内存管理
- 中断处理框架