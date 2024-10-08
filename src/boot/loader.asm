[org 0x1000]

dw 0x55aa ; 魔术，用于判断错误

; 打印"Loading Onix..."
mov si,loading
call print


detecting_memory:
    xor ebx,ebx ; ebx置零

    mov ax,0
    mov es,ax
    mov di,ards_buffer ; 结构体缓存位置

    mov edx,0x534d4150 ; 固定签名
.next:
    mov eax,0xe820 ; 子功能号
    mov ecx,20 ; ARDS结构体大小

    int 0x15 ; 系统调用

    jc error ; CF置位则出错

    add di,cx ; 缓存指针指向下一个结构体
    inc word [ards_count] ; 结构体个数加一

    cmp ebx,0 ; 判断是否检测结束
    jnz .next ; 

    mov si,detecting
    call print

    jmp prepare_protected_mode

error:
    mov si,.msg
    call print
    hlt
    jmp $
    .msg db "Loading Error!!!",13,10,0

print:
    mov ah,0x0e
.next:
    mov al,[si]
    cmp al,0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

loading:
    db "Loading Onit...",13,10,0

detecting:
    db "Detect Memory Success!!!",13,10,0


prepare_protected_mode:
    
    cli ; 关闭中断

    ; 打开 A20 线
    in al,  0x92
    or al, 0b10
    out 0x92, al

    ; 加载GDT
    lgdt [gdt_ptr]

    ; 启动保护模式
    mov eax,cr0
    or eax,1
    mov cr0,eax

    ; 用跳转来刷新缓存，启用保护模式
    jmp dword code_selector:protect_mode


[bits 32]
protect_mode:
    ; 初始化段寄存器
    mov ax,data_selector
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    mov esp,0x10000 ; 修改桟顶

    ; 加载系统
    mov edi,0x10000
    mov ecx,10
    mov bl,200
    call read_disk

    jmp dword code_selector:0x10000


jmp $

read_disk:

    ; out指令只能为dx和al
    ; 设置读写扇区数
    mov dx,0x1f2
    mov al,bl
    out dx,al

    inc dx ; 0x1f3 起始扇区的0-7位
    mov al,cl
    out dx,al

    inc dx ; 0x1f4 起始扇区的8-15位
    shr ecx,8
    mov al,cl
    out dx,al

    inc dx ; 0x1f5 起始扇区的16-23位
    shr ecx,8
    mov al,cl
    out dx,al

    inc dx ; 0x1f6
    shr ecx,8
    and cl,0b1111
    mov al,0b1110_0000
    or al,cl
    out dx,al    

    inc dx ; 0x1f7
    mov al,0x20
    out dx,al

    xor ecx,ecx
    mov ecx,0
    mov cl,bl ; 得到读写扇区数
    .read:
        push cx ; 读一个扇区时用到了cx,先保存其中数据
        call .waits ; 等到数据准备完毕
        call .reads
        pop cx
        loop .read
    ret

    .waits:
        mov dx,0x1f7
        .check:
            in al,dx
            jmp $+2 ; 直接跳转到下一行
            jmp $+2
            jmp $+2
            and al,0b1000_1000
            cmp al,0b0000_1000
            jnz .check
        ret
    .reads:
        mov dx,0x1f0 ; 硬盘读写数据端口,16bit
        mov cx,256 ; 一个扇区256个字
        .readw:
            in ax,dx
            jmp $+2 ; 一点延迟
            jmp $+2
            jmp $+2
            mov [edi],ax
            add edi,2
            loop .readw
        ret


code_selector equ (1<<3) 
data_selector equ (2<<3) 


gdt_ptr:
    dw (gdt_end - gdt_base) - 1
    dd gdt_base

gdt_base:
    dd 0, 0; NULL 描述符
gdt_code:
    dw memory_limit & 0xffff; 段界限 0 ~ 15 位
    dw memory_base & 0xffff; 基地址 0 ~ 15 位
    db (memory_base >> 16) & 0xff; 基地址 16 ~ 23 位
    ; 存在 - dlp 0 - S _ 代码 - 非依从 - 可读 - 没有被访问过
    db 0b_1_00_1_1_0_1_0;
    ; 4k - 32 位 - 不是 64 位 - 段界限 16 ~ 19
    db 0b1_1_0_0_0000 | (memory_limit >> 16) & 0xf;
    db (memory_base >> 24) & 0xff; 基地址 24 ~ 31 位
gdt_data:
    dw memory_limit & 0xffff; 段界限 0 ~ 15 位
    dw memory_base & 0xffff; 基地址 0 ~ 15 位
    db (memory_base >> 16) & 0xff; 基地址 16 ~ 23 位
    ; 存在 - dlp 0 - S _ 数据 - 向上 - 可写 - 没有被访问过
    db 0b_1_00_1_0_0_1_0;
    ; 4k - 32 位 - 不是 64 位 - 段界限 16 ~ 19
    db 0b1_1_0_0_0000 | (memory_limit >> 16) & 0xf;
    db (memory_base >> 24) & 0xff; 基地址 24 ~ 31 位
gdt_end:

memory_base equ 0 ; 内存开始的位置

; 内存界限4G/4K-1
memory_limit equ ((1024*1024*1024*4)/(4*1024))-1

ards_count:
    dw 0
ards_buffer:


