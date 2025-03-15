CC = gcc
ASM = nasm

ifeq ($(VERBOSE), 1)
  V=
  Q=
else
  V=@printf "\033[1;32m[Build]\033[0m $@ ...\n";
  Q=@
endif

C_SOURCES	:= $(shell find * -name "*.c")
S_SOURCES	:= $(shell find * -name "*.s")
OBJS		:= $(C_SOURCES:%.c=%.o) $(S_SOURCES:%.s=%.o)
DEPS		:= $(OBJS:%.o=%.d)

QEMU		:= qemu-system-x86_64
QEMU_FLAGS	:= -m 128M -machine q35 -serial stdio -audiodev pa,id=speaker -machine pcspk-audiodev=speaker

C_FLAGS		:= -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -g3 -I include
LD_FLAGS	:= -static -nostdlib -m elf_i386 -T ../res/linker.ld
AS_FLAGS	:= -f elf32

all: info Free95.iso

%.o: %.c
	$(V)$(CC) $(C_FLAGS) -c -o $@ $<

%.o: %.s
	$(V)$(ASM) $(AS_FLAGS) -o $@ $<

.PHONY: info
info:
	@printf "Free95 Compilation\n"

NXOSKRNL: $(OBJS)
	$(V)$(LD) $(LD_FLAGS) -o $@ $^

Free95.iso: NXOSKRNL
	@echo
	@printf "\033[1;32m[ISO]\033[0m Packing ISO file...\n"
	@mkdir -p iso/boot/grub
	@cp $< iso/boot/

	@echo 'set timeout=3' > iso/boot/grub/grub.cfg
	@echo 'set default=0' >> iso/boot/grub/grub.cfg

	@echo 'menuentry "Free95"{' >> iso/boot/grub/grub.cfg
	@echo '	multiboot /boot/NXOSKRNL' >> iso/boot/grub/grub.cfg
	@echo '	boot' >> iso/boot/grub/grub.cfg
	@echo '}' >> iso/boot/grub/grub.cfg

	@grub-mkrescue --locales="" --output=Free95.iso iso
	@rm -rf iso
	@printf "\033[1;32m[Done]\033[0m Compilation complete.\n"

.PHONY: clean
clean:
	$(Q)$(RM) $(OBJS) $(DEPS) NXOSKRNL Free95.iso

.PHONY: run
run: Free95.iso
	$(QEMU) $(QEMU_FLAGS) -cdrom $<

-include $(DEPS)