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
uint64_t  do_benchmark_fps();
uint64_t  do_benchmark_floating_point();
uint64_t  do_benchmark_hardware_access();


#endif
