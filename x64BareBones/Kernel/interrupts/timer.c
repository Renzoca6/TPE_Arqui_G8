#include <stdint.h>
#include "timer.h"
#include "pic.h"

// ---------------------------------------------------------------------
// Puertos y constantes del PIT
// ---------------------------------------------------------------------
#define PIT_CHANNEL0   0x40
#define PIT_COMMAND    0x43
#define PIT_BASE_FREQ  1193182   // Frecuencia base del PIT en Hz

// ---------------------------------------------------------------------
// Estado del timer
// ---------------------------------------------------------------------
// Contador global de ticks 
static volatile uint64_t g_ticks = 0;

// Frecuencia actual del timer: son los ticks por segundo
static uint32_t g_tick_hz = 0;

extern void outb(uint16_t port, uint8_t val);

// ---------------------------------------------------------------------
// Handler de IRQ del timer (llamado desde el interrupts dispatcher)
// ---------------------------------------------------------------------
void timer_on_irq(void) {
    g_ticks++;
}

// ---------------------------------------------------------------------
// Inicializa el PIT para que genere interrupciones periódicas
// ---------------------------------------------------------------------
void timer_init(uint32_t freq_hz) {
    if (freq_hz == 0) {
        freq_hz = 1000;      // Default: 1000 Hz (1 kHz)
    }
    g_tick_hz = freq_hz;

    // Calcular divisor (el PIT divide la frecuencia base)
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQ / freq_hz);

    // Configurar el PIT: canal 0, acceso low/high, modo 3 (square wave), binario
    outb(PIT_COMMAND, 0b00110110);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));        // low byte
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF)); // high byte
}

// ---------------------------------------------------------------------
// Devuelve la cantidad total de ticks desde que se inició
// ---------------------------------------------------------------------
uint64_t timer_ticks(void) {
    return g_ticks;
}

// ---------------------------------------------------------------------
// Devuelve el tiempo en milisegundos desde que se inició
// ---------------------------------------------------------------------
uint64_t timer_ms_since_boot(void) {
    if (g_tick_hz == 0)
        return 0;

    return (g_ticks * 1000ull) / g_tick_hz;
}

// ---------------------------------------------------------------------
// Sleep activo con hlt
// ---------------------------------------------------------------------p
#include "interrupts.h"
void sleep_ms(uint64_t ms) {
    if (g_tick_hz == 0)
        return;

    uint64_t start         = g_ticks;
    uint64_t ticks_to_wait = (ms * g_tick_hz) / 1000;

    //_hlt() ejecuta la instrucción de CPU HLT (halt). Esa instrucción detiene (suspende) 
    //la ejecución del procesador hasta que ocurra la próxima interrupción externa
    while ((g_ticks - start) < ticks_to_wait) {
       _hlt();
    }
}
