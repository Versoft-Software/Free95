#include <stdint.h>
#include "isr.h"
#include "vga.h"
#include <stdbool.h>

bool halted = false;
isr_t interrupt_handlers[256];

void KeBugCheck(registers_t regs)
{
    FillRectangle(0, 0, 640, 480, 0x00000000);
    KiPutString("Free95\n\nPress CTRL ALT DEL to restart your computer, you WILL lose\nunsaved data.\n\n");

    terminal_initialize_a(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

    print("FREE95\n\nPress CTRL+ALT+DELETE to restart your computer, you WILL lose unsaved data.\n\n");

	print("More details:\n");

    printhex(regs.int_no);

	if (regs.int_no == 13)
	{
		print(" : KMODE_GPF_HANDLER : General Protection Fault\n");
        KiPutString("\n\n\n\n\nGeneral Protection Fault\n\n");
        FillRectangle(0, 0, 640, 480, 0x0000FFFF);
	}
	else if (regs.int_no == 6)
    {
		print(" : KMODE_IOP_HANDLER : Illegal Opcode\n");
    }
    else if (regs.int_no == 0)
    {
        print(" : KMODE_DIV_HANDLER : Division by zero\n");
    }
	else
	{
		print(" : KMODE_UNHANDLED_EXCEPTION : Null\n");
	}

	print("ERR_CODE=");
    printhex(regs.err_code);
    print("\nEDI=");
    printhex(regs.edi);
    print("\nESI=");
    printhex(regs.esi);
    print("\nEBP=");
    printhex(regs.ebp);
    print("\nESP=");
    printhex(regs.esp);
    print("\nEBX=");
    printhex(regs.ebx);
    print("\nEDX=");
    printhex(regs.edx);
    print("\nECX=");
    printhex(regs.ecx);
    print("\nEAX=");
    printhex(regs.eax);

	int ctrl_pressed = 0;
    int alt_pressed = 0;

    while (1)
    {
        // Read PS/2 keyboard input
        uint8_t scancode = inb(0x60);

        // Handle key press
        if (scancode == 0x1D) // CTRL Pressed
            ctrl_pressed = 1;
        else if (scancode == 0x9D) // CTRL Released
            ctrl_pressed = 0;
        else if (scancode == 0x38) // ALT Pressed
            alt_pressed = 1;
        else if (scancode == 0xB8) // ALT Released
            alt_pressed = 0;
        else if (scancode == 0x53 && ctrl_pressed && alt_pressed) // DELETE Pressed with CTRL+ALT
        {
            outb(0x64, 0xFE);
            while(1);
        }
    }
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t regs)
{
    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
    else
    {
		KeBugCheck(regs);
    }
}

// This gets called from our ASM interrupt handler stub.
void irq_handler(registers_t regs)
{
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
    if (regs.int_no >= 40)
    {
        // Send reset signal to slave.
        outb(0xA0, 0x20);
    }
    // Send reset signal to master. (As well as slave, if necessary).
    outb(0x20, 0x20);

    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}
