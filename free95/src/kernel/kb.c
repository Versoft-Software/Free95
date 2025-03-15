#include "descriptorTables.h"
#include "isr.h"
#include "string.h"
#include "vga.h"
#include "kb.h"
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "tutorial.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

static bool shiftPressed = false;
static bool capsLockEnabled = false;
static bool ctrlPressed = false;
static bool mute = false;


static char inputBuffer[MAX_INPUT_LENGTH];
static int inputIndex = 0;

static volatile bool inputReady = false;
static char lastPressed;

int ctrl_pressed = 0;
int alt_pressed = 0;

int eeCounter = 1;

int nX = 0;
int nY = 0;
int nClicked = 0;

bool bHaltSys = false;

// Regular Chars
static char kb[] =
{
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',          
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,            
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0                       
};

// Chars for when using Shift/Caps Lock
static char kbShift[] = 
{
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int getMX()
{
    return nX;
}

int getMY()
{
    return nY;
}

int isClicked()
{
    return nClicked;
}

void setClicked(int n)
{
    nClicked = n;
}

void HaltKbdDrv()
{
    bHaltSys = true;
}

void keyboardHandler()
{
    if (!bHaltSys)
    {
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);

        char ascii;

        ascii = kb[scancode];
    }
}

char getLast()
{
    return lastPressed;
}

char *getInp()
{
    if (inputReady)
    {
        inputReady = false;
        return inputBuffer;
    }
}

char *getInpA()
{
    return inputBuffer;
}

bool getReady()
{
    return inputReady;
}

void init_keyboard() 
{
    register_interrupt_handler(IRQ1, keyboardHandler); // Register KB
}

void scanf(char* buffer, int maxLength) {
    inputReady = false;
    inputIndex = 0;

    // Wait until the user presses Enter
    while (!inputReady);

    // Copy the input to the provided buffer
    for (int i = 0; i < maxLength - 1 && inputBuffer[i] != '\0'; i++) {
        buffer[i] = inputBuffer[i];
    }

    // Null-terminate the user's buffer
    buffer[maxLength - 1] = '\0';
}
