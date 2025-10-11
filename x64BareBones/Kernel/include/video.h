#ifndef VIDEO_H
#define VIDEO_H
#include <stdint.h>

// ===================
// Primitivas de video
// ===================
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void vdClearScreen(void);

// ==============
// Texto en video
// ==============
void vdPrintChar(char c);
void vdBackspace(void);
void vdPrint(const char *str);
void vdPrintStyled(const char *str, uint32_t fColor, uint32_t bgColor);
void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor);
void vdNewline();

// =====================
// Utilidades numéricas
// =====================
void vdPrintDec(uint64_t value);
void vdPrintHex(uint64_t value);
void vdPrintBin(uint64_t value);
void vdPrintBase(uint64_t value, uint32_t base);


#endif // VIDEO_H
