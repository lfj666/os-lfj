[org 0x7c00]

; 设置屏幕模式为文本模式
mov ax,3
int 0x10

; 初始化段寄存器
mov ax,0
mov ds,ax
mov es,ax
mov ss,ax
mov fs,ax
mov sp,0x7c00


mov si, booting
call print


mov edi,0x1000 ;读取代目标内存
mov ecx,2 ; 起始扇区
mov bl,4 ; 扇区数量
call read_disk

cmp word [0x1000],0x55aa
jnz error  ; 魔数错误

xchg bx,bx

jmp 0:0x1002 ; 长跳转到loader中


jmp $

error:
    mov si,.msg
    call print
    hlt
    jmp $
    .msg db "Booting Error!!!",13,10,0


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


write_disk:

    ; out指令只能为dx和ax,al
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
    mov al,0x30 ;写硬盘
    out dx,al

    xor ecx,ecx
    mov ecx,0
    mov cl,bl ; 得到读写扇区数
    .write:
        push cx ; 读一个扇区时用到了cx,先保存其中数据
        call .writes
        call .waits ; 等到硬盘繁忙结束
        pop cx
        loop .write
    ret

    .waits:
        mov dx,0x1f7
        .check:
            in al,dx
            jmp $+2 ; 直接跳转到下一行
            jmp $+2
            jmp $+2
            and al,0b1000_0000
            cmp al,0b0000_0000
            jnz .check
        ret
    .writes:
        mov dx,0x1f0 ; 硬盘读写数据端口,16bit
        mov cx,256 ; 一个扇区256个字
        .writew:
            mov ax,[edi]
            out dx,ax
            jmp $+2 ; 一点延迟
            jmp $+2
            jmp $+2
            add edi,2
            loop .writew
        ret


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


booting:
    db "Booting Onix...",13,10,0


; 填充零
times 510-($-$$) db 0


; 主引导扇区最后两个字节必须是55aa
db 0x55,0xaa
