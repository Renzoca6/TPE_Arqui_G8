GLOBAL syscall
GLOBAL throw_invalid_opcode
GLOBAL throw_zero_division

SECTION .text

syscall:
    push rbx
    mov rax, rdi
    mov rbx, rsi
    int 80h
    pop rbx
    ret
    
throw_zero_division:
    mov rax, 0
    div rax
    ret

throw_invalid_opcode:
    ud2
    ret
