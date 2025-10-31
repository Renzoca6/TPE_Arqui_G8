// apps/tron/tron_game.c
#include "tronGame.h"
#include "../lib/syscall_call.h"   // present_fullframe, etc.
#include "../utils/tronUtils.h"

// internos
static inline void occupy_and_draw(TronGame* T, uint16_t c, uint16_t r,
                                   uint8_t who, uint32_t color, int target) {
    occ_set(T->occ, &T->grid, c, r, who);
    draw_cell(&T->grid, c, r, color, target);
}

void tron_map_clear(TronGame* T) {
    uint32_t total = (uint32_t)T->grid.cols * T->grid.rows;
    for (uint32_t i = 0; i < total; i++) T->occ[i] = 0;
}

void tron_map_add_border(TronGame* T) {
    const Grid* g = &T->grid;
    for (uint16_t c = 0; c < g->cols; c++) {
        occ_set(T->occ, g, c, 0,         255);
        occ_set(T->occ, g, c, g->rows-1, 255);
    }
    for (uint16_t r = 0; r < g->rows; r++) {
        occ_set(T->occ, g, 0,         r, 255);
        occ_set(T->occ, g, g->cols-1, r, 255);
    }
}

void tron_redraw_full(const TronGame* T, int target) {
    const Grid* g = &T->grid;
    for (uint16_t r = 0; r < g->rows; r++) {
        for (uint16_t c = 0; c < g->cols; c++) {
            uint8_t v = occ_get(T->occ, g, c, r);
            if (!v) continue;
            uint32_t color = 0x000000;
            if      (v == 1) color = T->p1.color;
            else if (v == 2) color = T->p2.color;
            else if (v == 255) color = 0x404040;
            draw_cell(g, c, r, color, target);
        }
    }
}

void tron_set_dir(Player* P, int8_t ndx, int8_t ndy) {
    if (P->dx == -ndx && P->dy == -ndy) return; // no 180°
    P->dx = ndx; P->dy = ndy;
}

int tron_try_step(TronGame* T, Player* P, int target) {
    const Grid* g = &T->grid;
    int nc = (int)P->col + (int)P->dx;
    int nr = (int)P->row + (int)P->dy;

    // si NO pusiste border, validá límites:
    if (nc < 0 || nr < 0 || nc >= g->cols || nr >= g->rows) return -1;

    if (occ_get(T->occ, g, (uint16_t)nc, (uint16_t)nr) != 0) return -1;

    P->col = (uint16_t)nc; P->row = (uint16_t)nr;
    occupy_and_draw(T, P->col, P->row, P->id_cell, P->color, target);
    return 0;
}

void tron_start(TronGame* T, uint8_t* occ_buffer,
                uint32_t x0, uint32_t y0,
                uint16_t cols, uint16_t rows, uint16_t cell_px) {
    T->grid.x0 = x0; T->grid.y0 = y0;
    T->grid.cols = cols; T->grid.rows = rows; T->grid.cell_px = cell_px;
    T->occ = occ_buffer;

    tron_map_clear(T);
    // tron_map_add_border(T); // si querés borde letal

    // spawns de ejemplo
    T->p1.col = cols/4;   T->p1.row = rows/2;   T->p1.dx = +1; T->p1.dy = 0;
    T->p1.id_cell = 1;    T->p1.color = 0x00FF00; T->p1.alive = 1;

    T->p2.col = 3*cols/4; T->p2.row = rows/2;   T->p2.dx = -1; T->p2.dy = 0;
    T->p2.id_cell = 2;    T->p2.color = 0x00AAFF; T->p2.alive = 1;

    occupy_and_draw(T, T->p1.col, T->p1.row, T->p1.id_cell, T->p1.color, 0);
    occupy_and_draw(T, T->p2.col, T->p2.row, T->p2.id_cell, T->p2.color, 0);

    // draw_grid_lines(&T->grid, 0x303030, 0); // opcional grilla
    present_fullframe();
}

void tron_tick(TronGame* T) {
    // (1) acá podés leer input y llamar tron_set_dir(...) para p1/p2

    // (2) avanzar
    if (T->p1.alive && tron_try_step(T, &T->p1, 0) != 0) T->p1.alive = 0;
    if (T->p2.alive && tron_try_step(T, &T->p2, 0) != 0) T->p2.alive = 0;

    // (3) HUD opcional (escribí strings con write_at_back si querés)

    // (4) presentar (si estás usando backbuffer)
    present_fullframe();
}
