#include <stdint.h>
#include "descriptorTables.h"
#include "isr.h"
#include "vga.h"
#include "string.h"

// Lets us access our ASM functions from our C code.
extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);

// Internal function prototypes.
static void init_gdt();
static void init_tss();
static void init_idt();
void gdt_set_gate(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);
static void idt_set_gate(uint8_t,uint32_t,uint16_t,uint8_t);

gdt_entry_t gdt_entries[6];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

tss_entry_t tss_entry;

// Extern the ISR handler array so we can nullify them on startup.
extern isr_t interrupt_handlers[];

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void init_descriptor_tables()
{

    // Initialise the global descriptor table.
    init_gdt();
    init_tss();
    // Initialise the interrupt descriptor table.
    init_idt();
    
    // Nullify all the interrupt handlers.
    memset(&interrupt_handlers, 0, sizeof(isr_t)*256);
}

static void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
	gdt_set_gate(5, (uint32_t)&tss_entry, sizeof(tss_entry) - 1, 0xE9, 0x00); // TSS entry
 
    gdt_flush((uint32_t)&gdt_ptr);
}

static uint8_t kernel_stack[8192]; // 8KB kernel stack

static void init_tss() {
    tss_entry.ss0 = 0x10; // Kernel data segment
    tss_entry.esp0 = (uint32_t)(kernel_stack + sizeof(kernel_stack));
    gdt_set_gate(5, (uint32_t)&tss_entry, sizeof(tss_entry) - 1, 0xE9, 0x00);
    asm volatile ("ltr %%ax" :: "a"(5 * 8)); // Load the TSS selector
}

// Set the value of one GDT entry.
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

__attribute__((naked)) void syscall_1(void) {
    __asm__ __volatile__ (
        "cli\n"                      // Disable interrupts
        "pushal\n"                   // Save all general-purpose registers
        "push %ds\n"                 // Save segment registers
        "push %es\n"
        "push %fs\n"
        "push %gs\n"
        "mov $0x10, %ax\n"           // Kernel data segment
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"

        // Syscall Dispatcher
        "cmp $2, %edi\n"             // Check for `print()` syscall
        "je syscall_print\n"         // Jump to `print()` handler
        "cmp $1, %edi\n"             // Check for `test_syscall`
        "je syscall_test\n"          // Jump to test handler
        "cmp $3, %edi\n"             // Check for `print()` syscall
        "je syscall_wait\n"         // Jump to `wait()` handler
        "cmp $4, %edi\n"
        "je syscall_vidmode\n"
        "cmp $176, %edi\n"
        "je syscall_writefile\n"
        "cmp $5, %edi\n"
        "je syscall_get\n"
        "cmp $6, %edi\n"
        "je syscall_mx\n"
        "jmp syscall_unknown\n"      // Unknown syscall

        "syscall_print:\n"
        "mov %esi, %eax\n"           // Pass user string pointer (from `ESI`)
        "push %eax\n"                // Push pointer to stack
        "call print\n"               // Call kernel `print` function
        "add $4, %esp\n"             // Adjust stack
        "jmp syscall_done\n"

        "syscall_writefile:\n"
        "mov %edi, %eax\n"           // Pass syscall number in EAX
        "push %edx\n"                // Length
        "push %esi\n"                // Buffer
        "push %ebx\n"                // FileHandle
        "call NtWriteFile\n"         // Call NtWriteFile function
        "add $12, %esp\n"            // Clean up the stack
        "jmp syscall_done\n"

        "syscall_wait:\n"
        "mov %esi, %eax\n"
        "push %eax\n"                // Push pointer to stack
        "call KiWait\n"
        "add $4, %esp\n"             // Adjust stack
        "jmp syscall_done\n"

        "syscall_vidmode:\n"
        "call HwVidSetMode\n"
        "jmp syscall_done\n"

        "syscall_get:\n"
        "mov %esi, %eax\n"          // Get user buffer pointer
        "mov %edx, %ebx\n"          // Get max length
        "call scanf\n"         // Call kernel function
        "jmp syscall_done\n"

        "syscall_test:\n"
        "push $test_msg\n"           // Handle test syscall
        "call print\n"
        "add $4, %esp\n"
        "jmp syscall_done\n"

        "syscall_mx:\n"
        "call mousedrv\n"
        "jmp syscall_done\n"

        "syscall_unknown:\n"
        "push $unknown_msg\n"
        "call print\n"
        "add $4, %esp\n"

        "syscall_done:\n"
        "pop %gs\n"                  // Restore segment registers
        "pop %fs\n"
        "pop %es\n"
        "pop %ds\n"
        "popal\n"                    // Restore all general-purpose registers
        "sti\n"                      // Re-enable interrupts
        "iret\n"                     // Return from interrupt
    );
}


// Data for messages
char hello_msg[] = "Hello, world!\n";
char test_msg[] = "Test syscall executed.\n";
char unknown_msg[] = "Unknown syscall number\n";

static void init_idt()
{
    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

    // Remap the irq table.
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
    idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
    idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);
    idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);
    idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);
    idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);
    idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);
    idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);
    idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);
    idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    idt_set_gate(0x2E, (uint32_t)syscall_1, 0x08, 0xEE); // 0x8E | 0x60

    idt_flush((uint32_t)&idt_ptr);
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}
