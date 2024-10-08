#ifndef ONIX_INTERRUPT_H
#define ONIX_INTERRUPT_H

#include <onix/type.h>

#define IDT_SIZE 256

typedef struct gate_t
{
    u16 offset0;  // 段内偏移地址0-15位
    u16 selector; // 代码段选择子
    u8 reserved; // 保留不用
    u8 type : 4;
    u8 segment : 1; // 0表示系统段
    u8 DPL : 2;
    u8 present : 1;
    u16 offset1;  // 段内偏移16-31位
} _packed gate_t;

void interrupt_init();

#endif