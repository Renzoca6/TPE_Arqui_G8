// ---------------------------------------------------------------------
// tron_game.c
// Lógica de arranque del juego Tron: menú, setup de partida y loop de rondas
// ---------------------------------------------------------------------
#include "../include/tron_game.h"
#include "../tron2/include/map.h"
#include "../tron2/include/player.h"
#include "../tron2/include/types.h"
#include "../include/syscall_call.h"     
#include "../tron2/include/config.h"
#include "../tron2/include/ui.h"
#include "../tron2/include/player_Intent.h"
#include "../tron2/include/AI.h"
#include <stdint.h>

// ---------------------------------------------------------------------
// Prepara el tablero para jugar rondas (borra pantalla, recalcula grilla y bordes).
// ---------------------------------------------------------------------
static void tron_reset_board(TronGame *game) {
    map_init(game);
    map_draw_grid_lines(game, 1);
    map_draw_border_lines(game, 1);

    // redibuja HUD
    score_update(game);

    // presenta el HUD
    present_fullframe();
}

// ---------------------------------------------------------------------
// Setup de match (una sola vez por partida):
// inicializa score, asocia p1/p2 al game y deja el tablero listo para arrancar.
// ---------------------------------------------------------------------
static void tron_setup_match(TronGame *game, Player *p1, Player *p2) {
    score_init(game);
    game->p1 = *p1;
    game->p2 = *p2;

    // esto ahora dibuja tablero + HUD + presenta
    tron_reset_board(game);
}

// ---------------------------------------------------------------------
// Prototipo de play_Game
// ---------------------------------------------------------------------
static void play_Game(TronGame *game, Player *p1, Player *p2, int mode);

// ---------------------------------------------------------------------
// Punto de entrada del juego Tron
// ---------------------------------------------------------------------
void startGame(void) {
    int mode = tron_show_start_menu();
    if (mode == 0){
        clearwindow(0x000000);
        return;
    }
    


    TronGame game;
    Player   p1, p2;

    // tablero limpio + HUD + score en 0
    tron_setup_match(&game, &p1, &p2);
    game.level = 1;                      // arrancamos siempre en nivel 1

    // juega el primer BO5
    play_Game(&game, &p1, &p2, mode);

    // Menú final y control de “continuar”
    for (;;) {
        clearwindow(0x000000);

        int toGo = 0;
        switch (mode) {
            case 1: { // SINGLE
                
                toGo = tron_show_end_menu(0, (game.score.p1 == 3 && game.score.p2 == 3) ? 2 : (game.score.p1 == 3) ? 1 : 0, game.level);
                break;
            }
            case 2: { // COOP
                // en coop no vamos a escalar dificultad
                toGo = tron_show_end_menu(1, (game.score.p1 == 3 && game.score.p2 == 3) ? 2 : (game.score.p1 == 3) ? 1 : 0, game.level);
                break;
            }
            default:
                return;
        }

        if (toGo == 2) {
            // “Continuar jugando”

            // Si estamos en single y el jugador REAL ganó, recién ahí subimos nivel
            if (mode == 1 && game.score.p1 == 3 && game.score.p2 < 3) {
                if (game.level < 99) {
                    game.level++;   // subir dificultad
                }
            }

            // guardo el nivel para no perderlo cuando reseteo el match
            uint8_t lvl = game.level;

            tron_setup_match(&game, &p1, &p2);  // esto resetea score
            game.level = lvl;                   // lo vuelvo a poner

            play_Game(&game, &p1, &p2, mode);
            continue;
        } else if (toGo == 1) {
            // “Volver al menú principal”
            clearwindow(0x000000);
            startGame(); // relanza todo
            return;
        } else {
            // Cualquier otra tecla: salir
            clearwindow(0x000000);
            return;
        }
    }
}

// ---------------------------------------------------------------------
// Calcula el tick (delay) según el nivel actual
// ---------------------------------------------------------------------
static uint32_t tron_tick_for_level(const TronGame *game) {
    uint32_t base = TRON_TICK_MS;               // valor base desde config
    uint8_t  lvl  = game->level ? game->level : 1;

    // cada nivel baja 8 ms, pero no menos de 20 ms
    uint32_t dec = (uint32_t)(lvl - 1) * 8u;

    if (base <= 20u) {                         
        return base;
    }

    if (dec >= base - 20u)
        return 20u;

    return base - dec;
}

// ---------------------------------------------------------------------
// Loop de juego por rondas
// ---------------------------------------------------------------------
static void play_Game(TronGame *game, Player *p1, Player *p2, int mode) {
    while (game->score.p1 < 3 && game->score.p2 < 3) {
        player_Intent p1_Intent = (player_Intent){  1, 0 };
        player_Intent p2_Intent = (player_Intent){ -1, 0 };

        player_spawn_center_left (game, p1, 1, TRON_P1_COLOR);
        player_spawn_center_right(game, p2, 2, TRON_P2_COLOR);

        occ_set(game, p1->col, p1->row, p1->id_cell);
        occ_set(game, p2->col, p2->row, p2->id_cell);

        map_draw_cell(game, p1->col, p1->row, p1->color, 0);
        map_draw_cell(game, p2->col, p2->row, p2->color, 0);

        bool     in_game = true;
        uint64_t start   = get_ms_since_boot();

        //calculo tick según el nivel actual
        uint32_t tick_ms = tron_tick_for_level(game);

        while (in_game) {
            uint64_t now     = get_ms_since_boot();
            uint64_t elapsed = now - start;

            if (elapsed < tick_ms) {
                // dormimos la diferencia para que el juego no corra demasiado rápido
                sleep_ms(tick_ms - elapsed);
                continue;
            }

            // volver a tomar referencia de tiempo
            start = get_ms_since_boot();

            // INPUT según modo
            switch (mode) {
                case 1: {
                    // player humano
                    tron_handle_input_edge(&p1_Intent);

                    // IA del bot
                    player_Intent bot_next = p2_Intent;
                    int decided = 0;

                    if (game->level >= 2) {
                        // nivel 2 o más: sigue al jugador
                        decided = ai_choose_dir_track(game, p2, p1, &bot_next);
                    } else {
                        // nivel 1: movimientos aleatorios
                        decided = ai_choose_dir_simple(game,p2, &bot_next);
                    }


                    if (decided) {
                        p2_Intent = bot_next;
                    }
                    break;
                }
                case 2: {
                    // modo coop
                    tron_handle_input_edge_coop(&p1_Intent, &p2_Intent);
                    break;
                }
                default:
                    return;
            }

            // actualizo posiciones
            int p2Action = player_action_tick(game, p2, p2_Intent);
            int p1Action = player_action_tick(game, p1, p1_Intent);

            // chequeo de colisiones / fin de ronda
            if (p2Action == 0 || p1Action == 0) {
                in_game = false;

                if (p2Action == 0 && p1Action == 0) {
                    game->score.p1++;
                    game->score.p2++;
                } else if (p1Action == 0) {
                    game->score.p2++;
                } else {
                    game->score.p1++;
                }

                score_update(game);
            }
        }

        // Entre rondas (pausa hasta que el usuario toque algo)
        if (game->score.p1 < 3 && game->score.p2 < 3) {
            while (1) {
                if (getchar() != 0)
                    break;
            }
        }

        // limpio ocupación
        map_free(game);
    }
}
