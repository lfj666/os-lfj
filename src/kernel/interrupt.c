#include <onix/interrupt.h>
#include <onix/global.h>
#include <onix/printk.h>
#include <onix/debug.h>
#include <onix/io.h>
#include <onix/stdlib.h>
#include <onix/assert.h>

#define ENTRY_SIZE 0x30
#define LOGK(fmt, args...) DEBUGK(fmt, ##args)
// #define LOGK(fmt, args...)

#define PIC_M_CTRL 0x20 // 主片的控制端口
#define PIC_M_DATA 0x21 // 主片的数据端口
#define PIC_S_CTRL 0xa0 // 从片的控制端口
#define PIC_S_DATA 0xa1 // 从片的数据端口
#define PIC_EOI 0x20    // 通知中断控制器中断结束


gate_t idt[IDT_SIZE];
pointer_t idt_ptr;

handler_t handler_table[IDT_SIZE];
extern handler_t handler_entry_table[ENTRY_SIZE];

static char *messages[] = {
    "#DE Divide Error\0",
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};

gate_t idt[IDT_SIZE];
pointer_t idt_ptr;

extern void interrupt_handler();

void send_eoi(int vector)
{
    if (vector >= 0x20 && vector < 0x28)
    {
        outb(PIC_M_CTRL,PIC_EOI);
    }
    else if (vector >= 0x28 && vector < 0x30)
    {
        outb(PIC_M_CTRL,PIC_EOI);
        outb(PIC_S_CTRL,PIC_EOI);
    } 
}

void exception_handler(
    int vector,
    u32 edi, u32 esi, u32 ebp, u32 esp,
    u32 ebx, u32 edx, u32 ecx, u32 eax,
    u32 gs, u32 fs, u32 es, u32 ds,
    u32 vector0, u32 error, u32 eip, u32 cs, u32 eflags)
{
    char *message = NULL;
    if (vector < 22)
    {
        message = messages[vector];
    }
    else
    {
        message = messages[15];
    }

    printk("\nEXCEPTION : %s \n", message);
    printk("   VECTOR : 0x%02X\n", vector);
    printk("    ERROR : 0x%08X\n", error);
    printk("   EFLAGS : 0x%08X\n", eflags);
    printk("       CS : 0x%02X\n", cs);
    printk("      EIP : 0x%08X\n", eip);
    printk("      ESP : 0x%08X\n", esp);
    hang();
}


u32 counter = 0;

void default_handler(int vector)
{
    send_eoi(vector); // 向中断控制器发送中断结束信息
    LOGK("[0x%x] default interrupt called %d...\n",vector,counter++);
}

void idt_init()
{
    for (size_t i = 0; i < ENTRY_SIZE; i++)
    {
        gate_t *gate = &idt[i];
        gate->offset0 = (u32)handler_entry_table[i] & 0xffff;
        gate->offset1 = ((u32)handler_entry_table[i] >> 16) & 0xffff;
        gate->selector = 1 << 3;
        gate->reserved = 0;
        gate->type = 0b1110;
        gate->segment = 0;
        gate->DPL = 0;
        gate->present = 1;
    }
    
    // 异常中断函数处理函数 
    for (size_t i = 0; i < 0x20; i++)
    {
        handler_table[i] = exception_handler;
    }
    // 外中断处理函数地址
    for (size_t i = 0x20; i < ENTRY_SIZE; i++)
    {
        handler_table[i] = default_handler;
    }

    idt_ptr.base = (u32)idt;
    idt_ptr.limit = sizeof(idt) - 1;
    
    asm volatile("lidt idt_ptr");
}

// 初始化中断控制器
void pic_init()
{
    outb(PIC_M_CTRL,0b00010001); // ICW1：边沿触发，级联8259，需要ICW4
    outb(PIC_M_DATA,0x20);       // ICW2：起始中断向量号0x20
    outb(PIC_M_DATA,0b00000100); // ICW3：IR2接从片
    outb(PIC_M_DATA,0b00000001); // ICW4：8086模式，正常EOI

    outb(PIC_S_CTRL,0b00010001);
    outb(PIC_S_DATA,0x28); // 起始中断向量号
    outb(PIC_S_DATA,2);
    outb(PIC_S_DATA,0b00000001);

    outb(PIC_M_DATA,0b11111111); // 关闭主片所有中断
    outb(PIC_S_DATA,0b11111111); // 关闭从片所有中断
}

// 在handler_table中注册中断处理函数
void set_interrupt_handler(u32 irq, handler_t handler)
{
    assert(irq >= 0 && irq < 16);
    handler_table[IRQ_MASTER_NR + irq] = handler;
}


    
void set_interrupt_mask(u32 irq, bool enable)
{
    assert(irq >= 0 && irq < 16);
    u16 port;
    if (irq < 8)
    {
        port = PIC_M_DATA;
    }
    else
    {
        port = PIC_S_DATA;
        irq -= 8;
    }
    if (enable)
    {
        outb(port, inb(port) & ~(1 << irq));
    }
    else
    {
        outb(port, inb(port) | (1 << irq));
    }
}

// 中断初始化
void interrupt_init()
{
    pic_init();
    idt_init();
}