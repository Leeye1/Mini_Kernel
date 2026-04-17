/*
 * 链接脚本 - 定义内核加载地址和入口点
 * RISC-V 64-bit 内核
 * 加载地址: 0x80200000 (避开 OpenSBI 在 0x80000000-0x80400000)
 */

OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_start)

/*
 * 内核虚拟地址空间布局
 * 0x80200000 开始，内核代码区域
 */
SECTIONS {
    /* 代码段 - 从 0x80200000 开始 */
    . = 0x80200000;
    
    .text : {
        _start = .;
        *(.text.boot)   /* 启动代码优先 */
        *(.text)        /* 其他代码 */
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
        _stack_top = .;
        . += 8192;      /* 8KB 栈空间 */
    }
    
    /DISCARD/ : {
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}