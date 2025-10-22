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
void keyboardPressed();
bool hasNextKey();
KeyBufferStruct getNextKey();


#endif