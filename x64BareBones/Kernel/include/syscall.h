#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;

} syscall_Registers;


#endif