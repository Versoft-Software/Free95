#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include "../base.h"

typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef uint64_t uint64;
typedef int INT;

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

VOID PutPixel(UINT32 x, UINT32 y, UINT32 color, UINT32 *framebuffer);
VOID FillRectangle(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 color, UINT32 *buf);
void VdiSetScreenRes(int w, int h);
void VdiInit();

#endif
