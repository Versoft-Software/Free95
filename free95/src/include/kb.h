#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stddef.h>
// Utility keys
#define CTRL 0x1D
#define CTRLR 0x9D
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define LSHIFTR 0xAA
#define RSHIFTR 0xB6
#define CAPSLCK 0x3A
#define ESC 0x01

// Function keys
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D
#define F4 0x3E
#define F5 0x3F
#define F6 0x40
#define F7 0x41
#define F8 0x42
#define F9 0x43
#define F10 0x44
#define F11 0x57
#define F12 0x58

// Arrow keys
#define UP 0x48
#define LEFT 0x4B
#define RIGHT 0x4D
#define DOWN 0x50

#define MAX_INPUT_LENGTH 256

void keyboardHandler();
char getLast();
void init_keyboard();
void scanf(char* buffer, int maxLength);
#endif
