; void touch_regs(void);
global touch_regs

section .text
touch_regs:
    ; Setear solo algunos registros callee-saved y extendidos
    ; que no se modifican fácilmente por el retorno
    
    mov     r12, 0xCCCCCCCCCCCCCCCC
    mov     r13, 0xDDDDDDDDDDDDDDDD
    mov     r14, 0xEEEEEEEEEEEEEEEE
    mov     r15, 0xFFFFFFFFFFFFFFFF
    mov     rbx, 0x2222222222222222
    
    ; Algunos registros que probablemente se mantengan
    mov     r8,  0x8888888888888888
    mov     r9,  0x9999999999999999
    mov     r10, 0xAAAAAAAAAAAAAAAA
    mov     r11, 0xBBBBBBBBBBBBBBBB
    
    ; Nota: RAX, RDI, RSI, RCX, RDX pueden cambiar al retornar
    ; pero R12-R15, RBX son callee-saved y deberían preservarse
    
    ret
