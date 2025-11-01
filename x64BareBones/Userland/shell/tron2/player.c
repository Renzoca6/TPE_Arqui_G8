#include "./include/player.h"
#include "./include/types.h"
#include "./include/map.h"
#include "../lib/syscall_call.h"


// Dónde dibujar: 0 = VRAM directa, 1 = back buffer (ajustá si querés)
#ifndef DRAW_TARGET
#define DRAW_TARGET 0
#endif

/* ------------------ Helpers locales ------------------ */

// Centro vertical del tablero (fila)
static inline uint16_t center_row(const Grid *g) {
    return (uint16_t)(g->rows / 2);
}

// Columna para el centro-izquierda: en 1/4 del ancho lógico
static inline uint16_t center_left_col(const Grid *g) {
    return (uint16_t)(g->cols / 4);
}

// Columna para el centro-derecha: en 3/4 del ancho lógico
static inline uint16_t center_right_col(const Grid *g) {
    return (uint16_t)((3 * g->cols) / 4);
}



void player_spawn_center_left (const TronGame *G, Player *p, uint8_t id, uint32_t color){
     if (!G || !p) return;

    p->col     = center_left_col(&G->grid);
    p->row     = center_row(&G->grid);
    p->dx      = 1;   // hacia la derecha (hacia el centro)
    p->dy      = 0;
    p->id_cell = id;
    p->color   = color;
}
void player_spawn_center_right(const TronGame *G, Player *p, uint8_t id, uint32_t color){
     if (!G || !p) return;

    p->col     = center_right_col(&G->grid);
    p->row     = center_row(&G->grid);
    p->dx      = -1;   // hacia la derecha (hacia el centro)
    p->dy      = 0;
    p->id_cell = id;
    p->color   = color;
}
void player_set_dir(Player *p, int8_t dx, int8_t dy){
    if (!p) return;
    if (!((dx == 0 && (dy == 1 || dy == -1)) ||
          (dy == 0 && (dx == 1 || dx == -1))))
        return;

    if (dx == -p->dx && dy == -p->dy) return;

    p->dx = dx;
    p->dy = dy;

}

int  player_step_and_paint(TronGame *G, Player *p){
    if (!G || !p) return 0;

    // Próxima celda lógica
    uint16_t nx = (uint16_t)(p->col + p->dx);
    uint16_t ny = (uint16_t)(p->row + p->dy);


    // 1) LÍMITE DEL TABLERO → MUERTE
    if (!grid_contains_cell(&G->grid, nx, ny)) {
        return 0;  // se fue del tablero → pierde
    }

    // Chequeo de ocupación (trails de cualquiera)
    if (occ_get( G, nx, ny) != 0)
        return 0;  // choca contra trail (propio o ajeno)

    // Reservar celda y pintar trail
    occ_set(G, nx, ny, p->id_cell);
    map_draw_cell(G, nx, ny, p->color, DRAW_TARGET); //

    // Avanzar player
    p->col = nx;
    p->row = ny;

    return 1;
} // 0=muere, 1=sigue

int player_action_tick(TronGame *G, Player *p,player_Intent intent) {
    // 2) aplicar intención → dirección efectiva

    player_set_dir(p, intent.x, intent.y);
    
    // 3) avanzar y pintar
    int alive1 = player_step_and_paint(G, p);
    if (!alive1){
        return 0;
    }
    else{
        return 1;
    }
}