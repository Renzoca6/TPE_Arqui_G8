#include "video.h"
#include "realTimeClock.h"
#include "font8x16.h"

// Prototipo de la función auxiliar que convierte enteros a string en base dada
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

// Buffer temporal para almacenar strings numéricos
static char buffer[64] = { '0' };

// Dirección base de la memoria de video en modo texto VGA (0xB8000)
static uint8_t * const video = (uint8_t*)0xB8000;

// Puntero que avanza según vamos escribiendo en pantalla
static uint8_t * currentVideo = (uint8_t*)0xB8000;

// Ancho y alto de la pantalla en modo texto (80 columnas x 25 filas)
static const uint32_t width = 80;
static const uint32_t height = 25 ;

// Imprime un string con colores específicos (fg y bg)
void ncPrintColor(const char * string, uint8_t fc, uint8_t bg){
	int i;
	for (i = 0; string[i] != 0; i++)
		ncPrintCharColor(string[i], fc, bg);
}

// Imprime un carácter con color de foreground (fc) y background (bg)
void ncPrintCharColor(char character, uint8_t fc, uint8_t bg) {
    uint8_t attr = (bg << 4) | (fc & 0x0F);  // arma el byte de atributo
    *currentVideo = character;               // byte 0 = carácter
    *(currentVideo + 1) = attr;              // byte 1 = atributo
    currentVideo += 2;                       // avanza a la próxima celda
}

// Inserta un salto de línea (llena con espacios hasta el inicio de la siguiente línea)
void ncNewline()
{
	do
	{
		ncPrintChar(' ');  // imprime espacios para completar la línea actual
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0); // hasta alinear al inicio
}

// Imprime un número decimal
void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrint(const char * string){
	return;
}

// Imprime un número en hexadecimal
void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

// Imprime un número en binario
void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

// Función genérica: convierte un valor a string en base dada y lo imprime
void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base); // convierte el número en string
    ncPrint(buffer);                 // lo imprime con ncPrint
}

// Limpia toda la pantalla con espacios y resetea el cursor
void ncClear()
{
	int i;
	for (i = 0; i <height * width; i++)
		video[i * 2] = ' ';  // pone espacio en cada celda (byte de carácter)
	currentVideo = video;     // reinicia el puntero al inicio de la VRAM
}
void ncPrintChar(char c){
	return;
}


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
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;
unsigned int x = 0, y = 0;

static const int FONT_W = 8;
static const int FONT_H = 16;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}


void vdPrint(const char * str){
	int i;
	for (i = 0; str[i] != 0; i++){
		vdPrintChar(str[i]);
	}
}

void vdPrintStyled(const char * str, uint32_t fColor, uint32_t bgColor){
	int i;
	for (i = 0; str[i] != 0; i++){
		vdPrintCharStyled(str[i], fColor, bgColor);
	}
}

void vdPrintChar(char c){
	vdPrintCharStyled(c, 0x00ffffff, 0x00000000);
}

void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor){
    // Si te pasás del borde, opcionalmente “clipeá”
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    // salto de linea
    if (c == '\n') {
        x = 0;
        y += FONT_H;  // alto del glyph
        return;
    }

	if (c == '\t') {
		vdPrintStyled("    ", fColor, bgColor);
        return;
    }

	if (c == '\b'){
		vdBackSpace();
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
            putPixel(color, px, py);
        }
    }
    //avanzo en X 
    x += FONT_W; 
}

void vdBackSpace(){
	const uint32_t W = VBE_mode_info->width;

	if (x >= FONT_W){
		x -= FONT_W;
		
		for (int i = x; i < x+ FONT_W; i++){
			for (int j = y; j < y+FONT_H; i++){
				putPixel(0x00000000, i, j);
			}
			
		}
		
	}else if (y > FONT_H){
		y -= FONT_H;
		x = W - 1;
	}

}

void vdBackspace(void) {
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    // 1) Mover cursor hacia atrás
    if (x >= FONT_W) {
        x -= FONT_W;
		
		
		for (uint32_t py = y; py < y + FONT_H; py++) {
        if (py >= H) break;
        for (uint32_t px = x; px < x + FONT_W; px++) {
            if (px >= W) break;
            putPixel(0x000000, px, py);
        }
    }
    } else if (y >= FONT_H) {
        // Subir una línea y saltar al final alineado a celdas de 8 px
        y -= FONT_H;
        x = (W / FONT_W) * FONT_W;
    } else {
        // Estamos en (0,0): nada para borrar
        return;
    }


}

void clearScreen(void){
    const uint32_t H = VBE_mode_info->height;

	for (int i = 0; i < y; i++){
		for (int j = 0; j < VBE_mode_info->width; i++)
		{
			putPixel(0x000000, i, j);
		}
		
	}
	x = 0;
	y = 0;
	
}


// Convierte un entero a string en base arbitraria (2, 10, 16, etc.)
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
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
