# 内核 API 参考

## 阶段 1 可用 API

### UART 接口

```c
// 初始化 UART
void uart_init(void);

// 发送一个字符
void uart_putc(char c);

// 接收一个字符，无数据返回 -1
int uart_getc(void);

// 发送字符串
void puts(char *s);
```

### 示例

```c
uart_init();           // 初始化
uart_putc('A');        // 发送字符
puts("Hello\n");       // 发送字符串
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

## 地址常量

| 常量 | 值 | 说明 |
|------|-----|------|
| UART_BASE | 0x10000000 | UART 基地址 |
| KERNEL_BASE | 0x80000000 | 内核加载地址 |

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