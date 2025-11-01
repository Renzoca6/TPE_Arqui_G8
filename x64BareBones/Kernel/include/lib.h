#ifndef LIB_H
#define LIB_H

#include <stdint.h>

/* ===================== */
/*   Memoria básica       */
/* ===================== */

/** Rellena un bloque de memoria. */
void *memset(void *destination, int32_t character, uint64_t length);

/** Copia un bloque de memoria. */
void *memcpy(void *destination, const void *source, uint64_t length);

/* ===================== */
/*   Información de CPU   */
/* ===================== */

/** Obtiene el vendor string de la CPU (CPUID). */
char *cpuVendor(char *result);

#endif /* LIB_H */
