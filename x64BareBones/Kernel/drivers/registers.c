#include "registers.h"
#include <stddef.h>

/* Exportamos los nombres desde un solo lugar */
const char * const REG_NAMES[REG_COUNT] = {
    "RAX", "RBX", "RCX", "RDX",
    "RSI", "RDI", "RBP",
    "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
    "RSP", "RIP", "RFLAGS"
};

static uint64_t snapshot[REG_COUNT] = {0};
static volatile bool snapshot_ready = false;

void regs_save(const uint64_t *frame) {
    /* frame no debe ser NULL; si querés, podés hardenear con un if */
    for (int i = 0; i < REG_COUNT; i++) {
        snapshot[i] = frame[i];
    }
    snapshot_ready = true;
}

bool regs_ready(void) {
    return snapshot_ready;
}

const uint64_t *regs_get(void) {
    return snapshot_ready ? snapshot : NULL;
}

void regs_clear(void) {
    snapshot_ready = false;
    /* Si querés limpiar datos viejos: for (int i=0;i<REG_COUNT;i++) snapshot[i]=0; */
}
