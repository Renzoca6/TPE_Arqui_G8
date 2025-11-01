#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

typedef struct {
    uint32_t x0, y0;        // origen del tablero (px)
    uint16_t cols, rows;    // tamaño lógico (celdas)
    uint16_t cell_px;       // px por celda
    uint32_t line_color;    //color de la grid
    uint32_t bg_color;    //color de la grid
    uint32_t border_color;    //color de la grid
} Grid;

typedef struct {
    uint16_t col, row;   // pos en celdas
    int8_t   dx, dy;     // dir en celdas (-1,0,1)
    uint8_t  id_cell;    // 1=P1, 2=P2
    uint32_t color;      // trail color
    uint8_t  alive;      // 1 vivo, 0 muerto
} Player;

typedef struct {
    uint16_t p1, p2;
} Score;

typedef struct {
    Grid    grid;
    uint8_t *occ;        // rows*cols bytes (0 libre, >0 ocupado)
    Player  p1, p2;
    Score score;
    uint8_t level;
} TronGame;

#endif
