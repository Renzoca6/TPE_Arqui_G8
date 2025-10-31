#ifndef TRON_UTILS_H
#define TRON_UTILS_H
#include <stddef.h>
#include <stdint.h>
#define MAX_PLAYERS 2


void draw_grid(uint32_t spacing, uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY);

// Grid lógico (en celdas)
typedef struct {
    uint32_t x0, y0;     // origen en píxeles donde empieza el tablero
    uint16_t cols, rows; // tamaño lógico en celdas
    uint16_t cell_px;    // tamaño de cada celda (10 px en tu caso)
} Grid;

// helpers de coordenadas
void cell_to_pixel(const Grid* g, uint16_t col, uint16_t row, uint32_t* px, uint32_t* py);
int  grid_contains_pixel(const Grid* g, uint32_t px, uint32_t py);

// helpers OCC (mapa de ocupación)
uint32_t occ_idx(const Grid* g, uint16_t col, uint16_t row);
uint8_t  occ_get(const uint8_t* occ, const Grid* g, uint16_t col, uint16_t row);
void     occ_set(uint8_t* occ, const Grid* g, uint16_t col, uint16_t row, uint8_t v);

// helpers de pintado
void draw_cell(const Grid* g, uint16_t col, uint16_t row, uint32_t color, int target);
void draw_grid_lines(const Grid* g, uint32_t color, int target);


#endif