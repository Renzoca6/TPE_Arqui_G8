default rel

GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq06Handler
GLOBAL _exception0Handler
GLOBAL _exception6Handler

;GLOBAL _exception0Handler

EXTERN syscall_handler
EXTERN irqDispatcher
;EXTERN exceptionDispatcher
extern regs_save
extern exceptionDispatcher


SECTION .text



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

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro


%macro SaveRegisters 0
mov [rel exc_regs], rax
mov [rel exc_regs + 8], rbx
mov [rel exc_regs + 2*8], rcx
mov [rel exc_regs + 3*8], rdx
mov [rel exc_regs + 4*8], rbp
mov [rel exc_regs + 5*8], rdi
mov [rel exc_regs + 6*8], rsi
mov [rel exc_regs + 7*8], r8
mov [rel exc_regs + 8*8], r9
mov [rel exc_regs + 9*8], r10
mov [rel exc_regs + 10*8], r11
mov [rel exc_regs + 11*8], r12
mov [rel exc_regs + 12*8], r13
mov [rel exc_regs + 13*8], r14
mov [rel exc_regs + 14*8], r15

lea rdi, [rel exc_regs]
call regs_save
%endmacro

; exceptionHandler macro
; args:
;   1 = id exception
;   2 = skip bytes (number of bytes to advance saved RIP so faulting
;       instruction is not re-executed). Optional, default 0.

%macro exceptionHandler 2
	pushState

	; advance saved RIP by %2 bytes; saved RIP is at [rsp + 15*8]
	add qword [rsp + 15*8], %2

	SaveRegisters

	mov rdi, %1
	call exceptionDispatcher

	popState
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
_exception0Handler:
	; Divide Error (DIV by zero) — skip 3 bytes (48 F7 F0) and dispatch
	exceptionHandler 0, 3

;OP Exception
_exception6Handler:
	; Invalid Opcode (UD2) — skip 2 bytes and dispatch
	exceptionHandler 6, 2


; syscall 
_irq06Handler:
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    sti

    ; guardar registros en la struct
    mov [registers], rax        ; rax (el que vino de userland, el nro de syscall)
    mov [registers+8], rbx
    mov [registers+16], rcx
    mov [registers+24], rdx
    mov [registers+32], rsi
    mov [registers+40], rdi

    mov rdi, registers
    call syscall_handler        ; acá el C te pone registers->rax = resultado

    ; recuperar RAX que dejó el kernel en la struct
    mov rax, [registers]        ; ❷ ahora sí: rax = registers->rax (el resultado de la syscall)

    ; signal pic EOI si corresponde
    ; ...

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    iretq
	 
haltcpu:
	cli
	hlt
	ret



SECTION .bss
	aux resq 1

SECTION .data
	registers:
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
	
; buffer temporal para snapshot de excepciones (15 regs x 8 bytes)
exc_regs:
	dq 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	
