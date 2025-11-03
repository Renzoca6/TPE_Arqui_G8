#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "types.h"

/* ===================== */
/*      Interfaz UI       */
/* ===================== */

/* Colores base del degradado */
#define TOP_COLOR    0x001A9BA0   // cyan/azul
#define BOTTOM_COLOR 0x00C4872B   // naranja/dorado

/** Muestra el menú de inicio. Retorna selección del usuario. */
int tron_show_start_menu(void);

/** Inicializa el marcador. */
void score_init(TronGame *G);

/** Actualiza el marcador en pantalla. */
void score_update(const TronGame *G);

/** Muestra el menú final. */
int tron_show_end_menu(bool coop, int won, int level);

#endif /* UI_H */
