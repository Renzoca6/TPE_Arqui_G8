
#include "interrupts.h"
#include "idt.h"
#include "pic.h"
#include "video.h"
#include "keyboard_handler.h"
#include "timer.h"
extern void enable_interrupts(void);



static void int_20();
static void int_21(uint64_t * registers);

void irqDispatcher(uint64_t irq, uint64_t * registers) {
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

void int_20() {
	timer_on_irq();
    pic_send_eoi(0); 
}

void int_21(uint64_t * registers) {
	keyboardPressed(registers);
	pic_send_eoi(0); 
}

// === Inicialización general de interrupciones ===
void init_interrupts(void) {
    idt_init();        // crear y cargar la IDT
    pic_init();        

    timer_init(1000);     // programar PIT a 1 Hz lo cual son 1 vez por segundo 
    pic_unmask_irq(0);      // habilitar IRQ0 (timer) es decir que puede detectar al timer si lo seteo en 1 ya no responderia ante el timer 
    pic_unmask_irq(1);
	

    enable_interrupts();
}
