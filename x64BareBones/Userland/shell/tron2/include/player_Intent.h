#ifndef PLAYER_INTENT_H
#define PLAYER_INTENT_H

#include <stdint.h>

/* ===================== */
/*   Dirección / Input    */
/* ===================== */

/** Representa la intención de movimiento de un jugador. */
typedef struct {
    int8_t x;
    int8_t y;
} player_Intent;

/* ===================== */
/*   Entrada del jugador  */
/* ===================== */

/** Procesa los eventos y actualiza la intención de un jugador. */
void tron_handle_input_edge(player_Intent *p1);

/** Versión cooperativa (dos jugadores). */
void tron_handle_input_edge_coop(player_Intent *p1, player_Intent *p2);

#endif /* PLAYER_INTENT_H */
