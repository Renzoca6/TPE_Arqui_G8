#ifndef VIDEO_H
#define VIDEO_H
#include <stdint.h>

typedef enum {
    PIXEL_VRAM,
    PIXEL_BACK
} PixelTarget;

// ===================
// Primitivas de video
// ===================
void putPixel24(uint32_t hexColor, uint32_t x, uint32_t y, PixelTarget target);

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

// =====================
// Utilidades BACK BUFFER
// =====================
void vdclearScreenDB(uint32_t color);
void present_fullframe(void);
int bytes_per_pixel(void);
uint32_t fb_size_bytes(void);

#endif // VIDEO_H
