#include "utils.h"
#include <stdint.h>
#include "../include/syscall_call.h"
#include <stdbool.h>


#include <stdint.h>

void print_centered_line_Vram(const char *text, uint64_t screen_w, int row_cells, uint32_t fColor, uint32_t bgColor, int fontSize);
void print_centered_line_Back(const char *text, uint64_t screen_w, int row_cells, uint32_t fColor, uint32_t bgColor, int fontSize);

int string_to_int(const char *str) {
    int result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

int is_numeric(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    while (*str) {
        if (*str < '0' || *str > '9') {
            return 0;
        }
        str++;
    }
    return 1;
}

//aux = 1 vram aux != 1 back
void print_centered_line(const char *text, uint64_t screen_w, int row_cells, uint32_t fColor, uint32_t bgColor, int fontSize,  bool vram) {
    if (vram){
        print_centered_line_Vram(text,  screen_w, row_cells,  fColor,  bgColor,  fontSize);
    }else {
        print_centered_line_Back(text,  screen_w, row_cells,  fColor,  bgColor,  fontSize);
    } 
    
}


void print_centered_line_Vram(const char *text, uint64_t screen_w, int row_cells, uint32_t fColor, uint32_t bgColor, int fontSize) {
    // ancho estimado de un caracter en píxeles
    const int char_w = fontSize;  

    // convertir ancho de pantalla en "columnas" aproximadas
    int cols = (int)(screen_w / char_w);

    // largo del texto
    int len = 0;
    while (text[len] != '\0') len++;

    // columna inicial centrada
    int col = 0;
    if (cols > len)
        col = (cols - len) / 2;

    // imprimir centrado con los colores dados
    write_at_vram(text, col, row_cells, fColor, bgColor);
}


void print_centered_line_Back(const char *text, uint64_t screen_w, int row_cells, uint32_t fColor, uint32_t bgColor, int fontSize) {
    // ancho estimado de un caracter en píxeles
    const int char_w = fontSize;  

    // convertir ancho de pantalla en "columnas" aproximadas
    int cols = (int)(screen_w / char_w);

    // largo del texto
    int len = 0;
    while (text[len] != '\0') len++;

    // columna inicial centrada
    int col = 0;
    if (cols > len)
        col = (cols - len) / 2;

    // imprimir centrado con los colores dados
    write_at_back(text, col, row_cells, fColor, bgColor);
}

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
    static const char digits[] = "0123456789ABCDEF";
    char temp[65];
    int i = 0;

    if (base < 2 || base > 16) {
        buffer[0] = '\0';
        return 0;
    }

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }

    while (value > 0) {
        temp[i++] = digits[value % base];
        value /= base;
    }

    for (int j = 0; j < i; j++) {
        buffer[j] = temp[i - j - 1];
    }

    buffer[i] = '\0';
    return i;
}