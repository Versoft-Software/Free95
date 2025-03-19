#ifndef TASK_H
#define TASK_H

#include "../config.h"
#include "../memory/paging/paging.h"

struct registers
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct process;
struct task
{
    /**
     * The page directory of the task
     */
    struct paging_4gb_chunk* page_directory;

    // The registers of the task when the task is not running
    struct registers registers;

	// Process
	struct process* process;

    // The next task in the linked list
    struct task* next;

    // Previous task in the linked list
    struct task* prev;
};

typedef struct task Thread;

struct task* task_new(struct process* process);
struct task* task_current();
struct task* task_get_next();
int task_free(struct task* task);


#endif
