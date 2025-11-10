#include "benchmark.h"
#include "video.h"
#include "timer.h"

static const uint32_t PIXELS = 5000;

// ---------------------------------------------------------------------
// Benchmark 1: FPS (frames por segundo)
// ---------------------------------------------------------------------
uint64_t benchmark_fps(void) {
    uint64_t frames = 0;
    uint64_t start = timer_ms_since_boot();

    // Dibuja por 1 segundo completo en el back y despues lo pone en la vram 
    // pintamos cada frame con un color distinto y presentamos al framebuffer
    // usamos vdclearScreenDB que ya hace esto.
    while (timer_ms_since_boot() - start < 1000) {
        uint32_t r = (uint32_t)((frames * 97) & 0x33);
        uint32_t g = (uint32_t)((frames * 57) & 0x33);
        uint32_t b = (uint32_t)((frames * 31) & 0x33);
        uint32_t color = (r << 16) | (g << 8) | b;


        vdclearScreenDB(color);

        frames++;
    }

    // Retorna la cantidad de frames dibujados en ese tiempo
    return frames;
}

// ---------------------------------------------------------------------
// Benchmark 2: Operaciones aritméticas (simulación de punto flotante)
// ---------------------------------------------------------------------
uint64_t benchmark_floating_point(void) {
    const uint64_t ITERATIONS = 1000000;
    uint64_t start = timer_ms_since_boot();

    uint64_t result = 0;

    for (uint64_t i = 1; i < ITERATIONS; i++) {
        // Simula operaciones de punto flotante usando enteros
        result += (i * 31415926u) / (i + 1u);
        result *= 999999u;
        result /= 1000000u;
    }

    uint64_t end = timer_ms_since_boot();
    uint64_t elapsed = end - start;

    if (elapsed == 0)
        elapsed = 1; // Evita división por 0

    // Iteraciones por milisegundo → *1000 para obtener por segundo
    return (ITERATIONS * 1000ull) / elapsed;
}

// ---------------------------------------------------------------------
// Benchmark 3: Acceso a hardware (velocidad de escritura en pantalla)
// ---------------------------------------------------------------------
uint64_t benchmark_hardware_access(void) {
    const uint32_t W = vdGetWidth();
    const uint32_t H = vdGetHeight();

    uint64_t written = 0;
    uint64_t start = timer_ms_since_boot();

    // Dibuja píxeles durante 50 ms
    while (timer_ms_since_boot() - start < 50) {
        for (uint32_t i = 0; i < PIXELS; i++) {
            uint32_t x = i % W;
            uint32_t y = (i / W) % H;
            putPixel(x, y, 0xFFFFFF, PIXEL_BACK);
            written++;
        }
    }

    uint64_t dt = timer_ms_since_boot() - start;
    if (dt == 0)
        dt = 1;

    return (written * 1000ull) / dt;
}

// ---------------------------------------------------------------------
// Benchmark 4: Latencia de acceso al timer del sistema
// ---------------------------------------------------------------------
uint64_t benchmark_timer_latency(void) {
    const uint64_t dur_ms = 100;               // medir durante ~100 ms
    uint64_t start  = timer_ms_since_boot();
    uint64_t end_ts = start + dur_ms;
    uint64_t calls  = 0;

    while (timer_ms_since_boot() < end_ts) {
        (void)timer_ms_since_boot();           // llamada directa al timer
        calls++;
    }

    uint64_t elapsed = timer_ms_since_boot() - start;
    if (elapsed == 0) {
        elapsed = 1;                           // evitar división por cero
    }

    return (calls * 1000ull) / elapsed;        // llamadas por segundo
}
