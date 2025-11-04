#ifndef SYSCALL_CALL_H
#define SYSCALL_CALL_H

#include <stddef.h>
#include <stdint.h>

// Funciones generales
void do_resize(char *N_Times);
int write(const char *buf);
int println(const char *buf);
int read(char *buf);
int printError(const char *buf);
void clearwindow(uint32_t *color);

// Fecha y hora
void get_time(void);
void get_date(void);

// Registros
void printRegisters(void);

// Benchmarks
uint64_t do_benchmark_fps(void);
uint64_t do_benchmark_floating_point(void);
uint64_t do_benchmark_hardware_access(void);
uint64_t do_benchmark_timer_latency(void);

// Escritura en pantalla
void write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
void write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
void get_screen_info(uint64_t *height, uint64_t *width);
void present_fullframe(void);

// Dimensiones
uint64_t get_screen_height(void);
uint64_t get_screen_width(void);

// Entrada y temporización
char getchar(void);
void sleep_ms(uint64_t ms);
uint64_t get_ms_since_boot(void);

// Video
void put_frame(void);
void putPixel(uint32_t color, uint32_t x, uint32_t y, int target);

// Entrada extendida
int get_multiple_chars_sys(char *buf, uint64_t max_len);

// Audio
void audio_play(uint32_t freq_hz);
void audio_stop(void);
void audio_beep(uint32_t freq_hz, uint32_t duration_ms);

#endif // SYSCALL_CALL_H
