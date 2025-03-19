/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    heap.c

Abstract:

    This module implements the kernel heap code.

--*/

#include "kheap.h"
#include "heap.h"
#include "../../config.h"
#include "../../../init/kernel.h"
#include "../memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init()
{
    int total_table_entries = FREE95_HEAP_SIZE_BYTES / FREE95_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(FREE95_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void* end = (void*)(FREE95_HEAP_ADDRESS + FREE95_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(FREE95_HEAP_ADDRESS), end, &kernel_heap_table);
    if (res < 0)
    {
        print("Failed to create heap\n");
    }

}

void* kmalloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size)
{
    void* ptr = kmalloc(size);
    if (!ptr)
        return 0;

    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void* ptr)
{
    heap_free(&kernel_heap, ptr);
}
