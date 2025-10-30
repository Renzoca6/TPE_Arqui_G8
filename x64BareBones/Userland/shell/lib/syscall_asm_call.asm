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

    ; Al volver, RAX = bits IEEE754 del double
    ; Mover bits a XMM0 para retornar double según SysV ABI
    movq xmm0, rax

    pop  rbx
    leave
    ret
    
sys_write_at_vram:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 6          ; ID = 6 -> write_at_VRAM

    mov  rbx, rdi        ; RBX = str
    mov  rcx, rsi        ; RCX = col
    mov  rsi, rcx        ; RSI = fColor
    mov  rdi, r8         ; RDI = bgColor

    int  80h

    pop  rbx
    leave
    ret

sys_write_at_back:
    push rbp
    mov  rbp, rsp

    push rbx

    mov  rax, 7          ; ID = 7 -> write_at_BACK

    mov  rbx, rdi        ; RBX = str
    mov  rcx, rsi        ; RCX = col
    mov  rsi, rcx        ; RSI = fColor
    mov  rdi, r8         ; RDI = bgColor

    int  80h

    pop  rbx
    leave
    ret
sys_get_screen_info:
    push rbp
    mov  rbp, rsp

    push rbx
    push rcx

    mov  rax, 8      ; ID = 8 -> getScreen_Info
    int  80h         ; al volver: RBX = height, RCX = width

    ; guardar en los punteros que nos pasó el caller
    mov  [rdi], rbx  ; *height = RBX
    mov  [rsi], rcx  ; *width  = RCX

    pop  rcx
    pop  rbx

    leave
    ret    
sys_present_fullframe:
    push rbp
    mov  rbp, rsp

    mov  rax, 9      ; ID = 9 -> present_fullframe
    int  80h

    leave
    ret