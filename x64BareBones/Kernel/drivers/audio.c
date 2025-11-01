// audioDriver.c
#include <stdint.h>
#include "audio.h"   
#include "timer.h"

// vienen de tu io.asm
extern uint8_t inb(uint16_t port);
extern void    outb(uint16_t port, uint8_t value);

// Frecuencia base del PIT
#define PIT_BASE_FREQ 1193182u

// Puertos usados
#define PIT_CMD_PORT   0x43
#define PIT_CH2_PORT   0x42
#define SPEAKER_PORT   0x61

// Comando: canal 2, acceso low+high, modo 3 (square wave), binario
#define PIT_SPKR_CMD   0xB6

static void pit_set_frequency(uint32_t freq_hz) {
    if (freq_hz == 0) {
        return; // evitar div 0
    }

    uint32_t divisor = PIT_BASE_FREQ / freq_hz;

    // modifico el canal 2 del pit
    outb(PIT_CMD_PORT, PIT_SPKR_CMD);

    // lo mando en dos partes 
    outb(PIT_CH2_PORT, (uint8_t)(divisor & 0xFF));        // low
    outb(PIT_CH2_PORT, (uint8_t)((divisor >> 8) & 0xFF)); // high
}

void play_sound(uint32_t freq_hz) {
    // 1) le digo al PIT que genere esa frecuencia en el canal 2
    pit_set_frequency(freq_hz);

    // 2) prendo el speaker (puerto 0x61, bits 0 y 1)
    uint8_t tmp = inb(SPEAKER_PORT);

    // si alguno de los dos no está prendido, lo prendo
    if ((tmp & 0x03) != 0x03) {
        outb(SPEAKER_PORT, tmp | 0x03);
    }
}

void stop_sound(void) {
    uint8_t tmp = inb(SPEAKER_PORT);
    // limpio los bits 0 y 1 → apago
    outb(SPEAKER_PORT, (uint8_t)(tmp & ~0x03));
}

void beep(uint32_t freq_hz, uint32_t duration_ms) {
    play_sound(freq_hz);
    sleep_ms(duration_ms);
    stop_sound();
}