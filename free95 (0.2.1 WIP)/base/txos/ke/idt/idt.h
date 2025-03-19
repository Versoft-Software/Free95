#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "../base.h"
#include "../fs/file.h"

#define DIVISION_BY_ZERO 0xF151

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    LPSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p, n, a, r, s) \
    do { \
        (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
        (p)->RootDirectory = (r); \
        (p)->Attributes = (ULONG)(a); \
        (p)->ObjectName = (n); \
        (p)->SecurityDescriptor = (s); \
        (p)->SecurityQualityOfService = NULL; \
    } while (0)


struct idt_desc
{
    uint16_t offset_1; // Offset bits 0 - 15
    uint16_t selector; // Selector thats in our GDT
    uint8_t zero; // Does nothing, unused set to zero
    uint8_t type_attr; // Descriptor type and attributes
    uint16_t offset_2; // Offset bits 16-31
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // Size of descriptor table -1
    uint32_t base; // Base address of the start of the interrupt descriptor table
} __attribute__((packed));


int NtOpenFileSyscall(
    PHANDLE FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PVOID IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
);

typedef struct _OSVERSIONINFOEXA
{
    DWORD dwOSVersionInfoSize;  // Size of this structure, in bytes.
    DWORD dwMajorVersion;       // Major version number of the OS.
    DWORD dwMinorVersion;       // Minor version number of the OS.
    DWORD dwBuildNumber;        // Build number of the OS.
    DWORD dwPlatformId;         // Platform identifier.
    CHAR  szCSDVersion[128];    // Null-terminated string for service pack info.
    WORD  wServicePackMajor;    // Major version number of the service pack.
    WORD  wServicePackMinor;    // Minor version number of the service pack.
    WORD  wSuiteMask;           // Bitmask for product suites available on the system.
    BYTE  wProductType;         // Additional information about the system type.
    BYTE  wReserved;            // Reserved for future use.
} OSVERSIONINFOEXA, *POSVERSIONINFOEXA, *LPOSVERSIONINFOEXA;

typedef OSVERSIONINFOEXA OSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXA LPOSVERSIONINFOEX;

void KeBugCheck(unsigned long BugCheckCode);
void idt_init();
void enable_interrupts();
void disable_interrupts();

#endif
