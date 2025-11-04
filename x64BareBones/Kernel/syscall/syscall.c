#include "syscall.h"
#include <stdint.h>
#include "video.h"
#include "keyboard_handler.h"
#include "realTimeClock.h"
#include "benchmark.h"
#include "timer.h"
#include "audio.h"
#include "io.h"

extern void enable_interrupts(void);
extern void disable_interrupts(void);

#define MAX_SYSCALLS 18

// ---------------------------------------------------------------------
// Handlers de cada syscall
// ---------------------------------------------------------------------
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

// ---------------------------------------------------------------------
// Tipo de puntero a función handler y tabla de syscalls
// ---------------------------------------------------------------------
typedef void (*SysCallHandler)(uint64_t *);

static SysCallHandler sysCallHandlers[MAX_SYSCALLS] = {
    syscall_read,               // 0: SYS_READ
    syscall_write,              // 1: SYS_WRITE
    syscall_clearwindow,        // 2: SYS_CLEAR_WINDOW
    syscall_getDate,            // 3: SYS_GET_DATE
    syscall_resize,             // 4: SYS_RESIZE
    syscall_benchmark,          // 5: SYS_BENCHMARK
    syscall_write_at_VRAM,      // 6: SYS_WRITE_AT_VRAM
    syscall_write_at_BACK,      // 7: SYS_WRITE_AT_BACK
    syscall_present_fullframe,  // 8: SYS_PRESENT_FULLFRAME
    syscall_getScreen_Info,     // 9: SYS_GET_SCREEN_INFO
    syscall_print_registers,    // 10: SYS_PRINT_REGISTERS
    syscall_getchar,            // 11: SYS_GETCHAR
    syscall_putPixel,           // 12: SYS_PUT_PIXEL
    syscall_get_ms_since_boot,  // 13: SYS_GET_MS_SINCE_BOOT
    syscall_sleep_ms,           // 14: SYS_SLEEP_MS
    syscall_kill_system,        // 15: SYS_KILL_SYSTEM
    syscall_audio,              // 16: SYS_AUDIO
    sycall_put_frame,           // 17: SYS_PUT_FRAME
};

// ---------------------------------------------------------------------
// Dispatcher principal: recibe el puntero al stack frame con registros
// ---------------------------------------------------------------------
void syscall_handler(uint64_t rax, uint64_t *registers) {
    // Layout de registers (después de pushState):
    // [0] = R15, [1] = R14, [2] = R13, [3] = R12, [4] = R11,
    // [5] = R10, [6] = R9,  [7] = R8,  [8] = RSI, [9] = RDI,
    // [10] = RBP, [11] = RDX, [12] = RCX, [13] = RBX, [14] = RAX

    if (rax < MAX_SYSCALLS) {
        sysCallHandlers[rax](registers);
    } else {
        registers[14] = -1;  // Error: syscall inválida
    }
}

// =====================================================================
// HANDLERS DE SYSCALLS
// =====================================================================

// ---------------------------------------------------------------------
// Dibuja un frame completo (limpia el back buffer)
// ---------------------------------------------------------------------
static void sycall_put_frame(uint64_t *registers) {
    putFrame();
}

// ---------------------------------------------------------------------
// Control del audio del speaker (play / stop / beep)
// ---------------------------------------------------------------------
static void syscall_audio(uint64_t *registers) {
    uint64_t op   = registers[13];           // RBX
    uint32_t freq = (uint32_t)registers[12]; // RCX
    uint32_t dur  = (uint32_t)registers[11]; // RDX

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
            beep(freq, dur);
            registers[14] = 0;
            break;

        default:
            registers[14] = (uint64_t)-1;
            break;
    }
}

// ---------------------------------------------------------------------
// Helper para write_at (usado por VRAM y BACK)
// ---------------------------------------------------------------------
static void syscall_write_at(uint64_t *registers, PixelTarget target) {
    // Layout snapshot: [13]=RBX, [12]=RCX, [11]=RDX, [8]=RSI, [9]=RDI
    const char *str   = (const char *)registers[13];  // RBX: texto
    int col           = (int)registers[12];           // RCX: columna (x)
    int fil           = (int)registers[11];           // RDX: fila (y)
    uint32_t fColor   = (uint32_t)registers[8];       // RSI: color fuente
    uint32_t bgColor  = (uint32_t)registers[9];       // RDI: color fondo

    vdPrintStyled_AT(str, col, fil, fColor, bgColor, target);
}

// ---------------------------------------------------------------------
// Pausa el sistema por X milisegundos (sleep activo con hlt)
// ---------------------------------------------------------------------
static void syscall_sleep_ms(uint64_t *registers) {
    // El userland te manda el tiempo en ms por RBX → registers[13]
    uint64_t ms = registers[13];

    sleep_ms(ms);
}

// ---------------------------------------------------------------------
// Devuelve los milisegundos desde el arranque
// ---------------------------------------------------------------------
static void syscall_get_ms_since_boot(uint64_t *registers) {
    registers[14] = timer_ms_since_boot();
}

// ---------------------------------------------------------------------
// Dibuja un píxel (userland → kernel → driver de video)
// ---------------------------------------------------------------------
static void syscall_putPixel(uint64_t *registers) {
    PixelTarget target;

    if (registers[8] == 0) {
        target = PIXEL_VRAM;
    } else {
        target = PIXEL_BACK;
    }

    putPixel(registers[13], registers[12], registers[11], target);
}

// ---------------------------------------------------------------------
// Escritura directa en VRAM o BACK buffer
// ---------------------------------------------------------------------
static void syscall_write_at_VRAM(uint64_t *registers) {
    syscall_write_at(registers, PIXEL_VRAM);
}

static void syscall_write_at_BACK(uint64_t *registers) {
    syscall_write_at(registers, PIXEL_BACK);
}

// ---------------------------------------------------------------------
// Devuelve información de la pantalla (ancho/alto)
// ---------------------------------------------------------------------
static void syscall_getScreen_Info(uint64_t *registers) {
    uint64_t which = registers[13];   // RBX: 0 = height, 1 = width

    if (which == 0) {
        registers[14] = vdGetHeight();  // RAX ← alto
    } else {
        registers[14] = vdGetWidth();   // RAX ← ancho
    }
}

// ---------------------------------------------------------------------
// Copia el back buffer completo al VRAM (present frame)
// ---------------------------------------------------------------------
static void syscall_present_fullframe(uint64_t *registers) {
    present_fullframe();
}

// ---------------------------------------------------------------------
// Imprime texto básico (write)
// ---------------------------------------------------------------------
static void syscall_write(uint64_t *registers) {
    if (registers[13] == 1) {  // RBX
        vdPrint((const char *)registers[12], PIXEL_VRAM);  // RCX
    } else {
        vdPrintStyled((const char *)registers[12], 0x00FFFFFF, 0x00FF0000, PIXEL_VRAM);
    }
}

// ---------------------------------------------------------------------
// Ejecuta benchmarks del sistema
// ---------------------------------------------------------------------
static void syscall_benchmark(uint64_t *registers) {
    // Habilitamos interrupciones mientras corre el benchmark
    enable_interrupts();

    uint64_t which = registers[13];  // RBX: cuál benchmark correr
    uint64_t res = 0;

    switch (which) {
        case 0: res = benchmark_fps();             break;
        case 1: res = benchmark_floating_point();  break;
        case 2: res = benchmark_hardware_access(); break;
        case 3: res = benchmark_timer_latency();   break;
        default: res = (uint64_t)-1;               break;
    }

    registers[14] = res;  // Resultado en RAX
    disable_interrupts();
}

// ---------------------------------------------------------------------
// Cambia el tamaño de la fuente de video
// ---------------------------------------------------------------------
static void syscall_resize(uint64_t *registers) {
    int s = str_to_uint_ignore_sign((char *)registers[13]);  // RBX

    // Validar / clamp
    if (s < 1) s = 1;
    if (s > 4) s = 4;

    vdSetFontScale(s);
}

// ---------------------------------------------------------------------
// Obtiene la fecha y hora del RTC
// ---------------------------------------------------------------------
static void syscall_getDate(uint64_t *registers) {
    if (registers[13] == 1) {  // RBX
        vdPrint(getDateString(), PIXEL_VRAM);
        return;
    }
    vdPrint(getTimeString(), PIXEL_VRAM);
}

// ---------------------------------------------------------------------
// Limpia la pantalla
// ---------------------------------------------------------------------
static void syscall_clearwindow(uint64_t *registers) {
    vdclearScreenDB(registers[13]);  // RBX: color
}

// ---------------------------------------------------------------------
// Lee caracteres desde el teclado (modo bloqueante)
// ---------------------------------------------------------------------
static void syscall_read(uint64_t *registers) {
    char *buf = (char *)registers[13];  // RBX
    int size = 0;

    clearKeyboardBuffer();
    enable_interrupts();

    while (1) {
        if (hasNextKey()) {
            KeyBufferStruct k = getNextKey();
            if (k.is_pressed) {
                if (k.key == '\n') {
                    vdPrintChar('\n', PIXEL_VRAM);
                    buf[size] = '\0';
                    registers[14] = (uint64_t)size;  // RAX ← cantidad leída
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

// ---------------------------------------------------------------------
// Lee una tecla con timeout corto (10 ms)
// ---------------------------------------------------------------------
static void syscall_getchar(uint64_t *registers) {
    char *user_buf   = (char *)registers[13];  // RBX: puntero al buffer
    uint64_t max_len = registers[12];          // RCX: tamaño máximo
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

                    // Convertir a minúscula si es letra
                    if (ch >= 'A' && ch <= 'Z') {
                        ch = ch + ('a' - 'A');
                    }

                    user_buf[written++] = ch;
                }

                last_event_time = timer_ms_since_boot();
                if (written >= max_len) break;
            }
        }

        if (timer_ms_since_boot() - last_event_time > timeout_ms) {
            break;
        }
    }

    if (written < max_len)
        user_buf[written] = '\0';

    registers[14] = written;  // RAX ← cantidad leída
    disable_interrupts();
}

// ---------------------------------------------------------------------
// Imprime el snapshot de registros guardado por Shift+Tab
// ---------------------------------------------------------------------
static void syscall_print_registers(uint64_t *registers) {
    uint64_t *user_buffer = (uint64_t *)registers[13];  // RBX = puntero al buffer del usuario
    
    if (areRegsSaved()) {
        uint64_t *saved = getSavedRegs();
        for (int i = 0; i < 20; i++) {  // REG_COUNT = 20
            user_buffer[i] = saved[i];
        }
        registers[14] = 0;  // Éxito
    } else {
        registers[14] = -1; // No hay snapshot
    }
}

// ---------------------------------------------------------------------
// Apaga el sistema (en QEMU manda el código de salida)
// ---------------------------------------------------------------------
static void syscall_kill_system(uint64_t *registers) {
    outb(0xF4, 0x00);
}
