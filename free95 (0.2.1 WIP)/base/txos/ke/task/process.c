#include "process.h"
#include "../config.h"
#include "../status.h"
#include "task.h"
#include "../memory/memory.h"
#include "../string/string.h"
#include "../fs/file.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../../init/kernel.h"

// The current process that is running
struct process* current_process = 0;

static struct process* processes[FREE95_MAX_PROCESSES] = {};

static void process_init(struct process* process)
{
    memset(process, 0, sizeof(struct process));
}

struct process* process_current()
{
    return current_process;
}

struct process* process_get(int process_id)
{
    if (process_id < 0 || process_id >= FREE95_MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[process_id];
}

static int process_load_binary(const char* filename, struct process* process)
{
    return 1;
}

static int process_load_data(const char* filename, struct process* process)
{
    int res = 0;
    res = process_load_binary(filename, process);
    return res;
}

int process_map_binary(struct process* process)
{
    int res = 0;
    paging_map_to(process->task->page_directory->directory_entry, (void*) FREE95_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

int process_map_memory(struct process* process)
{
    int res = 0;
    res = process_map_binary(process);
    return res;
}

int process_get_free_slot()
{
    for (int i = 0; i < FREE95_MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
            return i;
    }

    return -EISTKN;
}

int process_load(const char* filename, struct process** process)
{
    int res = 0;
    int process_slot = process_get_free_slot();
    if (process_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);
out:
    return res;
}

int process_load_for_slot(const char* filename, struct process** process, int process_slot)
{
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    program_stack_ptr = kzalloc(FREE95_USER_PROGRAM_STACK_SIZE);
    if (!program_stack_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    // Create a task
    task = task_new(_process);
    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
    }

    _process->task = task;

    res = process_map_memory(_process);
    if (res < 0)
    {
        goto out;
    }

    *process = _process;

    // Add the process to the array
    processes[process_slot] = _process;

out:
    if (ISERR(res))
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

       // Free the process data
    }
    return res;
}
