# Mini Kernel

从零开始构建的 RISC-V 64-bit 操作系统内核，用于学习目的。

## 功能特性

- **阶段 1**：最小内核（Boot + SBI Console + OpenSBI）
  - **启动流程**：RISC-V 汇编启动代码，栈指针初始化，C 语言主入口
  - **内存布局**：内核加载于 0x80200000（避开 OpenSBI 0x80000000-0x80400000），8KB 内核栈
  - **SBI Console 输出**：通过 OpenSBI 提供的 console 服务进行字符输出
  - **SBI 接口**：定义 SBI 调用约定，支持 Base/Console/Timer/Shutdown 扩展
  - **构建特性**：位置无关代码（-fpic）、禁用链接放松（--no-relax）

## 快速开始

```bash
# 构建
make

# 运行
make run

# 清理
make clean
```

## 项目结构

```
Mini_Kernel/
├── kernel/                 # 内核源码
│   ├── start.S            # 启动汇编（栈初始化、main 跳转、SBI 调用）
│   ├── main.c             # 主入口（SBI console 输出）
│   ├── types.h            # 基础类型定义（uint/int/指针类型）
│   ├── sbi.h              # SBI 接口（调用约定、函数 ID、内联函数）
│   └── linker.ld          # 链接脚本（内存布局：0x80200000-0x80412000）
├── docs/                  # 学习文档
│   ├── 00-overview.md     # 阶段计划和项目概览
│   ├── 01-boot.md         # 启动流程详解
│   ├── 03-api.md          # API 参考
│   ├── learning.md        # 学习资源
│   ├── qemu/              # QEMU 和 OpenSBI 资料
│   └── linker script/      # 链接脚本详解
├── Makefile               # 构建脚本
├── AGENTS.md              # Agent 指南
└── README.md              # 本文件
```

## 学习顺序

建议按以下顺序学习：

1. **理论基础**
   - [docs/00-overview.md](docs/00-overview.md) - 整体架构和阶段规划
   - [docs/01-boot.md](docs/01-boot.md) - RISC-V 启动流程
   - [docs/qemu/riscv引导流程.md](docs/qemu/riscv引导流程.md) - QEMU 与 OpenSBI

2. **源代码实现**
   - [kernel/linker.ld](kernel/linker.ld) - 内存布局（0x80200000 加载地址）
   - [kernel/start.S](kernel/start.S) - 启动汇编（栈初始化、SBI 调用）
   - [kernel/types.h](kernel/types.h) - 基础类型定义
   - [kernel/sbi.h](kernel/sbi.h) - SBI 接口和内联函数

3. **API 与编译**
   - [docs/03-api.md](docs/03-api.md) - SBI console API 参考
   - [Makefile](Makefile) - 构建流程（gcc 选项、链接脚本配置）

## 环境要求

- **RISC-V 工具链**：`riscv64-linux-gnu-gcc` / `riscv64-linux-gnu-ld`
- **模拟器**：`qemu-system-riscv64`
- **系统**：Linux

## 构建系统说明

### 关键编译选项

| 选项 | 用途 | 原因 |
|------|------|------|
| `-fpic` | 位置无关代码 | 解决重定位问题，允许内核在任意地址加载 |
| `-nostdlib` | 不链接标准库 | 避免 libc 依赖 |
| `-nostdinc` | 不使用标准头文件 | 避免与内核定义冲突 |
| `--no-relax` | 禁用链接放松 | 防止链接器优化导致的地址问题 |
| `-march=rv64gc` | RISC-V 64-bit GC | 包含 IMACFD 扩展 |

### 链接脚本关键地址

- **内核加载地址**：`0x80200000`（S-mode 特权态内存）
- **OpenSBI 范围**：`0x80000000 - 0x80400000`（M-mode 固件）
- **内核栈**：`0x80410000 - 0x80412000`（8KB）
- **栈顶（sp）**：`0x80412000`（递减式栈）

### Makefile 目标

```bash
make              # 构建内核（kernel/kernel.elf）
make run          # 运行内核（QEMU 模拟）
make debug        # 调试模式（GDB 调试 -S -s）
make disasm       # 生成反汇编（kernel/disasm.txt）
make syms         # 生成符号表（kernel/syms.txt）
make clean        # 清理编译产物
```

## 运行效果

```
OpenSBI v1.3
Platform: riscv-virtio,qemu
Domain0 Next Address: 0x80200000
Domain0 Next Mode: S-mode

Hello Kernel!
=================================
   Mini Kernel - RISC-V 64-bit
   Using OpenSBI Firmware
=================================

Hello Kernel!
Kernel is running...
Loaded at 0x80200000
```

## 阶段计划

- [x] 阶段 1: 最小内核（Boot + UART）
- [ ] 阶段 2: 内存管理与中断
- [ ] 阶段 3: 进程与调度
- [ ] 阶段 4: 文件系统
- [ ] 阶段 5: 完善与扩展

## 已知限制

- **无虚拟内存**：当前运行在物理地址上，未实现分页机制
- **单一模式**：仅支持 Supervisor 模式，无用户态
- **无中断处理**：尚未实现异常和中断处理
- **静态配置**：系统参数硬编码，无动态配置

## 许可证

MIT License