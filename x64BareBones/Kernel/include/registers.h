#ifndef REGISTERS_H_
#define REGISTERS_H_

#include <stdint.h>
#include <stdbool.h>

/* Orden del frame que esperamos recibir desde el stub de la ISR.
   Asegurate de que esto MATCHEE el push order del stub ASM. 
   
   Layout del stack después de una interrupción:
   1. pushState empuja: RAX, RBX, RCX, RDX, RBP, RDI, RSI, R8..R15 (15 registros)
   2. CPU ya había empujado (antes): RIP, CS, RFLAGS, RSP, SS (5 registros)
   
   Por lo tanto, el array registers[] tiene este orden:
   [0..14] = RAX..R15 (de pushState)
   [15] = RIP, [16] = CS, [17] = RFLAGS, [18] = RSP, [19] = SS (del CPU)
*/
typedef enum {
    /* Registros de propósito general (pusheados por pushState) */
    REG_RAX = 0,
    REG_RBX,
    REG_RCX,
    REG_RDX,
    REG_RBP,
    REG_RDI,
    REG_RSI,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    /* Registros pusheados automáticamente por el CPU en la interrupción */
    REG_RIP,      // [15]
    REG_CS,       // [16]
    REG_RFLAGS,   // [17]
    REG_RSP,      // [18] <- RSP de userland
    REG_SS,       // [19]
    /* Nota: DS, ES, FS, GS no están en este frame */
    REG_COUNT = 20
} reg_index_t;

/* Nombres legibles para debug/print. */
extern const char * const REG_NAMES[REG_COUNT];

/* API para imprimir registros (usa el snapshot de keyboard_handler) */
void     print_registers(void);              /* imprime los registros del snapshot */

#endif
