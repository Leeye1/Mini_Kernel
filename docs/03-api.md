# 内核 API 参考

## 阶段 1 可用 API

### SBI 调用约定

SBI v0.2+ 使用以下寄存器约定：
- `a0-a5`：参数（调用时传入，返回时 a0 为返回值）
- `a6`：Function ID (FID)
- `a7`：Extension ID (EID)
- `ecall`：触发 SBI 调用

**重要**：OpenSBI v1.3 运行时 SBI 版本为 1.0，已移除旧的 Console 扩展（EID=0x4）。
必须使用 **Legacy Console 扩展**（EID=0x1）才能正常输出字符。

### SBI Console 接口

```c
// 发送一个字符到控制台（使用 Legacy Console 扩展 EID=0x1, FID=0x0）
void sbi_console_putchar(int ch);

// 从控制台接收一个字符，无数据返回 -1
int sbi_console_getchar(void);
```

### 辅助函数

```c
// 发送字符（处理换行符自动转换为 \r\n）
static void putchar_console(char c);

// 发送字符串
static void puts_console(char *s);
```

### 示例

```c
putchar_console('A');              // 发送字符
puts_console("Hello\n");           // 发送字符串
int ch = sbi_console_getchar();    // 接收字符
```

## 类型定义

### 基本类型

```c
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long      uint64_t;
typedef signed char        int8_t;
typedef signed long        int64_t;

typedef uint64_t           addr_t;    // 地址类型
typedef uint64_t           paddr_t;   // 物理地址
typedef uint64_t           vaddr_t;   // 虚拟地址

typedef uint64_t           size_t;
```

### 布尔类型

```c
typedef int bool;
#define true  1
#define false 0
```

## 下阶段 API

### 内存管理（阶段 2）

```c
void *kmalloc(size_t size);   // 分配内存
void kfree(void *ptr);        // 释放内存
void *alloc_page(void);       // 分配物理页
void free_page(void *addr);   // 释放物理页
```

### 进程管理（阶段 3）

```c
int fork(void);               // 创建进程
int exec(char *path);         // 执行程序
int wait(int *status);        // 等待子进程
void exit(int status);        // 退出进程
int getpid(void);             // 获取当前进程 ID
```

### 文件系统（阶段 4）

```c
int open(char *path, int flags);  // 打开文件
int read(int fd, void *buf, int n);   // 读取
int write(int fd, void *buf, int n);  // 写入
int close(int fd);             // 关闭文件
```