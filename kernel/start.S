## RISC-V 内核启动代码
.section .text.boot /* 启动代码段，放在 .text 段的前面，确保链接器将其放在内核的起始位置 */
.global _start  /* 定义全局符号 _start，链接器会将其设置为程序的入口地址 */

# RISC-V 启动代码
# 1. 设置栈指针
# 2. 跳转到 C 代码 main()

_start:
    # 0x80420000 是内核栈顶（在内核空间上方）
    la sp, _stack_top

    # 跳转到 main 函数
    # 使用 jal（Jump And Link）指令
    # jal rd, offset: rd = pc + 4, pc = pc + offset
    # 这里 rd = ra（返回地址寄存器）
    jal main



# 预留栈空间 - 8KB 内核栈
.section .bss
.align 12   # 4KB 对齐
_stack:
    .space 8192
_stack_top: