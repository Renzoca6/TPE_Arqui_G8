#ifndef BENCHMARK_CALCULATIONS_H
#define BENCHMARK_CALCULATIONS_H

#include <stdint.h>

// Funciones de medición individuales
uint64_t putpixel_user(void);
uint64_t syscall_latency(void);
uint64_t memwrite_user(void);
uint64_t benchmark_fps(void);

#endif // BENCHMARK_CALCULATIONS_H
