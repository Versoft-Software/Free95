/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    file.c

Abstract:

    This module implements the virtual file system code.

--*/

#include "file.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../string/string.h"
#include "../disk/disk.h"
#include "../status.h"
#include "../../init/kernel.h"
#include "fat/fat16.h"

struct filesystem* filesystems[FREE95_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[FREE95_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem()
{
    int i = 0;
    for (i = 0; i < FREE95_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct filesystem* filesystem)
{
    struct filesystem** fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
    	DbgPrint("Error: There was a problem while inserting the filesystem!\n\rThe system will now be halted.\n\r");
        while(1) {}
    }

    *fs = filesystem;
}

static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());
}

void fs_load()
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM;
    for (int i = 0; i < FREE95_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= FREE95_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    // Descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];
}

struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs = 0;
    for (int i = 0; i < FREE95_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if(strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if(strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int fopen(const char* filename, const char* mode_str)
{
    int res = 0;
    struct path_root* root_path = pathparser_parse(filename, NULL);
    if (!root_path)
    {
        res = -EINVARG;
        goto out;
    }

    // We cannot have just a root path 0:/ 0:/test.txt
    if (!root_path->first)
    {
        res = -EINVARG;
        goto out;
    }

    // Ensure the disk we are reading from exists
    struct disk* disk = GetDisk(root_path->drive_no);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }

    if (!disk->filesystem)
    {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }

    void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if (ISERR(descriptor_private_data))
    {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&desc);
    if (res < 0)
    {
        goto out;
    }
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

out:
    // fopen shouldnt return negative values
    if (res < 0)
        res = 0;

    return res;
}

int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd)
{
    int res = 0;
    if (size == 0 || nmemb == 0 || fd < 1)
    {
        res = -EINVARG;
        goto out;
    }

    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }

    res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char*) ptr);
out:
    return res;
}
