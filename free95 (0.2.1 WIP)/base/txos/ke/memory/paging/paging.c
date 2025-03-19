/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    paging.c

Abstract:

    This module implements the kernel paging code.

--*/

#include "paging.h"
#include "../heap/kheap.h"
#include "../../status.h"
void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;
struct paging_4gb_chunk *paging_new_4gb(uint8_t flags)
{
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk *chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

void paging_switch(uint32_t *directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

void paging_free_4gb(struct paging_4gb_chunk *chunk)
{
    for (int i = 0; i < 1024; i++)
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t *table = (uint32_t *)(entry & 0xfffff000);
        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk)
{
    return chunk->directory_entry;
}

bool paging_is_aligned(void *addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -EINVARG;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

void* paging_align_address(void* ptr)
{
    if ((uint32_t)ptr % PAGING_PAGE_SIZE)
    {
        return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    }
    
    return ptr;
}
int paging_map(uint32_t* directory, void* virt, void* phys, int flags)
{
    if (((unsigned int)virt % PAGING_PAGE_SIZE) || ((unsigned int) phys % PAGING_PAGE_SIZE))
    {
        return -EINVARG;
    }

    return paging_set(directory, virt, (uint32_t) phys | flags);
}
int paging_map_range(uint32_t* directory, void* virt, void* phys, int count, int flags)
{
    int res = 0;
    for (int i = 0; i < count; i++)
    {
        res = paging_map(directory, virt, phys, flags);
        if (res == 0)
            break;
        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return res;
}

int paging_map_to(uint32_t *directory, void *virt, void *phys, void *phys_end, int flags)
{
    int res = 0;
    if ((uint32_t)virt % PAGING_PAGE_SIZE)
    {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys % PAGING_PAGE_SIZE)
    {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys_end % PAGING_PAGE_SIZE)
    {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)phys_end < (uint32_t)phys)
    {
        res = -EINVARG;
        goto out;
    }

    uint32_t total_bytes = phys_end - phys;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virt, phys, total_pages, flags);
out:
    return res;
}
int paging_set(uint32_t *directory, void *virt, uint32_t val)
{
    if (!paging_is_aligned(virt))
    {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virt, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    table[table_index] = val;

    return 0;
}
