// player.h
#ifndef PLAYER_H
#define PLAYER_H
#include "types.h"
#include "player_Intent.h"

void player_spawn_center_left (const TronGame *G, Player *p, uint8_t id, uint32_t color);
void player_spawn_center_right(const TronGame *G, Player *p, uint8_t id, uint32_t color);
void player_set_dir(Player *p, int8_t dx, int8_t dy);
int  player_step_and_paint(TronGame *G, Player *p); // 0=muere, 1=sigue
int player_action_tick(TronGame *G, Player *p,player_Intent intent);


#endif
