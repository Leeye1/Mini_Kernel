/*
 * 类型定义 - 基础数据类型
 * 提供内核中常用的类型别名
 */

#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

// 基础类型别名
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long      uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long        int64_t;

// 指针类型
typedef uint64_t           addr_t;    // 地址类型
typedef uint64_t           paddr_t;   // 物理地址
typedef uint64_t           vaddr_t;   // 虚拟地址
typedef uint64_t           uintptr_t; // 无符号指针
typedef int64_t            intptr_t;  // 有符号指针

// 大小类型
typedef uint64_t           size_t;
typedef int64_t            ssize_t;

// 状态类型
typedef int                int32;
typedef unsigned int       uint32;

// 空指针
#define NULL              ((void *)0)

// 布尔类型 (在 C 中不定义，使用整数代替)
// #define true  1
// #define false 0

#endif /* KERNEL_TYPES_H */