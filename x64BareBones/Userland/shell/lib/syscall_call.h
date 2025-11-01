#ifndef SYSCALL_CALL_H
#define SYSCALL_CALL_H
#include <stddef.h>
#include <stdint.h>


void do_resize(char * N_Times);
int write(const char* buf);         // fd fijo = 1
int println(const char* buf);
int read (char *buf);               // fd fijo = 0
int printError(const char* buf);
void clearwindow(uint32_t* color);
void get_time();
void get_date();
void printRegisters();
uint64_t  do_benchmark_fps();
uint64_t  do_benchmark_floating_point();
uint64_t  do_benchmark_hardware_access();

void write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
void write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
void get_screen_info(uint64_t *height, uint64_t *width);
void present_fullframe(void);

uint64_t get_screen_height(void);

uint64_t get_screen_width(void);
char getchar(void);
void sleep_ms(uint64_t ms);
uint64_t get_ms_since_boot(void);

void putPixel(uint32_t color, uint32_t x, uint32_t y, int target);

void audio_play(uint32_t freq_hz);
void audio_stop(void);
void audio_beep(uint32_t freq_hz, uint32_t duration_ms);

#endif
