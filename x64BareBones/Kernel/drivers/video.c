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

static const int FONT_W = 8;
static const int FONT_H = 16;

//Puntero al back buffer
static uint8_t g_back_static[1024 * 768 * 3];
static uint8_t *g_back = g_back_static;
VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

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

    // mover cursor una celda atrás
    if (x >= FONT_W) {
        x -= FONT_W;
    } else if (y >= FONT_H) {
        y -= FONT_H;
        x = (W / FONT_W) * FONT_W;
    } else {
        return; // ya en (0,0)
    }

    // borrar el bloque 8x16 en (x,y)
    for (uint32_t py = y; py < y + FONT_H && py < H; py++) {
        for (uint32_t px = x; px < x + FONT_W && px < W; px++) {
            putPixel(0x000000, px, py, PIXEL_VRAM);
        }
    }
}

void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor) {
    // Si te pasás del borde, opcionalmente “clipeá”
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    // salto de linea
    if (c == '\n') {
		vdNewline();
        return;
    }

	if (c == '\t') {
		vdPrintStyled("    ", fColor, bgColor);
        return;
    }

	if (c == '\b'){
		vdBackSpace();
		return;
	}
	
    // recorro las 16 filas
    for (int row = 0; row < FONT_H; row++) {
        // Cada fila es un byte: bit7 = columna 0, bit0 = columna 7
        unsigned char line = font8x16[(uint8_t)c][row];

        for (int col = 0; col < FONT_W; col++) {
            uint32_t px = x + (uint32_t)col;
            uint32_t py = y + (uint32_t)row;

            // para que haga un salto de linea si llegue al final
            if (px >= W){
				x=0;
				y += FONT_H;
			};

			if (py >= H){
				//Hay que hacer un scrool 
			}
			
            // máscara: 0x80 >> col
            uint8_t mask = (uint8_t)(0x80 >> col);
            uint32_t color = (line & mask) ? fColor : bgColor;
            putPixel(color, px, py, PIXEL_VRAM);
        }
    }
    //avanzo en X 
    x += FONT_W; 
}

void vdclearScreen(void) {
    for (uint32_t py = 0; py < VBE_mode_info->height; py++)
        for (uint32_t px = 0; px < VBE_mode_info->width; px++)
            putPixel(0x000000, px, py, PIXEL_VRAM);
    x = y = 0;
}

void vdNewline() {
	x=0;
	y+=FONT_H;
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


// --- BACK buffer ---
static inline uint32_t fb_size_bytes(void) {
	return (uint32_t)VBE_mode_info->pitch * VBE_mode_info->height;//tambaio del frame buffer
}

static void present_fullframe(void) {
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

// Prueba: llená el BACK con azul puro y logueá los 3 primeros bytes
void test_fill_back_blue(void){
    const uint32_t w = VBE_mode_info->width;
    const uint32_t h = VBE_mode_info->height;
    const uint32_t pitch = VBE_mode_info->pitch;
    if (!g_back){ vdPrint("ERR back null\n"); return; }
    if (VBE_mode_info->bpp != 24){ vdPrint("ERR bpp!=24\n"); return; }

    // Azul (BGR): B=FF, G=00, R=00
    for (uint32_t y=0; y<h; y++){
        uint8_t* row = g_back + y*pitch;
        for (uint32_t x=0; x<w; x++){
            uint8_t* px = row + x*3;
            px[0]=0xFF; px[1]=0x00; px[2]=0x00;
        }
        // opcional: limpiar padding
        for (uint32_t i=w*3; i<pitch; i++) row[i]=0x00;
    }
    vdPrint("back[0..2]= "); vdPrintHex(g_back[0]); vdPrint(" ");
    vdPrintHex(g_back[1]); vdPrint(" "); vdPrintHex(g_back[2]); vdPrint("\n");
}

void vdclearScreenDB(uint32_t color) {
	const uint32_t w     = VBE_mode_info->width;
	const uint32_t h     = VBE_mode_info->height;
	const uint32_t pitch = VBE_mode_info->pitch;

	for (uint32_t y = 0; y < h; y++) {
		uint8_t *row = g_back + y * pitch;

		uint8_t B =  color        & 0xFF;
		uint8_t G = (color >> 8)  & 0xFF;
		uint8_t R = (color >> 16) & 0xFF;
		for (uint32_t x = 0; x < w; x++) {
			uint8_t *px = row + x * 3;
			px[0] = B; px[1] = G; px[2] = R;
		}
		
	}

	present_fullframe();
	x = 0; y = 0;
}