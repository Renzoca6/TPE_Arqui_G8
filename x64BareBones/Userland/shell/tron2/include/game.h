#ifndef GAME_H
#define GAME_H

#include "types.h"

/* ===================== */
/*     Ciclo principal    */
/* ===================== */

/** Inicializa el juego. */
void game_init(TronGame *G);

/** Muestra cuenta regresiva antes de iniciar. */
void game_countdown(TronGame *G, int seconds);

/** Ejecuta el bucle principal del juego (bloquea hasta que haya ganador). */
void game_loop(TronGame *G);

#endif /* GAME_H */
