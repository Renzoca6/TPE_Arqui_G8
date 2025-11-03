#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <stdbool.h>
#include <stdint.h>

/* ===================== */
/*   Estructura de tecla  */
/* ===================== */

typedef struct {
    uint8_t scancode;
    char    key;
    bool    is_pressed;
} KeyBufferStruct;

/* ===================== */
/*   Buffer de teclado    */
/* ===================== */

/** Limpia el buffer de teclado. */
void clearKeyboardBuffer(void);

/** Handler principal llamado desde IRQ1. */
void keyboardPressed(uint64_t *registers);

/** Devuelve true si hay una tecla disponible. */
bool hasNextKey(void);

/** Devuelve la siguiente tecla del buffer. */
KeyBufferStruct getNextKey(void);

/** Bloquea hasta que el usuario presione Enter. */
void waitForEnter(void);

/* ===================== */
/*   Snapshot de registros */
/* ===================== */

/** Guarda los registros actuales (p.ej. con combinación de teclas). */
void updateRegs(uint64_t *registers);

/** Indica si hay un snapshot guardado. */
bool areRegsSaved(void);

/** Devuelve el puntero al snapshot guardado. */
uint64_t *getSavedRegs(void);

#endif /* KEYBOARD_HANDLER_H */
