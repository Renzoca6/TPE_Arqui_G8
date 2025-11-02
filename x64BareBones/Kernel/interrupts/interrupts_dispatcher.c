#include "interrupts.h"
#include "idt.h"
#include "pic.h"
#include "video.h"
#include "keyboard_handler.h"
#include "timer.h"

extern void enable_interrupts(void);

// ---------------------------------------------------------------------
// Prototipos locales
// ---------------------------------------------------------------------
static void int_20(void);
static void int_21(uint64_t *registers);

// ---------------------------------------------------------------------
// Dispatcher de IRQs
// ---------------------------------------------------------------------
void irqDispatcher(uint64_t irq, uint64_t *registers) {
    switch (irq) {
        case 0:
            int_20();
            break;
        case 1:
            int_21(registers);
            break;
    }
    return;
}

// ---------------------------------------------------------------------
// IRQ0: timer
// ---------------------------------------------------------------------
void int_20(void) {
    timer_on_irq();
    pic_send_eoi(0); 
}

// ---------------------------------------------------------------------
// IRQ1: teclado
// ---------------------------------------------------------------------
void int_21(uint64_t *registers) {
    keyboardPressed(registers);
    pic_send_eoi(1);   // aviso que fue la IRQ1 (teclado)
}

// ---------------------------------------------------------------------
// Inicialización general de interrupciones
// ---------------------------------------------------------------------
void init_interrupts(void) {
    idt_init();             // crear y cargar la IDT
    pic_init();        

    timer_init(1000);       // programar PIT a 1000 Hz (1 ms), no a 1 Hz
    pic_unmask_irq(0);      // habilitar IRQ0 (timer): si lo seteo en 1 ya no respondería ante el timer 
    pic_unmask_irq(1);      // habilitar IRQ1 (teclado)
	
    enable_interrupts();
}
