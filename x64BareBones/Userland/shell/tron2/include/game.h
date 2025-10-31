// game.h
#ifndef GAME_H
#define GAME_H

#include "types.h"

void game_init(TronGame *G);
void game_countdown(TronGame *G, int seconds);
void game_loop(TronGame *G); // bloquea hasta que haya ganador

#endif