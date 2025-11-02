// ---------------------------------------------------------------------
// audio.c - Control básico del altavoz del sistema (PC Speaker)
// ---------------------------------------------------------------------
#include <stdint.h>
#include "audio.h"
#include "timer.h"

// ---------------------------------------------------------------------
// Definiciones de puertos y constantes del PIT
// ---------------------------------------------------------------------
extern uint8_t inb(uint16_t port);
extern void    outb(uint16_t port, uint8_t value);

#define PIT_BASE_FREQ  1193182u  // Frecuencia base del PIT
#define PIT_CMD_PORT   0x43
#define PIT_CH2_PORT   0x42
#define SPEAKER_PORT   0x61
#define PIT_SPKR_CMD   0xB6      // Canal 2, acceso low+high, modo 3 (square wave)
#define SPK_MIN_FREQ   37u       // ~tono más grave usable
#define SPK_MAX_FREQ   32767u    // límite razonable para PIT canal 2

// ---------------------------------------------------------------------
// Función interna: configura la frecuencia del PIT (canal 2)
// ---------------------------------------------------------------------
static void pit_set_frequency(uint32_t freq_hz) {
    // Validación / clamp de frecuencia
    if (freq_hz < SPK_MIN_FREQ)
        freq_hz = SPK_MIN_FREQ;
    else if (freq_hz > SPK_MAX_FREQ)
        freq_hz = SPK_MAX_FREQ;

    uint32_t divisor = PIT_BASE_FREQ / freq_hz;
    if (divisor == 0)
        return;     // protección extra

    // Configura canal 2
    outb(PIT_CMD_PORT, PIT_SPKR_CMD);
    outb(PIT_CH2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CH2_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

// ---------------------------------------------------------------------
// Activa el speaker a una frecuencia determinada
// ---------------------------------------------------------------------
void play_sound(uint32_t freq_hz) {
    // Si es 0, lo tomamos como "no hay sonido"
    if (freq_hz == 0) {
        stop_sound();
        return;
    }

    pit_set_frequency(freq_hz);

    uint8_t tmp = inb(SPEAKER_PORT);
    if ((tmp & 0x03) != 0x03) {
        outb(SPEAKER_PORT, tmp | 0x03);
    }
}

// ---------------------------------------------------------------------
// Apaga el speaker
// ---------------------------------------------------------------------
void stop_sound(void) {
    uint8_t tmp = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, (uint8_t)(tmp & ~0x03));  // limpia bits 0 y 1
}

// ---------------------------------------------------------------------
// Emite un beep con duración en milisegundos
// ---------------------------------------------------------------------
void beep(uint32_t freq_hz, uint32_t duration_ms) {
    play_sound(freq_hz);
    sleep_ms(duration_ms);
    stop_sound();
}
