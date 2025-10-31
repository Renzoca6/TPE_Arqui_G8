SECTION .text
GLOBAL sys_resize
GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_clearwindow
GLOBAL sys_date_time
GLOBAL sys_benchmark
GLOBAL sys_write_at_vram
GLOBAL sys_write_at_back
GLOBAL sys_get_screen_info
GLOBAL sys_present_fullframe
GLOBAL sys_print_registers
global sys_getchar
global sys_putPixel
global sys_get_ms_since_boot

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

sys_date_time:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 3          ; id time
    mov  rbx, rdi        ; time 0 date 1 -> RBX
    int  80h             
    
    pop  rbx
    
    leave
    ret

sys_resize:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 4          ; id resize
    mov  rbx, rdi        ; N_Times -> RBX
    int  80h             
    
    pop  rbx
    
    leave
    ret

sys_benchmark:
    push rbp
    mov  rbp, rsp
    push rbx

    mov  rax, 5        ; syscall ID = 5
    mov  rbx, rdi      ; which: 0=fps, 1=floating, 2=vram
    int  80h

    ; Al volver del int 80h:
    ; RAX ya trae el resultado uint64_t del kernel.
    ; No hay que tocar xmm0 ni nada.

    pop  rbx
    leave
    ret
    
sys_write_at_vram:
    push rbp
    mov  rbp, rsp

    push rbx
    push r9           ; vamos a usar r9 como temporal

    mov  rax, 6       ; syscall id
    mov  r9, rcx          ; r9 = fColor
    mov  rbx, rdi         ; regs->rbx = str
    mov  rcx, rsi         ; regs->rcx = col
    mov  rsi, r9          ; regs->rsi = fColor
    mov  rdi, r8          ; regs->rdi = bgColor

    int  80h

    pop  r9
    pop  rbx
    leave
    ret
sys_write_at_back:
    push rbp
    mov  rbp, rsp

    push rbx
    push r9           ; vamos a usar r9 como temporal

    mov  rax, 7       ; syscall id
    mov  r9, rcx          ; r9 = fColor
    mov  rbx, rdi         ; regs->rbx = str
    mov  rcx, rsi         ; regs->rcx = col
    mov  rsi, r9          ; regs->rsi = fColor
    mov  rdi, r8          ; regs->rdi = bgColor

    int  80h

    pop  r9
    pop  rbx
    leave
    ret
    ret
sys_get_screen_info:
    push rbp
    mov  rbp, rsp
    push rbx

    mov  rax, 9       ; syscall id
    mov  rbx, rdi     ; which: 0=height, 1=width
    int  80h

    ; al volver, RAX YA TIENE el valor
    pop  rbx
    leave
    ret

sys_present_fullframe:
    push rbp
    mov  rbp, rsp

    mov  rax, 8     ; ID = 8 -> present_fullframe
    int  80h

    leave
    ret

sys_print_registers:
    push rbp
    mov  rbp, rsp

    mov  rax, 10      ; ID = 10 -> registers
    int  80h

    leave
    ret

sys_getchar:
    push rbp
    mov rbp, rsp

    mov rax, 11          ; ID de la syscall getchar
    int 80h             ; entra al kernel
    ; el kernel deja el char en RAX
    leave
    ret

sys_putPixel:
    push rbp
    mov rbp, rsp

    mov rax, 12         ;ID sycall
    mov rbx, rdi        ;color
    mov rcx, rsi        ;X
    mov rdx, rdx        ;y
    mov rsi, rcx        ;target

    int  80h

    leave
    ret
sys_get_ms_since_boot:
    push rbp
    mov  rbp, rsp

    mov  rax, 13       ; nuevo ID
    int  80h

    leave
    ret
