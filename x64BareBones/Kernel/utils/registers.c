#include "registers.h"
#include "keyboard_handler.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include <stddef.h>

/* Exportamos los nombres desde un solo lugar. Deben coincidir con pushState */
const char * const REG_NAMES[REG_COUNT] = {
    "RAX", "RBX", "RCX", "RDX",
    "RBP", "RDI", "RSI",
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15", "RSP",
    "RIP", "RFLAGS", "CS", "DS", "ES", "FS", "GS", "SS"
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


/* Helper: print a 64-bit value as 16 hex digits (zero-padded) */
static void printHexPadded(uint64_t v) {
    char buf[17];
    const char *hex = "0123456789ABCDEF";
    for (int i = 0; i < 16; i++) {
        uint8_t nibble = (v >> ((15 - i) * 4)) & 0xF;
        buf[i] = hex[nibble];
    }
    buf[16] = '\0';
    vdPrint(buf, PIXEL_VRAM);
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

    /* Decorative box header */
    vdPrint("+----------------------------------------------------+", PIXEL_VRAM);
    vdNewline();
    vdPrint("|                  REGISTER SNAPSHOT                 |", PIXEL_VRAM);
    vdNewline();
    vdPrint("+----------------------------------------------------+", PIXEL_VRAM);
    vdNewline();

    /* Print registers in two columns with fixed positions.
       Format for each printed row (interior width = 48 characters):
       |NAME: <16hex>    NAME: <16hex>|
       NAME is padded/truncated to 4 chars, values are 16 hex digits.
    */
    for (int i = 0; i < REG_COUNT; i += 2) {
            char namebuf[10];

        /* start left border */
        vdPrint("|", PIXEL_VRAM);

        /* Left column name (pad/truncate to 4 chars) */
        const char *n = REG_NAMES[i];
        int j = 0;
            while (j < 6 && n[j]) { namebuf[j] = n[j]; j++; }
            while (j < 6) { namebuf[j++] = ' '; }
            namebuf[6] = ':'; namebuf[7] = ' '; namebuf[8] = '\0';
        vdPrint(namebuf, PIXEL_VRAM);
        printHexPadded(regs[i]); /* prints 16 hex chars */

        /* spacing between columns */
        vdPrint("    ", PIXEL_VRAM);

        /* Right column */
        if (i + 1 < REG_COUNT) {
            const char *n2 = REG_NAMES[i+1];
            int k = 0;
                while (k < 6 && n2[k]) { namebuf[k] = n2[k]; k++; }
                while (k < 6) { namebuf[k++] = ' '; }
                namebuf[6] = ':'; namebuf[7] = ' '; namebuf[8] = '\0';
            vdPrint(namebuf, PIXEL_VRAM);
            printHexPadded(regs[i+1]);
        }

        /* end right border */
        vdPrint("|", PIXEL_VRAM);
        vdNewline();
    }

    /* Decorative footer */
    vdPrint("+----------------------------------------------------+", PIXEL_VRAM);
    vdNewline();
}
