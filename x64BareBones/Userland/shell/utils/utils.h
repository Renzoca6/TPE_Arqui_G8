#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>


uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);
void print_centered_line(const char *text, uint64_t screen_w, int row_cells, uint32_t fColor, uint32_t bgColor, int fontSize, bool aux);
int string_to_int(const char *str);
int is_numeric(const char *str);

#endif