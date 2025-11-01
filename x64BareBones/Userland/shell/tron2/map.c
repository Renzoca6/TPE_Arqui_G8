#include "./include/map.h"
#include "./include/config.h"
#include "../utils/utils.h"
#include "../lib/syscall_call.h"
#include <stdbool.h>
#include "./include/ui.h"

void  map_init(TronGame *G){
 uint64_t w = get_screen_width();
    uint64_t h = get_screen_height();

    // 1) origen del tablero
    G->grid.x0 = TRON_MARGIN_LEFT;
    G->grid.y0 = TRON_MARGIN_TOP;
    G->grid.line_color = TRON_GRID_COLOR_LINE;
    G->grid.bg_color = TRON_GRID_COLOR_BG;
    G->grid.border_color = TRON_GRID_COLOR_BORDER;
 
    // 2) tamaño de celda
    G->grid.cell_px = TRON_CELL_PX;

    // 3) ancho y alto útiles (ya sin márgenes)
    uint64_t usable_w = w - TRON_MARGIN_LEFT - TRON_MARGIN_RIGHT;
    uint64_t usable_h = h - TRON_MARGIN_TOP  - TRON_MARGIN_BOTTOM;

    // 4) convertir a celdas
    G->grid.cols = usable_w / TRON_CELL_PX;
    G->grid.rows = usable_h / TRON_CELL_PX;

    // 5) occ estática 
    static uint8_t occ_buffer[128 * 128];
    G->occ = occ_buffer;

    // 6) limpiar occ según el tamaño real
    uint32_t total = (uint32_t)G->grid.cols * (uint32_t)G->grid.rows;
    if (total > 128u * 128u) {
        total = 128u * 128u;   // por seguridad
    }

    for (uint32_t i = 0; i < total; i++) {
        G->occ[i] = 0;
    }
}


void map_draw_border_lines(const TronGame *G, int target){
    if (G == NULL)
        return;

    const Grid *g = &G->grid;

    uint32_t x0 = g->x0;
    uint32_t y0 = g->y0;
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

    // --- borde superior ---
    for (uint32_t y = y_top; y < y0; y++) {
        for (uint32_t x = x_left; x < x_right + t && x < screen_w; x++) {
            putPixel(color, x, y, target);
        }
    }

    // --- borde inferior ---
    for (uint32_t y = y_bottom; y < y_bottom + t && y < screen_h; y++) {
        for (uint32_t x = x_left; x < x_right + t && x < screen_w; x++) {
            putPixel(color, x, y, target);
        }
    }

    // --- borde izquierdo ---
    for (uint32_t y = y_top; y < y_bottom + t && y < screen_h; y++) {
        for (uint32_t x = x_left; x < x0 && x < screen_w; x++) {
            putPixel(color, x, y, target);
        }
    }

    // --- borde derecho ---
    for (uint32_t y = y_top; y < y_bottom + t && y < screen_h; y++) {
        for (uint32_t x = x_right; x < x_right + t && x < screen_w; x++) {
            putPixel(color, x, y, target);
        }
    }
}


void map_draw_grid_lines(const TronGame *G, int target) {
    const Grid *g = &G->grid;

    uint32_t startX = g->x0;
    uint32_t startY = g->y0;
    uint32_t endX   = g->x0 + (uint32_t)g->cols * g->cell_px;
    uint32_t endY   = g->y0 + (uint32_t)g->rows * g->cell_px;
 
    // 1) pintar fondo del tablero
    for (uint32_t y = startY; y < endY; y++) {
        for (uint32_t x = startX; x < endX; x++) {
            putPixel(g->bg_color, x, y, target);
        }
    }     

    // 2) líneas verticales
    for (uint16_t c = 0; c <= g->cols; c++) {
        uint32_t x = g->x0 + (uint32_t)c * g->cell_px;
        for (uint32_t y = g->y0; y < endY; y++) {
            putPixel(g->line_color, x, y, target);
        }
    }   
   
    // 3) líneas horizontales
    for (uint16_t r = 0; r <= g->rows; r++) {
        uint32_t y = g->y0 + (uint32_t)r * g->cell_px;
        for (uint32_t x = g->x0; x < endX; x++) {
            putPixel(g->line_color, x, y, target);
        }
    }
}


void  map_draw_cell(const TronGame *G, uint16_t col, uint16_t row, uint32_t color, int target){
    const Grid *g = &G->grid;

    uint32_t x0, y0;
    cell_to_pixel(g, col, row, &x0, &y0);

    // Dibuja la celda completa (cell_px x cell_px)
    for (uint32_t y = y0; y < y0 + g->cell_px; y++) {
        for (uint32_t x = x0; x < x0 + g->cell_px; x++) {
            putPixel(color, x, y, target);
        }
    }
    
}

void cell_to_pixel(const Grid *g, uint16_t col, uint16_t row, uint32_t *px, uint32_t *py) {
    // x = origen_x + columna * tamaño_celda
    if (px != 0) {
        *px = g->x0 + (uint32_t)col * g->cell_px;
    }

    // y = origen_y + fila * tamaño_celda
    if (py != 0) {
        *py = g->y0 + (uint32_t)row * g->cell_px;
    }
}


bool grid_contains_cell(const Grid *g, uint16_t col, uint16_t row) {
    return (col < g->cols && row < g->rows);
}


// Convierte (col, row) a índice lineal en el array occ[]
uint32_t occ_idx(const Grid *g, uint16_t col, uint16_t row) {
    return (uint32_t)row * g->cols + col;
}

// Devuelve 1 si la celda está ocupada, 0 si está libre o fuera de rango
uint8_t occ_get(const TronGame *G, uint16_t col, uint16_t row) {
    const Grid *g = &G->grid;

    if (!grid_contains_cell(g, col, row) || G->occ == NULL)
        return 0; // fuera del mapa o sin occ → libre

    uint32_t idx = occ_idx(g, col, row);
    return (G->occ[idx] > 0) ? 1 : 0;
}

// Marca una celda como ocupada (v != 0) o libre (v == 0)
void occ_set(TronGame *G, uint16_t col, uint16_t row, uint8_t v) {
    const Grid *g = &G->grid;

    if (!grid_contains_cell(g, col, row) || G->occ == NULL)
        return; // fuera del mapa o sin occ → ignorar

    uint32_t idx = occ_idx(g, col, row);
    G->occ[idx] = (v > 0) ? 1 : 0;
}

static void tron_clear_cell(const TronGame *G, uint16_t col, uint16_t row, int target) {
    const Grid *g = &G->grid;

    uint32_t x0, y0;
    cell_to_pixel(g, col, row, &x0, &y0);

    uint32_t size = g->cell_px;
    uint32_t x1 = x0 + size - 1;  // borde derecho
    uint32_t y1 = y0 + size - 1;  // borde inferior

    // 1) limpiar interior (dejamos 1 px de borde)
    for (uint32_t y = y0 + 1; y <= y1; y++) {
        for (uint32_t x = x0 + 1; x <= x1; x++) {
            putPixel(g->bg_color, x, y, target);
        }
    }

    // 2) redibujar los 4 bordes de la celda

    // arriba
    for (uint32_t x = x0; x <= x1; x++) {
        putPixel(g->line_color, x, y0, target);
    }

    // abajo
    for (uint32_t x = x0; x <= x1; x++) {
        putPixel(g->line_color, x, y1+1, target);
    }

    // izquierda
    for (uint32_t y = y0; y <= y1; y++) {
        putPixel(g->line_color, x0, y, target);
    }

    // derecha
    for (uint32_t y = y0; y <= y1; y++) {
        putPixel(g->line_color, x1+1, y, target);
    }
}

void map_free(TronGame *G) {
    if (G == NULL || G->occ == NULL)
        return;

    Grid *g = &G->grid;
    uint32_t cols = g->cols;
    uint32_t rows = g->rows;

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

