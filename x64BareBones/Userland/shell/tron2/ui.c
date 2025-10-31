#include <stdint.h>
#include <stdbool.h>

#include "../lib/syscall_call.h"   // get_screen_width, clearwindow, getchar_sys, sleep_ms, present_fullframe
#include "../utils/utils.h"        // print_centered_line



// Colores base (los que sacamos de tu imagen)
#define TOP_COLOR    0x001A9BA0   // cyan/azul
#define BOTTOM_COLOR 0x00C4872B   // naranja/dorado

//
static uint32_t lerp_color(uint32_t c1, uint32_t c2, uint32_t num, uint32_t den)
{
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
        char ch = getchar_sys();
        if (ch == 0) {
            sleep_ms(10);
            continue;
        }

        if (ch == 'A' || ch == 'a')
            return 1;   // un jugador
        if (ch == 'B' || ch == 'b')
            return 2;   // dos jugadores
        if (ch == '\n' || ch == '\r' || ch == 27)
            return 0;   // salir
    }

    return 0;
}