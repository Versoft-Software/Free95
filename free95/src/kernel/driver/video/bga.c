/*
 * Free 95
 *
 * NAME:
 *		bga.c
 *
 * DESCRIPTION:
 *		BGA driver.
 *
 * Author: Rainy101112 ...
 *
 */

#include "bga.h"
#include "base.h"
#include "stdint.h"
#include "common.h"

void BgaWriteRegister(UINT16 IndexValue, UINT16 DataValue)
{
	outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
	outw(VBE_DISPI_IOPORT_DATA, DataValue);
}

UINT16 BgaReadRegister(UINT16 IndexValue)
{
	outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
	return inw(VBE_DISPI_IOPORT_DATA);
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

void BgaDisable(void)
{
	BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}
