// ---------------------------------------------------------------------
// benchmark_calculations.c  (userland)
// Benchmarks que se ejecutan desde userland y llaman a las syscalls
// ---------------------------------------------------------------------
#include <stdint.h>
#include "../include/syscall_call.h"
#include "../include/benchmark_calculations.h"

// ---------------------------------------------------------------------
// 1) Benchmark de latencia de syscall (userland)
//
// Mide cuántas veces por segundo puedo llamar a una syscall muy chica.
// Devuelve: llamadas por segundo.
// ---------------------------------------------------------------------
uint64_t syscall_latency(void) {
    const uint64_t dur_ms = 100;               // medir ~100 ms
    uint64_t start  = get_ms_since_boot();
    uint64_t end_ts = start + dur_ms;

    uint64_t calls = 0;

    while (get_ms_since_boot() < end_ts) {
        (void) get_ms_since_boot();            // llamado  a syscall 
        calls++;
    }

    uint64_t elapsed = get_ms_since_boot() - start;
    if (elapsed == 0) {
        elapsed = 1;
    }

    return (calls * 1000ull) / elapsed;        // llamadas por segundo
}

// ---------------------------------------------------------------------
// 2) Benchmark de putPixel desde userland
//
// Es la misma lógica que el benchmark de kernel, pero pasando por syscall.
// Dibuja bloques de PIXELS píxeles durante 50 ms.
// Devuelve: píxeles por segundo.
// ---------------------------------------------------------------------
uint64_t putpixel_user(void) {
    // tamaño de pantalla desde userland
    const uint32_t W = get_screen_width();
    const uint32_t H = get_screen_height();

    // mismo valor que en el kernel
    const uint32_t PIXELS = 5000;

    uint64_t written = 0;
    uint64_t start   = get_ms_since_boot();

    // durante 50 ms dibujo "bloques" de 5000 píxeles cada vez
    while ((get_ms_since_boot() - start) < 50) {
        for (uint32_t i = 0; i < PIXELS; i++) {
            uint32_t x = i % W;
            uint32_t y = (i / W) % H;

            // userland -> syscall -> kernel
            // 1 = BACK
            putPixel(0x00FF00, x, y, 1);
            written++;
        }
    }

    uint64_t dt = get_ms_since_boot() - start;
    if (dt == 0) {
        dt = 1;
    }

    return (written * 1000ull) / dt;           // píxeles por segundo
}

// ---------------------------------------------------------------------
// 3) Benchmark de escrituras de memoria desde userland
//
// Escribe repetidamente en un buffer estático y mide bytes/s.
// Devuelve: bytes por segundo.
// ---------------------------------------------------------------------
#define MEMWRITE_SIZE   (64u * 1024u)          // 64 KB

static uint8_t mem_area[MEMWRITE_SIZE];

uint64_t memwrite_user(void) {
    const uint64_t dur_ms = 100;               // medir ~100 ms
    uint64_t start  = get_ms_since_boot();
    uint64_t end_ts = start + dur_ms;

    uint64_t written = 0;

    while (get_ms_since_boot() < end_ts) {
        for (uint32_t i = 0; i < MEMWRITE_SIZE; i++) {
            mem_area[i] = (uint8_t) i;         // escritura secuencial
            written++;                         // 1 byte por iteración
        }
    }

    uint64_t elapsed = get_ms_since_boot() - start;
    if (elapsed == 0) {
        elapsed = 1;
    }

    return (written * 1000ull) / elapsed;      // bytes por segundo
}

// ---------------------------------------------------------------------
// 4) Benchmark de FPS desde userland
//
// Misma lógica que el del kernel.
// ---------------------------------------------------------------------
uint64_t benchmark_fps(void) {
    uint64_t frames = 0;
    uint64_t start  = get_ms_since_boot();

    // misma lógica: en CADA vuelta se pregunta la hora
    while (get_ms_since_boot() - start < 1000) {
        put_frame();   // syscall que dibuja el frame completo
        frames++;      // contamos el frame
    }

    return frames;
}
