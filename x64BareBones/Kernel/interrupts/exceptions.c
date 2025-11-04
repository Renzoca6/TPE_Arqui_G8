#include "exceptions.h"
#include "keyboard_handler.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include "interrupts.h"
#include <stdbool.h>
#include <stddef.h>

// ---------------------------------------------------------------------
// Register names and count
// ---------------------------------------------------------------------
#define REG_COUNT 20

// Register indices machea el orden de pushState 
enum {
    REG_R15 = 0, REG_R14, REG_R13, REG_R12, REG_R11, REG_R10, REG_R9, REG_R8,
    REG_RSI, REG_RDI, REG_RBP, REG_RDX, REG_RCX, REG_RBX, REG_RAX,
    REG_RIP, REG_CS, REG_RFLAGS, REG_RSP, REG_SS
};

/* Register names matching pushState order */
static const char * const REG_NAMES[REG_COUNT] = {
    "R15", "R14", "R13", "R12", "R11", "R10", "R9", "R8",
    "RSI", "RDI", "RBP", "RDX", "RCX", "RBX", "RAX", 
    "RIP", "CS", "RFLAGS", "RSP", "SS"
};

// Declaraciones externas para reset de userland
extern void * getStackBase();
extern void * const shellAddress;  // Definido en kernel.c

// Variable estática para guardar el RSP inicial de userland
static uint64_t initial_userland_rsp = 0;

static void printHexPadded(uint64_t v);
static void print_registers(void);
static void handle_exception(const char *title, uint64_t *registers);

/* Tabla de nombres por ID que te interesan (podés extenderla si luego agregás más) */
static const struct {
    int         id;
    const char *msg;
} EXC_TABLE[] = {
    { EXC_DIVIDE_ERROR,   "Cannot Divide By Zero" },
    { EXC_INVALID_OPCODE, "Invalid Operation Code" }
};


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

// ---------------------------------------------------------------------
// Imprime los registros para excepciones
// ---------------------------------------------------------------------
static void print_registers(void) {
    const uint64_t *regs = getSavedRegs();

    if (regs == NULL) {
        vdPrint("Register snapshot not available", PIXEL_VRAM);
        vdNewline();
        return;
    }

    //Encabezado decorativo de la caja 
    vdPrint("+----------------------------------------------------+", PIXEL_VRAM); vdNewline();
    vdPrint("|                  REGISTER SNAPSHOT                 |", PIXEL_VRAM); vdNewline();
    vdPrint("+----------------------------------------------------+", PIXEL_VRAM); vdNewline();

    ///Imprime los registros en dos columnas 
    for (int i = 0; i < REG_COUNT; i += 2) {
        char namebuf[10];

       //inicio: borde izquierdo 
        vdPrint("|", PIXEL_VRAM);

        // Nombre columna izquierda (rellenar a 6 caracteres) 
        const char *n = REG_NAMES[i];
        int j = 0;
        while (j < 6 && n[j]) { namebuf[j] = n[j]; j++; }
        while (j < 6)          { namebuf[j++] = ' '; }
        namebuf[6] = ':'; namebuf[7] = ' '; namebuf[8] = '\0';
        vdPrint(namebuf, PIXEL_VRAM);
        printHexPadded(regs[i]);   // imprime 16 caracteres hexadecimales

        // espacio entre columnas 
        vdPrint("    ", PIXEL_VRAM);

        // Columna derecha 
        if (i + 1 < REG_COUNT) {
            const char *n2 = REG_NAMES[i + 1];
            int k = 0;
            while (k < 6 && n2[k]) { namebuf[k] = n2[k]; k++; }
            while (k < 6)          { namebuf[k++] = ' '; }
            namebuf[6] = ':'; namebuf[7] = ' '; namebuf[8] = '\0';
            vdPrint(namebuf, PIXEL_VRAM);
            printHexPadded(regs[i + 1]);
        }

        ///fin: borde derecho 
        vdPrint("|", PIXEL_VRAM);
        vdNewline();
    }

    
    vdPrint("+----------------------------------------------------+", PIXEL_VRAM);
    vdNewline();
}

// ---------------------------------------------------------------------
// Dispatcher de excepciones
// ---------------------------------------------------------------------
void exceptionDispatcher(int exception_id, uint64_t *registers) {
    /* selecciona la excepcion correspondiente */
    for (unsigned i = 0; i < sizeof(EXC_TABLE) / sizeof(EXC_TABLE[0]); i++) {
        if (exception_id == EXC_TABLE[i].id) {
            handle_exception(EXC_TABLE[i].msg, registers);
            return;
        }
    }

    /* Desconocida pero la mostramos igual */
    handle_exception("Unknown Exception", registers);
}

// ---------------------------------------------------------------------
// Manejo común de excepción
// ---------------------------------------------------------------------
static void handle_exception(const char *title, uint64_t *registers) {
    // Guardar el RSP inicial de userland la primera vez
    if (initial_userland_rsp == 0) {
        initial_userland_rsp = registers[REG_RSP];
    }

    vdNewline();
    vdPrint("Exception: ", PIXEL_VRAM);
    vdPrint(title, PIXEL_VRAM);
    vdNewline();

    // Guardar el snapshot de los registros en el momento de la excepción
    updateRegs(registers);
    print_registers();     

    vdNewline();
    vdPrint("Press ENTER to continue...", PIXEL_VRAM);

    waitForEnter();
    vdNewline();
    vdNewline();

    // Resetear RIP y RSP para volver al entry point de userland
    // (evita loop infinito al intentar re-ejecutar la instrucción que causó la excepción)
    uint64_t new_rip = (uint64_t)shellAddress;
    
    // Usar el RSP inicial guardado, no getStackBase() que puede variar
    uint64_t new_rsp = initial_userland_rsp;

    // El frame de registros en el stack tiene este layout:
    // pushState hace: RAX, RBX, RCX, RDX, RBP, RDI, RSI, R8..R15 (15 pushes)
    // Luego el CPU ya había pusheado (en orden): RIP, CS, RFLAGS, RSP, SS
    // Ahora el enum refleja correctamente este orden
    registers[REG_RIP] = new_rip;  // Resetear RIP al entry point de shell
    registers[REG_RSP] = new_rsp;  // Resetear RSP al valor inicial guardado
}
