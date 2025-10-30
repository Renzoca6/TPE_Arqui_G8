#include "exceptions.h"
#include "registers.h"
#include "keyboard_handler.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include "interrupts.h"
#include <stdbool.h>

static void handle_exception(const char *title);

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
    /*selecciona la excepcion correspondiente */
    for (unsigned i = 0; i < sizeof(EXC_TABLE)/sizeof(EXC_TABLE[0]); i++) {
        if (exception_id == EXC_TABLE[i].id) {
            handle_exception(EXC_TABLE[i].msg);
            return;
        }
    }

    /* Desconocida pero la mostramos igual */
    handle_exception("Unknown Exception");
}

static void handle_exception(const char *title)
{
    vdPrint("Exception: ", PIXEL_VRAM);
    vdPrint(title, PIXEL_VRAM);
    vdNewline();

    print_registers();     
}




