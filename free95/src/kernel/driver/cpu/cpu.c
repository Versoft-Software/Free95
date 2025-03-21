#include "cpu.h"
#include "vga.h"
#include "string.h"
#include "stdint.h"


// Function to get CPU information
void get_cpu_info() {
    uint32_t eax, ebx, ecx, edx;
    char cpu_brand[49];
   
    // Get CPU brand string part 1
    __asm__ __volatile__ (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0x80000002)
    );
    memcpy(cpu_brand, &eax, 4);
    memcpy(cpu_brand + 4, &ebx, 4);
    memcpy(cpu_brand + 8, &ecx, 4);
    memcpy(cpu_brand + 12, &edx, 4);

    // Get CPU brand string part 2
    __asm__ __volatile__ (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0x80000003)
    );
    memcpy(cpu_brand + 16, &eax, 4);
    memcpy(cpu_brand + 20, &ebx, 4);
    memcpy(cpu_brand + 24, &ecx, 4);
    memcpy(cpu_brand + 28, &edx, 4);

    // Get CPU brand string part 3
    __asm__ __volatile__ (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0x80000004)
    );
    memcpy(cpu_brand + 32, &eax, 4);
    memcpy(cpu_brand + 36, &ebx, 4);
    memcpy(cpu_brand + 40, &ecx, 4);
    memcpy(cpu_brand + 44, &edx, 4);

    cpu_brand[48] = '\0';
    print(cpu_brand); // Display the CPU model information
}

