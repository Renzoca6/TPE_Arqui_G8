#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void pic_init(void);
void pic_unmask_irq(uint8_t irq);
void pic_mask_irq(uint8_t irq);

#endif