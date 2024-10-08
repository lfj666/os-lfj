[bits 32]

section .text ; 代码段

global inb ; 将inb导出

inb:
    push ebp
    mov ebp,esp ; 保存栈帧

    xor eax,eax
    mov edx,[ebp+8] ; port
    in al,dx ; 将端口号dl的8bit输入到al

    jmp $+2
    jmp $+2
    jmp $+2

    leave ; 恢复桟帧
    ret


global outb
outb:
    push ebp
    mov ebp,esp

    mov edx,[ebp+8]  ; port
    mov eax,[ebp+12] ; value
    out dx,al

    jmp $+2
    jmp $+2
    jmp $+2

    leave
    ret


global inw
inw:
    push ebp
    mov ebp,esp

    mov edx,[ebp+8]
    in ax,dx

    jmp $+2
    jmp $+2
    jmp $+2

    leave
    ret

global outw
outw:
    push ebp
    mov ebp,esp

    mov edx,[ebp+8]  ; port
    mov eax,[ebp+12] ; value
    out dx,ax

    jmp $+2
    jmp $+2
    jmp $+2
    
    leave
    ret