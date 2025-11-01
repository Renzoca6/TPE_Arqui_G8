#include <stdint.h>
#include <stdbool.h>

#include "./include/ui.h"
#include "../lib/syscall_call.h"   // get_screen_width, clearwindow, getchar_sys, sleep_ms, present_fullframe
#include "../utils/utils.h"        // print_centered_line
#include "./include/types.h"
#include "./include/config.h"




static void tron_fill_rect_px(uint32_t x0, uint32_t y0,
                              uint32_t x1, uint32_t y1,
                              uint32_t color,
                              int target)
{
    for (uint32_t y = y0; y < y1; y++) {
        for (uint32_t x = x0; x < x1; x++) {
            putPixel(color, x, y, target);
        }
    }
}


void tron_draw_scores(const TronGame *G, int p1_score, int p2_score)
{
    // pantalla
    uint32_t screen_w = get_screen_width();
    uint32_t screen_h = get_screen_height();

    const Grid *g = &G->grid;

    // dónde está la grilla
    uint32_t grid_left  = g->x0;                               // debería ser TRON_MARGIN_LEFT
    uint32_t grid_right = g->x0 + g->cols * g->cell_px;        // = pantalla - TRON_MARGIN_RIGHT

    // áreas negras que vos dijiste:
    // izquierda: 0 → TRON_MARGIN_LEFT - TRON_CELL_PX
    uint32_t left_x0 = 0;
    uint32_t left_x1 = TRON_MARGIN_LEFT - TRON_CELL_PX;

    // derecha: (TRON_MARGIN_RIGHT + TRON_CELL_PX) contado desde el borde derecho de la grilla
    // pero como la grilla llega hasta screen_w - TRON_MARGIN_RIGHT,
    // la zona derecha empieza en:
    uint32_t right_x0 = grid_right + TRON_CELL_PX;
    uint32_t right_x1 = screen_w;

    // vertical: dejamos libres los degradés de arriba y abajo
    uint32_t y0 = TRON_MARGIN_TOP;
    uint32_t y1 = screen_h - TRON_MARGIN_BOTTOM;

    // 1) limpiamos las dos franjas con negro
    tron_fill_rect_px(left_x0,  y0, left_x1,  y1, 0x00000000, 0);  // 0 = VRAM
    tron_fill_rect_px(right_x0, y0, right_x1, y1, 0x00000000, 0);

    // 2) ahora escribimos el texto
    // tu write_at_vram escribe en CELDAS, no en px, así que convertimos:
    // asumo fuente 8x16 (vos la usás así en otros lados)
    const int CELL_W = 8;
    const int CELL_H = 16;

    // fila donde lo vamos a poner (aprox al medio de la parte útil)
    int row = (int)( (y0 + (y1 - y0)/2) / CELL_H );

    // ----- PLAYER 1 -----
    int col_left = (int)( (left_x0 + 8) / CELL_W );  // un pequeño padding
    write_at_vram("Player 1", col_left, row, 0x00FFFFFF, 0x00000000);

    char buf1[16];
    uintToBase((uint64_t)p1_score, buf1, 10);
    write_at_vram(buf1, col_left, row + 1, 0x00FFFFFF, 0x00000000);

    // ----- PLAYER 2 -----
    // columna de la derecha: convertimos el x de inicio del panel derecho a celdas
    int col_right = (int)( (right_x0 + 8) / CELL_W );
    write_at_vram("Player 2", col_right, row, 0x00FFFFFF, 0x00000000);

    char buf2[16];
    uintToBase((uint64_t)p2_score, buf2, 10);
    write_at_vram(buf2, col_right, row + 1, 0x00FFFFFF, 0x00000000);
}

//
uint32_t lerp_color(uint32_t c1, uint32_t c2, uint32_t num, uint32_t den){
    if (den == 0) den = 1;

    uint8_t r1 = (c1 >> 16) & 0xFF;
    uint8_t g1 = (c1 >>  8) & 0xFF;
    uint8_t b1 = (c1      ) & 0xFF;

    uint8_t r2 = (c2 >> 16) & 0xFF;
    uint8_t g2 = (c2 >>  8) & 0xFF;
    uint8_t b2 = (c2      ) & 0xFF;

    uint8_t r = (uint8_t)(r1 + ((int)r2 - (int)r1) * (int)num / (int)den);
    uint8_t g = (uint8_t)(g1 + ((int)g2 - (int)g1) * (int)num / (int)den);
    uint8_t b = (uint8_t)(b1 + ((int)b2 - (int)b1) * (int)num / (int)den);

    return (r << 16) | (g << 8) | b;
}

//Degrade arriba y abajo
void draw_top_bottom_bands(int target) {
    uint32_t w = get_screen_width();
    uint32_t h = get_screen_height();

    // Colores sacados de tu imagen
    const uint32_t DARK         = 0x00000000;

    // Cada banda ocupa 1/3
    uint32_t band_h = h / 4;
    if (band_h == 0) band_h = 1;

    // --- banda de arriba ---
    for (uint32_t dy = 0; dy < band_h; dy++) {
        uint32_t c = lerp_color(TOP_COLOR, DARK, dy, band_h);
        for (uint32_t x = 0; x < w; x++) {
            putPixel(c, x, dy, target);
        }
    }

    // --- centro negro ---
    for (uint32_t y = band_h; y < 2 * band_h && y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            putPixel(0x000000, x, y, target);
        }
    }

    // --- banda de abajo ---
    for (uint32_t i = 0; i < band_h; i++) {
        uint32_t y = h - band_h + i;
        if (y >= h) break;
        // degradé de naranja -> negro
        uint32_t c = lerp_color(BOTTOM_COLOR, DARK, band_h - 1 - i, band_h);
        for (uint32_t x = 0; x < w; x++) {
            putPixel(c, x, y, target);
        }
    }
}


//lineas negras
void tron_scanlines(int target)
{
    uint32_t w = get_screen_width();
    uint32_t h = get_screen_height();

    for (uint32_t y = 0; y < h; y++) {
        if ((y % 3) == 0) {  // cada 3 líneas
            for (uint32_t x = 0; x < w; x++) {
                // oscurecemos un poco: en lugar de leer el color, ponemos negro
                // (si tu motor soporta alpha, acá iría un blend)
                putPixel(0x000000, x, y, target);
            }
        }
    }
}

// armado del fondo 
void tron_draw_background(void){
    // 1 = BACK
    draw_top_bottom_bands(1);
    //tron_scanlines(1);  
    present_fullframe();
}

//el menui
int tron_show_start_menu(void)
{
    // letra más grande
    do_resize("2");

    // fondo TRON
    tron_draw_background();

    // colores de texto
    uint32_t bgColor     = 0x000000;   // negro
    uint32_t titleColor  = 0x00FFFF;   // cian brillante
    uint32_t textColor   = 0x99FFFF;   // azul claro
    uint32_t hintColor   = 0x44AAAA;   // azul más oscuro

    uint64_t sw = get_screen_width();

    // título más centrado (fila 5 aprox)
    print_centered_line("=== T R O N ===", sw, 7,  titleColor, bgColor, 16, false);

    // opciones
    print_centered_line("A) Un jugador",    sw, 9,  textColor, bgColor, 16, false);
    print_centered_line("B) Dos jugadores", sw, 10, textColor, bgColor, 16, false);
    print_centered_line("Enter) Salir",     sw, 11, textColor, bgColor, 16, false);

    print_centered_line("Presione una tecla para continuar...", sw, 14, hintColor, bgColor, 16, false);

    present_fullframe();
    do_resize("1");
    // loop de entrada
    while (1) {
        char ch = getchar();
        if (ch == 0) {
            sleep_ms(10);
            continue;
        }

        if (ch == 'a' )
            return 1;   // un jugador
        if (ch == 'b')
            return 2;   // dos jugadores
        if (ch == '\n' || ch == '\r' || ch == 27)
            return 0;   // salir
    }

    return 0;
}