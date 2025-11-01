#include "./include/player_Intent.h"
#include "./include/config.h"   // KEY_W, etc.

// Por defecto: P1 va a la derecha; P2 va a la izquierda


void tron_handle_input_edge(player_Intent p1) {
    char buf[20];
    int buf_dim = get_multiple_chars_sys(buf,20);
    while (buf_dim != 0){
        switch (buf[buf_dim-1]) {
            case KEY_W: p1 = (player_Intent){  0, -1 }; break;
            case KEY_S: p1 = (player_Intent){  0,  1 }; break;
            case KEY_A: p1 = (player_Intent){ -1,  0 }; break;
            case KEY_D: p1 = (player_Intent){  1,  0 }; break;
        }
    }
}


void tron_handle_input_edge_coop(player_Intent p1, player_Intent p2){
    char buf[20];
    int buf_dim = get_multiple_chars_sys(buf,20);


    while (buf_dim != 0){
        switch (buf[buf_dim-1]) {
            // --- Player 1 (WASD)
            case KEY_W: p1 = (player_Intent){  0, -1 }; break;
            case KEY_S: p1 = (player_Intent){  0,  1 }; break;
            case KEY_A: p1 = (player_Intent){ -1,  0 }; break;
            case KEY_D: p1 = (player_Intent){  1,  0 }; break;

            // --- Player 2 (Flechas)
            case KEY_UP:    p2 = (player_Intent){  0, -1 }; break;
            case KEY_DOWN:  p2 = (player_Intent){  0,  1 }; break;
            case KEY_LEFT:  p2 = (player_Intent){ -1,  0 }; break;
            case KEY_RIGHT: p2 = (player_Intent){  1,  0 }; break;
        }
    }

}