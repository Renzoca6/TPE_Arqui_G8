SECTION .text
GLOBAL sys_write
GLOBAL sys_read

sys_read:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 0          ; id read
    mov  rbx, rdi        ; dst -> RBX
    int  80h             ; RAX
    
    pop  rbx
    
    leave
    ret

sys_write:
    push rbp
    mov  rbp, rsp
    
    push rbx
    push rcx
    
    mov  rax, 1          ; id write(1)
    mov  rbx, rdi        ; fd (STDOUT)
    mov  rcx, rsi        ; puntero al buffer
    int  80h
    
    pop  rcx
    pop  rbx
    
    leave
    ret    

