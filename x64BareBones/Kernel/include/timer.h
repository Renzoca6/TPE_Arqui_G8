
#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>


void timer_init(uint32_t hz);
uint64_t timer_ticks(void);
uint64_t timer_ms_since_boot(void);
void timer_on_irq(void);
void sleep_ms(uint64_t ms);

#endif 
