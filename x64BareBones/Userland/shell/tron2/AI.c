#include <stdint.h>
#include "AI.h"
#include "map.h"            // occ_get, grid bounds (cols/rows)
#include "types.h"          // TronGame, Grid, Player
#include "player_Intent.h"  // player_Intent
#include "../lib/syscall_call.h" // get_ms_since_boot()

/* --- Parámetro: cada cuántos pasos intento “jitter” --- */
/*  Cambialo a 3 si querés que sea cada 3 */
#ifndef TRON_AI_JITTER_PERIOD
#define TRON_AI_JITTER_PERIOD 7
#endif

/* ---------------- Helpers muy chicos ---------------- */

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

/* Jitter simple: elige UNA de {recto, izq, der} según (ms % 3).
   Si esa opción es segura, setea out y devuelve 1; si no, devuelve 0. */
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

/*
   - Tiene un contador estático que persiste entre llamadas.
   - Si (count % TRON_AI_JITTER_PERIOD == 0) intenta el jitter.
   - Si el jitter NO puede (0), pasa al flujo simple.
   - Si no estamos en el múltiplo, va directo al flujo simple.
*/
int ai_choose_dir_simple(const TronGame *G,
                         const Player   *bot,
                         player_Intent  *out)
{
    if (!G || !bot || !out) return 0;

    static uint32_t s_count = 0;   // ← PERSISTE entre frames
    s_count++;

    const int fdx = bot->dx, fdy = bot->dy;

    /* 1) Cada N pasos, intento jitter:
          exactamente como vos querías: si jitter devuelve 1, NO entro al flujo de abajo.
          Si devuelve 0, sigo con recto→izq→der. */
    if ((s_count % TRON_AI_JITTER_PERIOD) == 0) {
        if (ai_jitter3_simple(G, bot, out)) {
            return 1;  // ya decidió por jitter
        }
        // si no pudo, sigo con flujo simple…
    }

    /* 2) Flujo simple: recto → izquierda → derecha */
    int fx = (int)bot->col + fdx;
    int fy = (int)bot->row + fdy;
    if (cell_is_free(G, fx, fy)) {
        out->x = (int8_t)fdx; out->y = (int8_t)fdy;
        return 1;
    }

    int ldx, ldy; dir_left(fdx, fdy, &ldx, &ldy);
    int lx = (int)bot->col + ldx;
    int ly = (int)bot->row + ldy;
    if (cell_is_free(G, lx, ly)) {
        out->x = (int8_t)ldx; out->y = (int8_t)ldy;
        return 1;
    }

    int rdx, rdy; dir_right(fdx, fdy, &rdx, &rdy);
    int rx = (int)bot->col + rdx;
    int ry = (int)bot->row + rdy;
    if (cell_is_free(G, rx, ry)) {
        out->x = (int8_t)rdx; out->y = (int8_t)rdy;
        return 1;
    }

    /* 3) No hay escapatoria → el caller puede dejar dirección anterior. */
    return 0;
}
