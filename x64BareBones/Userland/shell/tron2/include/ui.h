#ifndef UI_H
#define UI_H
#include <stddef.h>
#include <stdint.h>
#include "types.h"

// Colores base (los que sacamos de tu imagen)
#define TOP_COLOR    0x001A9BA0   // cyan/azul
#define BOTTOM_COLOR 0x00C4872B   // naranja/dorado

int tron_show_start_menu(void);
uint32_t lerp_color(uint32_t c1, uint32_t c2, uint32_t num, uint32_t den);




#endif