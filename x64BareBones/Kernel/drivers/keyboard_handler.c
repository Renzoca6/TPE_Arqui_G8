#include "keyboard_handler.h"
#include <stdbool.h>
#include <stdint.h>
#include "io.h"
#include <string.h>

#define REG_COUNT 20   // debe coincidir con el orden de pushState

// ---------------------------------------------------------------------
// Teclas especiales (flechas) y estado extendido
// ---------------------------------------------------------------------
static bool e0_prefix = false;

#define KEY_UP     ((char)0xF1)
#define KEY_DOWN   ((char)0xF2)
#define KEY_LEFT   ((char)0xF3)
#define KEY_RIGHT  ((char)0xF4)

extern void enable_interrupts(void);
extern void disable_interrupts(void);

// ---------------------------------------------------------------------
// Guardado de registros (snapshot con Shift+Tab)
// ---------------------------------------------------------------------
static bool     regsSaved = 0;
static uint64_t savedRegisters[REG_COUNT];
static uint64_t *lastRegsState = NULL;

void updateRegs(uint64_t *registers) {
    for (int i = 0; i < REG_COUNT; i++) {
        savedRegisters[i] = registers[i];
    }
    lastRegsState = savedRegisters;
    regsSaved = 1;
}

bool areRegsSaved(void) {
    return regsSaved;
}

uint64_t *getSavedRegs(void) {
    return lastRegsState;
}

// ---------------------------------------------------------------------
// Tablas de scancodes
// ---------------------------------------------------------------------
static char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',    // 0x00-0x0E
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,     // 0x0F-0x1D
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\','z','x', // 0x1E-0x2D
    'c','v','b','n','m',',','.','/', 0, '*', 0, ' ',                  // 0x2E-0x39
    // resto F1..F12 si hace falta
};

static char scancode_to_ascii_mayus[128] = {
    0,  27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P', 0, 0,'\n', 0,
    'A','S','D','F','G','H','J','K','L', 0, 0, 0, 0, 0,'Z','X',
    'C','V','B','N','M', 0, 0, 0, 0, 0, 0, ' ',
};

// ---------------------------------------------------------------------
// Buffer circular de teclado
// ---------------------------------------------------------------------
#define BUFFER_MAXLENGTH 32

static KeyBufferStruct buf[BUFFER_MAXLENGTH];
static int lastkey = 0;
static int nextkey = 0;
static int count   = 0;

static bool capsLock     = false;
static bool shiftPressed = false;

static inline bool bufferFull(void)  { return count == BUFFER_MAXLENGTH; }
static inline bool bufferEmpty(void) { return count == 0; }

static void pushEvent(KeyBufferStruct ev) {
    if (bufferFull()) {
        nextkey = (nextkey + 1) % BUFFER_MAXLENGTH;
        count--;
    }

    buf[lastkey] = ev;
    lastkey = (lastkey + 1) % BUFFER_MAXLENGTH;
    count++;
}

// ---------------------------------------------------------------------
// Carga de tecla normal (no E0)
// ---------------------------------------------------------------------
void addKeyToBuffer(uint8_t scancode, uint64_t *registers) {
    // Shift press
    if (scancode == 0x2A || scancode == 0x36) {
        shiftPressed = true;
        return;
    }
    // Shift release
    if (scancode == 0xAA || scancode == 0xB6) {
        shiftPressed = false;
        return;
    }
    // CapsLock toggle
    if (scancode == 0x3A) {
        capsLock = !capsLock;
        return;
    }

    // Shift + Tab → guardar registros
    if (scancode == 0x0F) {     // Tab
        if (shiftPressed && registers != NULL) {
            regsSaved = 1;
            updateRegs(registers);
            return;
        }
        // si no hay shift, sigue y procesa Tab normal
    }

    // Ignorar release (break codes)
    if (scancode & 0x80) {
        return;
    }

    KeyBufferStruct ev = {0};
    ev.scancode   = scancode;
    ev.is_pressed = true;

    char ch = capsLock
                ? scancode_to_ascii_mayus[scancode]
                : scancode_to_ascii[scancode];

    if (ch == 0)
        return;

    ev.key = ch;
    pushEvent(ev);
}

// ---------------------------------------------------------------------
// Handler principal de IRQ1
// ---------------------------------------------------------------------
void keyboardPressed(uint64_t *registers) {
    uint8_t sc = inb(0x60);

    // 1) Prefijo extendido
    if (sc == 0xE0) {
        e0_prefix = true;
        return;
    }

    // 2) Break code
    bool    released = (sc & 0x80) != 0;
    uint8_t code     = sc & 0x7F;

    if (e0_prefix) {
        // Flechas set 1: E0 48 (UP), E0 4B (DOWN), E0 50 (LEFT), E0 4D (RIGHT)
        if (!released) {
            char out = 0;

            // Mapeo a tokens propios
            switch (code) {
                case 0x48: out = KEY_UP;    break;
                case 0x4B: out = KEY_DOWN;  break;   
                case 0x50: out = KEY_LEFT;  break;   
                case 0x4D: out = KEY_RIGHT; break;
            }

            if (out) {
                KeyBufferStruct ev = {0};
                ev.scancode   = sc;
                ev.is_pressed = true;
                ev.key        = out;
                pushEvent(ev);
            }
        }

        e0_prefix = false;
        return;
    }

    // Si no era E0 → procesar como tecla normal
    addKeyToBuffer(sc, registers);
}

// ---------------------------------------------------------------------
// API de lectura desde userland/kernel
// ---------------------------------------------------------------------
bool hasNextKey(void) {
    return !bufferEmpty();
}

KeyBufferStruct getNextKey(void) {
    KeyBufferStruct empty = (KeyBufferStruct){0, 0, false};

    if (bufferEmpty())
        return empty;

    KeyBufferStruct ev = buf[nextkey];
    nextkey = (nextkey + 1) % BUFFER_MAXLENGTH;
    count--;
    return ev;
}

void clearKeyboardBuffer(void) {
    lastkey = 0;
    nextkey = 0;
    count   = 0;
}

// ---------------------------------------------------------------------
// Espera bloqueante hasta ENTER
// ---------------------------------------------------------------------
void waitForEnter(void) {
    clearKeyboardBuffer();
    enable_interrupts();

    while (1) {
        if (!hasNextKey())
            continue;

        KeyBufferStruct ev = getNextKey();
        if (ev.is_pressed && ev.key == '\n') {
            break;
        }
    }

    disable_interrupts();
}