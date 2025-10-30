#include "registers.h"
#include "keyboard_handler.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include <stddef.h>

/* Exportamos los nombres desde un solo lugar. Deben coincidir con pushState */
const char * const REG_NAMES[REG_COUNT] = {
    "RAX", "RBX", "RCX", "RDX",
    "RBP", "RDI", "RSI",
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15"
};

static uint64_t snapshot[REG_COUNT] = {0};
static volatile bool snapshot_ready = false;

void regs_save(const uint64_t *frame) {
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


/* Evita crash si no hay snapshot todavía */
void print_registers(void)
{
    const uint64_t *regs = regs_get();

    if (regs == NULL) {
        vdPrint("Register snapshot not available.", PIXEL_VRAM);
        vdNewline();
        return;
    }

    vdPrint("Register Status:", PIXEL_VRAM);
    vdNewline();

    for (int i = 0; i < REG_COUNT; i++) {
        vdPrint((char*)REG_NAMES[i], PIXEL_VRAM);
        vdPrint(": ", PIXEL_VRAM);
        vdPrintHex(regs[i], PIXEL_VRAM);
        vdNewline();
    }
    vdNewline();
}
