#ifndef AI_H
#define AI_H

#include <stdint.h>
#include "types.h"
#include "player_Intent.h"

/* ===================== */
/*     Inteligencia AI    */
/* ===================== */

/**
 * Estrategia simple: intenta avanzar recto, izquierda o derecha (sin U-turn).
 * Retorna 1 si eligió un movimiento válido, 0 si no hay espacio libre.
 */
int ai_choose_dir_simple(const TronGame *G, const Player *bot, player_Intent *out);

/**
 * Estrategia de seguimiento: el bot trata de alcanzar al target.
 */
int ai_choose_dir_track(const TronGame *G, const Player *bot, const Player *target, player_Intent *out);

#endif /* AI_H */
