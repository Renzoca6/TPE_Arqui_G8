#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

/* ===================== */
/*     Audio / Speaker    */
/* ===================== */

/**
 * Genera un tono en el speaker a la frecuencia indicada.
 * Debes llamar luego a stop_sound().
 */
void play_sound(uint32_t freq_hz);

/**
 * Detiene el sonido actual.
 */
void stop_sound(void);

/**
 * Emite un beep de duración fija (ms).
 */
void beep(uint32_t freq_hz, uint32_t duration_ms);

#endif /* AUDIO_H */
