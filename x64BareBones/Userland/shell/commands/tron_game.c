#include "./tron_game.h"
#include "../tron2/include/game.h"
#include "../tron2/include/map.h"
#include "../tron2/include/player.h"
#include "../tron2/include/types.h"
#include "../lib/syscall_call.h"
#include "../tron2/include/config.h"
#include "../tron2/include/ui.h"    
#include "../tron2/include/player_Intent.h"
#include <stdint.h>

void startGame(){
    
    int mode = tron_show_start_menu();
    //clearwindow(0x000000);


    
    TronGame game;
    player_Intent p1_Intent;
    map_init(&game);
    map_draw_grid_lines(&game , 1);

    Player p1, p2;
    player_spawn_center_left(&game, &p1, 1, TRON_P1_COLOR);
    player_spawn_center_right(&game, &p2, 2, TRON_P2_COLOR);

    occ_set(&game, p1.col, p1.row, p1.id_cell);
    occ_set(&game, p2.col, p2.row, p2.id_cell);

    map_draw_cell(&game, p1.col,p1.row, p1.color, 1);
    map_draw_cell(&game, p2.col,p2.row, p2.color, 1);

    present_fullframe();
    
    bool in_game = true;

    uint64_t start = get_ms_since_boot();
    while (in_game) {
        // a) esperar a próximo tick lógico
        uint64_t now = get_ms_since_boot();
        uint64_t elapsed = now - start;
        if (elapsed < TRON_TICK_MS) {
            sleep_ms(TRON_TICK_MS - elapsed);
            continue;
        }
        start = get_ms_since_boot();
        tron_handle_input_edge(p1_Intent);
        int aux = player_action_tick(&p1);
        if (aux != 0) {
            in_game = false;
            if (aux == 1){
                println("perdiste inutil");
            }
            else if(aux == 2){
                println("ganaste... no sos tan inutil al final (igual lo podrias haber hecho mejor)");
            }
            else if(aux == 3){
                println("empate, son los 2 horribles");
            }   
        }


    }

    map_free(&game);

    while (1)
    {
        /* code */
    }
    

    //clearwindow(0x000000);
    
}

