global loader
extern main
extern initializeKernelBinary

loader:
	call initializeKernelBinary	; Prepara el binario del kernel y obtiene la dirección de la pila
	mov rsp, rax				; Configura RSP (stack pointer) con la dirección de retorno
	call main
hang:
	cli
	hlt	; Detiene la CPU si main retorna
	jmp hang
