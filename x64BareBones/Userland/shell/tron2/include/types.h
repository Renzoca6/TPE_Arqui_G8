#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* ===================== */
/*     Estructuras base   */
/* ===================== */

/** Representa el tablero lógico del juego. */
typedef struct {
    uint32_t x0, y0;        // origen del tablero (en píxeles)
    uint16_t cols, rows;    // tamaño lógico (en celdas)
    uint16_t cell_px;       // tamaño de cada celda (px)
    uint32_t line_color;    // color de líneas
    uint32_t bg_color;      // color de fondo
    uint32_t border_color;  // color de borde
} Grid;

/** Representa un jugador. */
typedef struct {
    uint16_t col, row;   // posición en celdas
    int8_t   dx, dy;     // dirección (-1, 0, 1)
    uint8_t  id_cell;    // 1=P1, 2=P2
    uint32_t color;      // color del rastro
    uint8_t  alive;      // 1=vivo, 0=muerto
} Player;

/** Marcador de puntuación. */
typedef struct {
    uint16_t p1;
    uint16_t p2;
} Score;

/** Estado completo del juego Tron. */
typedef struct {
    Grid    grid;
    uint8_t *occ;        // mapa de ocupación (rows*cols)
    Player  p1;
    Player  p2;
    Score   score;
    uint8_t level;
} TronGame;

#endif /* TYPES_H */
