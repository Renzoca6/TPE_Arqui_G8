// ---------------------------------------------------------------------
// syscall_call.c
// Wrappers de userland para llamar a las syscalls de kernel
// ---------------------------------------------------------------------
#include "../include/syscall_call.h"

extern void     sys_resize(char *N_times);
extern int      sys_write(int fb, const char *buf);
extern int      sys_read(char *buf);
extern void     sys_clearwindow(uint32_t *color);
extern int      sys_date_time(int type);
extern uint64_t sys_benchmark(int type);
extern uint64_t sys_write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
extern uint64_t sys_write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor);
extern void     sys_present_fullframe(void);
extern int      sys_get_screen_info(int aux);
extern void     sys_putPixel(uint32_t color, uint32_t x, uint32_t y, uint32_t target);
extern uint64_t sys_getchar(char *buffer, uint64_t max_len);
extern int      sys_print_registers(uint64_t *buffer);   // retorna 0 si ok, -1 si no hay snapshot
extern void     touch_regs();
extern void     sys_sleep_ms(uint64_t ms);
extern uint64_t sys_get_ms_since_boot(void);
extern uint64_t sys_audio(uint64_t op, uint32_t freq, uint32_t dur_ms);
extern uint64_t sys_putframe(void);

#define STDERR   0
#define STDOUT  1
#define REG_COUNT 20

// ---------------------------------------------------------------------
// Video / frame
// ---------------------------------------------------------------------
void put_frame(void) {
    sys_putframe();
}

// ---------------------------------------------------------------------
// Tiempo
// ---------------------------------------------------------------------
uint64_t get_ms_since_boot(void) {
    return sys_get_ms_since_boot();
}

void sleep_ms(uint64_t ms) {
    sys_sleep_ms(ms);
}

// ---------------------------------------------------------------------
// Gráfico: target: 0 = PIXEL_VRAM, 1 = PIXEL_BACK
// ---------------------------------------------------------------------
void putPixel(uint32_t color, uint32_t x, uint32_t y, int target) {
    sys_putPixel(color, x, y, (uint32_t)target);
}

// ---------------------------------------------------------------------
// printeo de texto
// ---------------------------------------------------------------------
int write(const char *buf) {
    return sys_write(STDOUT, buf);
}

int println(const char *buf) {
    sys_write(STDOUT, buf);
    write("\n");
    return 1;
}

int printError(const char *buf) {
    return (sys_write(STDERR, buf) && write("\n"));
}

static void printHexPadded(uint64_t v) {
    char        buf[17];
    const char *hex = "0123456789ABCDEF";

    for (int i = 0; i < 16; i++) {
        uint8_t nibble = (v >> ((15 - i) * 4)) & 0xF;
        buf[i] = hex[nibble];
    }
    buf[16] = '\0';
    write(buf);
}

// ---------------------------------------------------------------------
// Screen info
// ---------------------------------------------------------------------
uint64_t get_screen_height(void) {
    return sys_get_screen_info(0);     // 0 = height
}

uint64_t get_screen_width(void) {
    return sys_get_screen_info(1);     // 1 = width
}

// ---------------------------------------------------------------------
// Present / backbuffer
// ---------------------------------------------------------------------
void present_fullframe(void) {
    sys_present_fullframe();
}

void write_at_back(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor) {
    sys_write_at_back(str, col, fil, fColor, bgColor);
}

void write_at_vram(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor) {
    sys_write_at_vram(str, col, fil, fColor, bgColor);
}


// ---------------------------------------------------------------------
// Lectura de  chars
// ---------------------------------------------------------------------
int get_multiple_chars_sys(char *buf, uint64_t max_len) {
    return (int)sys_getchar(buf, max_len);
}

char getchar(void) {
    char c;
    int  n = sys_getchar(&c, 1);
    return (n > 0) ? c : 0;
}

int read(char *buf) {
    return sys_read(buf);
}

// ---------------------------------------------------------------------
// Limpieza de pantalla
// ---------------------------------------------------------------------
void clearwindow(uint32_t *color) {
    sys_clearwindow(color);
}

// ---------------------------------------------------------------------
// Resize
// ---------------------------------------------------------------------
void do_resize(char *N_Times) {
    sys_resize(N_Times);
}

// ---------------------------------------------------------------------
// Date / Time
// ---------------------------------------------------------------------
void get_time(void) {
    sys_date_time(0);
}

void get_date(void) {
    sys_date_time(1);
}

// ---------------------------------------------------------------------
// Audio
// ---------------------------------------------------------------------
void audio_play(uint32_t freq_hz) {
    // op=0, freq=freq_hz, dur=0
    sys_audio(0, freq_hz, 0);
}

void audio_stop(void) {
    // op=1
    sys_audio(1, 0, 0);
}

void audio_beep(uint32_t freq_hz, uint32_t duration_ms) {
    // op=2
    sys_audio(2, freq_hz, duration_ms);
}

// ---------------------------------------------------------------------
// Benchmarks
// ---------------------------------------------------------------------
uint64_t do_benchmark_fps(void) {
    return sys_benchmark(0);
}

uint64_t do_benchmark_floating_point(void) {
    return sys_benchmark(1);
}

uint64_t do_benchmark_hardware_access(void) {
    return sys_benchmark(2);
}

uint64_t do_benchmark_timer_latency(void) {
    return sys_benchmark(3);
}


// orde de los registros según pushState
static const char *const REG_NAMES[REG_COUNT] = {
    "R15", "R14", "R13", "R12", "R11", "R10", "R9", "R8",
    "RSI", "RDI", "RBP", "RDX", "RCX", "RBX", "RAX",
    "RIP", "CS", "RFLAGS", "RSP", "SS"
};


void print_registers(void) {
    uint64_t regs[REG_COUNT];

    // syscall que copia los registros al buffer
    int result = sys_print_registers(regs);

    if (result == -1) {
        println("Register snapshot not available, press SHIFT+TAB");
        return;
    }

    /* Encabezado decorativo de la caja */
    println("+----------------------------------------------------+");
    println("|                  REGISTER SNAPSHOT                 |");
    println("+----------------------------------------------------+");

    /* Imprime los registros en dos columnas */
    for (int i = 0; i < REG_COUNT; i += 2) {
        char namebuf[10];

        // left border
        write("|");

        // Left column name (pad to 6 chars)
        const char *n = REG_NAMES[i];
        int         j = 0;
        while (j < 6 && n[j]) { namebuf[j] = n[j]; j++; }
        while (j < 6)         { namebuf[j++] = ' '; }
        namebuf[6] = ':'; namebuf[7] = ' '; namebuf[8] = '\0';
        write(namebuf);
        printHexPadded(regs[i]);

        // spacing
        write("    ");

        // Right column
        if (i + 1 < REG_COUNT) {
            const char *n2 = REG_NAMES[i + 1];
            int         k  = 0;
            while (k < 6 && n2[k]) { namebuf[k] = n2[k]; k++; }
            while (k < 6)          { namebuf[k++] = ' '; }
            namebuf[6] = ':'; namebuf[7] = ' '; namebuf[8] = '\0';
            write(namebuf);
            printHexPadded(regs[i + 1]);
        }

        // right border
        write("|");
        write("\n");
    }

    /* Pie decorativo (final de la caja) */
    println("+----------------------------------------------------+");
}

void printRegisters(void) {
    print_registers();
}
