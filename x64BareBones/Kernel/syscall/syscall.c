//#include "../include/syscall.h"
#include <stdint.h>
#include "video.h"
#include "keyboard_handler.h"
#include "realTimeClock.h"
#include "benchmark.h"
#include "registers.h"

extern void enable_interrupts(void);

#define MAX_SYSCALLS 13

// Forward declarations de handlers
static void syscall_read(uint64_t *registers);
static void syscall_write(uint64_t *registers);
static void syscall_clearwindow(uint64_t *registers);
static void syscall_getDate(uint64_t *registers);
static void syscall_resize(uint64_t *registers);
static void syscall_benchmark(uint64_t *registers);
static void syscall_write_at_VRAM(uint64_t *registers);
static void syscall_write_at_BACK(uint64_t *registers);
static void syscall_present_fullframe(uint64_t *registers);
static void syscall_getScreen_Info(uint64_t *registers);
static void syscall_print_registers(uint64_t *registers);
static void syscall_getchar(uint64_t *registers);
static void syscall_putPixel(uint64_t *registers);

// Tipo para punteros a funciones handler
typedef void (*SysCallHandler)(uint64_t *);

// Array de handlers indexado por número de syscall
static SysCallHandler sysCallHandlers[MAX_SYSCALLS] = {
    syscall_read,              // 0: SYS_READ
    syscall_write,             // 1: SYS_WRITE
    syscall_clearwindow,       // 2: SYS_CLEAR_WINDOW
    syscall_getDate,           // 3: SYS_GET_DATE
    syscall_resize,            // 4: SYS_RESIZE
    syscall_benchmark,         // 5: SYS_BENCHMARK
    syscall_write_at_VRAM,     // 6: SYS_WRITE_AT_VRAM
    syscall_write_at_BACK,     // 7: SYS_WRITE_AT_BACK
    syscall_present_fullframe, // 8: SYS_PRESENT_FULLFRAME
    syscall_getScreen_Info,    // 9: SYS_GET_SCREEN_INFO
    syscall_print_registers,   // 10: SYS_PRINT_REGISTERS
    syscall_getchar,           // 11: SYS_GETCHAR
    syscall_putPixel           // 12: SYS_PUT_PIXEL
};

// Dispatcher principal - recibe puntero al stack frame con registros
void syscall_handler (uint64_t rax, uint64_t *registers) {
    // Layout de registers (después de pushState):
    // registers[0] = r15
    // registers[1] = r14
    // registers[2] = r13
    // registers[3] = r12
    // registers[4] = r11
    // registers[5] = r10
    // registers[6] = r9
    // registers[7] = r8
    // registers[8] = rsi
    // registers[9] = rdi
    // registers[10] = rbp
    // registers[11] = rdx
    // registers[12] = rcx
    // registers[13] = rbx
    // registers[14] = rax
    
    
    if (rax < MAX_SYSCALLS) {
        sysCallHandlers[rax](registers);
    } else {
        registers[14] = -1;  // Error: syscall inválida
    }
}

// ============================================================================
// HANDLERS DE SYSCALLS
// ============================================================================

// Helper para write_at (usado por VRAM y BACK)
static void syscall_write_at(uint64_t *registers, PixelTarget target) {
    // Layout snapshot: [13]=RBX, [12]=RCX, [11]=RDX, [8]=RSI, [9]=RDI
    const char *str   = (const char *)registers[13];   // RBX: texto
    int col           = (int)registers[12];            // RCX: x en celdas
    int fil           = (int)registers[11];            // RDX: y en celdas
    uint32_t fColor   = (uint32_t)registers[8];        // RSI: color fuente
    uint32_t bgColor  = (uint32_t)registers[9];        // RDI: color fondo

    vdPrintStyled_AT(str, col, fil, fColor, bgColor, target);
}

static void syscall_putPixel(uint64_t *registers) {
    // En userland: 0 = PIXEL_VRAM, 1 = PIXEL_BACK
    PixelTarget target = (registers[8] == 0) ? PIXEL_VRAM : PIXEL_BACK;   // RSI
    putPixel(registers[13], registers[12], registers[11], target);   // RBX, RCX, RDX
}

static void syscall_write_at_VRAM(uint64_t *registers) {
    syscall_write_at(registers, PIXEL_VRAM);
}

static void syscall_write_at_BACK(uint64_t *registers) {
    syscall_write_at(registers, PIXEL_BACK);
}

static void syscall_getScreen_Info(uint64_t *registers) {
    uint64_t which = registers[13];   // RBX: 0 = height, 1 = width
    if (which == 0) {
        registers[14] = vdGetHeight();  // Retornar en RAX
    } else {
        registers[14] = vdGetWidth();   // Retornar en RAX
    }
}

static void syscall_present_fullframe(uint64_t *registers) {
    present_fullframe();
}

static void syscall_write(uint64_t *registers) {
    if (registers[13] == 1) {  // RBX
        vdPrint((const char*)registers[12], PIXEL_VRAM);  // RCX
    } else {
        vdPrintStyled((const char*)registers[12], 0x00ffffff, 0x00FF0000, PIXEL_VRAM);
    }
}

static void syscall_benchmark(uint64_t *registers) {
    // Habilitar interrupciones mientras corre el benchmark
    // para que el timer (IRQ0) avance y timer_ms_since_boot() progrese.
    enable_interrupts();
    uint64_t which = registers[13];  // RBX: cuál benchmark correr
    uint64_t res = 0;

    switch (which) {
        case 0: res = benchmark_fps();             break;
        case 1: res = benchmark_floating_point();  break;
        case 2: res = benchmark_hardware_access(); break;
        default: res = (uint64_t)-1;               break;
    }

    registers[14] = res;  // Retornar resultado en RAX
}

static void syscall_resize(uint64_t *registers) {
    int s = str_to_uint_ignore_sign((char *)registers[13]);  // RBX

    // Validar / clamp
    if (s < 1) s = 1;
    if (s > 4) s = 4;

    vdSetFontScale(s);
}

static void syscall_getDate(uint64_t *registers) {
    if (registers[13] == 1) {  // RBX
       vdPrint(getDateString(), PIXEL_VRAM);
       return;
    }
    vdPrint(getTimeString(), PIXEL_VRAM);
}

static void syscall_clearwindow(uint64_t *registers) {
    vdclearScreenDB(registers[13]);  // RBX
}

static void syscall_read(uint64_t *registers) {
    char *buf = (char *)registers[13];  // RBX
    int   size = 0;

    clearKeyBoardBuffer();
    enable_interrupts();

    while (1) {
        if (hasNextKey()) {
            KeyBufferStruct k = getNextKey();
            if (k.is_pressed) {
                if (k.key == '\n') {
                    vdPrintChar('\n', PIXEL_VRAM);
                    buf[size] = '\0';
                    registers[14] = (uint64_t)size;  // Retornar en RAX
                    return;
                } else if (k.key == '\b') {
                    if (size > 0) {
                        size--;
                        buf[size] = '\0';
                        vdBackSpace(PIXEL_VRAM);
                    }
                } else if (k.key) {
                    if (size + 1 < 256) {
                        buf[size++] = k.key;
                        vdPrintChar(k.key, PIXEL_VRAM);
                    } else {
                        vdPrintChar('\n', PIXEL_VRAM);
                        buf[size] = '\0';
                        registers[14] = (uint64_t)size;
                        return;
                    }
                }
            }
        }
    }
}

static void syscall_getchar(uint64_t *registers) {
    enable_interrupts();
    uint64_t start = timer_ms_since_boot();
    const uint64_t timeout_ms = 50;

    while (1) {
        if (hasNextKey()) {
            KeyBufferStruct k = getNextKey();
            if (k.is_pressed) {
                registers[14] = (uint64_t)k.key;  // Retornar en RAX
                return;
            }
        }

        if (timer_ms_since_boot() - start > timeout_ms) {
            registers[14] = 0;  // Timeout
            return;
        }
    }
}

static void syscall_print_registers(uint64_t *registers) {
    // Si hay snapshot de Shift+Tab, limpiarlo
    if (areRegsSaved()) {
        clearRegsSaved();
    }
    print_registers();
}
