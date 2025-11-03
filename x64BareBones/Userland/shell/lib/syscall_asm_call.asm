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
global sys_sleep_ms
global sys_kill_system
GLOBAL sys_audio
GLOBAL sys_putframe

; Stub genérico para syscalls
; ABI de Userland (SysV):
;   arg0=rdi, arg1=rsi, arg2=rdx, arg3=rcx, arg4=r8, arg5=r9
; El kernel espera los argumentos en los registros:
;   arg0=rbx, arg1=rcx, arg2=rdx, arg3=rsi, arg4=rdi
; Debemos reorganizar los registros antes de disparar int 0x80 y
; preservar rbx (callee-saved) para el llamador en C.
%macro SYSCALL 1
    push rbp
    mov rbp, rsp

    ; Preservar registro callee-saved usado a continuación
    push rbx

    ; Guardar arg3 (en RCX) temporalmente
    mov r10, rcx

    ; Remapear argumentos a lo que espera el kernel
    mov rbx, rdi     ; arg0 -> RBX
    mov rcx, rsi     ; arg1 -> RCX
    ; RDX ya contiene arg2
    mov rsi, r10     ; arg3 -> RSI
    mov rdi, r8      ; arg4 -> RDI

    ; Número de syscall
    mov rax, %1
    int 0x80

    ; Restaurar registro callee-saved
    pop rbx

    mov rsp, rbp
    pop rbp
    ret
%endmacro

sys_read:               SYSCALL 0
sys_write:              SYSCALL 1
sys_clearwindow:        SYSCALL 2
sys_date_time:          SYSCALL 3
sys_resize:             SYSCALL 4
sys_benchmark:          SYSCALL 5
sys_write_at_vram:      SYSCALL 6
sys_write_at_back:      SYSCALL 7
sys_present_fullframe:  SYSCALL 8
sys_get_screen_info:    SYSCALL 9
sys_print_registers:    SYSCALL 10 
sys_getchar:            SYSCALL 11
sys_putPixel:           SYSCALL 12
sys_get_ms_since_boot:  SYSCALL 13
sys_sleep_ms:           SYSCALL 14
sys_kill_system         SYSCALL 15
sys_audio:              SYSCALL 16
sys_putframe:           SYSCALL 17



