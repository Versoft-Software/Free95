/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    kernel.c

Abstract:

    This module implements the kernel initialization code.

--*/

#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "../ke/string/string.h"
#include "../ke/idt/idt.h"
#include "../ke/io/io.h"
#include "../ke/memory/heap/kheap.h"
#include "../ke/memory/paging/paging.h"
#include "../ke/memory/memory.h"
#include "../ke/disk/disk.h"
#include "../ke/fs/pparser.h"
#include "../ke/disk/streamer.h"
#include "../ke/fs/fat/fat16.h"
#include "../ke/graphics/graphics.h"
#include "../ke/gdt/gdt.h"
#include "../ke/config.h"
#include "../ke/task/tss.h"
#include "../ke/base.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

/* Hardware text mode color constants. */
enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}

void terminal_initialize()
{
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }   
}

void FirstTimeBootInit()
{
	video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLUE));
        }
    }   
}

void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

static int DbgInit()
{
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty
   if(insb(PORT + 0) != 0xAE)
   {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(PORT + 4, 0x0F);
   return 0;
}

int IoTransEmpty()
{
   return insb(PORT + 5) & 0x20;
}

void DbgPutc(char a)
{
   while (IoTransEmpty() == 0);

   outb(PORT, a);
}

void DbgPrint(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        DbgPutc(str[i]);
    }
}

int IoSerialReceive()
{
   return insb(PORT + 5) & 1;
}

char DbgGetc()
{
   while (IoSerialReceive() == 0);

   return insb(PORT);
}

char *strstr(const char *haystack, const char *needle)
{
    if (!*needle) return (char *)haystack;

    const char *h, *n;
    while (*haystack) {
        h = haystack;
        n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n) return (char *)haystack; // Found needle
        haystack++;
    }
    return NULL; // Not found
}

int fetchwh(const char *str, int *width, int *height)
{
    *width = 0;
    *height = 0;
    const char *p = str;

    // Parse width
    while (*p >= '0' && *p <= '9') {
        *width = *width * 10 + (*p - '0');
        p++;
    }
    if (*p != 'x') return 0; // Invalid format
    p++;

    // Parse height
    while (*p >= '0' && *p <= '9') {
        *height = *height * 10 + (*p - '0');
        p++;
    }
    return (*p == '\0' || *p == '\n') ? 2 : 0; // Success if end of string
}

static struct paging_4gb_chunk* kernel_chunk = 0;

struct tss tss;
struct gdt gdt_real[FREE95_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[FREE95_TOTAL_GDT_SEGMENTS] =
{
    {.base = 0x00, .limit = 0x00, .type = 0x00},                // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},             // User data segment
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .type = 0xE9}      // TSS Segment
};

extern void jump_usermode();

void NtDisplayString(const char *string)
{
    __asm__ __volatile__(
        "movl %0, %%eax\n\t"    // Load syscall number for NtDisplayString (0x25) into EAX
        "movl %1, %%ebx\n\t"    // Load the pointer to the string into EBX
        "int $0x2E\n\t"         // Trigger the syscall interrupt
        :
        : "r"(0x002e), "r"(string)
        : "eax", "ebx"
    );
}

int NtOpenFile(
    PHANDLE FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PVOID IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
)
{
    int syscallResult = 0;

    asm volatile(
        "mov $0x004f, %%eax\n"
        "mov %1, %%edx\n"
        "int $0x2e\n"
        "mov %%eax, %0\n"
        : "=r" (syscallResult)
        : "r" (ObjectAttributes)
        : "eax", "edx"
    );

    return syscallResult;
}

void (*global_start)();

void KiUserInit()
{
    NtDisplayString("\nWelcome to Free95.\n\n");

    CHAR fileNameBuffer[] = "0:/program.elf";
    UNICODE_STRING fileName;
    fileName.Length = sizeof(fileNameBuffer) - sizeof(CHAR);
    fileName.MaximumLength = sizeof(fileNameBuffer);
    fileName.Buffer = fileNameBuffer;
    OBJECT_ATTRIBUTES objAttrs;
    InitializeObjectAttributes(&objAttrs, &fileName, NULL, NULL, NULL);

    int syscallResult;

    syscallResult = NtOpenFile(0, 0, &objAttrs, 0, 0, 0);

    if(syscallResult)
    {
        NtDisplayString("File ");
        NtDisplayString(fileNameBuffer);
        NtDisplayString(" exists!\n");
    }
    else
    {
        NtDisplayString("File ");
        NtDisplayString(fileNameBuffer);
        NtDisplayString(" does not exist!\n");
    }

    NtDisplayString("Executing program...\n\n");

    global_start();

    NtDisplayString("\nDone executing program\n");
}

void* ELF_exe_buffer;

    typedef uint16_t Elf32_Half;
	typedef uint32_t Elf32_Word;
	typedef	int32_t  Elf32_Sword;
	typedef uint64_t Elf32_Xword;
	typedef	int64_t  Elf32_Sxword;
	typedef uint32_t Elf32_Addr;
	typedef uint32_t Elf32_Off;
	typedef uint16_t Elf32_Section;

    #define EI_NIDENT (16)

	typedef struct
	{
	    unsigned char e_ident[EI_NIDENT];
	    Elf32_Half	  e_type;
	    Elf32_Half	  e_machine;
	    Elf32_Word	  e_version;
	    Elf32_Addr	  e_entry;
	    Elf32_Off	  e_phoff;
	    Elf32_Off	  e_shoff;
	    Elf32_Word	  e_flags;
	    Elf32_Half	  e_ehsize;
	    Elf32_Half	  e_phentsize;
	    Elf32_Half	  e_phnum;
	    Elf32_Half	  e_shentsize;
	    Elf32_Half	  e_shnum;
	    Elf32_Half	  e_shstrndx;
	} Elf32_Ehdr;

    // e_type values
	enum
	{
	    ET_NONE = 0x0,
	    ET_REL,
	    ET_EXEC,
	    ET_DYN,
	    // ...
	};

	typedef struct
	{
	    Elf32_Word	p_type;
	    Elf32_Off	p_offset;
	    Elf32_Addr	p_vaddr;
	    Elf32_Addr	p_paddr;
	    Elf32_Word	p_filesz;
	    Elf32_Word	p_memsz;
	    Elf32_Word	p_flags;
	    Elf32_Word	p_align;
	} Elf32_Phdr;

	// p_type values
	enum
	{
	    PT_NULL = 0x0,
	    PT_LOAD,            // Loadable section
	    // ...
	};

void kernel_main()
{
	DbgInit();

	DbgPrint("kernel_main() called\n\r");

    FirstTimeBootInit();
    DbgPrint("VGA Text Mode Initialized\n\r");
    print("Free95 is starting...\n");

	memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, FREE95_TOTAL_GDT_SEGMENTS);

    gdt_load(gdt_real, sizeof(gdt_real));

    DbgPrint("GDT Initialized\n\r");

	kheap_init();

	DbgPrint("Kernel Heap Initialized\n\r");

	fs_init();

	DbgPrint("Filesystem Initialized\n\r");

    idt_init();

    DbgPrint("IDT Initialized\n\r");

    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss_load(0x28);
    
    DbgPrint("TSS Initialized\n\r");

    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    char *ptr = kzalloc(4096);
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void*)0x1000, (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    enable_paging();

	DbgPrint("Enabled Paging\n\rPaging Test:\n\r");

	char *ptr2 = (char*)0x1000;
	ptr2[0] = 'A';
	ptr2[1] = 'B';
	
	DbgPrint(ptr2);
	DbgPrint("\n\r");

	DiskInit();
	
	DbgPrint("Disk Driver Initialized\n\r");

    enable_interrupts();

    DbgPrint("Enabled Interrupts\n\r");

	int fd = fopen("0:/boot.ini", "r");
	int w = 640;
	int h = 480;

	if (fd)
	{
		DbgPrint("Boot config file exists\n\r");
		char buf[512];
		fread(buf, 511, 1, fd);
		buf[500] = 0x00;
		DbgPrint("\nRead boot.ini:\n");
		DbgPrint(buf);

		char *settings_section = strstr(buf, "[settings]");
	    if (settings_section)
	    {
	        char *res_start = strstr(settings_section, "res=");
	        if (res_start)
	        {
	            res_start += 4;

	            if (fetchwh(res_start, &w, &h) == 2)
	            {
	                DbgPrint("Resolution was successfully extracted.\n");
	            }
	            else
	            {
	                DbgPrint("Error: Failed to parse resolution.\n");
	            }
	        }
	        else
	        {
	            DbgPrint("Error: \"res=\" was not found in settings section.\n");
	        }
	    }
	    else
	    {
	        DbgPrint("Using default 640x480 resolution.\n");
	    }
	}
	else
	{
		DbgPrint("Error: Boot configuration file not found!\n\r");
		print("boot.ini not found\nFree95 failed to start.\n");
		return;
	}

	terminal_initialize();

	//VdiSetScreenRes(w, h);
	//VdiInit();
	//DbgPrint("\n\rBGA Graphics Driver Initialized\n\r");

	uint32_t *buffer = (uint32_t*)kmalloc(w  * h * 32 / 8);
	uint32_t *fb = (uint32_t*)0xFD000000;

	if (!buffer)
	{
		DbgPrint("\n\rFailed to allocate memory for Double Buffer!\n\r");
		return;
	}

	DbgPrint("Initializing User Mode\n");

    int fd2 = fopen("0:/program.elf", "r");

	if (fd2)
	{
		DbgPrint("Boot program found\n\r");
		char content[10240];
		fread(content, 10239, 1, fd2);
		//content[700] = 0x00;
		DbgPrint("\nRead bootprog:\n");
		DbgPrint(content);

        Elf32_Ehdr *ehdr = (Elf32_Ehdr*)content;
        if (ehdr->e_ident[0] != '\x7f' || ehdr->e_ident[1] != 'E')
        {
            print("\nError: Boot program is not a valid 32-bit ELF.\nFree95 failed to start.");
            return;
        }

        if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)
        {
            print("\nError: Boot program is not an executable or dynamic executable.\nFree95 failed to start.");
            return;
        }

        Elf32_Phdr* phdr = (Elf32_Phdr*)(content + ehdr->e_phoff);

        uint32_t mem_min   = 0xFFFFFFFF, mem_max = 0;
        uint32_t alignment = 4096;
        uint32_t align     = alignment;

        for (uint32_t i = 0; i < ehdr->e_phnum; i++)
        {
            if (phdr[i].p_type != PT_LOAD) continue;
            if (align < phdr[i].p_align) align = phdr[i].p_align;

            uint32_t mem_begin = phdr[i].p_vaddr;
            uint32_t mem_end = phdr[i].p_vaddr + phdr[i].p_memsz + align - 1;

            mem_begin &= ~(align - 1);
            mem_end &= ~(align - 1);

            if (mem_begin < mem_min) mem_min = mem_begin;
            if (mem_end > mem_max) mem_max = mem_end;
        }

        uint32_t buffer_size = mem_max - mem_min;
        uint32_t buffer_alignment = align - 1;
        ELF_exe_buffer = kmalloc(buffer_size);
        if (!ELF_exe_buffer)
        {
            print("\r\nError: Could not allocate enough memory for program.\r\nFree95 could not start.");
            kfree(content);
            return;
        }

        memset(ELF_exe_buffer, 0, buffer_size);
        for (uint32_t i = 0; i < ehdr->e_phnum; i++)
        {
            if (phdr[i].p_type != PT_LOAD) continue;

            uint32_t relative_offset = phdr[i].p_vaddr - mem_min;

            uint8_t* dst = (uint8_t*)ELF_exe_buffer + relative_offset;
            uint8_t* src = (uint8_t*)content + phdr[i].p_offset;
            uint32_t len = phdr[i].p_memsz;

            memcpy(dst, src, len);
        }

        void (*entryP)() = (void (*)())((uint8_t*)ELF_exe_buffer + (ehdr->e_entry - mem_min));

        //entryP();
        global_start = entryP;
	}
	else
	{
		DbgPrint("Error: Boot program not found\n\r");
		print("Boot program not found\nFree95 failed to start.\n");
		return;
	}

    jump_usermode();

//     while(1)
//     {
//     	memset(buffer, 0, w  * h * 32 / 8);
//     	FillRectangle(0, 0, w, h, 0xFFFFFFFF, buffer);
//     	FillRectangle(10, 10, 640, 480, 0xFF00FF00, buffer);
//     	memcpy(fb, buffer, w  * h * 32 / 8);
//     }
}
