/*
 * SBI 接口 - 通过 OpenSBI 提供的服务
 * 
 * SBI (Supervisor Binary Interface) 是 RISC-V 标准接口
 * 允许 S 态（Supervisor）内核通过 ecall 指令调用 M 态（Machine）固件提供的服务
 * 
 * SBI 调用约定（SBI v0.2+，来自 RISC-V 官方规范）：
 *   a7 = Extension ID (EID)    — 扩展编号，决定调用哪个功能组
 *   a6 = Function ID (FID)     — 功能编号，决定调用该扩展中的哪个函数
 *   a0-a5 = 参数               — 传递给 SBI 函数的参数
 *   ecall                      — 触发 SBI 调用，陷入 M 态
 *   返回: a0 = 错误码, a1 = 返回值
 * 
 * 重要：所有寄存器（除 a0/a1 外）在 SBI 调用后必须保持不变
 */

#ifndef KERNEL_SBI_H
#define KERNEL_SBI_H

#include "types.h"

/* ============================================================
 * SBI 扩展 ID (EID)
 * 每个 EID 代表一组相关功能
 * ============================================================ */

/* Legacy 扩展 (EID=0x01) — 早期 SBI 版本定义的兼容接口 */
#define SBI_EXT_LEGACY     0x01

/* Base 扩展 (EID=0x10) — SBI 规范信息查询 */
#define SBI_EXT_BASE       0x10

/* Timer 扩展 (EID=0x54494D45, 即 "TIME") — 定时器设置 */
#define SBI_EXT_TIMER      0x54494D45

/* IPI 扩展 (EID=0x73504920, 即 "sPI ") — 核间中断 */
#define SBI_EXT_IPI        0x73504920

/* RFENCE 扩展 (EID=0x52464E43, 即 "RFNC") — 远程 TLB 刷新 */
#define SBI_EXT_RFENCE     0x52464E43

/* HSM 扩展 (EID=0x48534D20, 即 "HSM ") — 心跳状态管理 */
#define SBI_EXT_HSM        0x48534D20

/* System Reset 扩展 (EID=0x53525354, 即 "SRST") — 系统复位/关机 */
#define SBI_EXT_SRST       0x53525354

/* ============================================================
 * SBI 函数 ID (FID) — 按扩展分类
 * ============================================================ */

/* Legacy Console 功能 (EID=0x01) */
#define SBI_LEGACY_CONSOLE_PUTCHAR    0x0    /* 输出一个字符 */
#define SBI_LEGACY_CONSOLE_GETCHAR    0x1    /* 读取一个字符 */

/* Base 扩展功能 (EID=0x10) */
#define SBI_BASE_GET_SPEC_VERSION     0x0    /* 获取 SBI 规范版本 */
#define SBI_BASE_GET_IMPL_ID          0x1    /* 获取实现 ID */
#define SBI_BASE_GET_IMPL_VERSION     0x2    /* 获取实现版本 */
#define SBI_BASE_PROBE_EXTENSION      0x3    /* 探测是否支持某个扩展 */
#define SBI_BASE_GET_MVENDORID        0x4    /* 获取厂商 ID */
#define SBI_BASE_GET_MARCHID          0x5    /* 获取架构 ID */
#define SBI_BASE_GET_MIMPID           0x6    /* 获取实现 ID */

/* Timer 扩展功能 (EID=0x54494D45) */
#define SBI_TIMER_SET_TIMER           0x0    /* 设置定时器 */

/* System Reset 扩展功能 (EID=0x53525354) */
#define SBI_SRST_RESET                0x0    /* 系统复位 */

/* ============================================================
 * SBI 错误码
 * ============================================================ */

#define SBI_SUCCESS                 0     /* 成功 */
#define SBI_ERR_FAILED             -1     /* 失败 */
#define SBI_ERR_NOT_SUPPORTED      -2     /* 不支持 */
#define SBI_ERR_INVALID_PARAM      -3     /* 参数无效 */
#define SBI_ERR_DENIED             -4     /* 被拒绝 */
#define SBI_ERR_INVALID_ADDRESS    -5     /* 地址无效 */
#define SBI_ERR_ALREADY_AVAILABLE  -6     /* 已可用 */

/* ============================================================
 * sbi_call — 执行 SBI 调用
 * 
 * 参数：
 *   eid  — Extension ID，决定调用哪个扩展
 *   fid  — Function ID，决定调用该扩展中的哪个函数
 *   arg0-arg5 — 6 个参数
 * 
 * 返回：
 *   a0 中的值（错误码）
 * 
 * 寄存器布局（SBI v0.2+ 规范）：
 *   a0-a5 = 参数
 *   a6    = FID
 *   a7    = EID
 *   ecall = 触发调用
 * 
 * 返回值：
 *   a0 = 错误码
 *   a1 = 附加值（本函数不返回）
 * ============================================================ */
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

/* ============================================================
 * sbi_console_putchar — 通过 SBI 输出一个字符
 * 
 * 使用 Legacy Console 扩展 (EID=0x01, FID=0x00)
 * 这是 OpenSBI v1.x 中唯一支持的字符输出方式
 * （旧的 Console 扩展 EID=0x04 已在 SBI v1.0 中被移除）
 * ============================================================ */
static inline void sbi_console_putchar(int ch) {
    sbi_call(SBI_EXT_LEGACY, SBI_LEGACY_CONSOLE_PUTCHAR, ch, 0, 0, 0, 0, 0);
}

/* ============================================================
 * sbi_console_getchar — 通过 SBI 读取一个字符
 * 
 * 返回：读取的字符，-1 表示无数据
 * ============================================================ */
static inline int sbi_console_getchar(void) {
    return (int)sbi_call(SBI_EXT_LEGACY, SBI_LEGACY_CONSOLE_GETCHAR,
                         0, 0, 0, 0, 0, 0);
}

/* ============================================================
 * sbi_set_timer — 设置定时器
 * 
 * 参数：
 *   time — 定时器到期时间（绝对时间，stime 寄存器的值）
 * ============================================================ */
static inline void sbi_set_timer(uint64_t time) {
    sbi_call(SBI_EXT_TIMER, SBI_TIMER_SET_TIMER, time, 0, 0, 0, 0, 0);
}

/* ============================================================
 * sbi_shutdown — 关闭系统
 * 
 * 使用 System Reset 扩展 (EID=0x53525354 "SRST", FID=0x00)
 * reset_type: 0=关机, 1=冷重启, 2=热重启
 * reset_reason: 0=无原因
 * ============================================================ */
static inline void sbi_shutdown(void) {
    sbi_call(SBI_EXT_SRST, SBI_SRST_RESET, 0, 0, 0, 0, 0, 0);
}

#endif /* KERNEL_SBI_H */
