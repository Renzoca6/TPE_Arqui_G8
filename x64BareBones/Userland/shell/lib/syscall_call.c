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
extern char sys_getchar();
extern char sys_print_registers();
extern void touch_regs();

#define STDIN   0
#define STDOUT  1

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
    touch_regs();
    sys_print_registers();
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

char getchar_sys(void) {
    return (char) sys_getchar();
}





//apagado
//registros del procesador
//fps
//ETC