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


void cell_to_pixel(const Grid* g, uint16_t col, uint16_t row, uint32_t* px, uint32_t* py) {
    *px = g->x0 + (uint32_t)col * g->cell_px;
    *py = g->y0 + (uint32_t)row * g->cell_px;
}

int grid_contains_pixel(const Grid* g, uint32_t px, uint32_t py) {
    uint32_t endX = g->x0 + (uint32_t)g->cols * g->cell_px;
    uint32_t endY = g->y0 + (uint32_t)g->rows * g->cell_px;
    return (px >= g->x0 && px < endX && py >= g->y0 && py < endY);
}

uint32_t occ_idx(const Grid* g, uint16_t col, uint16_t row) {
    return (uint32_t)row * g->cols + col;
}
uint8_t occ_get(const uint8_t* occ, const Grid* g, uint16_t col, uint16_t row) {
    return occ[occ_idx(g, col, row)];
}
void occ_set(uint8_t* occ, const Grid* g, uint16_t col, uint16_t row, uint8_t v) {
    occ[occ_idx(g, col, row)] = v;
}

void draw_cell(const Grid* g, uint16_t col, uint16_t row, uint32_t color, int target) {
    uint32_t x0, y0;
    cell_to_pixel(g, col, row, &x0, &y0);
    for (uint32_t y = 0; y < g->cell_px; y++) {
        for (uint32_t x = 0; x < g->cell_px; x++) {
            putPixel(color, x0 + x, y0 + y, target); // tu primitivo de pixel
        }
    }
}

void draw_grid_lines(const Grid* g, uint32_t color, int target) {
    uint32_t endX = g->x0 + (uint32_t)g->cols * g->cell_px;
    uint32_t endY = g->y0 + (uint32_t)g->rows * g->cell_px;

    for (uint16_t c = 0; c <= g->cols; c++) {
        uint32_t x = g->x0 + (uint32_t)c * g->cell_px;
        for (uint32_t y = g->y0; y < endY; y++) putPixel(color, x, y, target);
    }
    for (uint16_t r = 0; r <= g->rows; r++) {
        uint32_t y = g->y0 + (uint32_t)r * g->cell_px;
        for (uint32_t x = g->x0; x < endX; x++) putPixel(color, x, y, target);
    }
}

