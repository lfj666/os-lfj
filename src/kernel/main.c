#include <onix/onix.h>
#include <onix/type.h>
#include <onix/io.h>
#include <onix/string.h>
#include <onix/console.h>
#include <onix/stdarg.h>
#include <onix/printk.h>
#include <onix/assert.h>
#include <onix/debug.h>
#include <onix/task.h>

void test_args(int cnt, ...)
{
    va_list args;
    va_start(args,cnt); // 把第一个参数的地址赋给args

    int arg;
    while (cnt--)
    {
        arg = va_arg(args,int);
    }
    va_end(args);
    
}


void kernel_init()
{
    console_init();

    //gdt_init();

    task_init();

    return;
    
}