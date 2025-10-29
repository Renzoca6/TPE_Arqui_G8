#include "benchmark.h"
#include "video.h"
#include "timer.h"

static const int PIXELS = 100;

double benchmark_fps(){
    uint64_t start = timer_ms_since_boot();
    uint64_t frames = 0;
    // dibujo por 1 segundo
    while (timer_ms_since_boot - start < 1000){
        putFrame();
        frames++;
    }
    return (double)frames;
}

double benchmark_floating_point(){
    
}

double benchmark_hardware_access(){
  
}

