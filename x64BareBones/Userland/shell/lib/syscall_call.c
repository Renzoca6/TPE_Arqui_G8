#include "syscall_call.h"

extern void sys_resize(char * N_times);
extern int sys_write(int fb, const char* buf);
extern int sys_read (char *buf);
extern void sys_clearwindow(uint32_t* color);
extern int sys_date_time (int tpye);
extern uint64_t  sys_benchmark (int type);
extern uint64_t sys_write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
extern uint64_t sys_write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);


#define STDIN   0
#define STDOUT  1

int write(const char* buf){
    return sys_write(STDOUT, buf);
}

int println(const char* buf){
    sys_write(STDOUT, buf);
    write("\n");
    return 1;
}

//   uint64_t width, height;
//get_screen_info(&height, &width);

void get_screen_info(uint64_t *height, uint64_t *width) {
    sys_get_screen_info(height, width);
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



uint64_t  do_benchmark_fps(){
    sys_benchmark (0);
}
uint64_t  do_benchmark_floating_point(){
    sys_benchmark (1);
}
uint64_t  do_benchmark_hardware_access(){
    sys_benchmark (2);
}





//apagado
//registros del procesador
//fps
//ETC