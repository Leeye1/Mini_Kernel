## RISC-V Inline Assembly
### 输出字符到 UART：

```c
#define SBI_EXT_LEGACY     0x1 // Legacy Console 扩展 ID
#define SBI_LEGACY_CONSOLE_PUTCHAR    0x0    /* 输出一个字符 */

putchar_uart(char c)->
sbi_console_putchar((int)c)->
sbi_call(SBI_EXT_LEGACY, SBI_LEGACY_CONSOLE_PUTCHAR, ch, 0, 0, 0, 0, 0);
```

```c
//sbi_call 内联函数
static inline int64_t sbi_call(uint64_t eid, uint64_t fid,
                                uint64_t arg0, uint64_t arg1,
                                uint64_t arg2, uint64_t arg3,
                                uint64_t arg4, uint64_t arg5) {
    /* 使用具名寄存器变量，确保值放入正确的寄存器 */
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
        : "+r"(a0)          /* a0 既是输入也是输出（返回值） */
        : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
          "r"(a6), "r"(a7)
        : "memory"           /* 告知编译器内存可能被修改 */
    );

    return (int64_t)a0;
}
```