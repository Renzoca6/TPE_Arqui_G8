#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_init(void);
void idt_set_entry(int vector, uint64_t handler);
void idt_load(void *idtr);

#endif