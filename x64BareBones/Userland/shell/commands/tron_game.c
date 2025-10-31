#include "./tron_game.h"
#include "../tron2/include/game.h"
#include "../tron2/include/map.h"
#include "../tron2/include/player.h"
#include "../tron2/include/types.h"
#include "../lib/syscall_call.h"
#include "../tron2/include/config.h"
#include <stdint.h>

void startGame(){
    clearwindow(0x000000);

    TronGame game;
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
    int i = 0 ; 
    while (in_game) {
        // a) esperar a próximo tick lógico
        uint64_t now = get_ms_since_boot();
        uint64_t elapsed = now - start;
        if (elapsed < TRON_TICK_MS) {
            sleep_ms(TRON_TICK_MS - elapsed);
            continue;
        }
        start = get_ms_since_boot();

        // b) (en el futuro) procesar input no bloqueante y actualizar direcciones
        //    por ahora, sin input: siguen en su dirección actual

        // c) avanzar jugadores (pintan y reservan celdas; 0 = muere si choca)
        int alive1 = player_step_and_paint(&game, &p1);
        int alive2 = player_step_and_paint(&game, &p2);

        if (!alive1 || !alive2) {
            in_game = false;
        }

        // d) (opcional) backbuffer → present. Si usás backbuffer para todo, llamá present_fullframe();
        // present_fullframe();


    }
    //clearwindow(0x000000);
    
}

