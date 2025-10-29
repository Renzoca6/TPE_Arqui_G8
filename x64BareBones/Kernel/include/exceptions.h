#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdint.h>

/* IDs de Intel (x86_64): 0 = Divide Error, 6 = Invalid Opcode */
typedef enum {
    EXC_DIVIDE_ERROR   = 0x00,
    EXC_INVALID_OPCODE = 0x06
} exception_id_t;

void exceptionDispatcher(int exception_id);

#endif
