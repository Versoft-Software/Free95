;++
;
; Free95 20x Assembly
;
; You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
; If you do not agree to the terms, do not use the code.
;
;
; Module Name:
;
;    kernel.asm
;
; Abstract:
;
;    This module implements the assembly code necessary to jump
;    to C for 20x/TX.
;
;--

[BITS 32]

global _start
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Remap the master PIC
    mov al, 00010001b
    out 0x20, al ; Tell master PIC

    mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
    out 0x21, al

    mov al, 00000001b
    out 0x21, al
    ; End remap of the master PIC

    call kernel_main

    jmp $

times 512-($ - $$) db 0
