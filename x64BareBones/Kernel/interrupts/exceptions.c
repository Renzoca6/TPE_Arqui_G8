#include "exceptions.h"
#include "registers.h"
#include "keyboard_handler.h"
#include "video.h"   /* vdPrint, vdNewline, vdPrintHex, vdClear, etc. */
#include "interrupts.h"
#include <stdbool.h>

// Declaraciones externas para reset de userland
extern void * getStackBase();
extern void * const shellAddress;  // Definido en kernel.c

// Variable estática para guardar el RSP inicial de userland
static uint64_t initial_userland_rsp = 0;

static void handle_exception(const char *title, uint64_t * registers);

/* Tabla de nombres por ID que te interesan (podés extenderla si luego agregás más) */
static const struct {
    int        id;
    const char *msg;
} EXC_TABLE[] = {
    { EXC_DIVIDE_ERROR,   "Cannot Divide By Zero" },
    { EXC_INVALID_OPCODE, "Invalid Operation Code" }
};

void exceptionDispatcher(int exception_id, uint64_t * registers)
{
    /*selecciona la excepcion correspondiente */
    for (unsigned i = 0; i < sizeof(EXC_TABLE)/sizeof(EXC_TABLE[0]); i++) {
        if (exception_id == EXC_TABLE[i].id) {
            handle_exception(EXC_TABLE[i].msg, registers);
            return;
        }
    }

    /* Desconocida pero la mostramos igual */
    handle_exception("Unknown Exception", registers);
}

static void handle_exception(const char *title, uint64_t * registers)
{
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
    // 
    // Ahora el enum refleja correctamente este orden
    
    registers[REG_RIP] = new_rip;  // Resetear RIP al entry point de shell
    registers[REG_RSP] = new_rsp;  // Resetear RSP al valor inicial guardado
}




