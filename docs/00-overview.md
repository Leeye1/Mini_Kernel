# Mini Kernel 学习项目

## 概述

从零开始构建的 RISC-V 64-bit 操作系统内核，采用详细注释的教学风格。

## 阶段划分

### 阶段 1：最小内核（Boot）- 当前阶段 ✓
- **目标**：在 QEMU 上启动并通过 SBI 输出字符
- **文件**：
  - `kernel/start.S` - 启动汇编，设置栈，跳转 C，SBI 关机
  - `kernel/main.c` - 内核入口，SBI console 输出
  - `kernel/sbi.h` - SBI 接口定义（调用约定、Legacy Console 扩展）
  - `kernel/types.h` - 类型定义
  - `kernel/linker.ld` - 链接脚本

### 阶段 2：内存管理与中断
- 页表管理（Sv39）
- 物理内存管理（位图分配）
- 内核堆分配（kmalloc/kfree）
- 中断处理（SBI 调用）
- 时钟中断

### 阶段 3：进程与调度
- 进程结构（struct proc）
- 上下文切换
- 调度器（轮转）
- 系统调用框架
- 用户态支持

### 阶段 4：文件系统
- inode 结构
- 日志文件系统
- Buffer Cache
- 文件描述符

### 阶段 5：完善与扩展
- Shell
- 多核支持（SMP）
- 外设驱动（virtio）
- 进程间通信

## 构建与运行

```bash
# 构建
make

# 运行
make run

# 清理
make clean

# 反汇编
make disasm
```

## 目录结构

```
Mini_Kernel/
├── docs/              # 详细讲解文档
│   ├── 00-overview.md
│   ├── 01-boot.md
│   ├── 02-uart.md
│   └── 03-api.md
├── kernel/           # 内核源码
├── user/             # 用户程序
└── Makefile          # 构建脚本
```