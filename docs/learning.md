学习顺序建议：
1. docs/00-overview.md     → 整体架构和阶段计划
        ↓
2. docs/01-boot.md         → 启动流程（链接脚本 → 汇编 → C）
        ↓
3. kernel/linker.ld        → 内存布局（对照文档理解）
        ↓
4. kernel/start.S          → 启动汇编（设置栈、跳转）
        ↓
5. kernel/sbi.h            → SBI 接口（调用约定、Legacy Console 扩展）
        ↓
6. kernel/main.c           → 内核入口（SBI console 输出）
        ↓
7. Makefile                → 构建流程