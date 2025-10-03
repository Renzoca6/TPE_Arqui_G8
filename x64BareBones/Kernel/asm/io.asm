global inb
global outb

section .text

; Leer un byte desde puerto
; uint8_t inb(uint16_t port)
inb:
    mov dx, di        ; el argumento está en DI
    in  al, dx        ; leer del puerto DX a AL
    movzx eax, al     ; expandir a 32/64 bits
    ret

; Escribir un byte a puerto
; void outb(uint16_t port, uint8_t value)
outb:
    mov dx, di        ; primer argumento = puerto
    mov al, sil       ; segundo argumento (en SIL = 8 bits bajos de RSI)
    out dx, al        ; escribir AL en puerto DX
    ret
