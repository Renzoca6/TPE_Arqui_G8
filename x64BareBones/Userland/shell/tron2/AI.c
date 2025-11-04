// =====================================================================
// AI.c
// Lógica de IA para Tron: simple, persecución y corte de camino
// =====================================================================

#include <stdint.h>
#include "./include/AI.h"
#include "./include/map.h"            // occ_get, límites de la grilla
#include "./include/types.h"          // TronGame, Grid, Player
#include "./include/player_Intent.h"  // player_Intent
#include "../include/syscall_call.h"  // get_ms_since_boot()

// ---------------------------------------------------------------------
// Parámetros
// ---------------------------------------------------------------------

#define TRON_AI_JITTER_PERIOD 7   // Cada cuántos pasos intentar “jitter”


// ---------------------------------------------------------------------
// Helpers 
// ---------------------------------------------------------------------
static inline void dir_left (int dx, int dy, int *ox, int *oy)  { *ox =  dy; *oy = -dx; }
static inline void dir_right(int dx, int dy, int *ox, int *oy)  { *ox = -dy; *oy =  dx; }

static inline int cell_is_inside(const Grid *g, int c, int r) {
    return (c >= 0 && r >= 0 && c < (int)g->cols && r < (int)g->rows);
}

static inline int cell_is_free(const TronGame *G, int c, int r) {
    const Grid *g = &G->grid;
    if (!cell_is_inside(g, c, r)) return 0;
    return occ_get(G, (uint16_t)c, (uint16_t)r) == 0;
}

// ---------------------------------------------------------------------
// Jitter simple: elige UNA de {recto, izq, der} según (ms % 3).
// Si la celda es segura, setea out y devuelve 1; si no, devuelve 0.
// ---------------------------------------------------------------------
static int ai_jitter3_simple(const TronGame *G, const Player *bot, player_Intent *out) {
    uint32_t ms   = (uint32_t)get_ms_since_boot();
    uint32_t pick = ms % 3;  // 0=recto, 1=izq, 2=der

    int sdx = bot->dx, sdy = bot->dy;
    int ldx, ldy, rdx, rdy;
    dir_left (sdx, sdy, &ldx, &ldy);
    dir_right(sdx, sdy, &rdx, &rdy);

    int dx = sdx, dy = sdy;
    if (pick == 1) { dx = ldx; dy = ldy; }
    else if (pick == 2) { dx = rdx; dy = rdy; }

    int nx = (int)bot->col + dx;
    int ny = (int)bot->row + dy;

    if (cell_is_free(G, nx, ny)) {
        out->x = (int8_t)dx;
        out->y = (int8_t)dy;
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------
// IA simple con “jitter” periódico dependiente del nivel
// ---------------------------------------------------------------------
int ai_choose_dir_simple(const TronGame *G, const Player *bot, player_Intent *out) {
    if (!G || !bot || !out) return 0;

    static uint32_t s_count = 0;
    s_count++;

    const int fdx = bot->dx, fdy = bot->dy;

    // Período según nivel: 1->7, 2->6, 3->5, mínimo 2
    uint8_t lvl = G->level ? G->level : 1;
    uint32_t period = TRON_AI_JITTER_PERIOD;
    if (lvl > 1) {
        uint32_t dec = (uint32_t)(lvl - 1);
        period = (dec >= period - 2) ? 2 : (period - dec);
    }

    if ((s_count % period) == 0) {
        if (ai_jitter3_simple(G, bot, out)) return 1;
        // si no pudo, seguimos con el flujo simple
    }

    // Flujo simple: recto → izq → der
    int fx = (int)bot->col + fdx;
    int fy = (int)bot->row + fdy;
    if (cell_is_free(G, fx, fy)) { out->x = (int8_t)fdx; out->y = (int8_t)fdy; return 1; }

    int ldx, ldy; dir_left(fdx, fdy, &ldx, &ldy);
    int lx = (int)bot->col + ldx, ly = (int)bot->row + ldy;
    if (cell_is_free(G, lx, ly)) { out->x = (int8_t)ldx; out->y = (int8_t)ldy; return 1; }

    int rdx, rdy; dir_right(fdx, fdy, &rdx, &rdy);
    int rx = (int)bot->col + rdx, ry = (int)bot->row + rdy;
    if (cell_is_free(G, rx, ry)) { out->x = (int8_t)rdx; out->y = (int8_t)rdy; return 1; }

    return 0;
}

// ---------------------------------------------------------------------
// IA: sigue  al jugador (track)
// ---------------------------------------------------------------------
static inline int iabs(int x) { return x < 0 ? -x : x; }

int ai_choose_dir_track(const TronGame *G, const Player *bot, const Player *target, player_Intent *out) {
    if (!G || !bot || !target || !out) return 0;

    const Grid *grid = &G->grid;

    // Diferencias en celdas
    int dc = (int)target->col - (int)bot->col;   // + → player a la derecha
    int dr = (int)target->row - (int)bot->row;   // + → player abajo

    int cand_dx[4], cand_dy[4]; int n = 0;

    if (iabs(dc) >= iabs(dr)) {
        if (dc > 0) { cand_dx[n]= 1; cand_dy[n]= 0; n++; } else if (dc < 0) { cand_dx[n]=-1; cand_dy[n]= 0; n++; }
        if (dr > 0) { cand_dx[n]= 0; cand_dy[n]= 1; n++; } else if (dr < 0) { cand_dx[n]= 0; cand_dy[n]=-1; n++; }
    } else {
        if (dr > 0) { cand_dx[n]= 0; cand_dy[n]= 1; n++; } else if (dr < 0) { cand_dx[n]= 0; cand_dy[n]=-1; n++; }
        if (dc > 0) { cand_dx[n]= 1; cand_dy[n]= 0; n++; } else if (dc < 0) { cand_dx[n]=-1; cand_dy[n]= 0; n++; }
    }

    cand_dx[n] = bot->dx; cand_dy[n] = bot->dy; n++;


    for (int i = 0; i < n; i++) {
        int nx = (int)bot->col + cand_dx[i];
        int ny = (int)bot->row + cand_dy[i];
        if (nx >= 0 && ny >= 0 && nx < (int)grid->cols && ny < (int)grid->rows) {
            if (occ_get(G, (uint16_t)nx, (uint16_t)ny) == 0) {
                out->x = (int8_t)cand_dx[i];
                out->y = (int8_t)cand_dy[i];
                return 1;
            }
        }
    }

    // Si nada sirvió, usar la IA simple
    return ai_choose_dir_simple(G, bot, out);
}

// ---------------------------------------------------------------------
// IA: “cortar el camino” del jugador (cutoff)
// ---------------------------------------------------------------------
static inline int ai_abs(int x) { return x < 0 ? -x : x; }
