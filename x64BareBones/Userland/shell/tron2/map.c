// =====================================================================
// map.c
// Inicialización de grilla, dibujo, y manejo de ocupación (occ)
// =====================================================================

#include "./include/map.h"
#include "./include/config.h"
#include "../utils/utils.h"
#include "../include/syscall_call.h"
#include <stdbool.h>
#include "./include/ui.h"

// ---------------------------------------------------------------------
// Inicializa la grilla y limpia occ
// ---------------------------------------------------------------------
void map_init(TronGame *G) {
    uint64_t w = get_screen_width();
    uint64_t h = get_screen_height();

    // 1) Origen del tablero
    G->grid.x0          = TRON_MARGIN_LEFT;
    G->grid.y0          = TRON_MARGIN_TOP;
    G->grid.line_color  = TRON_GRID_COLOR_LINE;
    G->grid.bg_color    = TRON_GRID_COLOR_BG;
    G->grid.border_color= TRON_GRID_COLOR_BORDER;

    // 2) Tamaño de celda
    G->grid.cell_px = TRON_CELL_PX;

    // 3) Área útil (sin márgenes)
    uint64_t usable_w = w - TRON_MARGIN_LEFT - TRON_MARGIN_RIGHT;
    uint64_t usable_h = h - TRON_MARGIN_TOP  - TRON_MARGIN_BOTTOM;

    // 4) Convertir a celdas
    G->grid.cols = usable_w / TRON_CELL_PX;
    G->grid.rows = usable_h / TRON_CELL_PX;

    // 5) occ estática (128x128)
    static uint8_t occ_buffer[128 * 128];
    G->occ = occ_buffer;

    // 6) Limpiar occ según tamaño real (cap a 128x128 por seguridad)
    uint32_t total = (uint32_t)G->grid.cols * (uint32_t)G->grid.rows;
    if (total > 128u * 128u) total = 128u * 128u;

    for (uint32_t i = 0; i < total; i++) G->occ[i] = 0;
}

// ---------------------------------------------------------------------
// Dibuja un borde de grosor cell_px alrededor del tablero
// ---------------------------------------------------------------------
void map_draw_border_lines(const TronGame *G, int target) {
    if (!G) return;

    const Grid *g = &G->grid;

    uint32_t x0 = g->x0, y0 = g->y0;
    uint32_t w  = (uint32_t)g->cols * g->cell_px;
    uint32_t h  = (uint32_t)g->rows * g->cell_px;
    uint32_t t  = g->cell_px;   // grosor del borde
    uint32_t color = g->border_color;

    uint32_t screen_w = get_screen_width();
    uint32_t screen_h = get_screen_height();

    uint32_t x_left   = (x0 >= t) ? (x0 - t) : 0;
    uint32_t y_top    = (y0 >= t) ? (y0 - t) : 0;
    uint32_t x_right  = x0 + w;
    uint32_t y_bottom = y0 + h;

    if (x_right + t > screen_w)  x_right  = screen_w - t;
    if (y_bottom + t > screen_h) y_bottom = screen_h - t;

    // Superior
    for (uint32_t y = y_top; y < y0; y++)
        for (uint32_t x = x_left; x < x_right + t && x < screen_w; x++)
            putPixel(color, x, y, target);

    // Inferior
    for (uint32_t y = y_bottom; y < y_bottom + t && y < screen_h; y++)
        for (uint32_t x = x_left; x < x_right + t && x < screen_w; x++)
            putPixel(color, x, y, target);

    // Izquierdo
    for (uint32_t y = y_top; y < y_bottom + t && y < screen_h; y++)
        for (uint32_t x = x_left; x < x0 && x < screen_w; x++)
            putPixel(color, x, y, target);

    // Derecho
    for (uint32_t y = y_top; y < y_bottom + t && y < screen_h; y++)
        for (uint32_t x = x_right; x < x_right + t && x < screen_w; x++)
            putPixel(color, x, y, target);
}

// ---------------------------------------------------------------------
// Dibuja el tablero (fondo + líneas de grilla)
// ---------------------------------------------------------------------
void map_draw_grid_lines(const TronGame *G, int target) {
    const Grid *g = &G->grid;

    uint32_t startX = g->x0;
    uint32_t startY = g->y0;
    uint32_t endX   = g->x0 + (uint32_t)g->cols * g->cell_px;
    uint32_t endY   = g->y0 + (uint32_t)g->rows * g->cell_px;

    // 1) Fondo
    for (uint32_t y = startY; y < endY; y++)
        for (uint32_t x = startX; x < endX; x++)
            putPixel(g->bg_color, x, y, target);

    // 2) Líneas verticales
    for (uint16_t c = 0; c <= g->cols; c++) {
        uint32_t x = g->x0 + (uint32_t)c * g->cell_px;
        for (uint32_t y = g->y0; y < endY; y++)
            putPixel(g->line_color, x, y, target);
    }

    // 3) Líneas horizontales
    for (uint16_t r = 0; r <= g->rows; r++) {
        uint32_t y = g->y0 + (uint32_t)r * g->cell_px;
        for (uint32_t x = g->x0; x < endX; x++)
            putPixel(g->line_color, x, y, target);
    }
}

// ---------------------------------------------------------------------
// Pinta una celda completa del color indicado
// ---------------------------------------------------------------------
void map_draw_cell(const TronGame *G, uint16_t col, uint16_t row, uint32_t color, int target) {
    const Grid *g = &G->grid;

    uint32_t x0, y0;
    cell_to_pixel(g, col, row, &x0, &y0);

    for (uint32_t y = y0; y < y0 + g->cell_px; y++)
        for (uint32_t x = x0; x < x0 + g->cell_px; x++)
            putPixel(color, x, y, target);
}

void cell_to_pixel(const Grid *g, uint16_t col, uint16_t row, uint32_t *px, uint32_t *py) {
    if (px) *px = g->x0 + (uint32_t)col * g->cell_px;  // x = origen_x + col * tamaño
    if (py) *py = g->y0 + (uint32_t)row * g->cell_px;  // y = origen_y + fila * tamaño
}

bool grid_contains_cell(const Grid *g, uint16_t col, uint16_t row) {
    return (col < g->cols && row < g->rows);
}

uint32_t occ_idx(const Grid *g, uint16_t col, uint16_t row) {
    return (uint32_t)row * g->cols + col;
}

// ---------------------------------------------------------------------
// Lectura/escritura de ocupación
// ---------------------------------------------------------------------
uint8_t occ_get(const TronGame *G, uint16_t col, uint16_t row) {
    const Grid *g = &G->grid;
    if (!grid_contains_cell(g, col, row) || G->occ == NULL) return 0;
    uint32_t idx = occ_idx(g, col, row);
    return (G->occ[idx] > 0) ? 1 : 0;
}

void occ_set(TronGame *G, uint16_t col, uint16_t row, uint8_t v) {
    const Grid *g = &G->grid;
    if (!grid_contains_cell(g, col, row) || G->occ == NULL) return;
    uint32_t idx = occ_idx(g, col, row);
    G->occ[idx] = (v > 0) ? 1 : 0;
}

// ---------------------------------------------------------------------
// Limpia una celda dejando 1 px de borde (para rearmar la grilla con solo las celdas usadas)
// ---------------------------------------------------------------------
static void tron_clear_cell(const TronGame *G, uint16_t col, uint16_t row, int target) {
    const Grid *g = &G->grid;

    uint32_t x0, y0; cell_to_pixel(g, col, row, &x0, &y0);
    uint32_t size = g->cell_px;
    uint32_t x1 = x0 + size - 1;  // borde derecho
    uint32_t y1 = y0 + size - 1;  // borde inferior

    // 1) Interior (dejando 1 px de borde)
    for (uint32_t y = y0 + 1; y <= y1; y++)
        for (uint32_t x = x0 + 1; x <= x1; x++)
            putPixel(g->bg_color, x, y, target);

    // 2) Redibujar 4 bordes de la celda
    for (uint32_t x = x0; x <= x1; x++) putPixel(g->line_color, x, y0, target);     // arriba
    for (uint32_t x = x0; x <= x1; x++) putPixel(g->line_color, x, y1 + 1, target); // abajo (ver nota)
    for (uint32_t y = y0; y <= y1; y++) putPixel(g->line_color, x0, y, target);     // izquierda
    for (uint32_t y = y0; y <= y1; y++) putPixel(g->line_color, x1 + 1, y, target); // derecha (ver nota)
}

// ---------------------------------------------------------------------
// Limpia todo el tablero (solo celdas ocupadas)
// ---------------------------------------------------------------------
void map_free(TronGame *G) {
    if (!G || !G->occ) return;

    Grid *g = &G->grid;
    uint32_t cols = g->cols, rows = g->rows;

    for (uint16_t row = 0; row < rows; row++) {
        for (uint16_t col = 0; col < cols; col++) {
            uint32_t idx = (uint32_t)row * cols + col;
            if (G->occ[idx] != 0) {
                tron_clear_cell(G, col, row, 0);
                G->occ[idx] = 0;
            }
        }
    }
}
