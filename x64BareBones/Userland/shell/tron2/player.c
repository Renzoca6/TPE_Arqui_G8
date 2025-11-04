// =====================================================================
// player.c
// Spawns, dirección y avance/pintado del jugador
// =====================================================================

#include "./include/player.h"
#include "./include/types.h"
#include "./include/map.h"
#include "../include/syscall_call.h"

// Dónde dibujar: 0 = VRAM directa, 1 = back buffer 
#ifndef DRAW_TARGET
#define DRAW_TARGET 0
#endif

// ---------------------------------------------------------------------
// Helpers de posición de spawn
// ---------------------------------------------------------------------
static inline uint16_t center_row(const Grid *g)           { return (uint16_t)(g->rows / 2); }
static inline uint16_t center_left_col(const Grid *g)      { return (uint16_t)(g->cols / 4); }
static inline uint16_t center_right_col(const Grid *g)     { return (uint16_t)((3 * g->cols) / 4); }

// ---------------------------------------------------------------------
// Spawns
// ---------------------------------------------------------------------
void player_spawn_center_left(const TronGame *G, Player *p, uint8_t id, uint32_t color) {
    if (!G || !p) return;
    p->col = center_left_col(&G->grid);
    p->row = center_row(&G->grid);
    p->dx  = 1;    // hacia la derecha (hacia el centro)
    p->dy  = 0;
    p->id_cell = id;
    p->color   = color;
}

void player_spawn_center_right(const TronGame *G, Player *p, uint8_t id, uint32_t color) {
    if (!G || !p) return;
    p->col = center_right_col(&G->grid);
    p->row = center_row(&G->grid);
    p->dx  = -1;   // hacia la izquierda (hacia el centro)
    p->dy  = 0;
    p->id_cell = id;
    p->color   = color;
}

// ---------------------------------------------------------------------
// Dirección: valida giros 90° y evita que valla para atras
// ---------------------------------------------------------------------
void player_set_dir(Player *p, int8_t dx, int8_t dy) {
    if (!p) return;

    // Solo (±1,0) o (0,±1)
    if (!((dx == 0 && (dy == 1 || dy == -1)) || (dy == 0 && (dx == 1 || dx == -1))))
        return;

    // Evito que valla para atras exacta
    if (dx == -p->dx && dy == -p->dy) return;

    p->dx = dx;
    p->dy = dy;
}

// ---------------------------------------------------------------------
// Avanzar una celda y pintar el trail (1 = vivo, 0 = muere)
// ---------------------------------------------------------------------
int player_step_and_paint(TronGame *G, Player *p) {
    if (!G || !p) return 0;

    uint16_t nx = (uint16_t)(p->col + p->dx);
    uint16_t ny = (uint16_t)(p->row + p->dy);

    // Límite del tablero → muerte
    if (!grid_contains_cell(&G->grid, nx, ny)) return 0;

    // Colisión con trail (propio o ajeno) → muerte
    if (occ_get(G, nx, ny) != 0) return 0;

    // Reservar celda y pintar
    occ_set(G, nx, ny, p->id_cell);
    map_draw_cell(G, nx, ny, p->color, DRAW_TARGET);

    // Avanzar player
    p->col = nx;
    p->row = ny;

    return 1;
}

// ---------------------------------------------------------------------
// Aplicar intención, avanzar y reportar estado (1 vivo / 0 muerto)
// ---------------------------------------------------------------------
int player_action_tick(TronGame *G, Player *p, player_Intent intent) {
    player_set_dir(p, intent.x, intent.y);
    return player_step_and_paint(G, p);
}
