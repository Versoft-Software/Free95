/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    graphics.c

Abstract:

    This module implements an extremely simple Graphics Driver.

--*/

#include "graphics.h"

int nWidth = 320;
int nHeight = 200;

static inline VOID outpw(UINT16 port, UINT16 value)
{
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline UINT16 inpw(UINT16 port)
{
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

void WritePort(uint16_t port, uint8_t value)
{
    __asm__ __volatile__ ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t ReadPort(uint16_t port)
{
    uint8_t value;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void BgaDisable(void)
{
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

VOID PutPixel(UINT32 x, UINT32 y, UINT32 color, UINT32 *framebuffer)
{
    framebuffer[y * nWidth + x] = color;
}

VOID FillRectangle(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 color, UINT32 *buf)
{
	UINT32 i, j;

    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
        {
            PutPixel(i, j, color, buf);
        }
    }
}

void VdiSetScreenRes(int w, int h)
{
	nWidth = w;
	nHeight = h;
}

void VdiInit()
{
	BgaSetVideoMode(nWidth, nHeight, 0x20, 1, 1);
}
