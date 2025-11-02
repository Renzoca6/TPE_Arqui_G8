#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <video.h>
#include <realTimeClock.h>
#include "interrupts_dispatcher.h"
#include "keyboard_handler.h"
#include "syscall.h"

// ---------------------------------------------------------------------
// Variables externas definidas en el linker
// ---------------------------------------------------------------------
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

// ---------------------------------------------------------------------
// Constantes
// ---------------------------------------------------------------------
static const uint64_t PageSize = 0x1000;
void *const shellAddress = (void *)0x400000;  // visible globalmente

typedef int (*EntryPoint)();

// ---------------------------------------------------------------------
// Funciones auxiliares
// ---------------------------------------------------------------------
void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase(void) {
    return (void *)(
        (uint64_t)&endOfKernel +
        PageSize * 8 -              // Tamaño del stack (32 KiB)
        sizeof(uint64_t)            // Comienza en el tope del stack
    );
}

// ---------------------------------------------------------------------
// Inicialización del kernel y carga de módulos
// ---------------------------------------------------------------------
void *initializeKernelBinary(void) {
    void *moduleAddresses[] = { shellAddress };

    loadModules(&endOfKernelBinary, moduleAddresses);
    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}

// ---------------------------------------------------------------------
// Punto de entrada principal
// ---------------------------------------------------------------------
int main(void) {
    init_interrupts();
    ((EntryPoint)shellAddress)();
    return 0;
}
