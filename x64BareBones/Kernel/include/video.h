#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

typedef enum {
    PIXEL_VRAM,
    PIXEL_BACK
} PixelTarget;

/* ===================== */
/*  Primitivas de video  */
/* ===================== */
void putPixel(uint32_t hexColor, uint32_t x, uint32_t y, PixelTarget target);
void putFrame(void);

/* ============== */
/*    Getters     */
/* ============== */
uint32_t vdGetWidth(void);
uint32_t vdGetHeight(void);

/* ===================== */
/*   Texto en video      */
/* ===================== */
void vdPrint(const char *str, PixelTarget target);
void vdPrintStyled(const char *str, uint32_t fColor, uint32_t bgColor, PixelTarget target);
void vdPrintStyled_AT(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor, PixelTarget target);
void vdPrintChar(char c, PixelTarget target);
void vdBackSpace(PixelTarget target);
void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor, PixelTarget target);
void vdNewline(void);

/* ===================== */
/*  Utilidades BACK BUF  */
/* ===================== */
void vdclearScreenDB(uint32_t color);
void present_fullframe(void);
uint32_t fb_size_bytes(void);

/* ===================== */
/*  Escala de fuente     */
/* ===================== */
int vdSetFontScale(int s);
int vdGetFontScale(void);

/* ===================== */
/*  Utilidades varias    */
/* ===================== */
unsigned int str_to_uint_ignore_sign(const char *s);



static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
void vdPrintBase(uint64_t value, uint32_t base, PixelTarget target);
void vdPrintHex(uint64_t value, PixelTarget target);

#endif // VIDEO_H