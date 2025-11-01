// audioDriver.h
#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

void play_sound(uint32_t freq_hz);
void stop_sound(void);
void beep(uint32_t freq_hz, uint32_t duration_ms);   // si usás sleep_ms
// o
void beep_ms(uint32_t freq_hz, uint32_t duration_ms);

#endif