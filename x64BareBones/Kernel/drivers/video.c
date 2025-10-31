#include "video.h"
#include "font8x16.h"
// Buffer temporal para almacenar strings numéricos
static char buffer[64] = { '0' };

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint64_t  framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;
unsigned int x = 0, y = 0;

static const int BASE_FONT_W = 8;
static const int BASE_FONT_H = 16;

static int g_scale = 1;             // factor de escala (1..4)
#define CELL_W (BASE_FONT_W * g_scale)  // ancho de celda actual
#define CELL_H (BASE_FONT_H * g_scale)  // alto de celda actual


int vdSetFontScale(int s){
    g_scale = s;
    return g_scale;
}
int vdGetFontScale(void){
    return g_scale;
}



/*------------------------------ Prototipos internos -----------------------*/
static inline void fb_copy(uint8_t *dst, const uint8_t *src, uint32_t n);
static inline void fb_fill_row(uint8_t *row, uint32_t width, uint8_t B, uint8_t G, uint8_t R);
static void vdScrollUp_pixels(uint32_t rows);




//Puntero al back buffer
static uint8_t g_back_static[1024 * 768 * 3];
static uint8_t *g_back = g_back_static;
VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

uint32_t vdGetWidth(void)  { 
    return VBE_mode_info->width;  
}
uint32_t vdGetHeight(void) { 
    return VBE_mode_info->height; 
}

void putPixel(uint32_t color, uint32_t x, uint32_t y, PixelTarget target) {
    const uint32_t w = VBE_mode_info->width;
    const uint32_t h = VBE_mode_info->height;
    if (x >= w || y >= h) return;  // protección de límites

    const uint32_t pitch = VBE_mode_info->pitch;   // bytes por fila
    const uint8_t bpp = 3;                         // bytes por píxel (24 bpp)
    uint8_t *base;

    if (target == PIXEL_BACK && g_back) {
        base = g_back;                              // dibuja en back buffer
    } else {
        base = (uint8_t*)(uintptr_t)VBE_mode_info->framebuffer; // dibuja directo en VRAM
    }

    uint8_t *px = base + y * pitch + x * bpp;

    // El modo VBE usa formato BGR (Blue, Green, Red)
    px[0] =  color        & 0xFF;   // Blue
    px[1] = (color >> 8 ) & 0xFF;   // Green
    px[2] = (color >>16 ) & 0xFF;   // Red
}

void putFrame(){
    for (uint32_t y = 0; y < VBE_mode_info->height; y++) {
        for (uint32_t x = 0; x < VBE_mode_info->width; x++) {
                putPixel(0x000000, x, y, PIXEL_BACK); // fondo negro
        }
    }
}



// VRAM
void vdPrint(const char * str, PixelTarget target) {
	int i;
	for (i = 0; str[i] != 0; i++){
		vdPrintChar(str[i], target);
	}
}

void vdPrintStyled(const char * str, uint32_t fColor, uint32_t bgColor, PixelTarget target) {
	int i;
	for (i = 0; str[i] != 0; i++){
		vdPrintCharStyled(str[i], fColor, bgColor, target);
	}
}


void vdPrintStyled_AT(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor, PixelTarget target){
    if (!str) return;

    // guardar
    unsigned int old_x = x;
    unsigned int old_y = y;

    // setear
    if (col < 0) col = 0;
    if (fil < 0) fil = 0;
    x = (unsigned int)(col * CELL_W);
    y = (unsigned int)(fil * CELL_H);

    // imprimir
    vdPrintStyled(str, fColor, bgColor, target);

    // restaurar
    x = old_x;
    y = old_y;
}


void vdPrintChar(char c, PixelTarget target) {
	vdPrintCharStyled(c, 0x00ffffff, 0x00000000, target);
}

void vdBackSpace(PixelTarget target) {
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    if (x >= CELL_W) {
        x -= CELL_W;                       
    } else if (y >= CELL_H) {
        y -= CELL_H;                       
        x = (W / CELL_W) * CELL_W;         
    } else {
        return;
    }

    // borrar bloque de la celda actual
    for (uint32_t py = y; py < y + CELL_H && py < H; py++) {
        for (uint32_t px = x; px < x + CELL_W && px < W; px++) {
            putPixel(0x000000, px, py, target);
        }
    }
}


void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor, PixelTarget target) {
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    if (c == '\n') { vdNewline(); return; }
    if (c == '\t') { vdPrintStyled("    ", fColor, bgColor, target); return; }
    if (c == '\b') { vdBackSpace(target); return; }

    // Recorremos el glyph base 8x16
    for (int row = 0; row < BASE_FONT_H; row++) {
        unsigned char line = font8x16[(uint8_t)c][row];  
        for (int col = 0; col < BASE_FONT_W; col++) {
            uint8_t mask = (uint8_t)(0x80 >> col);
            uint32_t color = (line & mask) ? fColor : bgColor;

            
            int posX = x + col * g_scale;   
            int posY = y + row * g_scale;   
            for (int dy = 0; dy < g_scale; dy++) {
                uint32_t py = posY + dy;
                if (py >= H) continue;
                for (int dx = 0; dx < g_scale; dx++) {
                    uint32_t px = posX + dx;
                    if (px >= W) continue;
                    putPixel(color, px, py, target); 
                }
            }
        }
    }

    // avance de cursor a la derecha según celda
    x += CELL_W;                                

    // wrap horizontal simple
    if (x + CELL_W > W) {                       
        x = 0;
        y += CELL_H;
		if (y + CELL_H > H) {
        vdScrollUp_pixels(CELL_H);
    }
    }

    // TODO: scroll vertical si y+CELL_H > H
}

// Copia 'n' bytes sin libc
static inline void fb_copy(uint8_t *dst, const uint8_t *src, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) dst[i] = src[i];
}

// Llena 'n' bytes con el patrón B,G,R (24bpp) por fila
static inline void fb_fill_row(uint8_t *row, uint32_t width, uint8_t B, uint8_t G, uint8_t R) {
    for (uint32_t x = 0; x < width; x++) {
        uint8_t *px = row + x * 3;
        px[0] = B; px[1] = G; px[2] = R;
    }
}


static void vdScrollUp_pixels(uint32_t rows) {
    const uint32_t pitch = VBE_mode_info->pitch;
    const uint32_t w     = VBE_mode_info->width;
    const uint32_t h     = VBE_mode_info->height;

    if (rows == 0 || rows >= h) {
        // si te pasan algo raro, limpiá todo
        uint8_t *vram = (uint8_t*)(uintptr_t)VBE_mode_info->framebuffer;
        for (uint32_t y = 0; y < h; y++) {
            fb_fill_row(vram + y * pitch, w, 0x00, 0x00, 0x00);
        }
        x = 0; y = 0;
        return;
    }

    uint8_t *vram = (uint8_t*)(uintptr_t)VBE_mode_info->framebuffer;

    // 1) desplazar hacia arriba: fila src=y -> dst=y-rows
    for (uint32_t ysrc = rows; ysrc < h; ysrc++) {
        uint8_t *src = vram + ysrc       * pitch;
        uint8_t *dst = vram + (ysrc-rows)* pitch;
        fb_copy(dst, src, pitch);
    }

    // 2) limpiar la franja inferior (últimas 'rows' filas)
    for (uint32_t yclr = h - rows; yclr < h; yclr++) {
        uint8_t *row = vram + yclr * pitch;
        fb_fill_row(row, w, 0x00, 0x00, 0x00);  // negro
    }

    // 3) reubicar cursor en la última línea visible
    if (y >= rows) y -= rows; else y = 0;
    if (y + CELL_H > h) y = (h >= CELL_H) ? (h - CELL_H) : 0;
    if (x + CELL_W > w) x = 0;
}


void vdNewline(void) {
    const uint32_t H = VBE_mode_info->height;
    x = 0;
    y += CELL_H;
    if (y + CELL_H > H) {
        vdScrollUp_pixels(CELL_H);
        // y quedó ajustada adentro de vdScrollUp_pixels()
    }
}

unsigned int str_to_uint_ignore_sign(const char *s) {
    while (*s==' '||*s=='\t'||*s=='\r'||*s=='\n'||*s=='\v'||*s=='\f') s++;
    if (*s == '+' || *s == '-') s++;  // ignora signo

    unsigned int x = 0;
    while (*s >= '0' && *s <= '9') {
        x = x * 10u + (unsigned)(*s - '0');
        s++;
    }
    return x;  // sin chequeos de overflow/errores
}


// --- BACK buffer ---
inline uint32_t fb_size_bytes(void) {
	return (uint32_t)VBE_mode_info->pitch * VBE_mode_info->height;//tambaio del frame buffer
}

void present_fullframe(void) {
    uint8_t *vram  = (uint8_t*) (uintptr_t) VBE_mode_info->framebuffer;
    uint32_t pitch = VBE_mode_info->pitch; //cantidad de byts que ocupa una fila 
    uint32_t h     = VBE_mode_info->height;

    // voy copiando fila por fila 
    for (uint32_t y = 0; y < h; y++) {
        uint8_t *dst = vram  + y * pitch;// vream
        uint8_t *src = g_back + y * pitch;  // back buffer
        // para que sea mas eficiente tendria que hacerlo es asm 
        for (uint32_t i = 0; i < pitch; i++) dst[i] = src[i];
    }
}


void vdclearScreenDB(uint32_t color) {
    const uint32_t w     = VBE_mode_info->width;
    const uint32_t h     = VBE_mode_info->height;
    const uint32_t pitch = VBE_mode_info->pitch;

    uint8_t B =  color        & 0xFF;
    uint8_t G = (color >> 8 ) & 0xFF;
    uint8_t R = (color >> 16) & 0xFF;

    for (uint32_t y = 0; y < h; y++) {
        uint8_t *row = g_back + y * pitch;     // ← Puntero, no uint8_t
        fb_fill_row(row, w, B, G, R);
    }

    present_fullframe();

    x = 0;
    y = 0;
}

void vdPrintHex(uint64_t value, PixelTarget target) {
    vdPrintBase(value, 16, target);
}

void vdPrintBase(uint64_t value, uint32_t base, PixelTarget target) {
    uintToBase(value, buffer, base); // convierte el número en string
    vdPrint(buffer, target);                 // lo imprime con ncPrint
}


uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    // Calcula los dígitos sucesivos en la base elegida
    do
    {
        uint32_t remainder = value % base;   // resto en la base
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    // Termina el string con '\0'
    *p = 0;

    // Invierte el string (porque se construyó al revés)
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;  // devuelve cuántos dígitos tiene el número
}




