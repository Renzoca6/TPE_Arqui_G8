#include "video.h"
#include "font8x16.h"
#include "lib.h"

// ---------------------------------------------------------------------
// Estructura VBE y variables globales
// ---------------------------------------------------------------------
struct vbe_mode_info_structure {
    uint16_t attributes;
    uint8_t  window_a;
    uint8_t  window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t  w_char;
    uint8_t  y_char;
    uint8_t  planes;
    uint8_t  bpp;
    uint8_t  banks;
    uint8_t  memory_model;
    uint8_t  bank_size;
    uint8_t  image_pages;
    uint8_t  reserved0;

    uint8_t  red_mask;
    uint8_t  red_position;
    uint8_t  green_mask;
    uint8_t  green_position;
    uint8_t  blue_mask;
    uint8_t  blue_position;
    uint8_t  reserved_mask;
    uint8_t  reserved_position;
    uint8_t  direct_color_attributes;

    uint64_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t  reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

static char buffer[64] = { '0' };
static const int BASE_FONT_W = 8;
static const int BASE_FONT_H = 16;
static int g_scale = 1;

#define CELL_W (BASE_FONT_W * g_scale)
#define CELL_H (BASE_FONT_H * g_scale)

// ---------------------------------------------------------------------
// Estado de video
// ---------------------------------------------------------------------
static uint8_t g_back_static[1024 * 768 * 3];
static uint8_t *g_back = g_back_static;
VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

unsigned int x = 0, y = 0;

// ---------------------------------------------------------------------
// Escalado de fuente
// ---------------------------------------------------------------------
int vdSetFontScale(int s) {
    g_scale = s;
    return g_scale;
}

int vdGetFontScale(void) {
    return g_scale;
}

// ---------------------------------------------------------------------
// Funciones internas
// ---------------------------------------------------------------------
static inline void fb_copy(uint8_t *dst, const uint8_t *src, uint32_t n);
static inline void fb_fill_row(uint8_t *row, uint32_t width, uint8_t B, uint8_t G, uint8_t R);
static void vdScrollUp_pixels(uint32_t rows);

// ---------------------------------------------------------------------
// Acceso básico a framebuffer
// ---------------------------------------------------------------------
uint32_t vdGetWidth(void)  { return VBE_mode_info->width;  }
uint32_t vdGetHeight(void) { return VBE_mode_info->height; }

void putPixel(uint32_t color, uint32_t x, uint32_t y, PixelTarget target) {
    const uint32_t w = VBE_mode_info->width;
    const uint32_t h = VBE_mode_info->height;
    if (x >= w || y >= h) return;

    const uint32_t pitch = VBE_mode_info->pitch;
    const uint8_t  bpp   = 3;
    uint8_t *base = (target == PIXEL_BACK && g_back) ? g_back : (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;

    uint8_t *px = base + y * pitch + x * bpp;
    px[0] =  color        & 0xFF;
    px[1] = (color >> 8 ) & 0xFF;
    px[2] = (color >>16 ) & 0xFF;
}

void putFrame(void) {
    uint32_t total_bytes = VBE_mode_info->pitch * VBE_mode_info->height;
    memset(g_back, 0x00, total_bytes);
}

// ---------------------------------------------------------------------
// Funciones de impresión
// ---------------------------------------------------------------------
void vdPrint(const char *str, PixelTarget target) {
    for (int i = 0; str[i] != 0; i++)
        vdPrintChar(str[i], target);
}

void vdPrintStyled(const char *str, uint32_t fColor, uint32_t bgColor, PixelTarget target) {
    for (int i = 0; str[i] != 0; i++)
        vdPrintCharStyled(str[i], fColor, bgColor, target);
}

void vdPrintStyled_AT(const char *str, int col, int fil, uint32_t fColor, uint32_t bgColor, PixelTarget target) {
    if (!str) return;

    unsigned int old_x = x, old_y = y;

    if (col < 0) col = 0;
    if (fil < 0) fil = 0;
    x = (unsigned int)(col * CELL_W);
    y = (unsigned int)(fil * CELL_H);

    vdPrintStyled(str, fColor, bgColor, target);

    x = old_x;
    y = old_y;
}

void vdPrintChar(char c, PixelTarget target) {
    vdPrintCharStyled(c, 0x00ffffff, 0x00000000, target);
}

void vdBackSpace(PixelTarget target) {
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    if (x >= CELL_W) {
        x -= CELL_W;
    } else if (y >= CELL_H) {
        y -= CELL_H;
        x = (W / CELL_W) * CELL_W;
    } else {
        return;
    }

    for (uint32_t py = y; py < y + CELL_H && py < H; py++)
        for (uint32_t px = x; px < x + CELL_W && px < W; px++)
            putPixel(0x000000, px, py, target);
}

void vdPrintCharStyled(char c, uint32_t fColor, uint32_t bgColor, PixelTarget target) {
    const uint32_t W = VBE_mode_info->width;
    const uint32_t H = VBE_mode_info->height;

    if (c == '\n') { vdNewline(); return; }
    if (c == '\t') { vdPrintStyled("    ", fColor, bgColor, target); return; }
    if (c == '\b') { vdBackSpace(target); return; }

    for (int row = 0; row < BASE_FONT_H; row++) {
        unsigned char line = font8x16[(uint8_t)c][row];
        for (int col = 0; col < BASE_FONT_W; col++) {
            uint8_t mask = (uint8_t)(0x80 >> col);
            uint32_t color = (line & mask) ? fColor : bgColor;

            int posX = x + col * g_scale;
            int posY = y + row * g_scale;
            for (int dy = 0; dy < g_scale; dy++) {
                uint32_t py = posY + dy;
                if (py >= H) continue;
                for (int dx = 0; dx < g_scale; dx++) {
                    uint32_t px = posX + dx;
                    if (px >= W) continue;
                    putPixel(color, px, py, target);
                }
            }
        }
    }

    x += CELL_W;
    if (x + CELL_W > W) {
        x = 0;
        y += CELL_H;
        if (y + CELL_H > H)
            vdScrollUp_pixels(CELL_H);
    }
}

// ---------------------------------------------------------------------
// Funciones internas framebuffer
// ---------------------------------------------------------------------
static inline void fb_copy(uint8_t *dst, const uint8_t *src, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) dst[i] = src[i];
}

static inline void fb_fill_row(uint8_t *row, uint32_t width, uint8_t B, uint8_t G, uint8_t R) {
    for (uint32_t x = 0; x < width; x++) {
        uint8_t *px = row + x * 3;
        px[0] = B; px[1] = G; px[2] = R;
    }
}

static void vdScrollUp_pixels(uint32_t rows) {
    const uint32_t pitch = VBE_mode_info->pitch;
    const uint32_t w = VBE_mode_info->width;
    const uint32_t h = VBE_mode_info->height;

    uint8_t *vram = (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;

    if (rows == 0 || rows >= h) {
        for (uint32_t y = 0; y < h; y++)
            fb_fill_row(vram + y * pitch, w, 0x00, 0x00, 0x00);
        x = y = 0;
        return;
    }

    for (uint32_t ysrc = rows; ysrc < h; ysrc++) {
        uint8_t *src = vram + ysrc * pitch;
        uint8_t *dst = vram + (ysrc - rows) * pitch;
        fb_copy(dst, src, pitch);
    }

    for (uint32_t yclr = h - rows; yclr < h; yclr++)
        fb_fill_row(vram + yclr * pitch, w, 0x00, 0x00, 0x00);

    y = (y >= rows) ? (y - rows) : 0;
    if (y + CELL_H > h) y = (h >= CELL_H) ? (h - CELL_H) : 0;
    if (x + CELL_W > w) x = 0;
}

// ---------------------------------------------------------------------
// Utilidades
// ---------------------------------------------------------------------
void vdNewline(void) {
    const uint32_t H = VBE_mode_info->height;
    x = 0;
    y += CELL_H;
    if (y + CELL_H > H)
        vdScrollUp_pixels(CELL_H);
}

unsigned int str_to_uint_ignore_sign(const char *s) {
    while (*s==' '||*s=='\t'||*s=='\r'||*s=='\n'||*s=='\v'||*s=='\f') s++;
    if (*s == '+' || *s == '-') s++;
    unsigned int x = 0;
    while (*s >= '0' && *s <= '9') {
        x = x * 10u + (unsigned)(*s - '0');
        s++;
    }
    return x;
}

// ---------------------------------------------------------------------
// Back buffer y presentación
// ---------------------------------------------------------------------
inline uint32_t fb_size_bytes(void) {
    return (uint32_t)VBE_mode_info->pitch * VBE_mode_info->height;
}

void present_fullframe(void) {
    uint8_t *vram  = (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;
    uint32_t pitch = VBE_mode_info->pitch;
    uint32_t h     = VBE_mode_info->height;
    uint32_t total = pitch * h;
    memcpy(vram, g_back, total);
}

void vdclearScreenDB(uint32_t color) {
    const uint32_t w = VBE_mode_info->width;
    const uint32_t h = VBE_mode_info->height;
    const uint32_t pitch = VBE_mode_info->pitch;

    uint8_t B = color & 0xFF;
    uint8_t G = (color >> 8) & 0xFF;
    uint8_t R = (color >> 16) & 0xFF;

    for (uint32_t y = 0; y < h; y++) {
        uint8_t *row = g_back + y * pitch;
        fb_fill_row(row, w, B, G, R);
    }

    present_fullframe();
    x = y = 0;
}

void vdPrintHex(uint64_t value, PixelTarget target) {
    vdPrintBase(value, 16, target);
}

void vdPrintBase(uint64_t value, uint32_t base, PixelTarget target) {
    uintToBase(value, buffer, base);
    vdPrint(buffer, target);
}

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
    char *p = buffer;
    uint32_t digits = 0;

    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    *p = 0;

    char *p1 = buffer, *p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }

    return digits;
}
