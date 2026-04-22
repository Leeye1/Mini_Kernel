# OpenSBI 与 SBI 调用规范

## 什么是 OpenSBI？

OpenSBI 是 RISC-V 架构的 **Supervisor Binary Interface (SBI)** 的参考实现，它是一个运行在 **M 模式（Machine Mode）** 的固件。

### 为什么需要 SBI？

RISC-V 有多个特权级：
- **M 模式（Machine）**：最高特权级，可以直接操作所有硬件
- **S 模式（Supervisor）**：操作系统内核运行的特权级
- **U 模式（User）**：用户程序运行的特权级

操作系统内核（S 模式）不能直接操作某些硬件（如定时器、中断控制器），因为这些操作需要 M 模式的权限。SBI 就是 S 模式和 M 模式之间的"桥梁"——内核通过 `ecall` 指令请求 M 模式固件代为执行硬件操作。

```
┌─────────────────────────────────────────┐
│          U 模式（用户程序）               │
│          不能直接调用 SBI                 │
├─────────────────────────────────────────┤
│          S 模式（操作系统内核）            │
│          通过 ecall 调用 SBI              │
│          ┌──────────────────────┐        │
│          │  sbi_console_putchar  │        │
│          │  sbi_set_timer        │        │
│          │  sbi_shutdown         │        │
│          └──────────┬───────────┘        │
│                     │ ecall              │
├─────────────────────┼────────────────────┤
│                     ▼                    │
│          M 模式（OpenSBI 固件）           │
│          ┌──────────────────────┐        │
│          │  实际操作 UART 硬件    │        │
│          │  实际操作定时器        │        │
│          │  实际操作关机          │        │
│          └──────────────────────┘        │
├─────────────────────────────────────────┤
│              硬件层                       │
└─────────────────────────────────────────┘
```

## SBI 调用约定（SBI v0.2+）

根据 [RISC-V 官方 SBI 规范](https://github.com/riscv-non-isa/riscv-sbi-doc)，所有 SBI 函数共享统一的二进制编码格式：

### 寄存器布局

| 寄存器 | 作用 | 说明 |
|--------|------|------|
| `a7` | **Extension ID (EID)** | 扩展编号，决定调用哪个功能组 |
| `a6` | **Function ID (FID)** | 功能编号，决定调用该扩展中的哪个函数 |
| `a0`–`a5` | 参数 | 传递给 SBI 函数的参数 |
| `ecall` | 触发调用 | 陷入 M 模式执行 SBI 函数 |

### 返回值

SBI 调用返回一对值在 `a0` 和 `a1` 中：

```c
struct sbiret {
    long error;   // a0: 错误码，0 表示成功
    long value;   // a1: 附加值（具体含义取决于函数）
};
```

### 寄存器保存约定

**除 `a0` 和 `a1` 外，所有寄存器在 SBI 调用后必须保持不变**。这意味着 callee（OpenSBI）负责保存和恢复其他寄存器。

### 调用示例

```asm
# 输出字符 'A'（Legacy Console 扩展）
li a0, 65              # 参数0: 字符 'A' 的 ASCII 码
li a6, 0               # FID: 0 (putchar)
li a7, 0x01            # EID: 0x01 (Legacy Console)
ecall                  # 触发 SBI 调用
```

## SBI 扩展版本演进

### SBI v0.1（Legacy）
最早的 SBI 版本，使用单个寄存器编码所有功能：
- `a7` = SBI 功能号（0-9）
- 没有 EID/FID 的概念

### SBI v0.2+（当前标准）
引入 EID/FID 分离的设计：
- `a7` = EID（扩展 ID）
- `a6` = FID（功能 ID）
- 支持更多扩展，结构更清晰

### SBI v1.0（当前运行时版本）
- **移除了旧的 Console 扩展（EID=0x04）**
- Legacy Console（EID=0x01）仍然可用
- 新增更多扩展（SRST、PMU 等）

**重要**：当前 OpenSBI v1.3 运行时 SBI 版本为 1.0，必须使用 Legacy Console（EID=0x01）而非 Console 扩展（EID=0x04）。

## 常用 SBI 扩展

### Legacy Console (EID=0x01)

| FID | 函数 | 参数 | 返回值 |
|-----|------|------|--------|
| 0 | `sbi_console_putchar` | a0=字符 | 无 |
| 1 | `sbi_console_getchar` | 无 | a0=字符（-1 表示无数据）|

### Timer (EID=0x54494D45, 即 "TIME")

| FID | 函数 | 参数 | 返回值 |
|-----|------|------|--------|
| 0 | `sbi_set_timer` | a0=stime 绝对值 | 无 |

### System Reset (EID=0x53525354, 即 "SRST")

| FID | 函数 | 参数 | 返回值 |
|-----|------|------|--------|
| 0 | `sbi_system_reset` | a0=reset_type, a1=reset_reason | 无 |

reset_type: 0=关机, 1=冷重启, 2=热重启
reset_reason: 0=无原因

### Base (EID=0x10)

| FID | 函数 | 返回值 |
|-----|------|--------|
| 0 | `sbi_get_spec_version` | a0=SBI 规范版本 |
| 3 | `sbi_probe_extension` | a0=0 表示不支持，非 0 表示支持 |

## 代码中的实现

### sbi_call 函数

```c
static inline int64_t sbi_call(uint64_t eid, uint64_t fid,
                                uint64_t arg0, uint64_t arg1,
                                uint64_t arg2, uint64_t arg3,
                                uint64_t arg4, uint64_t arg5) {
    register uint64_t a0 asm("a0") = arg0;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;
    register uint64_t a4 asm("a4") = arg4;
    register uint64_t a5 asm("a5") = arg5;
    register uint64_t a6 asm("a6") = fid;
    register uint64_t a7 asm("a7") = eid;

    __asm__ __volatile__(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
          "r"(a6), "r"(a7)
        : "memory"
    );

    return (int64_t)a0;
}
```

**为什么使用具名寄存器变量？**
- GCC 内联汇编中，`register uint64_t a0 asm("a0")` 确保值放入指定寄存器
- 比在输入约束中做位运算更清晰、更可靠
- 符合 SBI 规范中 EID/FID 分开放置的要求

### 字符输出示例

```c
// 输出 'A'
sbi_console_putchar('A');
// 内部调用: sbi_call(0x01, 0x00, 'A', 0, 0, 0, 0, 0)
// 即: EID=0x01 (Legacy), FID=0x00 (putchar), arg0='A'
```

## 参考资料

- [RISC-V SBI 规范官方文档](https://github.com/riscv-non-isa/riscv-sbi-doc)
- [RISC-V SBI 规范 PDF（最新发布版）](https://github.com/riscv-non-isa/riscv-sbi-doc/releases)
- [OpenSBI 项目](https://github.com/riscv-software-src/opensbi)
