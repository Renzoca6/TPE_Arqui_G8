; ======================================================
; memutils.asm
; Implementación rápida de fast_memset y fast_memcpy
; ======================================================
; Convención SysV AMD64:
;   RDI = destino
;   RSI = valor o fuente
;   RDX = tamaño (bytes)
; ======================================================

; ------------------------------------------------------
; void *fast_memset(void *dest, int value, unsigned long size)
; ------------------------------------------------------
global fast_memset
fast_memset:
    cld                     ; asegurar dirección hacia adelante
    mov     rax, rdi        ; guardar puntero dest (para devolver)
    
    cmp     rdx, 8
    jb      .small_fill     ; si size < 8 -> rellenar en bytes

    ; construir un qword con el byte repetido: 0xVVVVVVVVVVVVVVVV
    movzx   eax, sil
    mov     rcx, rax
    shl     rcx, 8
    or      rax, rcx
    mov     rcx, rax
    shl     rcx, 16
    or      rax, rcx
    mov     rcx, rax
    shl     rcx, 32
    or      rax, rcx        ; RAX = valor repetido en 64 bits

    mov     rcx, rdx
    shr     rcx, 3          ; cantidad de qwords
    rep     stosq           ; escribir de a 8 bytes

    mov     rcx, rdx
    and     rcx, 7          ; bytes sobrantes
    movzx   eax, sil
    rep     stosb
    ret

.small_fill:
    movzx   eax, sil
    mov     rcx, rdx
    rep     stosb
    ret


; ------------------------------------------------------
; void *fast_memcpy(void *dst, const void *src, unsigned long size)
; ------------------------------------------------------
global fast_memcpy
fast_memcpy:
    cld                     ; asegurar dirección hacia adelante
    mov     rax, rdi        ; guardar puntero dst (para devolver)

    cmp     rdx, 8
    jb      .small_copy     ; si < 8 bytes -> copia en bytes

    mov     rcx, rdx
    shr     rcx, 3          ; cantidad de qwords
    rep     movsq           ; copiar de a 8 bytes

    mov     rcx, rdx
    and     rcx, 7          ; bytes restantes
    rep     movsb
    ret

.small_copy:
    mov     rcx, rdx
    rep     movsb
    ret
