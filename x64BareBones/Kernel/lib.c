#include <stdint.h>

extern void *fast_memset(void *dest, int value, unsigned long size);
extern void *fast_memcpy(void *dst, const void *src, unsigned long size);

void *memset(void *destination, int32_t c, uint64_t length) {
    return fast_memset(destination, c, length);
}

void *memcpy(void *destination, const void *source, uint64_t length) {
    // delegamos al asm
    return fast_memcpy(destination, source, length);
}