// =====================================================================
// player_Intent.c
// Lectura de entradas y decisión de intención para 1P y 2P
// =====================================================================

#include "./include/player_Intent.h"
#include "./include/config.h"       // KEY_W, KEY_S, KEY_A, KEY_D, KEY_UP, ...
#include "../include/syscall_call.h"
#include <stdint.h>
#include <stdbool.h>

// ---------------------------------------------------------------------
// Singleplayer: toma la última tecla válida (WASD) de un batch
// ---------------------------------------------------------------------
void tron_handle_input_edge(player_Intent *p1) {
    char buf[20];
    int n = get_multiple_chars_sys(buf, 20);
    for (int i = n - 1; i >= 0; --i) {
        switch (buf[i]) {
            case KEY_W: *p1 = (player_Intent){  0, -1 }; return;
            case KEY_S: *p1 = (player_Intent){  0,  1 }; return;
            case KEY_A: *p1 = (player_Intent){ -1,  0 }; return;
            case KEY_D: *p1 = (player_Intent){  1,  0 }; return;
        }
    }
    // si no apareció nada válido, se mantiene la intención anterior
}

// ---------------------------------------------------------------------
// Coop: en un barrido decide P1 (WASD) y P2 (flechas)
// ---------------------------------------------------------------------
void tron_handle_input_edge_coop(player_Intent *p1, player_Intent *p2) {
    char buf[20];
    int n = get_multiple_chars_sys(buf, 20);

    bool p1decided = false;
    bool p2decided = false;

    for (int i = n - 1; i >= 0 && (!p1decided || !p2decided); --i) {
        char k = buf[i];

        if (!p1decided) {
            switch (k) {
                case KEY_W: *p1 = (player_Intent){  0, -1 }; p1decided = true; break;
                case KEY_S: *p1 = (player_Intent){  0,  1 }; p1decided = true; break;
                case KEY_A: *p1 = (player_Intent){ -1,  0 }; p1decided = true; break;
                case KEY_D: *p1 = (player_Intent){  1,  0 }; p1decided = true; break;
                default: break;
            }
            if (p1decided && p2decided) break;
        }

        if (!p2decided) {
            switch (k) {
                case KEY_UP:    *p2 = (player_Intent){  0, -1 }; p2decided = true; break;
                case KEY_DOWN:  *p2 = (player_Intent){  0,  1 }; p2decided = true; break;
                case KEY_LEFT:  *p2 = (player_Intent){ -1,  0 }; p2decided = true; break;
                case KEY_RIGHT: *p2 = (player_Intent){  1,  0 }; p2decided = true; break;
                default: break;
            }
        }
    }
    // si alguno no decidió, mantiene su intención anterior
}
