/*
 * Free95
 *
 * Module name:
 *      kernel.c
 *
 * Description:
 *      20x Kernel
 *
 * Author: Kap Petrov
 *
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Read https://wiki.osdev.org/Bochs_VBE_Extensions */

// This is the port to write the index value to
#define VBE_DISPI_IOPORT_INDEX 0x01CE
// This is the port to write the data value to
#define VBE_DISPI_IOPORT_DATA 0x01CF

// Indexes
#define VBE_DISPI_INDEX_ID 0
#define VBE_DISPI_INDEX_XRES 1
#define VBE_DISPI_INDEX_YRES 2
#define VBE_DISPI_INDEX_BPP 3
#define VBE_DISPI_INDEX_ENABLE 4
#define VBE_DISPI_INDEX_BANK 5
#define VBE_DISPI_INDEX_VIRT_WIDTH 6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET 8
#define VBE_DISPI_INDEX_Y_OFFSET 9

// Values
#define VBE_DISPI_ID5 0xB0C4

#define VBE_DISPI_BANK_ADDRESS          0xA0000
  #define VBE_DISPI_BANK_SIZE_KB          64

  #define VBE_DISPI_MAX_XRES              1024
  #define VBE_DISPI_MAX_YRES              768

  #define VBE_DISPI_IOPORT_INDEX          0x01CE
  #define VBE_DISPI_IOPORT_DATA           0x01CF

  #define VBE_DISPI_INDEX_ID              0x0
  #define VBE_DISPI_INDEX_XRES            0x1
  #define VBE_DISPI_INDEX_YRES            0x2
  #define VBE_DISPI_INDEX_BPP             0x3
  #define VBE_DISPI_INDEX_ENABLE          0x4
  #define VBE_DISPI_INDEX_BANK            0x5
  #define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
  #define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
  #define VBE_DISPI_INDEX_X_OFFSET        0x8
  #define VBE_DISPI_INDEX_Y_OFFSET        0x9

  #define VBE_DISPI_ID0                   0xB0C0
  #define VBE_DISPI_ID1                   0xB0C1
  #define VBE_DISPI_ID2                   0xB0C2
  #define VBE_DISPI_ID3                   0xB0C3
  #define VBE_DISPI_ID4                   0xB0C4

  #define VBE_DISPI_DISABLED              0x00
  #define VBE_DISPI_ENABLED               0x01
  #define VBE_DISPI_VBE_ENABLED           0x40
  #define VBE_DISPI_NOCLEARMEM            0x80

  #define VBE_DISPI_LFB_PHYSICAL_ADDRESS  0xE0000000

#define VBE_DISPI_LFB_ENABLED 0x40
#define BITMAP_SIZE 20
#define S_WIDTH 640

#include "vga.h"
#include "string.h"
#include "descriptorTables.h"
#include "isr.h"
#include "kb.h"

#include "pmm.h"
#include "kheap.h"
#include "win32.h"
#include "hello.h"
#include "reshell.h"
#include "freever.h"

typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef uint64_t uint64;

volatile uint32_t timer_ticks = 0;
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_FREQUENCY 1193182
uint32_t current_frequency = 0;

void set_pit_frequency(uint32_t frequency) 
{
    current_frequency = frequency; // Store the frequency globally
    uint16_t divisor = PIT_FREQUENCY / frequency;
    
    outb(PIT_COMMAND_PORT, 0x36);  // Channel 0, LSB/MSB, Mode 3 (square wave)
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF)); // Low byte
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF)); // High byte
}

// Timer interrupt handler
void timer_handler() 
{
    timer_ticks++;               // Increment the tick counter
    outb(0x20, 0x20);            // Send End of Interrupt (EOI) to PIC
}

// Function to initialize the timer
void init_timer(uint32_t frequency) 
{
    set_pit_frequency(frequency);

    // Register the timer interrupt handler for IRQ0
    register_interrupt_handler(IRQ0, timer_handler);
}

void KiWait(uint32_t milliseconds)
{
    uint32_t start_ticks = timer_ticks;
    uint32_t ticks_to_wait = milliseconds * (PIT_FREQUENCY / 1000) / current_frequency;

    while ((timer_ticks - start_ticks) < ticks_to_wait) {
        // Busy wait
        asm volatile("sti\nhlt"); // Optionally halt CPU to reduce power consumption
    }
}

static inline VOID outpw(UINT16 port, UINT16 value) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline UINT16 inpw(UINT16 port) {
    UINT16 value;
    __asm__ __volatile__ ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void BgaWriteRegister(UINT16 IndexValue, UINT16 DataValue)
{
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outpw(VBE_DISPI_IOPORT_DATA, DataValue);
}

UINT16 BgaReadRegister(UINT16 IndexValue)
{
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inpw(VBE_DISPI_IOPORT_DATA);
}

INT BgaIsAvailable(VOID)
{
    return (BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID5);
}

VOID BgaSetVideoMode(UINT32 Width, UINT32 Height, UINT32 BitDepth, INT UseLinearFrameBuffer, INT ClearVideoMemory)
{
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth);
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}

#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_MISC_WRITE 0x3C2
#define VGA_SEQ_INDEX  0x3C4
#define VGA_SEQ_DATA   0x3C5
#define VGA_ATTR_INDEX 0x3C0
#define VGA_ATTR_DATA  0x3C1
#define VGA_GRAPH_INDEX 0x3CE
#define VGA_GRAPH_DATA  0x3CF

void WritePort(uint16_t port, uint8_t value) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t ReadPort(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void BgaDisable(void) {
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

void VgaSetTextMode(void) {
    // Disable BGA mode
    BgaDisable();

    // Write to the miscellaneous output register to set text mode clock
    WritePort(VGA_MISC_WRITE, 0x67);

    // Program the sequencer registers
    WritePort(VGA_SEQ_INDEX, 0x00); WritePort(VGA_SEQ_DATA, 0x03); // Reset
    WritePort(VGA_SEQ_INDEX, 0x01); WritePort(VGA_SEQ_DATA, 0x01); // Clocking mode
    WritePort(VGA_SEQ_INDEX, 0x02); WritePort(VGA_SEQ_DATA, 0x0F); // Map mask
    WritePort(VGA_SEQ_INDEX, 0x03); WritePort(VGA_SEQ_DATA, 0x00); // Character map
    WritePort(VGA_SEQ_INDEX, 0x04); WritePort(VGA_SEQ_DATA, 0x0E); // Memory mode

    // Unlock CRTC registers
    WritePort(VGA_CRTC_INDEX, 0x11);
    WritePort(VGA_CRTC_DATA, ReadPort(VGA_CRTC_DATA) & 0x7F);

    // Program the CRTC registers
    uint8_t crtc_values[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for (uint8_t i = 0; i < sizeof(crtc_values); i++) {
        WritePort(VGA_CRTC_INDEX, i);
        WritePort(VGA_CRTC_DATA, crtc_values[i]);
    }

    // Program the graphics registers
    uint8_t graphics_values[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0xFF};
    for (uint8_t i = 0; i < sizeof(graphics_values); i++) {
        WritePort(VGA_GRAPH_INDEX, i);
        WritePort(VGA_GRAPH_DATA, graphics_values[i]);
    }

    // Program the attribute registers
    for (uint8_t i = 0; i < 0x10; i++) {
        WritePort(VGA_ATTR_INDEX, i);
        WritePort(VGA_ATTR_DATA, i);
    }
    for (uint8_t i = 0x10; i < 0x20; i++) {
        WritePort(VGA_ATTR_INDEX, i);
        WritePort(VGA_ATTR_DATA, 0x00);
    }

    // Enable attribute registers
    WritePort(VGA_ATTR_INDEX, 0x20);

    // Set VGA memory to 0xB8000
    uint16_t *vga_memory = (uint16_t *)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga_memory[i] = 0x0720; // Space character with white text on black background
    }
}


VOID PutPixel(UINT32 x, UINT32 y, UINT32 color)
{
    UINT32 *framebuffer = (UINT32*)0xFD000000;
    framebuffer[y * S_WIDTH + x] = color;
}

VOID FillRectangle(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 color)
{
    UINT32 i, j;

    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
        {
            PutPixel(i, j, color);
        }
    }
}

/* Thank you, skymoose, on your opinion i upgraded font to be Windows 95ish. */
const UINT8 font_8x16[] = {
    /* A */
    0x08, 0x08, 0x1C, 0x14, 0x14, 0x14, 0x22, 0x22,
    0x3E, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00,
    /* B */
    0x3E, 0x21, 0x21, 0x21, 0x3E, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* C */
    0x1C, 0x22, 0x41, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,
    /* D */
    0x3E, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* E */
    0x3F, 0x20, 0x20, 0x20, 0x20, 0x3E, 0x20, 0x20,
    0x20, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* F */
    0x3F, 0x20, 0x20, 0x20, 0x20, 0x3E, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* G */
    0x1C, 0x22, 0x41, 0x40, 0x40, 0x47, 0x41, 0x41,
    0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* H */
    0x21, 0x21, 0x21, 0x21, 0x21, 0x3F, 0x21, 0x21,
    0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* I */
    0x3C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* J */
    0x1E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* K */
    0x21, 0x22, 0x24, 0x28, 0x30, 0x28, 0x24, 0x22,
    0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* L */
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* M */
    0x21, 0x33, 0x33, 0x2D, 0x2D, 0x2D, 0x21, 0x21,
    0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* N */
    0x21, 0x31, 0x31, 0x29, 0x29, 0x25, 0x25, 0x23,
    0x23, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* O */
    0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* P */
    0x3E, 0x21, 0x21, 0x21, 0x21, 0x21, 0x3E, 0x20,
    0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Q */
    0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    0x49, 0x26, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* R */
    0x3E, 0x21, 0x21, 0x21, 0x21, 0x21, 0x3E, 0x28,
    0x24, 0x22, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* S */
    0x1E, 0x21, 0x21, 0x20, 0x1C, 0x02, 0x01, 0x01,
    0x01, 0x21, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* T */
    0x3F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* U */
    0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* V */
    0x21, 0x21, 0x21, 0x21, 0x21, 0x12, 0x12, 0x12,
    0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* W */
    0x21, 0x21, 0x21, 0x21, 0x21, 0x2D, 0x2D, 0x36,
    0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* X */
    0x21, 0x21, 0x12, 0x12, 0x0C, 0x0C, 0x0C, 0x12,
    0x12, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Y */
    0x21, 0x21, 0x21, 0x12, 0x12, 0x0C, 0x0C, 0x0C,
    0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Z */
    0x3F, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x20, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* Space (8x16) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* ! */
    0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* ( */
    0x00, 0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00,
    /* ) */
    0x00, 0x20, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00,
    /* 1 */
    0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00,
    /* . */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0 */
    0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    0x41, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,
    /* 2 */
    0x1C, 0x22, 0x41, 0x01, 0x02, 0x04, 0x08, 0x10,
    0x20, 0x40, 0x41, 0x7F, 0x00, 0x00, 0x00, 0x00,
    /* 3 */
    0x1C, 0x22, 0x41, 0x01, 0x01, 0x0E, 0x01, 0x01,
    0x01, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,
    /* 4 */
    0x02, 0x06, 0x0A, 0x0A, 0x12, 0x12, 0x22, 0x42,
    0x7F, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00,
    /* 5 */
    0x7F, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x01, 0x01,
    0x01, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,
    /* 6 */
    0x1C, 0x22, 0x41, 0x40, 0x40, 0x5E, 0x61, 0x41,
    0x41, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,
    /* 7 */
    0x7F, 0x01, 0x01, 0x02, 0x02, 0x04, 0x08, 0x10,
    0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00,
    /* 8 */
    0x1C, 0x22, 0x41, 0x41, 0x41, 0x22, 0x1C, 0x22,
    0x41, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00,
    /* 9 */
    0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x23, 0x1D,
    0x01, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,
    /* , */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18,
    0x10, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* : */
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* a */
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x21, 0x21, 0x3F,
    0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* b */
    0x20, 0x20, 0x20, 0x20, 0x2E, 0x31, 0x21, 0x21,
    0x21, 0x21, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* c */
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x21, 0x20, 0x20,
    0x20, 0x21, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* d */
    0x01, 0x01, 0x01, 0x01, 0x1D, 0x23, 0x21, 0x21,
    0x21, 0x21, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* e */
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x21, 0x21, 0x3F,
    0x20, 0x21, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* f */
    0x0E, 0x11, 0x10, 0x10, 0x3C, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* g */
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x21, 0x21, 0x21,
    0x1F, 0x01, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* h */
    0x20, 0x20, 0x20, 0x20, 0x2E, 0x31, 0x21, 0x21,
    0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* i */
    0x00, 0x10, 0x00, 0x00, 0x30, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* j */
    0x00, 0x08, 0x00, 0x00, 0x18, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x30, 0x00, 0x00, 0x00, 0x00,
    /* k */
    0x20, 0x20, 0x20, 0x20, 0x22, 0x24, 0x28, 0x30,
    0x28, 0x24, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* l */
    0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* m */
    0x00, 0x00, 0x00, 0x00, 0x36, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* n */
    0x00, 0x00, 0x00, 0x00, 0x2E, 0x31, 0x21, 0x21,
    0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* o */
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* p */
    0x00, 0x00, 0x00, 0x00, 0x2E, 0x31, 0x21, 0x21,
    0x31, 0x2E, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00,
    /* q */
    0x00, 0x00, 0x00, 0x00, 0x1D, 0x23, 0x21, 0x21,
    0x23, 0x1F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
    /* r */
    0x00, 0x00, 0x00, 0x00, 0x2E, 0x31, 0x21, 0x20,
    0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* s */
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x1E, 0x01,
    0x01, 0x01, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* t */
    0x10, 0x10, 0x10, 0x10, 0x3C, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* u */
    0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x21,
    0x21, 0x23, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* v */
    0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x12,
    0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* w */
    0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x2D,
    0x2D, 0x2D, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* x */
    0x00, 0x00, 0x00, 0x00, 0x21, 0x12, 0x0C, 0x0C,
    0x0C, 0x12, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* y */
    0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x1F,
    0x01, 0x01, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* z */
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,

};

VOID PutChar(CHAR ch, UINT32 x_coord, UINT32 y_coord, UINT32 color) {
    UINT32 index = 0;

    if (ch >= 'A' && ch <= 'Z') {
        index = (ch - 'A') * 16;
    } else if (ch >= 'a' && ch <= 'z') {
        index = (ch - 'a' + 43) * 16;
    } else if (ch >= '0' && ch <= '9') {
        index = (ch - '0' + 32) * 16;
    } else {
        switch (ch) {
            case ' ': index = 26 * 16; break;
            case '!': index = 27 * 16; break;
            case '(': index = 28 * 16; break;
            case ')': index = 29 * 16; break;
            case '.': index = 31 * 16; break;
            case ',': index = 41 * 16; break;
            case ':': index = 42 * 16; break;
            default: return;
        }
    }

    for (UINT32 row = 0; row < 16; row++) {
        UINT8 data = font_8x16[index + row];
        for (UINT32 col = 0; col < 8; col++) {
            if (data & (0x80 >> col)) {
                PutPixel(x_coord + col, y_coord + row, color);
            }
        }
    }
}

VOID KiPutString(const CHAR* str, INT x, INT y)
{
    int ax = x;
	int ay = y;

	while (*str)
	{
		if (*str == '\n')
		{
			ax = x;
			ay += 16;
		}
		else
		{
            PutChar(*str, ax, ay, 0xFFFFFFFF);
			ax += 10;
        }
		// Handle screen wrapping
		if (ax >= 640)
		{
			ax = 0;
			ay += 16;
		}

		str++;
	}
}

VOID KiPutStringC(const CHAR* str, INT x, INT y, UINT color)
{
    int ax = x;
	int ay = y;

	while (*str)
	{
		if (*str == '\n')
		{
			ax = x;
			ay += 16;
		}
		else
		{
            PutChar(*str, ax, ay, color);
			ax += 10;
        }
		// Handle screen wrapping
		if (ax >= 640)
		{
			ax = 0;
			ay += 16;
		}

		str++;
	}
}

VOID KiDrawBitmap(INT startX, INT startY, INT bitmap[22][22], UINT32 color)
{
    for (INT y = 0; y < 22; y++) {
        for (INT x = 0; x < 22; x++) {
            if (bitmap[y][x] == 1) {
                PutPixel(startX + x, startY + y, color);
            }
        }
    }
}

int mouseCursorBitmap[22][22] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

extern void jump_usermode();

void NtPutString(const char *str) {
    asm volatile (
        "mov %0, %%edi\n"       // Pass syscall number in EDI
        "mov %1, %%esi\n"       // Pass string pointer in ESI
        "int $0x2E\n"           // Trigger syscall
        :
        : "r"(2), "r"(str)
        : "edi", "esi"
    );
}

void NtSleep(uint32_t ms)
{
    asm volatile (
        "mov %0, %%edi\n"       // Pass syscall number in EDI
        "mov %1, %%esi\n"       // Pass milliseconds pointer in ESI
        "int $0x2E\n"           // Trigger syscall
        :
        : "r"(3), "r"(ms)
        : "edi", "esi"
    );
}

void NtVdiSetPDEV()
{
    asm volatile (
        "mov %0, %%edi\n"       // Pass syscall number in EDI
        "int $0x2E\n"           // Trigger syscall
        :
        : "r"(4)
        : "edi"
    );
}

void HwVidSetMode()
{
    BgaSetVideoMode(640, 480, 0x20, 1, 1);
}

#define MEMORY_SIZE (1024 * 1024) // 1 MB total memory
#define BLOCK_SIZE 4096           // 4 KB block size

#define MULTIBOOT_MAGIC_HEADER      0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC  0x2BADB002

/* The Multiboot header. */
typedef struct {
    uint32 magic;
    uint32 flags;
    uint32 checksum;
    uint32 header_addr;
    uint32 load_addr;
    uint32 load_end_addr;
    uint32 bss_end_addr;
    uint32 entry_addr;
} MULTIBOOT_HEADER;

/* The symbol table for a.out. */
typedef struct {
    uint32 tabsize;
    uint32 strsize;
    uint32 addr;
    uint32 reserved;
} AOUT_SYMBOL_TABLE;

/* The section header table for ELF. */
typedef struct {
    uint32 num;
    uint32 size;
    uint32 addr;
    uint32 shndx;
} ELF_SECTION_HEADER_TABLE;

typedef struct {
    /* required, defined in boot.s */
    uint32 flags;

    /* available low-high memory from BIOS, present if flags[0] is set(MEMINFO in boot.s) */
    uint32 mem_low;
    uint32 mem_high;

    /* "root" partition, present if flags[1] is set(BOOTDEVICE in boot.s) */
    uint32 boot_device;

    /* kernel command line, present if flags[2] is set(CMDLINE in boot.s) */
    uint32 cmdline;

    /* no of modules loaded, present if flags[3] is set(MODULECOUNT in boot.s) */
    uint32 modules_count;
    uint32 modules_addr;

    /* symbol table info, present if flags[4] & flags[5] is set(SYMT in boot.s) */
    union {
        AOUT_SYMBOL_TABLE aout_sym;
        ELF_SECTION_HEADER_TABLE elf_sec;
    } u;

    /* memory mapping, present if flags[6] is set(MEMMAP in boot.s) */
    uint32 mmap_length;
    uint32 mmap_addr;

    /* drive info, present if flags[7] is set(DRIVE in boot.s) */
    uint32 drives_length;
    uint32 drives_addr;

    /* ROM configuration table, present if flags[8] is set(CONFIGT in boot.s) */
    uint32 config_table;

    /* boot loader name, present if flags[9] is set(BOOTLDNAME in boot.s) */
    uint32 boot_loader_name;

    /* Advanced Power Management(APM) table, present if flags[10] is set(APMT in boot.s) */
    uint32 apm_table;

    /* video info, present if flags[11] is set(VIDEO in boot.s) */
    uint32 vbe_control_info;
    uint32 vbe_mode_info;
    uint16 vbe_mode;
    uint16 vbe_interface_seg;
    uint16 vbe_interface_off;
    uint16 vbe_interface_len;

    /* video framebufer info, present if flags[12] is set(VIDEO_FRAMEBUF in boot.s)  */
    uint64 framebuffer_addr;
    uint32 framebuffer_pitch;
    uint32 framebuffer_width;
    uint32 framebuffer_height;
    uint8 framebuffer_bpp;
    uint8 framebuffer_type;  // indexed = 0, RGB = 1, EGA = 2

} MULTIBOOT_INFO;


typedef enum {
    MULTIBOOT_MEMORY_AVAILABLE = 1,
    MULTIBOOT_MEMORY_RESERVED,
    MULTIBOOT_MEMORY_ACPI_RECLAIMABLE,
    MULTIBOOT_MEMORY_NVS,
    MULTIBOOT_MEMORY_BADRAM
} MULTIBOOT_MEMORY_TYPE;

typedef struct {
    uint32 size;
    uint32 addr_low;
    uint32 addr_high;
    uint32 len_low;
    uint32 len_high;
    MULTIBOOT_MEMORY_TYPE type;
} MULTIBOOT_MEMORY_MAP;

// symbols from linker.ld for section addresses
extern uint8 __kernel_section_start;
extern uint8 __kernel_section_end;
extern uint8 __kernel_text_section_start;
extern uint8 __kernel_text_section_end;
extern uint8 __kernel_data_section_start;
extern uint8 __kernel_data_section_end;
extern uint8 __kernel_rodata_section_start;
extern uint8 __kernel_rodata_section_end;
extern uint8 __kernel_bss_section_start;
extern uint8 __kernel_bss_section_end;

typedef struct {
    struct {
        uint32 k_start_addr;
        uint32 k_end_addr;
        uint32 k_len;
        uint32 text_start_addr;
        uint32 text_end_addr;
        uint32 text_len;
        uint32 data_start_addr;
        uint32 data_end_addr;
        uint32 data_len;
        uint32 rodata_start_addr;
        uint32 rodata_end_addr;
        uint32 rodata_len;
        uint32 bss_start_addr;
        uint32 bss_end_addr;
        uint32 bss_len;
    } kernel;

    struct {
        uint32 total_memory;
    } system;

    struct {
        uint32 start_addr;
        uint32 end_addr;
        uint32 size;
    } available;
} KERNEL_MEMORY_MAP;

KERNEL_MEMORY_MAP g_kmap;

int get_kernel_memory_map(KERNEL_MEMORY_MAP *kmap, MULTIBOOT_INFO *mboot_info) {
    uint32 i;

    if (kmap == NULL) return -1;
    kmap->kernel.k_start_addr = (uint32)&__kernel_section_start;
    kmap->kernel.k_end_addr = (uint32)&__kernel_section_end;
    kmap->kernel.k_len = ((uint32)&__kernel_section_end - (uint32)&__kernel_section_start);

    kmap->kernel.text_start_addr = (uint32)&__kernel_text_section_start;
    kmap->kernel.text_end_addr = (uint32)&__kernel_text_section_end;
    kmap->kernel.text_len = ((uint32)&__kernel_text_section_end - (uint32)&__kernel_text_section_start);

    kmap->kernel.data_start_addr = (uint32)&__kernel_data_section_start;
    kmap->kernel.data_end_addr = (uint32)&__kernel_data_section_end;
    kmap->kernel.data_len = ((uint32)&__kernel_data_section_end - (uint32)&__kernel_data_section_start);

    kmap->kernel.rodata_start_addr = (uint32)&__kernel_rodata_section_start;
    kmap->kernel.rodata_end_addr = (uint32)&__kernel_rodata_section_end;
    kmap->kernel.rodata_len = ((uint32)&__kernel_rodata_section_end - (uint32)&__kernel_rodata_section_start);

    kmap->kernel.bss_start_addr = (uint32)&__kernel_bss_section_start;
    kmap->kernel.bss_end_addr = (uint32)&__kernel_bss_section_end;
    kmap->kernel.bss_len = ((uint32)&__kernel_bss_section_end - (uint32)&__kernel_bss_section_start);

    kmap->system.total_memory = mboot_info->mem_low + mboot_info->mem_high;

    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        // make sure kernel is loaded at 0x100000 by bootloader(see linker.ld)
        if (mmap->addr_low == kmap->kernel.text_start_addr) {
            // set available memory starting from end of our kernel, leaving 1MB size for functions exceution
            kmap->available.start_addr = kmap->kernel.k_end_addr + 1024 * 1024;
            kmap->available.end_addr = mmap->addr_low + mmap->len_low;
            // get availabel memory in bytes
            kmap->available.size = kmap->available.end_addr - kmap->available.start_addr;
            return 0;
        }
    }

    return -1;
}

FileNode *con;
int recount = 0;

FileNode *getCon()
{
    return con;
}

#define MaxThreadCount 10
int TaskCounter = 0;
TaskControlBlock Tasks[MaxThreadCount];

void PsCreateSystemThread(TaskControlBlock h)
{
    Tasks[TaskCounter] = h;
    TaskCounter++;
}

void PsKillSystemThread(int ProcessId)
{
    Tasks[ProcessId].IsActive = false;
}

static uint32_t TaskIndex = 0;

uint32_t KiGetTask()
{
	return TaskIndex;
}

void KiSetTask(uint32_t i)
{
    TaskIndex = i;
}

char *KiGetTnBP(uint32_t i)
{
    return Tasks[i].Name;
}

void launch()
{
    TaskControlBlock App = {VerMain, true, "freever.exe"};
    PsCreateSystemThread(App);
}

void KiUserInit()
{
    TaskControlBlock R = {ReshellMain, true, "reshell.exe"};
    TaskControlBlock I = {WinMain, true, "hello.exe"};

    PsCreateSystemThread(R);
    //PsCreateSystemThread(I);

    memset(con->Name, 0, sizeof(con->Name));
    strcpy(con->Name, "CON");
    memset(con->Content, 0, sizeof(con->Content));
    strcpy(con->Content, "\n\n\n\n\n\n\n");

    setPosition(5, 0);

    NtVdiSetPDEV();
    FillRectangle(0, 0, 640, 480, 0x00000000);
    KiPutString("VERSOFT FREE95 (C) 2025\nALL RIGHTS RESERVED", 200, 0);
    KiPutString("Starting Free95...", 250, 270);
    FillRectangle(300, 220, 40, 40, RGB(255, 255, 255));
    NtSleep(500);
    FillRectangle(300, 220, 40, 40, RGB(255, 0, 0));
    NtSleep(500);
    FillRectangle(300, 220, 40, 40, RGB(0, 255, 0));
    NtSleep(500);
    FillRectangle(300, 220, 40, 40, RGB(0, 0, 255));
    NtSleep(1300);
    FillRectangle(0, 0, 640, 480, 0xFF00807F);

    while(1)
    {
        FillRectangle(mouse_getx(), mouse_gety(), 20, 20, 0xFF00807F);
        mouse_poll();
        FillRectangle(mouse_getx(), mouse_gety(), 20, 20, RGB(0, 0, 0));

        if (Tasks[TaskIndex].IsActive)
        {
            Tasks[TaskIndex].Task();
        }

        NtSleep(1);

        TaskIndex = (TaskIndex + 1) % TaskCounter;
    }
}

void mousedrv()
{
    FillRectangle(mouse_getx(), mouse_gety(), 10, 10, 0xFFFFFFFF);
}

#define MOUSE_COMMAND_PORT 0x64
#define MOUSE_DATA_PORT    0x60

// extern void mouse_write(uint8_t data); // mouse.c
// extern uint8_t mouse_read(); // mouse.c

void mouse_write(uint8_t data) {
    // Wait for the command port to be ready
    while ((inb(MOUSE_COMMAND_PORT) & 0x02) != 0);
    outb(MOUSE_COMMAND_PORT, 0xD4);  // Tell the controller we're sending a mouse command
    // Wait for the data port to be ready
    while ((inb(MOUSE_COMMAND_PORT) & 0x02) != 0);
    outb(MOUSE_DATA_PORT, data);
}

uint8_t mouse_read() {
    // Wait for data to be available
    while ((inb(MOUSE_COMMAND_PORT) & 0x01) == 0);
    return inb(MOUSE_DATA_PORT);
}

void mouse_initialize() {
    // Enable the auxiliary PS/2 mouse device
    outb(MOUSE_COMMAND_PORT, 0xA8);

    // Enable the mouse device itself
    mouse_write(0xF4);  // Enable mouse packet streaming
    mouse_read();       // Acknowledge byte (should be 0xFA)
}

typedef struct {
    int x, y;             // Mouse coordinates
    bool left, right;     // Mouse button states
    bool middle;
} MouseState;

MouseState mouse_state = {0};

void mouse_poll() {
    // Read the first byte of the packet
    uint8_t status = mouse_read();

    // Read the X and Y movement data
    int8_t mouse_x = (int8_t)mouse_read();
    int8_t mouse_y = (int8_t)mouse_read();

    // Update mouse coordinates
    mouse_state.x += mouse_x;
    mouse_state.y -= mouse_y;  // Y direction is typically inverted

    // Update button states
    mouse_state.left = status & 0x01;
    mouse_state.right = status & 0x02;
    mouse_state.middle = status & 0x04;

    // Optional: Clamp coordinates to screen boundaries
    if (mouse_state.x < 0) mouse_state.x = 0;
    if (mouse_state.y < 0) mouse_state.y = 0;
    // Add max boundary checks for your screen dimensions if needed
}

int mouse_getx() {
    return mouse_state.x;
}

int mouse_gety() {
    return mouse_state.y;
}

bool mouse_button_left() {
    return mouse_state.left;
}

bool mouse_button_right() {
    return mouse_state.right;
}

bool mouse_button_middle() {
    return mouse_state.middle;
}


void kernelMain(unsigned long magic, unsigned long addr)
{
    MULTIBOOT_INFO *mboot_info;

    terminal_initialize();
    init_descriptor_tables();
    init_timer(1000);
    init_keyboard();
    mouse_initialize();

	terminal_initialize_a(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_DARK_GREY));
	print("Free95 [Version 0.2.0]\n\n");

    mboot_info = (MULTIBOOT_INFO *)addr;
    memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));
    if (get_kernel_memory_map(&g_kmap, mboot_info) < 0)
    {
        print("Error: Free95 was unable to get the Kernel Memory Map\nFree95 could not start.\n");
        return;
    }

    pmm_init(g_kmap.available.start_addr, g_kmap.available.size);

    pmm_init_region(g_kmap.available.start_addr, PMM_BLOCK_SIZE * 10);

    // Trololololol
    KiWait(500);

    HwVidSetMode();

    jump_usermode();
}
