// apps/tron/tron_main.c
#include <stdint.h>
#include <stddef.h>
#include "../lib/syscall_call.h"
#include "tronGame.h"

#define TRON_COLS   60
#define TRON_ROWS   40
#define TRON_CELL   16

static uint8_t OCC[TRON_COLS * TRON_ROWS]; // buffer estático de ocupación

int start(void) {
    TronGame game;
    // Elegí origen para centrar: (x0,y0)
    uint64_t H=0,W=0;
    get_screen_info(&H,&W); // si ya la tenés; si no, fijá W/H hardcode
    uint32_t board_w = TRON_COLS * TRON_CELL;
    uint32_t board_h = TRON_ROWS * TRON_CELL;
    uint32_t x0 = (W > board_w) ? (uint32_t)((W - board_w)/2) : 0;
    uint32_t y0 = (H > board_h) ? (uint32_t)((H - board_h)/2) : 0;

    tron_start(&game, OCC, x0, y0, TRON_COLS, TRON_ROWS, TRON_CELL);

    // loop simple (sincronizalo con timer si querés)
    while (1) {
        // TODO: leer teclado (sys_read no bloqueante, o tu buffer de teclado)
        //       y hacer tron_set_dir(&game.p1, dx,dy), tron_set_dir(&game.p2, dx,dy)

        tron_tick(&game);

        // (Opcional) sleep de unos ms para controlar velocidad (si tenés syscall de timer)
        // sys_sleep_ms(80);
    }
}
