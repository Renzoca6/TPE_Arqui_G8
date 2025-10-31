// game.h
#pragma once
#include "types.h"

void game_init(TronGame *G);
void game_countdown(TronGame *G, int seconds);
void game_loop(TronGame *G); // bloquea hasta que haya ganador
