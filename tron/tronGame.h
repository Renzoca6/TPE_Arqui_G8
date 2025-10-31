// apps/tron/tron_game.h
#pragma once
#include <stdint.h>
#include "../utils/tronUtils.h"

typedef struct {
    uint16_t col, row;
    int8_t   dx, dy;     // dirección en celdas (-1,0,1)
    uint8_t  id_cell;    // 1 = P1, 2 = P2
    uint32_t color;
    uint8_t  alive;
} Player;

typedef struct {
    Grid     grid;
    uint8_t* occ;   // rows*cols bytes
    Player   p1, p2;
} TronGame;

// setup / ciclo
void tron_start(TronGame* T, uint8_t* occ_buffer,
                uint32_t x0, uint32_t y0,
                uint16_t cols, uint16_t rows, uint16_t cell_px);

void tron_tick(TronGame* T); // avanza 1 paso (lee input si lo agregás dentro)
int  tron_try_step(TronGame* T, Player* P, int target);

void tron_map_clear(TronGame* T);
void tron_map_add_border(TronGame* T);        // opcional
void tron_redraw_full(const TronGame* T, int target);

void tron_set_dir(Player* P, int8_t ndx, int8_t ndy);
