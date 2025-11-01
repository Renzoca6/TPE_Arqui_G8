// AI.h
#ifndef AI_H
#define AI_H
#include <stdint.h>
#include "types.h"     // TronGame, Player
#include "player_Intent.h"

// Intenta elegir: recto -> izquierda -> derecha (sin U-turn).
// Retorna 1 si eligió algo seguro, 0 si no había casillas libres.
int ai_choose_dir_simple(const TronGame *G,
                         const Player   *bot,
                         player_Intent  *out);



int ai_choose_dir_track(const TronGame *G,
                        const Player   *bot,
                        const Player   *target,
                        player_Intent  *out);        
                        
                        int ai_choose_dir_cutoff(const TronGame *G,
                         const Player   *bot,
                         const Player   *target,
                         player_Intent  *out);
#endif
