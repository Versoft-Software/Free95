/*
 * Free 95
 *
 * NAME:
 *		bga.h
 *
 * DESCRIPTION:
 *		BGA driver.
 *
 * Author: Rainy101112 ...
 *
 */

#ifndef BGA_H
#define BGA_H

#include "base.h"
#include "stddef.h"
#include "stdint.h"

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

#define VBE_DISPI_BANK_ADDRESS 0xA0000
#define VBE_DISPI_BANK_SIZE_KB 64

#define VBE_DISPI_MAX_XRES 1024
#define VBE_DISPI_MAX_YRES 768

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_ID0 0xB0C0
#define VBE_DISPI_ID1 0xB0C1
#define VBE_DISPI_ID2 0xB0C2
#define VBE_DISPI_ID3 0xB0C3
#define VBE_DISPI_ID4 0xB0C4

#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_VBE_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS 0xE0000000

#define VBE_DISPI_LFB_ENABLED 0x40
#define BITMAP_SIZE 20
#define S_WIDTH 640

void BgaWriteRegister(UINT16 IndexValue, UINT16 DataValue);
UINT16 BgaReadRegister(UINT16 IndexValue);
INT BgaIsAvailable(VOID);
VOID BgaSetVideoMode(UINT32 Width, UINT32 Height, UINT32 BitDepth, INT UseLinearFrameBuffer, INT ClearVideoMemory);
void BgaDisable(void);

#endif
