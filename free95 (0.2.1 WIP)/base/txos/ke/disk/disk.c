/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    disk.c

Abstract:

    This module implements the disk driver code.

--*/

#include "../io/io.h"
#include "disk.h"
#include "../config.h"
#include "../status.h"
#include "../memory/memory.h"

struct disk disk;

int disk_read_sector(int lba, int total, void* buf)
{
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* ptr = (unsigned short*) buf;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }

    }
    return 0;
}

void DiskInit()
{
    memset(&disk, 0, sizeof(disk));
    disk.type = FREE95_DISK_TYPE_REAL;
    disk.sector_size = FREE95_SECTOR_SIZE;
    disk.filesystem = fs_resolve(&disk);
    disk.id = 0;
}

struct disk* GetDisk(int index)
{
    if (index != 0)
        return 0;
    
    return &disk;
}

int DiskReadBlk(struct disk* idisk, unsigned int lba, int total, void* buf)
{
    if (idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}
