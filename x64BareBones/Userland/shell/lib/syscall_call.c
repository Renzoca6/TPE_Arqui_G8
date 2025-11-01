#include "syscall_call.h"

extern void sys_resize(char * N_times);
extern int sys_write(int fb, const char* buf);
extern int sys_read (char *buf);
extern void sys_clearwindow(uint32_t* color);
extern int sys_date_time (int tpye);
extern uint64_t  sys_benchmark (int type);
extern uint64_t sys_write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
extern uint64_t sys_write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
extern void sys_present_fullframe();
extern int sys_get_screen_info(int aux);
extern void sys_putPixel(uint32_t color, uint32_t x, uint32_t y, uint32_t target);
extern uint64_t sys_getchar(char *buffer, uint64_t max_len);
extern char sys_print_registers();
extern void touch_regs();
extern void sys_sleep_ms(uint64_t ms);
extern uint64_t sys_get_ms_since_boot();
extern uint64_t sys_audio(uint64_t op, uint32_t freq, uint32_t dur_ms);   // <--- NUEVO

#define STDIN   0
#define STDOUT  1

uint64_t get_ms_since_boot(void) {
    return sys_get_ms_since_boot();
}

void sleep_ms(uint64_t ms){
    sys_sleep_ms(ms);
}

// target: 0 = PIXEL_VRAM, 1 = PIXEL_BACK
void putPixel(uint32_t color, uint32_t x, uint32_t y, int target) {
    sys_putPixel(color, x, y, target);
}

int write(const char* buf){
    return sys_write(STDOUT, buf);
}

int println(const char* buf){
    sys_write(STDOUT, buf);
    write("\n");
    return 1;
}

uint64_t get_screen_height(void) {
    return sys_get_screen_info(0);  // 0 = height
}

uint64_t get_screen_width(void) {
    return sys_get_screen_info(1);  // 1 = width
}

void present_fullframe(void) {
    sys_present_fullframe();
}

void write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor){
    sys_write_at_back(str,col,fil,fColor,bgColor);
}

void write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor){
    sys_write_at_vram(str,col,fil,fColor,bgColor);
}

int printError(const char* buf){
    return (sys_write(STDIN, buf) && write("\n"));
}

int read (char *buf){
    return sys_read(buf);
}

void clearwindow(uint32_t* color){
    sys_clearwindow(color);
}

void do_resize(char * N_Times){
    return sys_resize(N_Times);
}

void get_time(){
    sys_date_time (0);
}

void get_date(){
    sys_date_time (1);
}

void printRegisters(){
    sys_print_registers();
}

void audio_play(uint32_t freq_hz) {
    // op=0, freq=freq_hz, dur=0
    sys_audio(0, freq_hz, 0);
}

void audio_stop(void) {
    // op=1
    sys_audio(1, 0, 0);
}

void audio_beep(uint32_t freq_hz, uint32_t duration_ms) {
    // op=2
    sys_audio(2, freq_hz, duration_ms);
}


uint64_t  do_benchmark_fps(){
    return sys_benchmark (0);
}
uint64_t  do_benchmark_floating_point(){
    return sys_benchmark (1);
}
uint64_t  do_benchmark_hardware_access(){
    return sys_benchmark (2);
}

int get_multiple_chars_sys(char *buf, uint64_t max_len) {
    return (int) sys_getchar(buf, max_len);
}

char getchar(void) {
    char c;
    int n = sys_getchar(&c, 1);
    return (n > 0) ? c : 0;
}



//apagado
//registros del procesador
//fps
//ETC