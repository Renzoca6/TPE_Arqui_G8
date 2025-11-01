#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/* ===================== */
/*   IRQ handlers (ASM)   */
/* ===================== */

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _irq06Handler(void);

/* ===================== */
/*  Exception handlers    */
/* ===================== */

void _exception0Handler(void);   /* Divide error    */
void _exception6Handler(void);   /* Invalid opcode  */

/* ===================== */
/*     Utilidades CPU     */
/* ===================== */

/** Ejecuta HLT. */
void _hlt(void);

/** Detiene la CPU. */
void haltcpu(void);

#endif /* INTERRUPTS_H */
