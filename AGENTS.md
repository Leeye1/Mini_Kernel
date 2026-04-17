# Mini Kernel - Agent 指南

## 项目概述

从零开始的 RISC-V 64-bit 操作系统内核，用于学习目的。

## 快速开始

```bash
# 构建
make

# 运行
make run

# 清理
make clean
```

## 构建系统

- **工具链**: `riscv64-linux-gnu-gcc`
- **链接器**: `riscv64-linux-gnu-ld`
- **模拟器**: `qemu-system-riscv64`
- **关键选项**:
  - `-fpic` - 解决重定位问题（重要！）
  - `-nostdlib -nostdinc` - 无标准库
  - `--no-relax` - 禁用链接放松

## 目录结构

```
kernel/           # 内核源码
  start.S        # 启动汇编
  main.c         # 主入口
  uart.c         # 串口驱动
  types.h        # 类型定义
  linker.ld      # 链接脚本
docs/            # 文档
  00-overview.md # 阶段计划
  01-boot.md     # 启动流程
  02-uart.md     # UART 驱动
  03-api.md      # API 参考
```

## 阶段状态

- [x] 阶段 1: 最小内核（Boot + UART）
- [ ] 阶段 2: 内存管理与中断
- [ ] 阶段 3: 进程与调度
- [ ] 阶段 4: 文件系统
- [ ] 阶段 5: 完善与扩展

## 已知限制

- 暂未实现分页（直接运行在物理地址）
- 字符串常量有重定位问题（用 putchar_uart 逐字符输出）
- 内核加载到 0x80200000 避开 OpenSBI (0x80000000)
- nographic 模式下用直接 UART 输出而非 SBI console

## 文档位置

详细学习文档见 `docs/` 目录，按阶段顺序阅读。