#include "benchmark.h"
#include "video.h"
#include "timer.h"

static const int PIXELS = 100;

double benchmark_fps(){
    //guardo el tiempo inicial
    uint64_t start = timer_ms_since_boot();
    uint64_t frames = 0;
    // dibujo por 1 segundo
    while (timer_ms_since_boot - start < 1000){
        //escribo en toda la pantalla 
        putFrame();
        frames++;
    }
    //retortno la cantidad de frames que se imprimieron en ese tiempo 
    return (double)frames;
}

double benchmark_floating_point() {
    //cantidad de operaciones 
    const uint64_t ITERATIONS = 1000000;
    uint64_t start = timer_ms_since_boot();

    double result = 0;
    //ejucuta un millon de operaciones 
    for (uint64_t i = 1; i < ITERATIONS; i++) {
        result += (i * 3.14159265358979) / (i + 1.0);
        result *= 0.999999;
    }
    // tiempo cuando termino 
    uint64_t end = timer_ms_since_boot();
    uint64_t elapsed = end - start;

    if (elapsed == 0) elapsed = 1; // evitar división por 0

    double ops_per_ms = (double)ITERATIONS / (double)elapsed;

    return ops_per_ms * 1000.0; // operaciones por segundo
}

double benchmark_hardware_access(void) {
    // Obtengo el tamanio de la pantalla 
    const uint32_t W = vdGetWidth();
    const uint32_t H = vdGetHeight();

    uint64_t t0 = timer_ms_since_boot();
    //printeo una cantidad de pixeles fija 
    for (int i = 0; i < PIXELS; i++) {
        uint32_t x = (uint32_t)i % W;
        uint32_t y = (uint32_t)(i / W) % H;
        putPixel(x, y, 0xFFFFFF, PIXEL_BACK);
    }
    //calculo cuanto tardo en imprimirlos 
    uint64_t dt = timer_ms_since_boot() - t0;
    if (dt == 0) dt = 1;
    return (double)PIXELS * 1000.0 / (double)dt; // píxeles/seg
}

