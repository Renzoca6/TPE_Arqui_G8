#ifndef SYSCALL_CALL_H
#define SYSCALL_CALL_H
#include <stddef.h>

int write(const char* buf);       // fd fijo = 1
int read (char *buf);       // fd fijo = 0

#endif
