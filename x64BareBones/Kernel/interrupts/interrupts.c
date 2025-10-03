
#include "interrupts.h"
#include "idt.h"
#include "pic.h"
#include <naiveConsole.h>
#include <keyboard_handler.h>

// contador de ticks
static uint64_t ticks = 0;

// timmer tick
void timer_tick(void) {
    ticks++;
    if (ticks % (18*5) == 0) { 
        ncPrint("Tick");

    }
}

void keyboard_handler(void) {
    keyboardPressed();
}



// === Inicialización general de interrupciones ===
void init_interrupts(void) {
    idt_init();        // crear y cargar la IDT
    pic_init();        

    //pit_set_frequency(100);     // programar PIT a 1 Hz lo cual son 1 vez por segundo 
    pic_unmask_irq(0);      // habilitar IRQ0 (timer) es decir que puede detectar al timer si lo seteo en 1 ya no responderia ante el timer 
    pic_unmask_irq(1);

    __asm__ __volatile__("sti");     // habilitar interrupciones globales
}
