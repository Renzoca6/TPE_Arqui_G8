GLOBAL throw_invalid_opcode
GLOBAL throw_zero_division

SECTION .text

throw_zero_division:
    mov rax, 0
    div rax
    ret

throw_invalid_opcode:
    ud2
    ret
