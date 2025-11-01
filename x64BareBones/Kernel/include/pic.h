#ifndef PIC_H
#define PIC_H

#include <stdint.h>

/* ===================== */
/*    PIC (8259)          */
/* ===================== */

/** Inicializa el PIC. */
void pic_init(void);

/** Habilita una línea IRQ. */
void pic_unmask_irq(uint8_t irq);

/** Deshabilita una línea IRQ. */
void pic_mask_irq(uint8_t irq);

/** Envía End Of Interrupt. */
void pic_send_eoi(uint8_t irq);

#endif /* PIC_H */
