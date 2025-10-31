; void touch_regs(void);
global touch_regs

section .text
touch_regs:
    ; --- registros generales ---
    mov     rax, 0x1111111111111111
    mov     rbx, 0x2222222222222222
    mov     rcx, 0x3333333333333333
    mov     rdx, 0x4444444444444444

    ; base/frame
    mov     rbp, 0x5555555555555555

    ; argumentos
    mov     rdi, 0x6666666666666666
    mov     rsi, 0x7777777777777777

    ; extendidos
    mov     r8,  0x8888888888888888
    mov     r9,  0x9999999999999999
    mov     r10, 0xAAAAAAAAAAAAAAAA
    mov     r11, 0xBBBBBBBBBBBBBBBB
    mov     r12, 0xCCCCCCCCCCCCCCCC
    mov     r13, 0xDDDDDDDDDDDDDDDD
    mov     r14, 0xEEEEEEEEEEEEEEEE
    mov     r15, 0xFFFFFFFFFFFFFFFF

    ; --- RFLAGS: ejemplo, setear CF ---
    pushfq              ; guardar flags actuales
    pop     rax
    or      rax, 1      ; CF = 1
    push    rax
    popfq               ; ahora RFLAGS tiene CF=1

    ; --- DS / ES (opcional) ---
    ; ojo: solo si tu GDT tiene estos selectores válidos
    ; mov     ax, 0x10
    ; mov     ds, ax
    ; mov     es, ax

    ; NO tocamos RSP ni RIP ni CS/SS
    ret
