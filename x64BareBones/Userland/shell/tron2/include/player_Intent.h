#ifndef PLAYER_INTENT_H
#define PLAYER_INTENT_H
#include <stdint.h>

typedef struct{
    int8_t x;
    int8_t y;
} player_Intent;

// procesa todos los eventos pendientes y actualiza las intenciones
void tron_handle_input_edge(player_Intent p1);
void tron_handle_input_edg_coop(player_Intent p1, player_Intent p2);

#endif