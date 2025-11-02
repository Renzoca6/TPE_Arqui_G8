#include <stdint.h>
#include "./include/AI.h"
#include "./include/map.h"            // occ_get, grid bounds (cols/rows)
#include "./include/types.h"          // TronGame, Grid, Player
#include "./include/player_Intent.h"  // player_Intent
#include "../include/syscall_call.h" // get_ms_since_boot()

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

    static uint32_t s_count = 0;
    s_count++;

    const int fdx = bot->dx, fdy = bot->dy;

    /* NUEVO: período según nivel */
    uint8_t lvl = G->level ? G->level : 1;
    // nivel 1 → 7 (lo que tenías)
    // nivel 2 → 6
    // nivel 3 → 5
    // mínimo 2
    uint32_t period = TRON_AI_JITTER_PERIOD;
    if (lvl > 1) {
        uint32_t dec = (uint32_t)(lvl - 1);
        if (dec >= period - 2)
            period = 2;
        else
            period = period - dec;
    }

    if ((s_count % period) == 0) {
        if (ai_jitter3_simple(G, bot, out)) {
            return 1;
        }
        // si no pudo, sigo con flujo simple…
    }

    /* flujo simple: recto → izq → der (igual que antes) */
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

    return 0;
}


/* =========================================================
   IA que “persigue” a un jugador:
   - trata de ir hacia la columna del player
   - luego hacia la fila del player
   - solo toma una dirección si la celda está libre
   - si nada sirve, cae a la IA simple
   ========================================================= */
static inline int iabs(int x) { return x < 0 ? -x : x; }

int ai_choose_dir_track(const TronGame *G,
                        const Player   *bot,
                        const Player   *target,
                        player_Intent  *out)
{
    if (!G || !bot || !target || !out)
        return 0;

    const Grid *grid = &G->grid;

    /* diferencia en celdas */
    int dc = (int)target->col - (int)bot->col;   // + → player a la derecha
    int dr = (int)target->row - (int)bot->row;   // + → player abajo

    /* vamos a armar una lista de direcciones “deseadas” en orden */
    int cand_dx[4];
    int cand_dy[4];
    int n = 0;

    /* 1) elegimos eje principal: el que está más lejos */
    if (iabs(dc) >= iabs(dr)) {
        /* primero horizontal hacia el player */
        if (dc > 0) { cand_dx[n] =  1; cand_dy[n] =  0; n++; }   // ir derecha
        else if (dc < 0) { cand_dx[n] = -1; cand_dy[n] =  0; n++; } // ir izquierda

        /* después vertical hacia el player */
        if (dr > 0) { cand_dx[n] =  0; cand_dy[n] =  1; n++; }   // ir abajo
        else if (dr < 0) { cand_dx[n] =  0; cand_dy[n] = -1; n++; } // ir arriba
    } else {
        /* primero vertical hacia el player */
        if (dr > 0) { cand_dx[n] =  0; cand_dy[n] =  1; n++; }   // ir abajo
        else if (dr < 0) { cand_dx[n] =  0; cand_dy[n] = -1; n++; } // ir arriba

        /* después horizontal hacia el player */
        if (dc > 0) { cand_dx[n] =  1; cand_dy[n] =  0; n++; }   // ir derecha
        else if (dc < 0) { cand_dx[n] = -1; cand_dy[n] =  0; n++; } // ir izquierda
    }

    /* 2) añadimos como opción la dirección actual (para no hacer cosas raras) */
    cand_dx[n] = bot->dx;
    cand_dy[n] = bot->dy;
    n++;

    /* 3) probamos en ese orden */
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

    /* 4) si nada de lo anterior sirvió, usamos tu IA de antes */
    return ai_choose_dir_simple(G, bot, out);
}



/* =========================================================
   IA que intenta CORTAR EL CAMINO al player.
   Estrategia:
   1. Predice 3 posiciones futuras del player (según su dx,dy).
   2. Para cada posición futura, busca un movimiento del bot que lo acerque
      MÁS RÁPIDO a esa posición (horizontal o vertical primero).
   3. Si alguna de esas direcciones está libre, la usa.
   4. Si no pudo, prueba con "track" (seguir al player).
   5. Si tampoco, cae a la IA simple de siempre.
   ========================================================= */
static inline int ai_abs(int x) { return x < 0 ? -x : x; }

/* chiquita: ¿esta celda (c,r) está dentro y libre? */
static int ai_cell_free(const TronGame *G, int c, int r) {
    return cell_is_free(G, c, r);  // ya la tenías
}

/* mueve bot 1 paso hacia (tc,tr) si se puede */
static int ai_move_towards(const TronGame *G,
                           const Player   *bot,
                           int             tc,   // target col
                           int             tr,   // target row
                           player_Intent  *out)
{
    int bc = (int)bot->col;
    int br = (int)bot->row;

    /* prioridad: el eje donde más diferencia hay */
    int dc = tc - bc;
    int dr = tr - br;

    int cand_dx[4];
    int cand_dy[4];
    int n = 0;

    if (ai_abs(dc) >= ai_abs(dr)) {
        /* primero horizontal hacia el target */
        if (dc > 0) { cand_dx[n] =  1; cand_dy[n] =  0; n++; }
        else if (dc < 0) { cand_dx[n] = -1; cand_dy[n] =  0; n++; }

        /* después vertical hacia el target */
        if (dr > 0) { cand_dx[n] =  0; cand_dy[n] =  1; n++; }
        else if (dr < 0) { cand_dx[n] =  0; cand_dy[n] = -1; n++; }
    } else {
        /* primero vertical */
        if (dr > 0) { cand_dx[n] =  0; cand_dy[n] =  1; n++; }
        else if (dr < 0) { cand_dx[n] =  0; cand_dy[n] = -1; n++; }

        /* después horizontal */
        if (dc > 0) { cand_dx[n] =  1; cand_dy[n] =  0; n++; }
        else if (dc < 0) { cand_dx[n] = -1; cand_dy[n] =  0; n++; }
    }

    /* por último, su dirección actual (para no hacer giros tontos) */
    cand_dx[n] = bot->dx;
    cand_dy[n] = bot->dy;
    n++;

    for (int i = 0; i < n; i++) {
        int nx = bc + cand_dx[i];
        int ny = br + cand_dy[i];
        if (ai_cell_free(G, nx, ny)) {
            out->x = (int8_t)cand_dx[i];
            out->y = (int8_t)cand_dy[i];
            return 1;
        }
    }

    return 0;
}

int ai_choose_dir_cutoff(const TronGame *G,
                         const Player   *bot,
                         const Player   *target,
                         player_Intent  *out)
{
    if (!G || !bot || !target || !out)
        return 0;

    /* 1) predecimos hasta 3 pasos del player */
    int tdx = target->dx;
    int tdy = target->dy;

    /* si el player está quieto (raro), no sirve cortar, pasamos a track/simple */
    if ((tdx == 0 && tdy == 0))
        goto fallback;

    /* posiciones futuras del player */
    int fut_c[3];
    int fut_r[3];

    fut_c[0] = (int)target->col + tdx;
    fut_r[0] = (int)target->row + tdy;

    fut_c[1] = fut_c[0] + tdx;
    fut_r[1] = fut_r[0] + tdy;

    fut_c[2] = fut_c[1] + tdx;
    fut_r[2] = fut_r[1] + tdy;

    /* 2) para cada futura, intentamos mover el bot HACIA esa celda */
    for (int k = 0; k < 3; k++) {
        int tc = fut_c[k];
        int tr = fut_r[k];

        /* la celda que queremos cortar puede estar ocupada por el trail del player;
           igual intentamos pararnos en la celda previa al corte. */
        if (!cell_is_inside(&G->grid, tc, tr))
            continue;

        if (ai_move_towards(G, bot, tc, tr, out)) {
            return 1;
        }
    }

fallback:
    /* 3) si no pudo cortar, que lo persiga primero... */
    if (ai_choose_dir_track(G, bot, target, out))
        return 1;

    /* 4) ...y si tampoco, que haga lo de siempre */
    return ai_choose_dir_simple(G, bot, out);
}