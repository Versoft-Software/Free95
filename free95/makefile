# Licensed under GNU GPL 3.0
CC  := gcc
CXX := g++

# x86/32 = x86
# x86/64 = x64
# arm/32 = arm
# arm/64 = aarch64
PLATFORM ?= x86/32

SRC   := ./src
BIN   := ./build/bin
OBJ   := ./build/free95/obj

LDFLAGS := -T ./res/linker.ld -nostdlib

ifeq ($(word 1,$(subst /, ,$(PLATFORM))),x86)
ifeq ($(word 2,$(subst /, ,$(PLATFORM))),32)
	LDFLAGS += -m elf_i386
endif
endif

SOURCES := $(wildcard $(SRC)/**.c)
ASMSRC  := $(wildcard $(SRC)/**.s)
ASMOBJ  := $(patsubst $(SRC)/%.s,$(OBJ)/%.s.o,$(ASMSRC))
COBJ    := $(patsubst $(SRC)/%.c,$(OBJ)/%.c.o,$(SOURCES))
CFLAGS= -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -Wno-error -Wno-implicit-function-declaration -Wno-int-conversion
ASFLAGS=-felf32

all: $(COBJ) $(ASMOBJ) link 

link:
	mkdir -p $(BIN)
	ld $(LDFLAGS) -o $(BIN)/nxoskrnl.nef $(ASMOBJ) $(COBJ)

$(OBJ)/%.s.o: $(SRC)/**.s
	mkdir -p $(dir $@)
	nasm $(ASFLAGS) src/$*.s -o $@

$(OBJ)/%.c.o: $(SRC)/**.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) src/$*.c -o $@

run:
	qemu-system-i386 -kernel $(BIN)/nxoskrnl.nef

clean:
	rm -rf ./build/*
