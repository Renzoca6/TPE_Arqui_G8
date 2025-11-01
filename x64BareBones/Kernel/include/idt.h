#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* ===================== */
/*   IDT (Interrupt DT)   */
/* ===================== */

/**
 * Inicializa la estructura de la IDT en memoria.
 */
void idt_init(void);

/**
 * Configura una entrada de la IDT.
 */
void idt_set_entry(int vector, uint64_t handler);

/**
 * Carga la IDT en el registro IDTR (lidt).
 */
void idt_load(void *idtr);

#endif /* IDT_H */
