#ifndef SYSCALL_CALL_H
#define SYSCALL_CALL_H
#include <stddef.h>
#include <stdint.h>

int write(const char* buf);         // fd fijo = 1
int println(const char* buf);
int read (char *buf);               // fd fijo = 0
int printError(const char* buf);
int clearwindow(uint32_t* color);


#endif
