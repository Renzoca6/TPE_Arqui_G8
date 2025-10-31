#include <stdint.h>
#include "../lib/syscall_call.h"
#include "tronUtils.h"



//draw_grid(10, 100, 100, 921, 701);
void draw_grid(uint32_t spacing, uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY) {
    uint32_t color  = 0x303030;   // gris oscuro para la grilla

    if (endX < startX || endY< startY || spacing == 0){
        return;
    }
    
    // Líneas verticales
    for (uint64_t x = startX; x <= endX; x += spacing) {
        for (uint64_t y = startY; y < endY; y++) {
            putPixel(color, x, y, 0);
        }
    }

    // Líneas horizontales
    for (uint64_t y = startY; y < endY; y += spacing) {
        for (uint64_t x = startX; x < endX; x++) {
            putPixel(color, x, y, 0);
        }
    }
}