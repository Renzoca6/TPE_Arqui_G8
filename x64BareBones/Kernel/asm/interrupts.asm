default rel

; === Timer Stub (IRQ0 - vector 0x20) ===
global timer_tick_asm
extern timer_tick ;declaro la funcion de C 



section .text
timer_tick_asm:
    ; salvar registros
    ;como estamos corriendo algo y es interumpido debo guardar todos los registro en el caso de que se usen mas hay que agregarlos 
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call  timer_tick    ;llamo a la funcion de C 

    ; Avisamos al PIC maestro que ya procesamos la interrupción IRQ0
    mov   dx, 0x20         ; Puerto de comando del PIC maestro
    mov   al, 0x20         ; Comando EOI (End Of Interrupt)
    out   dx, al           ; Escribimos el comando en el puerto → PIC queda listo para otra IRQ


    ; restaurar registros
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq  ; Retorna de la interrupción, restaurando RIP, CS, RFLAGS, etc.



;para a agregar mas interrupciones baSta con copiar el codigo de arriba y cambiar a la funcion es llamada 
global keyboard_acm
extern keyboard_handler
keyboard_acm:
    ; salvar registros
    ;como estamos corriendo algo y es interumpido debo guardar todos los registro en el caso de que se usen mas hay que agregarlos 
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call  keyboard_handler    ;llamo a la funcion de C 

    ; Avisamos al PIC maestro que ya procesamos la interrupción IRQ0
    mov   dx, 0x20         ; Puerto de comando del PIC maestro
    mov   al, 0x20         ; Comando EOI (End Of Interrupt)
    out   dx, al           ; Escribimos el comando en el puerto → PIC queda listo para otra IRQ


    ; restaurar registros
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq  ; Retorna de la interrupción, restaurando RIP, CS, RFLAGS, etc.

