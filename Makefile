# Mini Kernel Makefile
# RISC-V 64-bit 最小内核构建脚本

# 工具链
CC = riscv64-linux-gnu-gcc
LD = riscv64-linux-gnu-ld
OBJCOPY = riscv64-linux-gnu-objcopy
QEMU = qemu-system-riscv64

# 编译选项
# -nostdinc: 不使用标准库
# -fno-builtin: 不使用内置函数（避免与标准库冲突）
# -fno-pie: 不生成位置无关代码
# -fpic: 生成位置无关代码（解决重定位问题）
# -march=rv64gc: RISC-V 64-bit GC 扩展（IMACFD）
# -mabi=lp64: 64-bit ABI
CFLAGS = -nostdinc -nostdlib -fno-builtin -fno-pie -fpic \
         -march=rv64gc -mabi=lp64 \
         -Wall -Wextra -g \
         -Ikernel

# 链接选项
# -nostdlib: 不使用标准库
# -Tkernel/linker.ld: 使用自定义链接脚本
# --no-relax: 禁用链接放松优化
LDFLAGS = -nostdlib -Tkernel/linker.ld --no-relax

# 内核目标文件
KERNEL_OBJS = kernel/start.o kernel/main.o
KERNEL = kernel/kernel.elf

# 默认目标
all: $(KERNEL)

# 构建内核
$(KERNEL): $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

# 编译规则
kernel/start.o: kernel/start.S
	$(CC) $(CFLAGS) -c -o $@ $<

kernel/main.o: kernel/main.c kernel/types.h kernel/sbi.h
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理
clean:
	rm -f kernel/*.o kernel/kernel.elf kernel/disasm.txt kernel/syms.txt

# 运行（QEMU）
# 使用默认的 OpenSBI 固件
run: $(KERNEL)
	$(QEMU) -machine virt -m 128M -kernel $(KERNEL) -nographic

# 运行并调试
debug: $(KERNEL)
	$(QEMU) -machine virt -m 128M -kernel $(KERNEL) -nographic -s -S &

# 反汇编
disasm: $(KERNEL)
	riscv64-linux-gnu-objdump -d $(KERNEL) > kernel/disasm.txt
	@echo "Disassembly saved to kernel/disasm.txt"

# 查看内核符号
syms: $(KERNEL)
	riscv64-linux-gnu-nm -n $(KERNEL) > kernel/syms.txt
	@echo "Symbols saved to kernel/syms.txt"

.PHONY: all clean run debug disasm syms