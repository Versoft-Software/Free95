/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    program.c

Abstract:

    A sample program.

--*/

void NtDisplayString(const char *string)
{
    __asm__ __volatile__(
        "movl $0x002e, %%eax\n\t"  // Load syscall number for NtDisplayString into EAX
        "movl %0, %%ebx\n\t"     // Load the pointer to the string into EBX
        "int $0x2E\n\t"          // Trigger the syscall interrupt
        :
        : "r"(string)
        : "eax", "ebx"
    );
}

void _start()
{
	char text[23] = "Hello from an program!\n";
    NtDisplayString(text);
    //asm volatile("hlt");
}
