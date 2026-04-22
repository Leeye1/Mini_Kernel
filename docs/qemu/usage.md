##  阅读 ELF 文件头
### opensbi
```bash
riscv64-linux-gnu-readelf -l /usr/share/qemu/opensbi-riscv64-generic-fw_dynamic.elf 2>/dev/null | head -20
```
输出：
```
Elf 文件类型为 EXEC (可执行文件)
Entry point 0x80000000
There are 6 program headers, starting at offset 64

程序头：
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  RISCV_ATTRIBUT 0x000000000002dc80 0x0000000000000000 0x0000000000000000
                 0x0000000000000057 0x0000000000000000  R      0x1
  LOAD           0x0000000000001000 0x0000000080000000 0x0000000080000000
                 0x000000000002a850 0x000000000002a850  R E    0x1000
  LOAD           0x000000000002c000 0x0000000080040000 0x0000000080040000
                 0x0000000000001c80 0x00000000000051c0  RW     0x1000
  DYNAMIC        0x000000000002da20 0x0000000080041a20 0x0000000080041a20
                 0x0000000000000110 0x0000000000000110  RW     0x8
  GNU_EH_FRAME   0x0000000000027788 0x0000000080026788 0x0000000080026788
                 0x000000000000131c 0x000000000000131c  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
```
- **Entry point**: 0x80000000（OpenSBI 固件入口地址）

### kernel
```bash
riscv64-linux-gnu-readelf -l ./kernel/kernel.elf 2>/dev/null
```
输出：
```

Elf 文件类型为 EXEC (可执行文件)
Entry point 0x80200000
There are 4 program headers, starting at offset 64

程序头：
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  RISCV_ATTRIBUT 0x0000000000001530 0x0000000000000000 0x0000000000000000
                 0x0000000000000057 0x0000000000000000  R      0x1
  LOAD           0x0000000000001000 0x0000000080200000 0x0000000080200000
                 0x0000000000000530 0x0000000000003000  RWE    0x1000
  LOAD           0x0000000000000000 0x0000000080410000 0x0000000080410000
                 0x0000000000000000 0x0000000000002000  RW     0x1000
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10

 Section to Segment mapping:
  段节...
   00     .riscv.attributes 
   01     .text .rodata .got .got.plt .bss 
   02     .stack 
   03 
```

## Run
```
lee@pop-os:~/Mini_Kernel$ make run
qemu-system-riscv64 -machine virt -m 128M -kernel kernel/kernel.elf -nographic
```
- `-machine virt`：使用 QEMU virt 平台
- `-m 128M`：分配 128MB 内存
- `-kernel kernel/kernel.elf`：加载内核 ELF 文件
- `-nographic`：禁用图形输出，使用串口输出

输出：
```
OpenSBI v1.3
   ____                    _____ ____ _____
  / __ \                  / ____|  _ \_   _|
 | |  | |_ __   ___ _ __ | (___ | |_) || |
 | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
 | |__| | |_) |  __/ | | |____) | |_) || |_
  \____/| .__/ \___|_| |_|_____/|___/_____|
        | |
        |_|

Platform Name             : riscv-virtio,qemu
Platform Features         : medeleg
Platform HART Count       : 1
Platform IPI Device       : aclint-mswi
Platform Timer Device     : aclint-mtimer @ 10000000Hz
Platform Console Device   : uart8250
Platform HSM Device       : ---
Platform PMU Device       : ---
Platform Reboot Device    : sifive_test
Platform Shutdown Device  : sifive_test
Platform Suspend Device   : ---
Platform CPPC Device      : ---
Firmware Base             : 0x80000000
Firmware Size             : 322 KB
Firmware RW Offset        : 0x40000
Firmware RW Size          : 66 KB
Firmware Heap Offset      : 0x48000
Firmware Heap Size        : 34 KB (total), 2 KB (reserved), 9 KB (used), 22 KB (free)
Firmware Scratch Size     : 4096 B (total), 760 B (used), 3336 B (free)
Runtime SBI Version       : 1.0

Domain0 Name              : root
Domain0 Boot HART         : 0
Domain0 HARTs             : 0*
Domain0 Region00          : 0x0000000002000000-0x000000000200ffff M: (I,R,W) S/U: ()
Domain0 Region01          : 0x0000000080040000-0x000000008005ffff M: (R,W) S/U: ()
Domain0 Region02          : 0x0000000080000000-0x000000008003ffff M: (R,X) S/U: ()
Domain0 Region03          : 0x0000000000000000-0xffffffffffffffff M: (R,W,X) S/U: (R,W,X)
Domain0 Next Address      : 0x0000000080200000
Domain0 Next Arg1         : 0x0000000087e00000
Domain0 Next Mode         : S-mode
Domain0 SysReset          : yes
Domain0 SysSuspend        : yes

Boot HART ID              : 0
Boot HART Domain          : root
Boot HART Priv Version    : v1.12
Boot HART Base ISA        : rv64imafdch
Boot HART ISA Extensions  : time,sstc
Boot HART PMP Count       : 16
Boot HART PMP Granularity : 4
Boot HART PMP Address Bits: 54
Boot HART MHPM Count      : 16
Boot HART MIDELEG         : 0x0000000000001666
Boot HART MEDELEG         : 0x0000000000f0b509
Hello Kernel!
=================================
   Mini Kernel - RISC-V 64-bit
   Using OpenSBI Firmware
=================================

Hello Kernel!
Kernel is running...
Loaded at 0x80200000

Calling SBI shutdown...
```