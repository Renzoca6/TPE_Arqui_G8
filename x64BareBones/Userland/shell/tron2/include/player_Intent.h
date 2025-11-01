#ifndef PLAYER_INTENT_H
#define PLAYER_INTENT_H
#include <stdint.h>

typedef struct{
    int8_t x;
    int8_t y;
} g_player_intent;

// procesa todos los eventos pendientes y actualiza las intenciones
void tron_handle_input_edge(g_player_intent p1);
void tron_handle_input_edg_coop(g_player_intent p1, g_player_intent p2);

#endif