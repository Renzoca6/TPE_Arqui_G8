#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

void ncPrint(const char * string);
void ncPrintColor(const char * string, uint8_t fc, uint8_t bg);
void ncPrintCharColor(char character, uint8_t fc, uint8_t bg);
void ncPrintTime();
void ncPrintChar(char character);
void ncNewline();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();


void vdPrint(const char * str);
void ncPrintChar(char c);
void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor);
void vdPrintStyled(const char * str, uint32_t fColor, uint32_t bgColor);
void vdBackspace(void);
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void clearScreen(void);

#endif