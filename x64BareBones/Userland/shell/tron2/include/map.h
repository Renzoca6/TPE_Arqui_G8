// map.h
#ifndef MAP_H
#define MAP_H

#include "types.h"
#include <stdint.h>

void  map_init(TronGame *G, uint64_t scrH, uint64_t scrW);
void  map_free(TronGame *G);
void  cell_to_pixel(const Grid *g, uint16_t col, uint16_t row, uint32_t *px, uint32_t *py);
int   grid_contains_cell(const Grid *g, uint16_t col, uint16_t row);
uint32_t occ_idx(const Grid *g, uint16_t col, uint16_t row);
uint8_t  occ_get(const TronGame *G, uint16_t col, uint16_t row);
void     occ_set(TronGame *G, uint16_t col, uint16_t row, uint8_t v);
void  map_draw_grid_lines(const TronGame *G, uint32_t color, int target);
void  map_draw_cell(const TronGame *G, uint16_t col, uint16_t row, uint32_t color, int target);

#endif