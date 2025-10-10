#include "keyboard_handler.h"
#include <stdio.h>
#include <stdbool.h>
#include "io.h"
#include <string.h>

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

void addKeyToBuffer(uint8_t scancode) {
    if (scancode == 0x3A) { 
        capsLock = !capsLock;
        return; 
    }

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

void keyboardPressed(void) {
    uint8_t sc = inb(0x60);
    addKeyToBuffer(sc);
}

bool hasNext(void) {
    return !bufferEmpty();
}

KeyBufferStruct getNext(void) {
    KeyBufferStruct empty = (KeyBufferStruct){0,0,false};
    if (bufferEmpty()) return empty;

    KeyBufferStruct ev = buf[nextkey];
    nextkey = (nextkey + 1) % BUFFER_MAXLENGTH;
    count--;
    return ev;
}
void clearBuffer(void) {
    lastkey = nextkey = count = 0;
}

