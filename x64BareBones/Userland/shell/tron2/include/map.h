#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

/* ===================== */
/*       Mapa / Grid      */
/* ===================== */

void map_init(TronGame *G);
void map_free(TronGame *G);

/* ===================== */
/*     Dibujo visual      */
/* ===================== */
void map_draw_grid_lines(const TronGame *G, int target);
void map_draw_border_lines(const TronGame *G, int target);
void map_draw_cell(const TronGame *G, uint16_t col, uint16_t row, uint32_t color, int target);

/* ===================== */
/*     Utilidades grid    */
/* ===================== */
bool grid_contains_cell(const Grid *g, uint16_t col, uint16_t row);
void cell_to_pixel(const Grid *g, uint16_t col, uint16_t row, uint32_t *px, uint32_t *py);

/* ===================== */
/*  Ocupación de celdas   */
/* ===================== */
uint32_t occ_idx(const Grid *g, uint16_t col, uint16_t row);
uint8_t  occ_get(const TronGame *G, uint16_t col, uint16_t row);
void     occ_set(TronGame *G, uint16_t col, uint16_t row, uint8_t v);

#endif /* MAP_H */
