#include "exceptions.h"
#include "registers.h"
#include "keyboard_handler.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include "interrupts.h"
#include <stddef.h>        /* NULL */
#include <stdbool.h>

#define ENTER_CHAR '\n'    /* Unificamos comparación por carácter */

static void handle_exception(const char *title);
static void print_registers_safe(void);

/* Tabla de nombres por ID que te interesan (podés extenderla si luego agregás más) */
static const struct {
    int        id;
    const char *msg;
} EXC_TABLE[] = {
    { EXC_DIVIDE_ERROR,   "Cannot Divide By Zero" },
    { EXC_INVALID_OPCODE, "Invalid Operation Code" }
};

void exceptionDispatcher(int exception_id)
{
    /* Si querés, podés mantener IF=0 durante todo el manejo para evitar anidación.
       Si necesitás teclado, documentá por qué habilitás IF. */
    for (unsigned i = 0; i < sizeof(EXC_TABLE)/sizeof(EXC_TABLE[0]); i++) {
        if (exception_id == EXC_TABLE[i].id) {
            handle_exception(EXC_TABLE[i].msg);
            return;
        }
    }

    /* Desconocida pero la mostramos igual */
    handle_exception("Unknown Exception");
}

/* -------- helpers -------- */

static void handle_exception(const char *title)
{
    /* Si decidís habilitar interrupciones para leer teclado, hacelo consciente:
       _sti();   // <- Sólo si tu teclado depende de IF, y sabiendo el riesgo.
    */

    vdPrint("Exception: ", PIXEL_VRAM);
    vdPrint(title, PIXEL_VRAM);
    vdNewline();

    print_registers_safe();



     
}

/* Evita crash si no hay snapshot todavía */
static void print_registers_safe(void)
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
        //vdPrintHex(regs[i]);
        vdNewline();
    }
}


