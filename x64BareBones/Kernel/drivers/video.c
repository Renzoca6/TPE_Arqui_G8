#include "video.h"
#include "font8x16.h"

// Prototipo de la función auxiliar que convierte enteros a string en base dada
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

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

int vdSetFontScale(int s);   // retorna escala efectiva aplicada
int vdGetFontScale(void);

// ★ Implementación:
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
                putPixel(x, y, 0x000000, PIXEL_BACK); // fondo negro
        }
    }
}



// VRAM
void vdPrint(const char * str) {
	int i;
	for (i = 0; str[i] != 0; i++){
		vdPrintChar(str[i]);
	}
}

void vdPrintStyled(const char * str, uint32_t fColor, uint32_t bgColor) {
	int i;
	for (i = 0; str[i] != 0; i++){
		vdPrintCharStyled(str[i], fColor, bgColor);
	}
}

void vdPrintChar(char c) {
	vdPrintCharStyled(c, 0x00ffffff, 0x00000000);
}

void vdBackSpace(void) {
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
            putPixel(0x000000, px, py, PIXEL_VRAM);
        }
    }
}


void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor) {
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    if (c == '\n') { vdNewline(); return; }
    if (c == '\t') { vdPrintStyled("    ", fColor, bgColor); return; }
    if (c == '\b') { vdBackSpace(); return; }

    // Recorremos el glyph base 8x16
    for (int row = 0; row < BASE_FONT_H; row++) {
        unsigned char line = font8x16[(uint8_t)c][row];  
        for (int col = 0; col < BASE_FONT_W; col++) {
            uint8_t mask = (uint8_t)(0x80 >> col);
            uint32_t color = (line & mask) ? fColor : bgColor;

            // ★ bloque escalado g_scale × g_scale
            int posX = x + col * g_scale;   // ★ origen X del bloque
            int posY = y + row * g_scale;   // ★ origen Y del bloque
            for (int dy = 0; dy < g_scale; dy++) {
                uint32_t py = posY + dy;
                if (py >= H) continue;
                for (int dx = 0; dx < g_scale; dx++) {
                    uint32_t px = posX + dx;
                    if (px >= W) continue;
                    putPixel(color, px, py, PIXEL_VRAM); 
                }
            }
        }
    }

    // avance de cursor a la derecha según celda
    x += CELL_W;                                

    // wrap horizontal simple
    if (x + CELL_W > W) {                       // ★ si no entra el próximo
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





// Convierte un entero a string en base arbitraria (2, 10, 16, etc.)
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
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

// Imprime un número decimal
void vdPrintDec(uint64_t value) {
	vdPrintBase(value, 10);
}

// Imprime un número en hexadecimal
void vdPrintHex(uint64_t value) {
	vdPrintBase(value, 16);
}

// Imprime un número en binario
void vdPrintBin(uint64_t value) {
	vdPrintBase(value, 2);
}

// Función genérica: convierte un valor a string en base dada y lo imprime
void vdPrintBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base); // convierte el número en string
    vdPrint(buffer);                 // lo imprime con ncPrint
}

int intToStrSimple(int num, char* str) {
    int i = 0;
    
    if (num == 0) {
        str[i++] = '0';
    } else {
        while (num > 0) {
            str[i++] = (num % 10) + '0';
            num /= 10;
        }
    }

    str[i] = '\0';
    
    // Invertir la cadena
    int inicio = 0;
    int fin = i - 1;
    while (inicio < fin) {
        char temp = str[inicio];
        str[inicio] = str[fin];
        str[fin] = temp;
        inicio++;
        fin--;
    }
    
    return i;
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
    uint32_t pitch = VBE_mode_info->pitch;
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

	for (uint32_t y = 0; y < h; y++) {
		for (uint32_t x = 0; x < w; x++) {
			putPixel(color, x,y, PIXEL_BACK);
		}
		
	}

	present_fullframe();
	x = 0; y = 0;
}


void vdPrintHex64(uint64_t value) {
    const char *hex = "0123456789ABCDEF";
    vdPrint("0x");  // prefijo estándar
    for (int i = 60; i >= 0; i -= 4) {
        uint8_t nibble = (value >> i) & 0xF;
        vdPrintChar(hex[nibble]);
    }
}

void vdPrintDouble(double value, int precision) {
    if (value < 0) {
        vdPrintChar('-');
        value = -value;
    }

    // Parte entera
    uint64_t intPart = (uint64_t)value;
    double frac = value - (double)intPart;

    // Convertir parte entera a decimal normal
    char temp[32];
    int i = 0;
    if (intPart == 0)
        temp[i++] = '0';
    else {
        while (intPart > 0 && i < 31) {
            temp[i++] = '0' + (intPart % 10);
            intPart /= 10;
        }
    }
    while (i > 0)
        vdPrintChar(temp[--i]);

    // Si no se quieren decimales, terminamos
    if (precision <= 0)
        return;

    vdPrintChar('.');

    // Mostrar parte fraccionaria
    for (int j = 0; j < precision; j++) {
        frac *= 10.0;
        int digit = (int)frac;
        vdPrintChar('0' + digit);
        frac -= digit;
    }
}