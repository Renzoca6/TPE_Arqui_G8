SECTION .text
GLOBAL sys_write
GLOBAL sys_read

sys_read:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 0          ; número de syscall: read
    mov  rbx, rdi        ;
    int  80h             ; int 0x80

    pop  rbx

    mov  rsp, rbp
    pop  rbp
    ret


sys_write:
    push rbp
    mov  rbp, rsp
    push rax
    push rbx
    push rcx

    mov  rax, 1          ; número de syscall: write
    mov  rbx, rdi        ; fd
    mov  rcx, rsi        ; buffer
    int  80h             ; int 0x80

    pop  rcx
    pop  rbx
    pop  rax

    mov  rsp, rbp
    pop  rbp
    ret

