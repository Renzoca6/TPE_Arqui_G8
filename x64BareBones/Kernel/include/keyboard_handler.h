#ifndef KEY_BOARD_HANDLER_H
#define KEY_BOARD_HANDLER_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t scancode;
    char key;
    bool is_pressed;
} KeyBufferStruct;

void clearKeyBoardBuffer();
void keyboardPressed(uint64_t * registers);
bool hasNextKey();
KeyBufferStruct getNextKey();
void waitForEnter();

// Funciones para guardado de registros (snapshot con Shift+Tab)
void updateRegs(uint64_t * registers);
char areRegsSaved();
uint64_t* getSavedRegs();
void clearRegsSaved();


#endif