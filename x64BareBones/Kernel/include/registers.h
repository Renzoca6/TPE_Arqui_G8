#ifndef REGISTERS_H_
#define REGISTERS_H_

#include <stdint.h>
#include <stdbool.h>

/* Orden del frame que esperamos recibir desde el stub de la ISR.
   Asegurate de que esto MATCHEE el push order del stub ASM. */
typedef enum {
    /* Debe coincidir con el macro pushState en asm/interrupts.asm */
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
    REG_COUNT
} reg_index_t;

/* Nombres legibles para debug/print. */
extern const char * const REG_NAMES[REG_COUNT];

/* API */
void     regs_save(const uint64_t *frame);   /* copia ‘frame’ a un snapshot interno */
bool     regs_ready(void);                   /* hay snapshot disponible */
const uint64_t *regs_get(void);              /* puntero const al snapshot (puede ser NULL si no hay) */
void     regs_clear(void);                   /* limpia el snapshot (opcional) */

/* --- Compatibilidad con tu código actual --- */
static inline void makeBackup(uint64_t *regs) { regs_save(regs); }
static inline char  isBackupDone(void)        { return regs_ready() ? 1 : 0; }
/* OJO: cambia el tipo de retorno a const; si querés 100% compatible, hacé un cast en exceptions.c */
static inline const uint64_t *getRegs(void)   { return regs_get(); }

#endif
