#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

/* ===================== */
/*     Benchmarks         */
/* ===================== */

/**
 * Mide la velocidad de dibujado / FPS aproximados.
 */
uint64_t benchmark_fps(void);

/**
 * Mide operaciones de punto flotante.
 */
uint64_t benchmark_floating_point(void);

/**
 * Mide acceso a hardware (memoria de video, puertos, etc.).
 */
uint64_t benchmark_hardware_access(void);

#endif /* BENCHMARK_H */
