SECTION .text
GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_clearwindow

sys_read:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 0          ; id read
    mov  rbx, rdi        ; BUF -> RBX
    int  80h             
    
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
    mov  rcx, rsi        ; buffer
    int  80h
    
    pop  rcx
    pop  rbx
    
    leave
    ret    

sys_clearwindow:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 2          ; id read
    mov  rbx, rdi        ; color -> RBX
    int  80h             
    
    pop  rbx
    
    leave
    ret
