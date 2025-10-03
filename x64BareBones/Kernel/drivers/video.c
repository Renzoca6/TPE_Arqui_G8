#include "video.h"
#include "realTimeClock.h"

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

// Imprime un string carácter a carácter usando ncPrintChar
void ncPrint(const char * string)
{
	int i;
	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

// Imprime un carácter en la posición actual con el atributo ya existente en VRAM
void ncPrintChar(char character)
{
	*currentVideo = character;   // escribe el carácter (byte 0 de la celda)
	currentVideo += 2;           // avanza 2 bytes (carácter + atributo)
}

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
