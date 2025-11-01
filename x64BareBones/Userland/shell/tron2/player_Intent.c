#include "./include/player_Intent.h"
#include "./include/input.h"
#include "./include/config.h"   // KEY_W, etc.

// Por defecto: P1 va a la derecha; P2 va a la izquierda


void tron_handle_input_edge(g_player_intent p1) {
    char * buf[20];
    int buf_dim = get_multiple_chars_sys(buf,20);
    while (buf_dim != 0){
        switch (buf[buf_dim-1]) {
            case KEY_W: p1 = (g_player_intent){  0, -1 }; break;
            case KEY_S: p1 = (g_player_intent){  0,  1 }; break;
            case KEY_A: p1 = (g_player_intent){ -1,  0 }; break;
            case KEY_D: p1 = (g_player_intent){  1,  0 }; break;
        }
    }
}


void tron_handle_input_edge_coop(g_player_intent p1, g_player_intent p2,){
    char * buf[20];
    int buf_dim = get_multiple_chars_sys(buf,20);


    while (buf_dim != 0){
        switch (buf[buf_dim-1]) {
            // --- Player 1 (WASD)
            case KEY_W: p1 = (g_player_intent){  0, -1 }; break;
            case KEY_S: p1 = (g_player_intent){  0,  1 }; break;
            case KEY_A: p1 = (g_player_intent){ -1,  0 }; break;
            case KEY_D: p1 = (g_player_intent){  1,  0 }; break;

            // --- Player 2 (Flechas)
            case KEY_UP:    p2 = (g_player_intent){  0, -1 }; break;
            case KEY_DOWN:  p2 = (g_player_intent){  0,  1 }; break;
            case KEY_LEFT:  p2 = (g_player_intent){ -1,  0 }; break;
            case KEY_RIGHT: p2 = (g_player_intent){  1,  0 }; break;
        }
    }

}