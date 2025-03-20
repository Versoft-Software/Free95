/*
 * Free95
 *
 * Module name:
 *      kernel.c
 *
 * Description:
 *      20x Kernel
 *
 * Author: Kap Petrov
 *
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "vga.h"
#include "string.h"
#include "descriptorTables.h"
#include "isr.h"
#include "kb.h"

#include "pmm.h"
#include "kheap.h"
#include "win32.h"
#include "hello.h"
#include "reshell.h"
#include "freever.h"
#include "mouse.h"
#include "vga.h"
#include "bga.h"
#include "timer.h"

extern const unsigned char font_8x16[];

VOID PutPixel(UINT32 x, UINT32 y, UINT32 color)
{
    UINT32 *framebuffer = (UINT32 *)0xFD000000;
    framebuffer[y * S_WIDTH + x] = color;
}

VOID FillRectangle(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 color)
{
    UINT32 i, j;

    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
        {
            PutPixel(i, j, color);
        }
    }
}

VOID PutChar(CHAR ch, UINT32 x_coord, UINT32 y_coord, UINT32 color)
{
    UINT32 index = 0;

    if (ch >= 'A' && ch <= 'Z')
    {
        index = (ch - 'A') * 16;
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        index = (ch - 'a' + 43) * 16;
    }
    else if (ch >= '0' && ch <= '9')
    {
        index = (ch - '0' + 32) * 16;
    }
    else
    {
        switch (ch)
        {
        case ' ':
            index = 26 * 16;
            break;
        case '!':
            index = 27 * 16;
            break;
        case '(':
            index = 28 * 16;
            break;
        case ')':
            index = 29 * 16;
            break;
        case '.':
            index = 31 * 16;
            break;
        case ',':
            index = 41 * 16;
            break;
        case ':':
            index = 42 * 16;
            break;
        default:
            return;
        }
    }

    for (UINT32 row = 0; row < 16; row++)
    {
        UINT8 data = font_8x16[index + row];
        for (UINT32 col = 0; col < 8; col++)
        {
            if (data & (0x80 >> col))
            {
                PutPixel(x_coord + col, y_coord + row, color);
            }
        }
    }
}

VOID KiPutString(const CHAR *str, INT x, INT y)
{
    int ax = x;
    int ay = y;

    while (*str)
    {
        if (*str == '\n')
        {
            ax = x;
            ay += 16;
        }
        else
        {
            PutChar(*str, ax, ay, 0xFFFFFFFF);
            ax += 10;
        }
        // Handle screen wrapping
        if (ax >= 640)
        {
            ax = 0;
            ay += 16;
        }

        str++;
    }
}

VOID KiPutStringC(const CHAR *str, INT x, INT y, UINT color)
{
    int ax = x;
    int ay = y;

    while (*str)
    {
        if (*str == '\n')
        {
            ax = x;
            ay += 16;
        }
        else
        {
            PutChar(*str, ax, ay, color);
            ax += 10;
        }
        // Handle screen wrapping
        if (ax >= 640)
        {
            ax = 0;
            ay += 16;
        }

        str++;
    }
}

VOID KiDrawBitmap(INT startX, INT startY, INT bitmap[22][22], UINT32 color)
{
    for (INT y = 0; y < 22; y++)
    {
        for (INT x = 0; x < 22; x++)
        {
            if (bitmap[y][x] == 1)
            {
                PutPixel(startX + x, startY + y, color);
            }
        }
    }
}

int mouseCursorBitmap[22][22] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

extern void jump_usermode();

void NtPutString(const char *str)
{
    asm volatile(
        "mov %0, %%edi\n" // Pass syscall number in EDI
        "mov %1, %%esi\n" // Pass string pointer in ESI
        "int $0x2E\n"     // Trigger syscall
        :
        : "r"(2), "r"(str)
        : "edi", "esi");
}

void NtSleep(uint32_t ms)
{
    asm volatile(
        "mov %0, %%edi\n" // Pass syscall number in EDI
        "mov %1, %%esi\n" // Pass milliseconds pointer in ESI
        "int $0x2E\n"     // Trigger syscall
        :
        : "r"(3), "r"(ms)
        : "edi", "esi");
}

void NtVdiSetPDEV()
{
    asm volatile(
        "mov %0, %%edi\n" // Pass syscall number in EDI
        "int $0x2E\n"     // Trigger syscall
        :
        : "r"(4)
        : "edi");
}

void HwVidSetMode()
{
    BgaSetVideoMode(640, 480, 0x20, 1, 1);
}

#define MEMORY_SIZE (1024 * 1024) // 1 MB total memory
#define BLOCK_SIZE 4096           // 4 KB block size

#define MULTIBOOT_MAGIC_HEADER 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* The Multiboot header. */
typedef struct
{
    uint32 magic;
    uint32 flags;
    uint32 checksum;
    uint32 header_addr;
    uint32 load_addr;
    uint32 load_end_addr;
    uint32 bss_end_addr;
    uint32 entry_addr;
} MULTIBOOT_HEADER;

/* The symbol table for a.out. */
typedef struct
{
    uint32 tabsize;
    uint32 strsize;
    uint32 addr;
    uint32 reserved;
} AOUT_SYMBOL_TABLE;

/* The section header table for ELF. */
typedef struct
{
    uint32 num;
    uint32 size;
    uint32 addr;
    uint32 shndx;
} ELF_SECTION_HEADER_TABLE;

typedef struct
{
    /* required, defined in boot.s */
    uint32 flags;

    /* available low-high memory from BIOS, present if flags[0] is set(MEMINFO in boot.s) */
    uint32 mem_low;
    uint32 mem_high;

    /* "root" partition, present if flags[1] is set(BOOTDEVICE in boot.s) */
    uint32 boot_device;

    /* kernel command line, present if flags[2] is set(CMDLINE in boot.s) */
    uint32 cmdline;

    /* no of modules loaded, present if flags[3] is set(MODULECOUNT in boot.s) */
    uint32 modules_count;
    uint32 modules_addr;

    /* symbol table info, present if flags[4] & flags[5] is set(SYMT in boot.s) */
    union
    {
        AOUT_SYMBOL_TABLE aout_sym;
        ELF_SECTION_HEADER_TABLE elf_sec;
    } u;

    /* memory mapping, present if flags[6] is set(MEMMAP in boot.s) */
    uint32 mmap_length;
    uint32 mmap_addr;

    /* drive info, present if flags[7] is set(DRIVE in boot.s) */
    uint32 drives_length;
    uint32 drives_addr;

    /* ROM configuration table, present if flags[8] is set(CONFIGT in boot.s) */
    uint32 config_table;

    /* boot loader name, present if flags[9] is set(BOOTLDNAME in boot.s) */
    uint32 boot_loader_name;

    /* Advanced Power Management(APM) table, present if flags[10] is set(APMT in boot.s) */
    uint32 apm_table;

    /* video info, present if flags[11] is set(VIDEO in boot.s) */
    uint32 vbe_control_info;
    uint32 vbe_mode_info;
    uint16 vbe_mode;
    uint16 vbe_interface_seg;
    uint16 vbe_interface_off;
    uint16 vbe_interface_len;

    /* video framebufer info, present if flags[12] is set(VIDEO_FRAMEBUF in boot.s)  */
    uint64 framebuffer_addr;
    uint32 framebuffer_pitch;
    uint32 framebuffer_width;
    uint32 framebuffer_height;
    uint8 framebuffer_bpp;
    uint8 framebuffer_type; // indexed = 0, RGB = 1, EGA = 2

} MULTIBOOT_INFO;

typedef enum
{
    MULTIBOOT_MEMORY_AVAILABLE = 1,
    MULTIBOOT_MEMORY_RESERVED,
    MULTIBOOT_MEMORY_ACPI_RECLAIMABLE,
    MULTIBOOT_MEMORY_NVS,
    MULTIBOOT_MEMORY_BADRAM
} MULTIBOOT_MEMORY_TYPE;

typedef struct
{
    uint32 size;
    uint32 addr_low;
    uint32 addr_high;
    uint32 len_low;
    uint32 len_high;
    MULTIBOOT_MEMORY_TYPE type;
} MULTIBOOT_MEMORY_MAP;

// symbols from linker.ld for section addresses
extern uint8 __kernel_section_start;
extern uint8 __kernel_section_end;
extern uint8 __kernel_text_section_start;
extern uint8 __kernel_text_section_end;
extern uint8 __kernel_data_section_start;
extern uint8 __kernel_data_section_end;
extern uint8 __kernel_rodata_section_start;
extern uint8 __kernel_rodata_section_end;
extern uint8 __kernel_bss_section_start;
extern uint8 __kernel_bss_section_end;

typedef struct
{
    struct
    {
        uint32 k_start_addr;
        uint32 k_end_addr;
        uint32 k_len;
        uint32 text_start_addr;
        uint32 text_end_addr;
        uint32 text_len;
        uint32 data_start_addr;
        uint32 data_end_addr;
        uint32 data_len;
        uint32 rodata_start_addr;
        uint32 rodata_end_addr;
        uint32 rodata_len;
        uint32 bss_start_addr;
        uint32 bss_end_addr;
        uint32 bss_len;
    } kernel;

    struct
    {
        uint32 total_memory;
    } system;

    struct
    {
        uint32 start_addr;
        uint32 end_addr;
        uint32 size;
    } available;
} KERNEL_MEMORY_MAP;

KERNEL_MEMORY_MAP g_kmap;

int get_kernel_memory_map(KERNEL_MEMORY_MAP *kmap, MULTIBOOT_INFO *mboot_info)
{
    uint32 i;

    if (kmap == NULL)
        return -1;
    kmap->kernel.k_start_addr = (uint32)&__kernel_section_start;
    kmap->kernel.k_end_addr = (uint32)&__kernel_section_end;
    kmap->kernel.k_len = ((uint32)&__kernel_section_end - (uint32)&__kernel_section_start);

    kmap->kernel.text_start_addr = (uint32)&__kernel_text_section_start;
    kmap->kernel.text_end_addr = (uint32)&__kernel_text_section_end;
    kmap->kernel.text_len = ((uint32)&__kernel_text_section_end - (uint32)&__kernel_text_section_start);

    kmap->kernel.data_start_addr = (uint32)&__kernel_data_section_start;
    kmap->kernel.data_end_addr = (uint32)&__kernel_data_section_end;
    kmap->kernel.data_len = ((uint32)&__kernel_data_section_end - (uint32)&__kernel_data_section_start);

    kmap->kernel.rodata_start_addr = (uint32)&__kernel_rodata_section_start;
    kmap->kernel.rodata_end_addr = (uint32)&__kernel_rodata_section_end;
    kmap->kernel.rodata_len = ((uint32)&__kernel_rodata_section_end - (uint32)&__kernel_rodata_section_start);

    kmap->kernel.bss_start_addr = (uint32)&__kernel_bss_section_start;
    kmap->kernel.bss_end_addr = (uint32)&__kernel_bss_section_end;
    kmap->kernel.bss_len = ((uint32)&__kernel_bss_section_end - (uint32)&__kernel_bss_section_start);

    kmap->system.total_memory = mboot_info->mem_low + mboot_info->mem_high;

    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP))
    {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        // make sure kernel is loaded at 0x100000 by bootloader(see linker.ld)
        if (mmap->addr_low == kmap->kernel.text_start_addr)
        {
            // set available memory starting from end of our kernel, leaving 1MB size for functions exceution
            kmap->available.start_addr = kmap->kernel.k_end_addr + 1024 * 1024;
            kmap->available.end_addr = mmap->addr_low + mmap->len_low;
            // get availabel memory in bytes
            kmap->available.size = kmap->available.end_addr - kmap->available.start_addr;
            return 0;
        }
    }

    return -1;
}

FileNode *con;
int recount = 0;

FileNode *getCon()
{
    return con;
}

#define MaxThreadCount 10
int TaskCounter = 0;
TaskControlBlock Tasks[MaxThreadCount];

void PsCreateSystemThread(TaskControlBlock h)
{
    Tasks[TaskCounter] = h;
    TaskCounter++;
}

void PsKillSystemThread(int ProcessId)
{
    Tasks[ProcessId].IsActive = false;
}

static uint32_t TaskIndex = 0;

uint32_t KiGetTask()
{
    return TaskIndex;
}

void KiSetTask(uint32_t i)
{
    TaskIndex = i;
}

char *KiGetTnBP(uint32_t i)
{
    return Tasks[i].Name;
}

void launch()
{
    TaskControlBlock App = {VerMain, true, "freever.exe"};
    PsCreateSystemThread(App);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"

void KiUserInit()
{
    TaskControlBlock R = {ReshellMain, true, "reshell.exe"};
    TaskControlBlock I = {WinMain, true, "hello.exe"};

    PsCreateSystemThread(R);
    // PsCreateSystemThread(I);

    memset(con->Name, 0, sizeof(con->Name));
    strcpy(con->Name, "CON");
    memset(con->Content, 0, sizeof(con->Content));
    strcpy(con->Content, "\n\n\n\n\n\n\n");

    setPosition(5, 0);

    NtVdiSetPDEV();
    FillRectangle(0, 0, 640, 480, 0x00000000);
    KiPutString("VERSOFT FREE95 (C) 2025\nALL RIGHTS RESERVED", 200, 0);
    KiPutString("Starting Free95...", 250, 270);
    FillRectangle(300, 220, 40, 40, RGB(255, 255, 255));
    NtSleep(500);
    FillRectangle(300, 220, 40, 40, RGB(255, 0, 0));
    NtSleep(500);
    FillRectangle(300, 220, 40, 40, RGB(0, 255, 0));
    NtSleep(500);
    FillRectangle(300, 220, 40, 40, RGB(0, 0, 255));
    NtSleep(1300);
    FillRectangle(0, 0, 640, 480, 0xFF00807F);

    while (1)
    {
        FillRectangle(mouse_getx(), mouse_gety(), 20, 20, 0xFF00807F);
        mouse_poll();
        FillRectangle(mouse_getx(), mouse_gety(), 20, 20, RGB(0, 0, 0));

        if (Tasks[TaskIndex].IsActive)
        {
            Tasks[TaskIndex].Task();
        }

        NtSleep(1);

        TaskIndex = (TaskIndex + 1) % TaskCounter;
    }
}

void kernelMain(unsigned long magic, unsigned long addr)
{
    MULTIBOOT_INFO *mboot_info;

    terminal_initialize();
    init_descriptor_tables();
    init_timer(1000);
    init_keyboard();
    mouse_initialize();

    terminal_initialize_a(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_DARK_GREY));
    print("Free95 [Version 0.2.9]\n\n");

    mboot_info = (MULTIBOOT_INFO *)addr;
    memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));
    if (get_kernel_memory_map(&g_kmap, mboot_info) < 0)
    {
        print("Error: Free95 was unable to get the Kernel Memory Map\nFree95 could not start.\n");
        return;
    }

    pmm_init(g_kmap.available.start_addr, g_kmap.available.size);

    pmm_init_region(g_kmap.available.start_addr, PMM_BLOCK_SIZE * 10);

    // Trololololol
    KiWait(500);

    HwVidSetMode();

    jump_usermode();
}

#pragma GCC diagnostic pop
