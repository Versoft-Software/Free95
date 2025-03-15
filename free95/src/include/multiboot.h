#include "stdint.h"
#include "stddef.h"
#include "base.h"

#define MEMORY_SIZE (1024 * 1024) // 1 MB total memory
#define BLOCK_SIZE 4096			  // 4 KB block size

#define MULTIBOOT_MAGIC_HEADER 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* The Multiboot header. */
typedef struct
{
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
} MULTIBOOT_HEADER;

/* The symbol table for a.out. */
typedef struct
{
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
} AOUT_SYMBOL_TABLE;

/* The section header table for ELF. */
typedef struct
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
} ELF_SECTION_HEADER_TABLE;

typedef struct
{
	/* required, defined in boot.s */
	uint32_t flags;

	/* available low-high memory from BIOS, present if flags[0] is set(MEMINFO in boot.s) */
	uint32_t mem_low;
	uint32_t mem_high;

	/* "root" partition, present if flags[1] is set(BOOTDEVICE in boot.s) */
	uint32_t boot_device;

	/* kernel command line, present if flags[2] is set(CMDLINE in boot.s) */
	uint32_t cmdline;

	/* no of modules loaded, present if flags[3] is set(MODULECOUNT in boot.s) */
	uint32_t modules_count;
	uint32_t modules_addr;

	/* symbol table info, present if flags[4] & flags[5] is set(SYMT in boot.s) */
	union
	{
		AOUT_SYMBOL_TABLE aout_sym;
		ELF_SECTION_HEADER_TABLE elf_sec;
	} u;

	/* memory mapping, present if flags[6] is set(MEMMAP in boot.s) */
	uint32_t mmap_length;
	uint32_t mmap_addr;

	/* drive info, present if flags[7] is set(DRIVE in boot.s) */
	uint32_t drives_length;
	uint32_t drives_addr;

	/* ROM configuration table, present if flags[8] is set(CONFIGT in boot.s) */
	uint32_t config_table;

	/* boot loader name, present if flags[9] is set(BOOTLDNAME in boot.s) */
	uint32_t boot_loader_name;

	/* Advanced Power Management(APM) table, present if flags[10] is set(APMT in boot.s) */
	uint32_t apm_table;

	/* video info, present if flags[11] is set(VIDEO in boot.s) */
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

	/* video framebufer info, present if flags[12] is set(VIDEO_FRAMEBUF in boot.s)  */
	uint64 framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t framebuffer_bpp;
	uint8_t framebuffer_type; // indexed = 0, RGB = 1, EGA = 2

} MULTIBOOT_INFO;

typedef enum
{
	MULTIBOOT_MEMORY_AVAILABLE = 1,
	MULTIBOOT_MEMORY_RESERVED,
	MULTIBOOT_MEMORY_ACPI_RECLAIMABLE,
	MULTIBOOT_MEMORY_NVS,
	MULTIBOOT_MEMORY_BADRAM
} MULTIBOOT_MEMORY_TYPE;

// multiboot.h
typedef struct
{
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) MULTIBOOT_MEMORY_MAP;

// symbols from linker.ld for section addresses
extern uint8_t __kernel_section_start;
extern uint8_t __kernel_section_end;
extern uint8_t __kernel_text_section_start;
extern uint8_t __kernel_text_section_end;
extern uint8_t __kernel_data_section_start;
extern uint8_t __kernel_data_section_end;
extern uint8_t __kernel_rodata_section_start;
extern uint8_t __kernel_rodata_section_end;
extern uint8_t __kernel_bss_section_start;
extern uint8_t __kernel_bss_section_end;

typedef struct
{
	struct
	{
		uint32_t k_start_addr;
		uint32_t k_end_addr;
		uint32_t k_len;
		uint32_t text_start_addr;
		uint32_t text_end_addr;
		uint32_t text_len;
		uint32_t data_start_addr;
		uint32_t data_end_addr;
		uint32_t data_len;
		uint32_t rodata_start_addr;
		uint32_t rodata_end_addr;
		uint32_t rodata_len;
		uint32_t bss_start_addr;
		uint32_t bss_end_addr;
		uint32_t bss_len;
	} kernel;

	struct
	{
		uint32_t total_memory;
	} system;

	struct
	{
		uint32_t start_addr;
		uint32_t end_addr;
		uint32_t size;
	} available;
} KERNEL_MEMORY_MAP;

KERNEL_MEMORY_MAP g_kmap;
