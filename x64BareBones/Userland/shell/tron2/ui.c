// =====================================================================
// ui.c
// Fondo TRON, menús y marcador de puntaje
// =====================================================================

#include <stdint.h>
#include <stdbool.h>

#include "./include/ui.h"
#include "../include/syscall_call.h"  // get_screen_width, clearwindow, getchar, sleep_ms, present_fullframe
#include "../utils/utils.h"           // print_centered_line, uintToBase
#include "./include/types.h"
#include "./include/config.h"

// ---------------------------------------------------------------------
// Scoreboard
// ---------------------------------------------------------------------
void score_init(TronGame *G) {
    G->score.p1 = 0;
    G->score.p2 = 0;

    do_resize("2");
    write_at_back("--P1--",  1, 6, TRON_P1_COLOR, 0x000000);
    write_at_back(" Score:", 0, 7, TRON_P1_COLOR, 0x000000);
    write_at_back("0",       3, 8, TRON_P1_COLOR, 0x000000);

    write_at_back("--P2--", 57, 6, TRON_P2_COLOR, 0x000000);
    write_at_back("Score:", 57, 7, TRON_P2_COLOR, 0x000000);
    write_at_back("0",      59, 8, TRON_P2_COLOR, 0x000000);
    do_resize("1");
}

void score_update(const TronGame *G) {
    char buf[8];

    // aumentar tamaño de letra
    do_resize("2");

    // Player 1
    uintToBase((uint64_t)G->score.p1, buf, 10);
    write_at_vram(buf, 3, 8, TRON_P1_COLOR, 0x000000);

    // Player 2
    uintToBase((uint64_t)G->score.p2, buf, 10);
    write_at_vram(buf, 59, 8, TRON_P2_COLOR, 0x000000);

    // restaurar tamaño normal
    do_resize("1");
}

// ---------------------------------------------------------------------
// Interpolación de color (lerp)
// ---------------------------------------------------------------------
uint32_t lerp_color(uint32_t c1, uint32_t c2, uint32_t num, uint32_t den) {
    if (den == 0) den = 1;

    uint8_t r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = (c1) & 0xFF;
    uint8_t r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = (c2) & 0xFF;

    uint8_t r = (uint8_t)(r1 + ((int)r2 - (int)r1) * (int)num / (int)den);
    uint8_t g = (uint8_t)(g1 + ((int)g2 - (int)g1) * (int)num / (int)den);
    uint8_t b = (uint8_t)(b1 + ((int)b2 - (int)b1) * (int)num / (int)den);

    return (r << 16) | (g << 8) | b;
}

// ---------------------------------------------------------------------
// Degradé superior e inferior
// ---------------------------------------------------------------------
void draw_top_bottom_bands(int target) {
    uint32_t w = get_screen_width();
    uint32_t h = get_screen_height();

    const uint32_t DARK = 0x00000000;
    uint32_t band_h = h / TRON_BAND; if (band_h == 0) band_h = 1;

    // Banda superior
    for (uint32_t dy = 0; dy < band_h; dy++) {
        uint32_t c = lerp_color(TOP_COLOR, DARK, dy, band_h);
        for (uint32_t x = 0; x < w; x++) putPixel(c, x, dy, target);
    }

    // Centro negro
    for (uint32_t y = band_h; y < 2 * band_h && y < h; y++)
        for (uint32_t x = 0; x < w; x++)
            putPixel(0x000000, x, y, target);

    // Banda inferior
    for (uint32_t i = 0; i < band_h; i++) {
        uint32_t y = h - band_h + i; if (y >= h) break;
        uint32_t c = lerp_color(BOTTOM_COLOR, DARK, band_h - 1 - i, band_h);
        for (uint32_t x = 0; x < w; x++) putPixel(c, x, y, target);
    }
}

// ---------------------------------------------------------------------
// Scanlines (opcional, desactivado por defecto)
// ---------------------------------------------------------------------
void tron_scanlines(int target) {
    uint32_t w = get_screen_width();
    uint32_t h = get_screen_height();
    for (uint32_t y = 0; y < h; y++){
        if ((y % 3) == 0){
            for (uint32_t x = 0; x < w; x++){
                putPixel(0x000000, x, y, target);
            }
        }
    }
}

// ---------------------------------------------------------------------
// Fondo TRON (se dibuja en back y se presenta)
// ---------------------------------------------------------------------
void tron_draw_background(void) {
    draw_top_bottom_bands(1);  // 1 = BACK
    // tron_scanlines(1);
    present_fullframe();
}

// ---------------------------------------------------------------------
// Menú inicial
// ---------------------------------------------------------------------
int tron_show_start_menu(void) {
    do_resize("2");
    tron_draw_background();

    uint32_t bgColor    = 0x000000;
    uint32_t titleColor = 0x00FFFF; // cian
    uint32_t textColor  = 0x99FFFF; // azul claro
    uint32_t hintColor  = 0x44AAAA; // azul más oscuro

    uint64_t sw = get_screen_width();

    print_centered_line("=== T R O N ===", sw,  7, titleColor, bgColor, 16, false);
    print_centered_line("A) Single Player", sw, 9, textColor,  bgColor, 16, false);
    print_centered_line("B) Two Players",   sw,10, textColor,  bgColor, 16, false);
    print_centered_line("Enter) Exit",      sw,11, textColor,  bgColor, 16, false);
    print_centered_line("Press any key to continue...", sw, 14, hintColor, bgColor, 16, false);

    present_fullframe();
    do_resize("1");

    while (1) {
        char ch = getchar();
        if (ch == 0) { sleep_ms(10); continue; }
        if (ch == 'a') return 1;
        if (ch == 'b') return 2;
        if (ch == '\n' || ch == '\r' || ch == 27) return 0;
    }
}

// ---------------------------------------------------------------------
// Menú de fin de ronda
// ---------------------------------------------------------------------
int tron_show_end_menu(bool coop, int won, int level) {
    do_resize("2");
    tron_draw_background();

    uint32_t bgColor    = 0x000000;
    uint32_t titleColor = won ? 0x00FF00 : 0xFF0000;  // verde si ganó, rojo si perdió
    uint32_t textColor  = 0x99FFFF;
    uint32_t hintColor  = 0x44AAAA;

    uint64_t sw = get_screen_width();

    char title[64], numBuf[16];
    uintToBase(won ? level + 1 : level, numBuf, 10);

        // Construir mensaje
    char *p = title;
    if (coop) {
        const char *msg;
        if (won == 1)       msg = "PLAYER 1 WINS!";
        else if (won == 0)  msg = "PLAYER 2 WINS!";
        else /* won == 2 */ msg = "TIE!";
        while (*msg) *p++ = *msg++;
        *p = '\0';
    } else {
        const char *msg;
        if (won == 1)       msg = "YOU WIN! NEXT LEVEL ";
        else if (won == 0)  msg = "YOU LOSE. RETRY LEVEL ";
        else /* won == 2 */ msg = "TIE. RETRY LEVEL ";
        while (*msg) *p++ = *msg++;
        char *n = numBuf; while (*n) *p++ = *n++;
        *p = '\0';
    }


    print_centered_line(title, sw, 7,  titleColor, bgColor, 16, false);
    print_centered_line("A) Return to main menu", sw, 9,  textColor, bgColor, 16, false);
    print_centered_line("B) Continue playing",    sw,10, textColor, bgColor, 16, false);
    print_centered_line("Press any key to continue...", sw, 13, hintColor, bgColor, 16, false);

    present_fullframe();
    do_resize("1");

    while (1) {
        char ch = getchar();
        if (ch == 0) { sleep_ms(10); continue; }
        if (ch == 'a') return 1;
        if (ch == 'b') return 2;
        if (ch == '\n' || ch == '\r' || ch == 27) return 0;
    }
}
