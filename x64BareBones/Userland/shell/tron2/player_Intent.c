#include "./include/player_Intent.h"
#include "./include/config.h"   // KEY_W, etc.
#include "../include/syscall_call.h"
#include <stdint.h>
#include <stdbool.h>
// Por defecto: P1 va a la derecha; P2 va a la izquierda


void tron_handle_input_edge(player_Intent *p1) {
    char buf[20];
    int buf_dim = get_multiple_chars_sys(buf,5);
    bool flag = true;
    while (buf_dim != 0 && flag){
        switch (buf[buf_dim-1]) {
            case KEY_W: *p1 = (player_Intent){  0, -1 };
            flag = false;
            break;
            case KEY_S: *p1 = (player_Intent){  0,  1 };
            flag = false;
            break;
            case KEY_A: *p1 = (player_Intent){ -1,  0 };
            flag = false;
            break;
            case KEY_D: *p1 = (player_Intent){  1,  0 }; 
            flag = false;
            break;
        }
        buf_dim--;
    }
}


void tron_handle_input_edge_coop(player_Intent *p1, player_Intent *p2){
    char buf[20];
    int buf_dim = get_multiple_chars_sys(buf,20);
    
    bool p1decided = false;
    bool p2decided = false;

    while (buf_dim != 0 && !p1decided && !p2decided){
        switch (buf[buf_dim-1]) {
            // --- Player 1 (WASD)
            if (!p1decided){
            case KEY_W: *p1 = (player_Intent){  0, -1 }; break;
            case KEY_S: *p1 = (player_Intent){  0,  1 }; break;
            case KEY_A: *p1 = (player_Intent){ -1,  0 }; break;
            case KEY_D: *p1 = (player_Intent){  1,  0 }; break;
            }

            // --- Player 2 (Flechas)
            if (!p2decided){
            case KEY_UP:    *p2 = (player_Intent){  0, -1 }; break;  
            case KEY_DOWN:  *p2 = (player_Intent){  0,  1 }; break;
            case KEY_LEFT:  *p2 = (player_Intent){ -1,  0 }; break;
            case KEY_RIGHT: *p2 = (player_Intent){  1,  0 }; break;
            }
        }
        buf_dim--;
    }

}