#ifndef KERNEL_H
#define KERNEL_H

#define PORT 0x3f8          // COM1 serial port
#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define FREE95_MAX_PATH 108

void kernel_main();
void terminal_initialize();
void print(const char* str);
void DbgPrint(const char* str);

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif
