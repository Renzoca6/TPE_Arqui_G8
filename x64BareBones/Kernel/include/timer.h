#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* ===================== */
/*     Timer / PIT        */
/* ===================== */

/** Inicializa el timer con la frecuencia indicada (Hz). */
void timer_init(uint32_t hz);

/** Devuelve la cantidad de ticks desde el arranque. */
uint64_t timer_ticks(void);

/** Devuelve los milisegundos desde el arranque. */
uint64_t timer_ms_since_boot(void);

/** Llamado desde IRQ0 en cada tick. */
void timer_on_irq(void);

/** Duerme la CPU la cantidad indicada de ms. */
void sleep_ms(uint64_t ms);

#endif /* TIMER_H */
