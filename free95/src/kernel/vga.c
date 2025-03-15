#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "vga.h"
#include "string.h"

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

// Update the hardware cursor position
void update_cursor(size_t row, size_t column) {
    uint16_t position = row * VGA_WIDTH + column;

    // Write to VGA cursor control registers
    outb(0x3D4, 0x0F); // Select the low byte
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E); // Select the high byte
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

void setPosition(size_t col, size_t row)
{
    terminal_row = row;
    terminal_column = col;
}

void terminal_initialize(void) 
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    update_cursor(terminal_row, terminal_column);
}

void terminal_initialize_a(uint8_t color) 
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = color;
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    update_cursor(terminal_row, terminal_column);
}

void terminal_setcolor(uint8_t color) 
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t src_index = y * VGA_WIDTH + x;
            const size_t dest_index = (y - 1) * VGA_WIDTH + x;
            terminal_buffer[dest_index] = terminal_buffer[src_index];
        }
    }
    // Clear the last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, VGA_HEIGHT - 1);
    }
    terminal_row--;
}

void terminal_putchar(char c) 
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    } else if (c == '\t') {
        size_t tab_width = 4;
        size_t next_tab_stop = (terminal_column + tab_width) & ~(tab_width - 1);
        
        while (terminal_column < next_tab_stop) {
            if (terminal_column < VGA_WIDTH) {
                terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
                terminal_column++;
            } else {
                terminal_column = 0;
                terminal_row++;
                if (terminal_row == VGA_HEIGHT) {
                    terminal_scroll();
                }
                break;
            }
        }
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    } else if (c == '\r') {
        terminal_column = 0;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_scroll();
            }
        }
    }
    update_cursor(terminal_row, terminal_column);
}

void printWrite(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void print(const char* data) 
{
    printWrite(data, strlen(data));
}

void intToStr(int N, char *str) {
    int i = 0;

    int sign = N;

    if (N < 0)
        N = -N;

    while (N > 0) 
    {
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 

    if (sign < 0) 
    {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--)
    {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void putstr(const char* str, size_t x, size_t y)
{
    size_t column = x;
    size_t row = y;

    for (size_t i = 0; str[i] != '\0'; i++)
    {
        char c = str[i];

        if (c == '\n') {
            column = x; // Reset column to the initial x
            row++;
            if (row == VGA_HEIGHT) {
                terminal_scroll();
                row = VGA_HEIGHT - 1;
            }
        } else {
            terminal_putentryat(c, terminal_color, column, row);
            column++;
            if (column == VGA_WIDTH) {
                column = 0;
                row++;
                if (row == VGA_HEIGHT) {
                    terminal_scroll();
                    row = VGA_HEIGHT - 1;
                }
            }
        }
    }
}


void printInt(const int data) 
{
    if (data != 0)
    {
        char *strdata;
        intToStr(data, strdata);
        print(strdata);
    }
    else
    {
        print("0");
    }
}

void intToHexStr(unsigned int N, char *str) {
    const char hexDigits[] = "0123456789ABCDEF";
    int i = 0;

    if (N == 0) {
        str[i++] = '0';
    } else {
        while (N > 0) {
            str[i++] = hexDigits[N % 16];
            N /= 16;
        }
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void printhex(const unsigned int data) {
    char strdata[12]; // Enough to hold "0x" + 8 hex digits + '\0'
    if (data != 0) {
        strdata[0] = '0';
        strdata[1] = 'x';
        intToHexStr(data, &strdata[2]);
        print(strdata);
    } else {
        print("0x00");
    }
}

