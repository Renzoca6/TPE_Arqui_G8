#ifndef SYSCALL_CALL_H
#define SYSCALL_CALL_H
#include <stddef.h>

long write(const void *buf, size_t count); // fd fijo = 1
long read (void *buf, size_t count);       // fd fijo = 0

#endif
