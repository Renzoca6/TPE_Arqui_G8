#include "keyboard_handler.h"
#include <stdio.h>
#include <stdbool.h>
#include "io.h"
#include <string.h>
#include "registers.h"  // For REG_COUNT

extern void enable_interrupts(void);
extern void disable_interrupts(void);

// === Guardado de registros (snapshot con Shift+Tab) ===
static char regsSaved = 0;
static uint64_t savedRegisters[REG_COUNT]; 
static uint64_t * lastRegsState = NULL;

void updateRegs(uint64_t * registers){
    for(int i = 0; i < REG_COUNT; i++){
        savedRegisters[i] = registers[i];
    }
    lastRegsState = savedRegisters; 
}

char areRegsSaved(){
    return regsSaved;
}

uint64_t* getSavedRegs(){
    return lastRegsState;
}

void clearRegsSaved(){
    regsSaved = 0;
    // No resetear lastRegsState - mantener el snapshot disponible
}
// === Fin guardado de registros ===


static char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // 0x00-0x0E
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,   // 0x0F-0x1D
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\','z','x', // 0x1E-0x2D
    'c','v','b','n','m',',','.','/', 0, '*', 0, ' ',                // 0x2E-0x39
    // el resto lo podés completar si querés (F1–F12, etc.)
};

static char scancode_to_ascii_mayus[128] = {
    0,  27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P', 0, 0,'\n', 0,
    'A','S','D','F','G','H','J','K','L', 0, 0, 0, 0, 0,'Z','X',
    'C','V','B','N','M', 0, 0, 0, 0, 0, 0, ' ',
};

#define BUFFER_MAXLENGTH 32
static KeyBufferStruct buf[BUFFER_MAXLENGTH];
static int lastkey = 0;   
static int nextkey = 0;   
static int count = 0;  //dim

static bool capsLock = false;
static bool shiftPressed = false;  // Track shift state

static inline bool bufferFull(void) {
    return count == BUFFER_MAXLENGTH;
}
static inline bool bufferEmpty(void) { 
    return count == 0;
}

static void pushEvent(KeyBufferStruct ev) {
    if (bufferFull()) {
        nextkey = (nextkey + 1) % BUFFER_MAXLENGTH;
        count--;
    }
    buf[lastkey] = ev;
    lastkey = (lastkey + 1) % BUFFER_MAXLENGTH;
    count++;
}

void addKeyToBuffer(uint8_t scancode, uint64_t * registers) {
    // Manejar shift (press y release)
    if (scancode == 0x2A || scancode == 0x36) {  // Left/Right Shift press
        shiftPressed = true;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {  // Left/Right Shift release
        shiftPressed = false;
        return;
    }
    
    // CapsLock toggle
    if (scancode == 0x3A) { 
        capsLock = !capsLock;
        return; 
    }

    // Shift+Tab: Guardar snapshot de registros
    if (scancode == 0x0F) {  // Tab
        if (shiftPressed && registers != NULL) {
            regsSaved = 1;
            updateRegs(registers);
            return;
        }
        // Si no hay shift, procesar tab normalmente (continúa abajo)
    }

    // Ignorar release events
    if (scancode & 0x80) {
        return;
    }

    KeyBufferStruct ev = {0};
    ev.scancode   = scancode;
    ev.is_pressed = true;

    // depende si son mayus o normales
    char ch = capsLock ? scancode_to_ascii_mayus[scancode]:scancode_to_ascii[scancode];

    // si no esta en la tabla retorno, TALVEZ PODEMOS HACER UNA EXCEPCION
    if (ch == 0) return;

    ev.key = ch;
    pushEvent(ev);
}

void keyboardPressed(uint64_t * registers) {
    uint8_t sc = inb(0x60);
    addKeyToBuffer(sc, registers);
}

bool hasNextKey(void) {
    return !bufferEmpty();
}

KeyBufferStruct getNextKey(void) {
    KeyBufferStruct empty = (KeyBufferStruct){0,0,false};
    if (bufferEmpty()) return empty;

    KeyBufferStruct ev = buf[nextkey];
    nextkey = (nextkey + 1) % BUFFER_MAXLENGTH;
    count--;
    return ev;
}
void clearKeyBoardBuffer(void) {
    lastkey = nextkey = count = 0;
}

void waitForEnter(void) {
    clearKeyBoardBuffer();      //limpio cualquier tecla previa
    enable_interrupts(); 
    while (1) {
        if (!hasNextKey()) continue;

        KeyBufferStruct ev = getNextKey();

        if (ev.is_pressed && ev.key == '\n') {
            break; // Se presionó Enter → salir
        }
    }
    disable_interrupts();    
}

