section .asm

extern int21h_handler
extern syscall_handler
extern no_interrupt_handler

global int21h
global int2eh
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts

enable_interrupts:
	sti
	ret

disable_interrupts:
	cli
	ret

idt_load:
    push ebp
    mov ebp, esp

	mov ebx, [ebp+8]
	lidt [ebx]
	pop ebp
    ret

int21h:
	cli
	pushad
	call int21h_handler
	popad
	sti
	iret

int2eh:
	pushad

	push esp

	push eax
	call syscall_handler

	mov dword[tmp_res], eax
	add esp, 8

	popad
	mov eax, [tmp_res]
	iretd

no_interrupt:
	cli
	pushad
	call no_interrupt_handler
	popad
	sti
	iret

section .data
tmp_res: dd 0
