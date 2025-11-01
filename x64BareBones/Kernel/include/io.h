#ifndef IO_H
#define IO_H

#include <stdint.h>

/* ===================== */
/*   Puertos de E/S       */
/* ===================== */

/**
 * Lee 1 byte desde el puerto indicado.
 */
uint8_t inb(uint16_t port);

/**
 * Escribe 1 byte en el puerto indicado.
 */
void outb(uint16_t port, uint8_t value);

#endif /* IO_H */