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

#endif