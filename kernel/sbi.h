/*
 * SBI 接口 - 通过 OpenSBI 提供的服务
 * 
 * SBI (Supervisor Binary Interface) 是 RISC-V 标准接口
 * 允许内核调用固件提供的服务
 */

#ifndef KERNEL_SBI_H
#define KERNEL_SBI_H

#include "types.h"

/*
 * SBI 调用约定
 * a0-a7: 参数/返回值
 * a7: SBI 函数 ID
 * 
 * SBI 函数 ID 格式:
 *   bits 0-23: function ID
 *   bits 24-31: extension ID
 */

/* SBI 扩展 ID */
#define SBI_EXT_BASE       0x0
#define SBI_EXT_LEGACY     0x1
#define SBI_EXT_TIMER      0x0
#define SBI_EXT_IPI        0x1
#define SBI_EXT_RFENCE     0x2
#define SBI_EXT_HSM        0x3
#define SBI_EXT_SRST       0x53525354

/* SBI 函数 ID - Base Extension */
#define SBI_BASE_GET_SPEC_VERSION   0x0
#define SBI_BASE_GET_IMPL_ID        0x1
#define SBI_BASE_GET_IMPL_VERSION   0x2
#define SBI_BASE_PROBE_EXTENSION    0x3
#define SBI_BASE_GET_MVENDORID      0x4
#define SBI_BASE_GET_MARCHID        0x5
#define SBI_BASE_GET_MIMPID         0x6

/* SBI 函数 ID - Console */
#define SBI_CONSOLE_PUTCHAR         0x0
#define SBI_CONSOLE_GETCHAR         0x1

/* SBI 函数 ID - Timer */
#define SBI_TIMER_SET_TIMER         0x0

/* SBI 函数 ID - Shutdown */
#define SBI_SHUTDOWN                0x0

/* SBI 错误码 */
#define SBI_SUCCESS                 0
#define SBI_ERR_FAILED             -1
#define SBI_ERR_NOT_SUPPORTED      -2
#define SBI_ERR_INVALID_PARAM      -3
#define SBI_ERR_DENIED             -4
#define SBI_ERR_INVALID_ADDRESS    -5
#define SBI_ERR_ALREADY_AVAILABLE  -6

/*
 * 执行 SBI 调用
 * 
 * @fid: 函数 ID (bits 0-23: function ID, bits 24-31: extension ID)
 * @arg0-arg5: 参数
 * @return: 返回值
 */
static inline int64_t sbi_call(uint64_t fid, uint64_t arg0, 
                                 uint64_t arg1, uint64_t arg2,
                                 uint64_t arg3, uint64_t arg4,
                                 uint64_t arg5) {
    register uint64_t a0 asm("a0") = arg0;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;
    register uint64_t a4 asm("a4") = arg4;
    register uint64_t a5 asm("a5") = arg5;
    register uint64_t a6 asm("a6") = fid & 0xFFFFFF;
    register uint64_t a7 asm("a7") = (fid >> 24) & 0xFF;

    __asm__ __volatile__(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory"
    );

    return (int64_t)a0;
}

/*
 * 发送字符到控制台（通过 SBI Legacy Console）
 */
static inline void sbi_console_putchar(int ch) {
    sbi_call(SBI_EXT_LEGACY << 24 | SBI_CONSOLE_PUTCHAR, ch, 0, 0, 0, 0, 0);
}

/*
 * 从控制台读取字符（通过 SBI）
 * 返回: 读取的字符，-1 表示无数据
 */
static inline int sbi_console_getchar(void) {
    return (int)sbi_call(SBI_EXT_LEGACY << 24 | SBI_CONSOLE_GETCHAR, 0, 0, 0, 0, 0, 0);
}

/*
 * 设置定时器（通过 SBI）
 * 
 * @time: 定时器到期时间（相对于当前时间）
 */
static inline void sbi_set_timer(uint64_t time) {
    sbi_call(SBI_EXT_TIMER << 24 | SBI_TIMER_SET_TIMER, time, 0, 0, 0, 0, 0);
}

/*
 * 关闭系统（通过 SBI）
 */
static inline void sbi_shutdown(void) {
    sbi_call(SBI_EXT_BASE << 24 | SBI_SHUTDOWN, 0, 0, 0, 0, 0, 0);
}

#endif /* KERNEL_SBI_H */