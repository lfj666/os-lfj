global task_switch
task_switch:
    push ebp
    mov ebp,esp

    push ebx
    push esi
    push edi

    mov eax,esp
    and eax,0xfffff000

    mov [eax],esp

    mov eax,[ebp + 8]
    mov esp,[eax] ;堆栈转换

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret ;将[esp]->eip