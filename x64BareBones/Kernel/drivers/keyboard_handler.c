#include <keyboard_handler.h>
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
static KeyBufferStruct keyBuffer[BUFFER_MAXLENGTH];
static bool hasnext = true;
static int bufferLength = 0;
static int nextKey = 0;
static bool capsLock = false;


bool bufferFull(){
    return bufferLength >= BUFFER_MAXLENGTH;
}

void addKeyToBuffer(uint8_t scancode){
    if (scancode == 0x3A){// es  el capsLock
        capsLock = !capsLock;
        return;
    }
    
    keyBuffer[bufferLength].scancode = scancode;

    bool pressed = (scancode & 0x80) == 0;

    if (capsLock){
        keyBuffer[bufferLength].key = scancode_to_ascii_mayus[scancode];
    }else keyBuffer[bufferLength].key = scancode_to_ascii[scancode];
    
    keyBuffer[bufferLength].is_pressed = pressed;
    
    bufferLength++;
}

void keyboardPressed(){
    if (bufferFull()){
        if (nextKey == bufferLength){
            nextKey++;
        }
        bufferLength = 0;
        addKeyToBuffer(inb(0x60));
    }
    addKeyToBuffer(inb(0x60));
}

bool hasNext(void) {
    // Hay siguiente si todavía no leíste todos los elementos cargados
    return nextKey < bufferLength;
}

KeyBufferStruct getNext(void) {
    KeyBufferStruct empty = (KeyBufferStruct){0, 0, false};
    if (!hasNext()) return empty;

    KeyBufferStruct aux = keyBuffer[nextKey++];
    // Si acabás de consumir el último, reseteá para reutilizar el buffer
    if (nextKey >= bufferLength) {
        nextKey = 0;
        bufferLength = 0;
    }
    return aux;
}

void clearBuffer(void) {
    nextKey = 0;
    bufferLength = 0;
}

