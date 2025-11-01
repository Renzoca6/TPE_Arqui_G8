#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "types.h"
#include "player_Intent.h"

/* ===================== */
/*       Jugadores        */
/* ===================== */

/** Posiciona al jugador 1 (izquierda central). */
void player_spawn_center_left(const TronGame *G, Player *p, uint8_t id, uint32_t color);

/** Posiciona al jugador 2 (derecha central). */
void player_spawn_center_right(const TronGame *G, Player *p, uint8_t id, uint32_t color);

/** Cambia la dirección actual del jugador. */
void player_set_dir(Player *p, int8_t dx, int8_t dy);

/**
 * Avanza un paso y pinta su celda.
 * Retorna 0 si colisiona, 1 si sigue.
 */
int player_step_and_paint(TronGame *G, Player *p);

/**
 * Ejecuta la acción según la intención actual (input o IA).
 */
int player_action_tick(TronGame *G, Player *p, player_Intent intent);

#endif /* PLAYER_H */
