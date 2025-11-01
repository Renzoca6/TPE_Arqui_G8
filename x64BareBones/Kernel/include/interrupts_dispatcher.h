#ifndef INTERRUPTS_DISPATCHER_H
#define INTERRUPTS_DISPATCHER_H

/* ===================== */
/*  Dispatcher de IRQs    */
/* ===================== */

/**
 * Llamado en cada tick del timer (IRQ0).
 */
void timer_tick(void);

/**
 * Inicializa el sistema de interrupciones
 * (PIC, timer, handlers, etc.).
 */
void init_interrupts(void);

#endif /* INTERRUPTS_DISPATCHER_H */
