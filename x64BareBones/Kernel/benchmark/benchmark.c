#include "benchmark.h"
#include "video.h"
#include "timer.h"

static const int PIXELS = 5000;
uint64_t benchmark_fps() {
    //guardo el tiempo inicial
    uint64_t frames = 0;
    uint64_t start = timer_ms_since_boot();
    // dibujo por 1 segundo
    while (timer_ms_since_boot() - start < 1000) {
        //escribo en toda la pantalla 
        putFrame();
        frames++;
    }

    //retortno la cantidad de frames que se imprimieron en ese tiempo 
    return frames;
}

uint64_t benchmark_floating_point() {
    //cantidad de operaciones 
    const uint64_t ITERATIONS = 1000000;
    uint64_t start = timer_ms_since_boot();

    uint64_t result = 0;
    //ejucuta un millon de operaciones 
    for (uint64_t i = 1; i < ITERATIONS; i++) {
        // simulamos operaciones de punto flotante con enteros para evitar FPU/SSE
        result += (i * 31415926u) / (i + 1u);   // aproximación de π * 10^7
        result *= 999999u;
        result /= 1000000u;
    }

    // tiempo cuando termino 
    uint64_t end = timer_ms_since_boot();
    uint64_t elapsed = end - start;

    if (elapsed == 0) elapsed = 1; // evitar división por 0

    // operaciones por milisegundo → *1000 para operaciones por segundo
    uint64_t ops_per_sec = (ITERATIONS * 1000ull) / elapsed;

    return ops_per_sec; // operaciones por segundo
}

uint64_t benchmark_hardware_access() {
    //tamanio de la patalla
    const uint32_t W = vdGetWidth();
    const uint32_t H = vdGetHeight();

    //tiempo de inicio y contador
    uint64_t written = 0;
    uint64_t start = timer_ms_since_boot();
    
    //dutante 10ms dibujo pixeles 
    while (timer_ms_since_boot() - start < 50) { 
        for (int i = 0; i < PIXELS; i++) {
            uint32_t x = (uint32_t)i % W;
            uint32_t y = (uint32_t)(i / W) % H;
            putPixel(x, y, 0xFFFFFF, PIXEL_BACK);
            written++;
        }
    }
    
    uint64_t dt = timer_ms_since_boot() - start;  
    if (dt == 0) {
        dt = 1;
    }

    return (written * 1000ull) / dt; 
}