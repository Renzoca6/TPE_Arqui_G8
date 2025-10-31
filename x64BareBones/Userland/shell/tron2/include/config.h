#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define TRON_CELL_PX          10       // tamaño de celda en píxeles
#define TRON_MARGIN_LEFT      128
#define TRON_MARGIN_TOP       128
#define TRON_MARGIN_RIGHT     128
#define TRON_MARGIN_BOTTOM    64
#define TRON_GRID_COLOR_LINE  0x303030 //0x303030
#define TRON_GRID_COLOR_BG    0x000000
#define TRON_P1_COLOR         0x00FF00
#define TRON_P2_COLOR         0x00AAFF
#define TRON_BG_COLOR         0x000000
#define TRON_TICK_MS          80

// teclas (ajustá a tu keycode)
#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'
#define KEY_UP    0x4800  // si usás scancodes, ajustá
#define KEY_LEFT  0x4B00
#define KEY_DOWN  0x5000
#define KEY_RIGHT 0x4D00

#endif