#include <stdio.h>
#include <onix/type.h>

typedef struct descriptor // 8字节
{
    u16 limit_low; // 段界限0-15位
    u32 base_low:24; // 基地址0-23位
    u8 type:4; // 段类型
    u8 segment:1; // 1为代码段或数据段，0表示系统段
    u8 DPL:2; // 描述符特权等级
    u8 present:1; // 是在在内存中
    u8 limit_high:4; // 段界限16-19位
    u8 avaliable:1; // 不用了
    u8 long_mode:1; // 64位扩展标识
    u8 big:1; // 32位还是16位
    u8 granularity:1; // 粒度4kB或1B
    u8 base_high; // 基地址24-31位
}_packed descriptor;

int main()
{
    printf("size of u8 %d\n",sizeof(u8));
    printf("size of u16 %d\n",sizeof(u16));
    printf("size of u32 %d\n",sizeof(u32));
    printf("size of u64 %d\n",sizeof(u64));
    printf("size of int8 %d\n",sizeof(int8));
    printf("size of int16 %d\n",sizeof(int16));
    printf("size of int32 %d\n",sizeof(int32));
    printf("size of descroptor %d\n",sizeof(descriptor));
    return 0;
}