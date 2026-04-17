.section .text.boot
.global _start

# RISC-V 启动代码
# 1. 设置栈指针
# 2. 跳转到 C 代码 main()

_start:
    # sp 寄存器：栈指针
    # 0x80040000 是内核栈顶（在内核空间上方）
    # 栈向下增长，所以设置为栈的高地址
    la sp, _stack_top

    # 跳转到 main 函数
    # 使用 jal（Jump And Link）指令
    # jal rd, offset: rd = pc + 4, pc = pc + offset
    # 这里 rd = ra（返回地址寄存器）
    jal main

# 无限循环 - 如果 main 返回则停机
_loop:
    wfi     # Wait For Interrupt（低功耗等待）
    j _loop

# 预留栈空间 - 8KB 内核栈
.section .bss
.align 12   # 4KB 对齐
_stack:
    .space 8192
_stack_top: