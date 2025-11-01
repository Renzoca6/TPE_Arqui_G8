#include "./tron_game.h"
#include "../tron2/include/game.h"
#include "../tron2/include/map.h"
#include "../tron2/include/player.h"
#include "../tron2/include/types.h"
#include "../lib/syscall_call.h"
#include "../tron2/include/config.h"
#include "../tron2/include/ui.h"
#include "../tron2/include/player_Intent.h"
#include "../tron2/include/AI.h"
#include <stdint.h>

/* Prepara el tablero para jugar rondas (borra pantalla, recalcula grilla y bordes).
   NO toca el score. Usala antes de play_Game y también cuando el usuario elige “Continuar”. */
static void tron_reset_board(TronGame *game) {

    map_init(game);
    map_draw_grid_lines(game, 1);
    map_draw_border_lines(game, 1);

    // <- redibuja HUD
    score_update(game);

    // <- presenta (si el HUD va a back buffer, esto lo hace visible)
    present_fullframe();
}


/* Setup de match (una sola vez por partida): inicializa score,
   asocia p1/p2 al game y deja el tablero listo para arrancar. */
static void tron_setup_match(TronGame *game, Player *p1, Player *p2) {
    score_init(game);
    game->p1 = *p1;
    game->p2 = *p2;

    tron_reset_board(game);   // esto ahora dibuja tablero + HUD + presenta
}



/* ---------- PROTOTIPO (misma lógica, sólo sale a file-scope) ---------- */
static void play_Game(TronGame *game, Player *p1, Player *p2, int mode);

/* ================================ */
void startGame(void) {
    int mode = tron_show_start_menu();

    TronGame game;
    Player p1, p2;

    tron_setup_match(&game, &p1, &p2);   // tablero limpio + HUD + score en 0
    game.level = 1;                      // ← NUEVO: arrancamos siempre en nivel 1

    play_Game(&game, &p1, &p2, mode);    // juega primer BO5

    // Menú final y control de “continuar”
    for (;;) {
        clearwindow(0x000000);

        int toGo = 0;
        switch (mode) {
            case 1: { // SINGLE
                int gano_p1 = (game.score.p1 == 3 && game.score.p2 < 3);
                // ahora sí pasamos el level real
                toGo = tron_show_end_menu(0, gano_p1 ? 1 : 0, game.level);
                break;
            }
            case 2: { // COOP
                int gano_p1 = (game.score.p1 == 3);
                // en coop no vamos a escalar dificultad, pero igual pasamos level
                toGo = tron_show_end_menu(1, gano_p1 ? 1 : 2, game.level);
                break;
            }
            default:
                return;
        }

        if (toGo == 2) {
            /* “Continuar jugando” */

            // Si estamos en single y el jugador REAL ganó, recién ahí subimos nivel
            if (mode == 1 && game.score.p1 == 3 && game.score.p2 < 3) {
                if (game.level < 99)
                    game.level++;   // subir dificultad
            }

            // guardo el nivel para no perderlo cuando reseteo el match
            uint8_t lvl = game.level;

            tron_setup_match(&game, &p1, &p2);  // esto resetea score
            game.level = lvl;                   // ← lo vuelvo a poner

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

static uint32_t tron_tick_for_level(const TronGame *game) {
    uint32_t base = TRON_TICK_MS;   // lo que ya tenés en config
    uint8_t  lvl  = game->level ? game->level : 1;

    // cada nivel baja 8 ms, pero no menos de 20 ms
    uint32_t dec = (uint32_t)(lvl - 1) * 8u;

    if (base <= 20u)      // por si alguien puso un valor ridículo en config
        return base;

    if (dec >= base - 20u)
        return 20u;

    return base - dec;
}


/* =============================================================== */
/*             MISMA LÓGICA QUE TENÍAS, FUERA DE startGame         */
/* =============================================================== */
static void play_Game(TronGame *game, Player *p1, Player *p2, int mode){
    while (game->score.p1 < 3 && game->score.p2 < 3) {

        player_Intent p1_Intent = (player_Intent){  1, 0 };
        player_Intent p2_Intent = (player_Intent){ -1, 0 };

        player_spawn_center_left (game, p1, 1, TRON_P1_COLOR);
        player_spawn_center_right(game, p2, 2, TRON_P2_COLOR);

        occ_set(game, p1->col, p1->row, p1->id_cell);
        occ_set(game, p2->col, p2->row, p2->id_cell);

        map_draw_cell(game, p1->col, p1->row, p1->color, 0);
        map_draw_cell(game, p2->col, p2->row, p2->color, 0);

        bool in_game = true;
        uint64_t start = get_ms_since_boot();

        /* NUEVO: calculo tick según el nivel actual */
        uint32_t tick_ms = tron_tick_for_level(game);

        while (in_game) {
            uint64_t now     = get_ms_since_boot();
            uint64_t elapsed = now - start;
            if (elapsed < tick_ms) {
                sleep_ms(tick_ms - elapsed);
                continue;
            }
            start = get_ms_since_boot();

            // INPUT según modo
            switch (mode) {
                case 1: {
                    tron_handle_input_edge(&p1_Intent);
                    player_Intent bot_next = p2_Intent; // fallback
                    if (ai_choose_dir_simple(game, p2, &bot_next)) {
                        p2_Intent = bot_next;
                    }
                    break;
                }
                case 2: {
                    tron_handle_input_edge_coop(&p1_Intent, &p2_Intent);
                    break;
                }
                default:
                    return;
            }

            int p1Action = player_action_tick(game, p1, p1_Intent);
            int p2Action = player_action_tick(game, p2, p2_Intent);

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

        // Entre rondas
        if (game->score.p1 < 3 && game->score.p2 < 3) {
            while (1) {
                if (getchar() != 0) break;
            }
        }

        map_free(game);
    }
}


