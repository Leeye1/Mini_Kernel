## 链接脚本主要功能
- 定义内存布局
- 合并输入段 - 将多个目标文件合并成输出段
- 定义符号 - 暴露关键地址给C代码
- 控制对齐 - 满足硬件要求
```
. = ALIGN(4096);       # 页对齐（MMU要求）
. = ALIGN(16);         # 栈对齐（ABI要求）
. = ALIGN(8);          # 64位变量对齐
```
-  丢弃无用段 - 减小内核体积

## C语言程序组成部分
```c
// kernel.c - 内核主程序
#include <stdint.h>

// 1. 未初始化的全局变量 -> .bss
int global_uninit;
int global_uninit2[100];

// 2. 已初始化的全局变量 -> .data
int global_init = 42;
char global_str[] = "Hello Kernel";

// 3. 只读数据 -> .rodata
const int const_value = 100;
const char* const msg = "Read Only String";

// 4. 普通函数 -> .text
void normal_function(void) {
    static int static_var = 10;  // 静态局部变量 -> .data
    int local = 0;                // 栈上分配
    local++;
}

// 5. 启动代码 -> .text.boot (特殊段)
__attribute__((section(".text.boot")))
void _start(void) {
    // 启动代码必须放在最前面
    asm volatile("la sp, _stack_top");
    kmain();
}

// 6. 内联汇编也可以放在特殊段
__attribute__((section(".text.early")))
void early_init(void) {
    // 早期初始化代码
}

// 7. 对齐要求的数据
__attribute__((aligned(4096)))
char aligned_buffer[4096];  // 页对齐的缓冲区
```