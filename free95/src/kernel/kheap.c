#include "kheap.h"
#include "vga.h"
#include "pmm.h"

// Heap metadata
void *g_kheap_start_addr = NULL, *g_kheap_end_addr = NULL;
unsigned long g_total_size = 0;
unsigned long g_total_used_size = 0;
KHEAP_BLOCK *g_head = NULL;

/**
 * Initialize heap with start and end addresses.
 */
int kheap_init(void *start_addr, void *end_addr) {
    if (start_addr >= end_addr) {
        print("Failed to init kheap\n");
        return -1;
    }
    g_kheap_start_addr = start_addr;
    g_kheap_end_addr = end_addr;
    g_total_size = (unsigned long)(end_addr - start_addr);
    g_total_used_size = 0;
    g_head = NULL;
    return 0;
}

/**
 * Increases heap memory by requested size and returns its address.
 */
void *kbrk(int size) {
    if (size <= 0 || g_total_used_size + size > g_total_size)
        return NULL;
    
    void *addr = g_kheap_start_addr + g_total_used_size;
    g_total_used_size += size;
    return addr;
}

/**
 * Returns the largest free memory block (Worst Fit Algorithm).
 */
KHEAP_BLOCK *worst_fit(int size) {
    KHEAP_BLOCK *best = NULL, *temp = g_head;
    while (temp) {
        if (temp->metadata.is_free && temp->metadata.size >= size) {
            if (!best || temp->metadata.size > best->metadata.size)
                best = temp;
        }
        temp = temp->next;
    }
    return best;
}

/**
 * Allocates a new heap block at the end of the heap.
 */
KHEAP_BLOCK *allocate_new_block(int size) {
    KHEAP_BLOCK *new_block = (KHEAP_BLOCK *)kbrk(sizeof(KHEAP_BLOCK));
    if (!new_block) return NULL;
    
    new_block->metadata.is_free = false;
    new_block->metadata.size = size;
    new_block->data = kbrk(size);
    new_block->next = NULL;

    if (!g_head) {
        g_head = new_block;
    } else {
        KHEAP_BLOCK *temp = g_head;
        while (temp->next) temp = temp->next;
        temp->next = new_block;
    }

    return new_block;
}

/**
 * Allocates memory. Uses worst-fit if possible; otherwise, expands heap.
 */
void *kmalloc(int size) {
    if (size <= 0) return NULL;

    KHEAP_BLOCK *block = worst_fit(size);
    if (block) {
        block->metadata.is_free = false;
        return block->data;
    }

    block = allocate_new_block(size);
    return block ? block->data : NULL;
}

/**
 * Allocates and zeroes out memory.
 */
void *kcalloc(int n, int size) {
    if (n <= 0 || size <= 0) return NULL;
    void *mem = kmalloc(n * size);
    if (mem) memset(mem, 0, n * size);
    return mem;
}

/**
 * Resizes a previously allocated block.
 */
void *krealloc(void *ptr, int size) {
    if (!ptr) return kmalloc(size);
    if (size <= 0) return NULL;

    KHEAP_BLOCK *temp = g_head;
    while (temp) {
        if (temp->data == ptr) {
            KHEAP_BLOCK *new_block = allocate_new_block(size);
            if (!new_block) return NULL;

            int copy_size = (temp->metadata.size > size) ? size : temp->metadata.size;
            memcpy(new_block->data, ptr, copy_size);

            temp->metadata.is_free = true;
            return new_block->data;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * Frees a previously allocated block.
 */
void kfree(void *addr) {
    KHEAP_BLOCK *temp = g_head;
    while (temp) {
        if (temp->data == addr) {
            temp->metadata.is_free = true;
            return;
        }
        temp = temp->next;
    }
}

/**
 * Prints all allocated heap blocks (debugging).
 */
void kheap_print_blocks() {
    KHEAP_BLOCK *temp = g_head;
    print("Block Size: ");
    printInt(sizeof(KHEAP_BLOCK));
    print("\n");
}
