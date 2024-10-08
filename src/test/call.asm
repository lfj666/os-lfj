[bits 32]

extern exit

test:
    push $
    ret
global main

main:
    ; push 5
    ; push eax

    ; pop ebx
    ; pop ecx

    ; push 0

    ; pusha

    ; popa  ; 弹出时会忽略esp

    call test

    call exit  ;