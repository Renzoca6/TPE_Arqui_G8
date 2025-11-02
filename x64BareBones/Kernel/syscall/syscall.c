#include "syscall.h"
#include <stdint.h>
#include "video.h"
#include "keyboard_handler.h"
#include "realTimeClock.h"
#include "benchmark.h"
#include "timer.h"
#include "audio.h"


extern void enable_interrupts(void);
extern void disable_interrupts(void);

#define MAX_SYSCALLS 18

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
static void syscall_get_ms_since_boot(uint64_t *registers);
static void syscall_sleep_ms(uint64_t *registers);
static void syscall_kill_system(uint64_t *registers);
static void syscall_audio(uint64_t *registers);
static void sycall_put_frame(uint64_t *registers);


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
    syscall_putPixel,           // 12: SYS_PUT_PIXEL
    syscall_get_ms_since_boot, // 13: syscall_get_ms_since_boot
    syscall_sleep_ms,          // 14: syscall_sleep_ms
    syscall_kill_system,        //15: syscall_kill_system
    syscall_audio,              //16: sycall_audio
    sycall_put_frame,           //17 putfrmae
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
static void sycall_put_frame(uint64_t *registers){
    putFrame();
}

static void syscall_audio(uint64_t *registers) {
    uint64_t op    = registers[13];          // RBX
    uint32_t freq  = (uint32_t)registers[12]; // RCX
    uint32_t dur   = (uint32_t)registers[11]; // RDX

    switch (op) {
        case 0: // play
            if (freq == 0) {
                registers[14] = (uint64_t)-1;
                return;
            }
            play_sound(freq);
            registers[14] = 0;
            break;

        case 1: // stop
            stop_sound();
            registers[14] = 0;
            break;

        case 2: // beep
            // beep probablemente use sleep_ms, así que habilitamos IRQ
            enable_interrupts();
            beep(freq, dur);
            registers[14] = 0;
            break;

        default:
            registers[14] = (uint64_t)-1;
            break;
    }
}

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


static void syscall_sleep_ms(uint64_t *registers) {
    // el userland te manda el ms en RBX → registers[13]
    uint64_t ms = registers[13];

    // MUY importante: estamos dentro de una interrupción, así que
    // tenemos que volver a habilitar las interrupciones para que
    // el timer pueda seguir incrementando g_ticks.
    enable_interrupts();

    sleep_ms(ms);
}
static void syscall_get_ms_since_boot(uint64_t *registers){
    registers[14] = timer_ms_since_boot();
}


static void syscall_putPixel(uint64_t *  registers){
    PixelTarget target;
    if (registers[8] == 0){
        target = PIXEL_VRAM;
    }else{ target = PIXEL_BACK;}
    
    putPixel(registers[13], registers[12], registers[11] ,target);

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

    clearKeyboardBuffer();
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
    disable_interrupts();
}

static void syscall_getchar(uint64_t *registers) {
    // args desde userland
    char *user_buf     = (char *)registers[13];  // RBX: puntero al buffer
    uint64_t max_len   = registers[12];          // RCX: tamaño máximo
    const uint64_t timeout_ms = 10;

    enable_interrupts();

    uint64_t last_event_time = timer_ms_since_boot();
    uint64_t written = 0;
    while (1) {
        if (hasNextKey()) {
            KeyBufferStruct k = getNextKey();
            if (k.is_pressed) {
                if (written < max_len) {
                    char ch = (char)k.key;

                    // --- convertir a minúscula si es letra ---
                    if (ch >= 'A' && ch <= 'Z') {
                        ch = ch + ('a' - 'A');
                    }

                    user_buf[written] = ch;
                    written++;
                }
                last_event_time = timer_ms_since_boot();

                if (written >= max_len) {
                    break;
                }
            }
        }

        if (timer_ms_since_boot() - last_event_time > timeout_ms) {
            break;
        }
    }

    if (written < max_len) {
        user_buf[written] = '\0';
    }

    // devolver cantidad de teclas leídas
    registers[14] = written;    // RAX
    disable_interrupts();
}


static void syscall_print_registers(uint64_t *registers) {
    uint64_t *user_buffer = (uint64_t *)registers[13];  // RBX = puntero al buffer del usuario
    
    if (areRegsSaved()) {
        uint64_t *saved = getSavedRegs();
        // Copiar los REG_COUNT (20) registros al buffer del usuario
        for (int i = 0; i < 20; i++) {  // REG_COUNT = 20
            user_buffer[i] = saved[i];
        }
        registers[14] = 0;  // RAX = 0 (success)
    } else {
        registers[14] = -1;  // RAX = -1 (no hay snapshot)
    }
}
#include "io.h"
static void syscall_kill_system(uint64_t *registers){
    outb(0xF4, 0x00);
}