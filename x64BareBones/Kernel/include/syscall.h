#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
} syscall_Registers;

void syscall_handler(syscall_Registers * regs);
int syscall_write(syscall_Registers *regs);
void syscall_read(syscall_Registers *regs);

#endif