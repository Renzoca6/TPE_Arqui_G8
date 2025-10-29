default rel

global _exception0Handler
global _exception6Handler

extern regs_save
extern exceptionDispatcher

; push/pop macros (mismo orden que pushState en interrupts.asm)
%macro pushState 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popState 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

SECTION .data
; buffer temporal donde armamos el frame que pasamos a regs_save
exc_regs:
    dq 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

SECTION .text

_exception0Handler:
    ; Divide Error (vector 0)
    pushState

    ; avanzar el RIP guardado (hardware puso RIP + CS + RFLAGS antes de los pushes)
    ; offset = 15 regs * 8 bytes = 120
    add qword [rsp + 120], 3    ; salto sobre la instrucción DIV (3 bytes: 48 F7 F0)

    mov [rel exc_regs], rax
    mov [rel exc_regs+8], rbx
    mov [rel exc_regs+16], rcx
    mov [rel exc_regs+24], rdx
    mov [rel exc_regs+32], rbp
    mov [rel exc_regs+40], rdi
    mov [rel exc_regs+48], rsi
    mov [rel exc_regs+56], r8
    mov [rel exc_regs+64], r9
    mov [rel exc_regs+72], r10
    mov [rel exc_regs+80], r11
    mov [rel exc_regs+88], r12
    mov [rel exc_regs+96], r13
    mov [rel exc_regs+104], r14
    mov [rel exc_regs+112], r15

    lea rdi, [rel exc_regs]
    call regs_save

    mov rdi, 0
    call exceptionDispatcher

    popState
    iretq


_exception6Handler:
    ; Invalid Opcode (vector 6)
    pushState

    ; saltar la instrucción UD2 (2 bytes)
    add qword [rsp + 120], 2

    mov [rel exc_regs], rax
    mov [rel exc_regs+8], rbx
    mov [rel exc_regs+16], rcx
    mov [rel exc_regs+24], rdx
    mov [rel exc_regs+32], rbp
    mov [rel exc_regs+40], rdi
    mov [rel exc_regs+48], rsi
    mov [rel exc_regs+56], r8
    mov [rel exc_regs+64], r9
    mov [rel exc_regs+72], r10
    mov [rel exc_regs+80], r11
    mov [rel exc_regs+88], r12
    mov [rel exc_regs+96], r13
    mov [rel exc_regs+104], r14
    mov [rel exc_regs+112], r15

    lea rdi, [rel exc_regs]
    call regs_save

    mov rdi, 6
    call exceptionDispatcher

    popState
    iretq
global 