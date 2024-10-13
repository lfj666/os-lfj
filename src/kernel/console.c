#include <onix/type.h>
#include <onix/console.h>
#include <onix/string.h>

#define CRT_ADDR_REG 0x3d4
#define CRT_DATA_REG 0x3d5
#define CRT_CURSOR_H 0xe
#define CRT_CURSOR_L 0xf
#define CRT_START_ADDR_H 0xc // 显存中已字符为单位的开始显示位置高位
#define CRT_START_ADDR_L 0xd // 

#define MEM_BASE 0xb8000  // 显卡内存开始位置
#define MEM_SIZE 0x4000  // 显卡内存大小
#define MEM_END (MEM_BASE+MEM_SIZE)  // 显卡内存结束位置
#define WIDTH 80 // 屏幕文本列数
#define HEIGHT 25 // 屏幕文本行数
#define ROW_SIZE (WIDTH * 2) // 每行字节数
#define SCR_SIZE (ROW_SIZE * HEIGHT) // 屏幕字节数

#define ASCII_NUL 0x00
#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07
#define ASCII_BS 0x08  // \a
#define ASCII_HT 0x09  // \b
#define ASCII_LF 0x0a // \t
#define ASCII_VT 0x0b // \n
#define ASCII_FF 0x0c // \v
#define ASCII_CR 0x0d // \f
#define ASCII_DEL 0x7f // \r

static u32 screen; // 记录当前显示器开始的内存位置
static u32 pos; // 记录光标的内存位置
static u32 x,y; // 当前光标的位置x和y轴坐标，由pos计算而来

static u8 attr = 7; // 字符样式
static u16 erase = 0x0720; // 空格

// 将
static void get_screen()
{
    outb(CRT_ADDR_REG,CRT_START_ADDR_H); // 开始内存高位
    screen = inb(CRT_DATA_REG) << 8;
    outb(CRT_ADDR_REG,CRT_START_ADDR_L); // 开始内存低位
    screen |= inb(CRT_DATA_REG);

    screen <<= 1; // 字符数*2=字节数
    screen += MEM_BASE;

}

static void set_screen()
{
    outb(CRT_ADDR_REG,CRT_START_ADDR_H);
    outb(CRT_DATA_REG,(screen - MEM_BASE) >> 9 & 0xff);
    outb(CRT_ADDR_REG,CRT_START_ADDR_L);
    outb(CRT_DATA_REG,(screen - MEM_BASE) >> 1 & 0xff);
}

static void get_cursor()
{
    outb(CRT_ADDR_REG,CRT_CURSOR_H);
    pos = inb(CRT_DATA_REG) << 8;
    outb(CRT_ADDR_REG,CRT_CURSOR_L);
    pos |= inb(CRT_DATA_REG);

    get_screen();

    pos <<= 1;
    pos += MEM_BASE;

    u32 delta = (pos - screen) >> 1;
    x = delta % WIDTH; // 光标横轴
    y = delta / WIDTH; // 光标竖轴
}

static void set_cursor()
{
    outb(CRT_ADDR_REG,CRT_CURSOR_H);
    outb(CRT_DATA_REG,(pos - MEM_BASE) >> 9 & 0xff);
    outb(CRT_ADDR_REG,CRT_CURSOR_L);
    outb(CRT_DATA_REG,(pos - MEM_BASE) >> 1 & 0xff);
}

void console_init()
{
    console_clear();

}


void console_clear()
{
    screen = MEM_BASE;
    pos = MEM_BASE;
    x = y = 0;
    set_cursor();
    set_screen();

    u16 *ptr = (u16 *)MEM_BASE;
    while (ptr < (u16 *)MEM_END)
        *ptr++ = erase;    

}
static void command_bs()
{
    if (x)
    {
        x--;
        pos -= 2;
        *(u16 *)pos = erase;
    }
}
static void command_del()
{
    *(u16 *)pos = erase;
}

static void command_cr()
{
    pos -= (x << 1);
    x = 0;
}

static void scroll_up()
{
    if (screen + SCR_SIZE + ROW_SIZE > MEM_END)
    {
        memcpy((void *)MEM_BASE,(const void *)screen,SCR_SIZE);
        pos -= (screen - MEM_BASE);
        screen = MEM_BASE;
    }
    u32 *ptr = (u32 *)(screen + SCR_SIZE);
    for (size_t i = 0; i < WIDTH; i++)
        *ptr++ = erase;        
    screen += ROW_SIZE;
    pos += ROW_SIZE;
    
    // 刷新屏幕
    set_screen();
}

static void command_lf() // \n
{
    if (y+1 < HEIGHT)
    {
        y++;
        pos += ROW_SIZE;
        return;
    }
    scroll_up();
}


extern void start_beep();

// 将buf中的内容写到控制台中
void console_write(char *buf,u32 count)
{
    char ch;
    char *ptr = pos; // 当前光标指向位置的内存地址
    while (count--)
    {
        ch = *buf++;
        switch (ch)
        {
        case ASCII_NUL:
            break;
        case ASCII_ENQ:
            break;
        case ASCII_BEL: // \a
            start_beep();
            break;
        case ASCII_BS:
            command_bs();
            break;
        case ASCII_HT:
            break;
        case ASCII_LF:
            command_lf();
            command_cr();
            break;
        case ASCII_VT:
            break;
        case ASCII_FF:
            command_lf();
            break;
        case ASCII_CR: 
            command_cr();
            break;
        case ASCII_DEL:
            command_del();
            break;
        default:
            if (x >= WIDTH)
            {
                // x = 0;
                // pos -= ROW_SIZE;
                // command_lf();
                x = 0;
                if (++y >= HEIGHT)
                    scroll_up();                

            }
            *((char *)pos) = ch;
            pos++;
            *((char *)pos) = attr;
            pos++;


            x++;
            break;
        }
        set_cursor();
    }
}