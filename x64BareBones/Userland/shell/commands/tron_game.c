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
void startGame(void)
{
    int mode = tron_show_start_menu();

    TronGame game;
    Player p1, p2;

    tron_setup_match(&game, &p1, &p2);   // tablero limpio + HUD visible + score 0-0
    play_Game(&game, &p1, &p2, mode);    // juega primer BO5

    // Menú final y control de “continuar”
    for (;;) {
        clearwindow(0x000000);

        int toGo = 0;
        switch (mode) {
            case 1: { // SINGLE
                if (game.score.p1 == 3 && game.score.p2 == 3)
                    toGo = tron_show_end_menu(0, 0, 1);
                else if (game.score.p1 == 3)
                    toGo = tron_show_end_menu(0, 1, 1);
                else
                    toGo = tron_show_end_menu(0, 0, 1);
                break;
            }
            case 2: { // COOP
                if (game.score.p1 == 3 && game.score.p2 == 3)
                    toGo = tron_show_end_menu(1, 2, 1);
                else if (game.score.p1 == 3)
                    toGo = tron_show_end_menu(1, 1, 1);
                else
                    toGo = tron_show_end_menu(1, 2, 1);
                break;
            }
            default:
                return; // startGame es void
        }

        if (toGo == 2) {
            // “Continuar jugando”: empezar un match nuevo YA (resetea tablero y score)
            tron_setup_match(&game, &p1, &p2);
            play_Game(&game, &p1, &p2, mode);
            continue;  // vuelve a mostrar el end menu al terminar ese match
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


/* =============================================================== */
/*             MISMA LÓGICA QUE TENÍAS, FUERA DE startGame         */
/* =============================================================== */
static void play_Game(TronGame *game, Player *p1, Player *p2, int mode)
{
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

        while (in_game) {
            uint64_t now = get_ms_since_boot();
            uint64_t elapsed = now - start;
            if (elapsed < TRON_TICK_MS) {
                sleep_ms(TRON_TICK_MS - elapsed);
                continue;
            }
            start = get_ms_since_boot();

            // INPUT según modo (idéntico a tu código)
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
                    println("tie, both players lost");
                    game->score.p1++;
                    game->score.p2++;
                } else if (p1Action == 0) {
                    println("player 2 wins");
                    game->score.p2++;
                } else {
                    println("player 1 wins");
                    game->score.p1++;
                }

                score_update(game); // tu display de score
            }
        }

        // Entre rondas: esperar tecla para seguir (si nadie llegó a 3)
        if (game->score.p1 < 3 && game->score.p2 < 3) {
            while (1) {
                if (getchar() != 0) break;
            }
        }

        // Limpiar tablero para la próxima ronda
        map_free(game);
    }
}


