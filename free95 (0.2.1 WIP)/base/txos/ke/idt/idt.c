/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    idt.c

Abstract:

    This module implements the Interrupt Descriptor Table code.

--*/

#include "idt.h"
#include "../config.h"
#include "../../init/kernel.h"
#include "../memory/memory.h"
#include "../io/io.h"
#include "../bug.h"

#define RING3 0xEE

struct idt_desc idt_descriptors[FREE95_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void int2eh();
extern void no_interrupt();

void int21h_handler()
{
    // TODO: Keyboard driver here!
}

int NtOpenFileSyscall(
    PHANDLE FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PVOID IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
)
{
    return fopen(ObjectAttributes->ObjectName->Buffer, "r");
}

int NtReadFileSyscall(
    int FileHandle,
    HANDLE Event,
    PVOID ApcRoutine,
    PVOID ApcContext,
    PVOID IoStatusBlock,
    PVOID Buffer,
    ULONG Length,
    ULONG ByteOffset,
    PULONG Key
)
{
    return fread(Buffer, Length - 1, 1, FileHandle);
}

void *syscall_dispatcher(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3,
                         uint32_t arg4, uint32_t arg5, uint32_t arg6, uint32_t arg7, uint32_t arg8, uint32_t arg9)
{
    uint32_t result = 0;

    switch (syscall_number)
    {
        case 0x002e:
            // Only uncomment while debugging this syscall
            // print("NtDisplayString() syscall called\n");
            print((char*)arg1);
            DbgPrint((char*)arg1);
            result = 1;
            break;

        case 0x004f:
            // Only uncomment while debugging this syscall
            // print("NtOpenFile() syscall called\n");
            result = NtOpenFileSyscall(0, 0, (POBJECT_ATTRIBUTES)arg3, 0, 0, 0);
            break;

        default:
            print("Syscall failed with RESULT: -1\n");
            result = -1;
            break;
    }

    return (void*)result;
}

void syscall_handler()
{
    uint32_t syscall_number;
    uint32_t arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9;

    asm volatile("mov %%eax, %0" : "=r"(syscall_number));
    asm volatile("mov %%ebx, %0" : "=r"(arg1));
    asm volatile("mov %%ecx, %0" : "=r"(arg2));
    asm volatile("mov %%edx, %0" : "=r"(arg3));

    asm volatile("mov 4(%%esp), %0" : "=r"(arg4));
    asm volatile("mov 8(%%esp), %0" : "=r"(arg5));
    asm volatile("mov 12(%%esp), %0" : "=r"(arg6));
    asm volatile("mov 16(%%esp), %0" : "=r"(arg7));
    asm volatile("mov 20(%%esp), %0" : "=r"(arg8));
    asm volatile("mov 24(%%esp), %0" : "=r"(arg9));

    syscall_dispatcher(syscall_number, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}


void no_interrupt_handler()
{
	outb(0x20, 0x20);
}

void idt_zero()
{
    KeBugCheck(KMODE_DIV_ZERO);
}

void idt_pf()
{
    KeBugCheck(KMODE_PAGE_FAULT);
}

void idt_gpf()
{
    KeBugCheck(KMODE_GPF);
}

void idt_df()
{
    KeBugCheck(KMODE_DF);
}

void idt_snp()
{
    KeBugCheck(KMODE_INVOP);
}

void idt_inv()
{
    KeBugCheck(KMODE_SNP);
}

void idt_imp()
{
	KeBugCheck(IMP_CRASH);
}

void idt_set(int interrupt_no, void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = RING3;
    desc->offset_2 = (uint32_t) address >> 16;
}

void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

	for (int i = 0; i < FREE95_TOTAL_INTERRUPTS; i++)
	{
		idt_set(i, no_interrupt);
	}

    idt_set(0x2E, int2eh);

    idt_set(0, idt_zero);
    idt_set(14, idt_pf);
    idt_set(13, idt_gpf);
    idt_set(6, idt_inv);
    idt_set(8, idt_df);
    idt_set(11, idt_snp);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}
