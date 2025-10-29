#include "exceptions.h"
#include "registers.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include <stddef.h>        /* NULL */
#include <stdbool.h>

#define ENTER_CHAR '\n'    /* Unificamos comparación por carácter */

static void handle_exception(const char *title);
static void wait_for_enter(void);
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

    vdPrint("Exception: ");
    vdPrint(title);
    vdNewline();

    print_registers_safe();

    vdNewline();
    vdPrint("Press ENTER to return to shell...");
    vdNewline();

    wait_for_enter();

    vdClear();
}

/* Evita crash si no hay snapshot todavía */
static void print_registers_safe(void)
{
    const uint64_t *regs = regs_get();

    if (regs == NULL) {
        vdPrint("Register snapshot not available.");
        vdNewline();
        return;
    }

    vdPrint("Register Status:");
    vdNewline();

    for (int i = 0; i < REG_COUNT; i++) {
        vdPrint((char*)REG_NAMES[i]);
        vdPrint(": ");
        vdPrintHex(regs[i]);
        vdNewline();
    }
}

/* Implementación minimalista basada en getChar()/readChar() del driver que tengas.
   Si no tenés esa API, dejá este helper como wrapper del que ya usabas. */
static void wait_for_enter(void)
{
    /* Si tu entorno expone getChar() no bloqueante, usar un loop:
       int c;
       while ((c = kbd_getchar()) != ENTER_CHAR) { }
       Para mantener compatibilidad con tu ejemplo, dejamos una versión genérica.
    */

    int c = 0;
    do {
        /* Reemplazá por tu función concreta de lectura de teclado/STDIN */
        c = vdReadChar();      /* <- Ejemplo. Si no existe, usá la que tengas. */
    } while (c != ENTER_CHAR);
}
