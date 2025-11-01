#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

/* ===================== */
/*   Excepciones de CPU   */
/* ===================== */

typedef enum {
    EXC_DIVIDE_ERROR   = 0x00,
    EXC_INVALID_OPCODE = 0x06
} exception_id_t;

/**
 * Despacha la excepción recibida desde el handler ASM.
 */
void exceptionDispatcher(int exception_id, uint64_t *registers);

#endif /* EXCEPTIONS_H */
