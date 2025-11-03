#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/* ===================== */
/*   Parámetros del juego */
/* ===================== */

#define TRON_CELL_PX          10
#define TRON_MARGIN_LEFT      128
#define TRON_MARGIN_TOP       128
#define TRON_MARGIN_RIGHT     128
#define TRON_MARGIN_BOTTOM    64

#define TRON_GRID_COLOR_LINE   0x303030
#define TRON_GRID_COLOR_BG     0x000000
#define TRON_GRID_COLOR_BORDER 0x444444
#define TRON_P1_COLOR          0xC4872B
#define TRON_P2_COLOR          0x1A9BA0
#define TRON_BG_COLOR          0x000000

#define TRON_TICK_MS           50
#define TRON_BAND              4

/* ===================== */
/*     Controles          */
/* ===================== */

#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'
#define KEY_UP    ((char)0xF1)
#define KEY_LEFT  ((char)0xF2)
#define KEY_DOWN  ((char)0xF3)
#define KEY_RIGHT ((char)0xF4)

#endif /* CONFIG_H */
