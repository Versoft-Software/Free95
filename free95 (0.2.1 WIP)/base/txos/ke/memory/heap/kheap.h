#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

void kheap_init();
void* kmalloc(size_t size);
void* kzalloc(size_t size);
void kfree(void* ptr);

#endif
