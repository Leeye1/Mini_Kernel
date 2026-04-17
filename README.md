# Mini Kernel

从零开始构建的 RISC-V 64-bit 操作系统内核，用于学习目的。

## 功能特性

- **阶段 1**：最小内核（Boot + UART + OpenSBI）
  - 启动汇编 (start.S)
  - 链接脚本 (0x80200000 加载地址)
  - 直接 UART 输出
  - SBI 接口调用

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
├── kernel/           # 内核源码
│   ├── start.S       # 启动汇编
│   ├── main.c        # 主入口
│   ├── uart.c        # UART 驱动
│   ├── types.h       # 类型定义
│   ├── sbi.h         # SBI 接口
│   └── linker.ld     # 链接脚本
├── docs/             # 学习文档
│   ├── 00-overview.md   # 阶段计划
│   ├── 01-boot.md       # 启动流程
│   ├── 02-uart.md       # UART 驱动
│   └── 03-api.md       # API 参考
├── Makefile          # 构建脚本
└── AGENTS.md         # Agent 指南
```

## 学习顺序

建议按以下顺序学习：

1. `docs/00-overview.md` - 整体架构
2. `docs/01-boot.md` - 启动流程
3. `kernel/linker.ld` - 内存布局
4. `kernel/start.S` - 启动汇编
5. `kernel/sbi.h` - SBI 接口
6. `kernel/main.c` - 内核入口
7. `Makefile` - 构建流程

## 环境要求

- RISC-V 工具链：`riscv64-linux-gnu-gcc`
- QEMU：`qemu-system-riscv64`

## 运行效果

```
OpenSBI v1.3
Platform: riscv-virtio,qemu
Domain0 Next Address: 0x80200000
Domain0 Next Mode: S-mode

Hello Kernel!
Mini Kernel - RISC-V 64-bit
Using OpenSBI Firmware
```

## 阶段计划

- [x] 阶段 1: 最小内核（Boot + UART）
- [ ] 阶段 2: 内存管理与中断
- [ ] 阶段 3: 进程与调度
- [ ] 阶段 4: 文件系统
- [ ] 阶段 5: 完善与扩展

## 许可证

MIT License