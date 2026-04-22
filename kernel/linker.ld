/*
 * 链接脚本 - 定义内核加载地址和入口点
 * RISC-V 64-bit 内核
 * 加载地址: 0x80200000 (避开 OpenSBI 在 0x80000000-0x80400000)
 */

OUTPUT_FORMAT("elf64-littleriscv") /* 输出 64位小端 RISC-V ELF 格式 */
ENTRY(_start) /* 定义入口点为 _start，链接器会将其设置为程序的入口地址 */

/*
 * 内核虚拟地址空间布局
 * 0x80200000 开始，内核代码区域
 */
SECTIONS {
    /* 代码段 - 从 0x80200000 开始 */
    . = 0x80200000;
    
    /* 代码段 - 包含启动代码和其他内核代码 */
    .text : {
        *(.text.boot)   /* 启动代码优先 */
        *(.text)        /* 所有目标文件的text段合并到这里 */
    } = 0
    
    /* 只读数据 */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* 已初始化数据 */
    .data : {
        *(.data)
    }
    
    /* 未初始化数据 */
    .bss : {
        *(.bss)
        *(.bss.*)
        . = ALIGN(8);
        _end = .;
    }
    
    /* 栈区域 - 放在 OpenSBI 区域上方 */
    . = 0x80410000;
    .stack : {
        . = ALIGN(16);
        # 定义一个符号，记录当前位置（栈区域的低地址边界）
        _stack_bottom = .;   # 栈底（低地址）0x80410000
        . += 8192;
        _stack_top = .;      # 栈顶（高地址）0x80412000
    }
    
    /DISCARD/ : {
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}