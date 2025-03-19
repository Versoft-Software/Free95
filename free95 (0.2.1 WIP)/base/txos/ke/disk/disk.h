#ifndef DISK_H
#define DISK_H

#include "../fs/file.h"

typedef unsigned int FREE95_DISK_TYPE;

// Physical hard disk
#define FREE95_DISK_TYPE_REAL 0

struct disk
{
	FREE95_DISK_TYPE type;
	int sector_size;
	int id;
	struct filesystem *filesystem;
	void *fs_private;
};

void DiskInit();
struct disk *GetDisk(int index);
int DiskReadBlk(struct disk *idisk, unsigned int lba, int total, void *buf);

#endif
