FILES = ./build/kernel.asm.o ./build/kernel.o ./build/loader.o ./build/user.asm.o ./build/graphics.o ./build/disk/disk.o ./build/bug.o ./build/disk/streamer.o ./build/task/process.o ./build/task/task.o ./build/task/tss.asm.o ./build/fs/pparser.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/string/string.o
INCLUDES = -I./base/txos
FLAGS = -v -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-unused-variable -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc -B/usr/local/bin/i686-elf-
all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	# Copy files to the OS image
	sudo mkdir /mnt/z
	sudo mount -t vfat ./bin/os.bin /mnt/z
	sudo cp ./boot.ini /mnt/z
	sudo cp ./font.psf /mnt/z
	sudo cp ./open.exe /mnt/z
	sudo cp ./hello.exe /mnt/z
	sudo cp ./stop.exe /mnt/z
	sudo cp ./freever.exe /mnt/z
	sudo cp ./reboot.exe /mnt/z
	sudo cp ./test.bat /mnt/z
	sudo cp ./bsod.exe /mnt/z
	sudo cp ./blorp.exe /mnt/z
	sudo cp ./lsbin.exe /mnt/z
	sudo umount /mnt/z
	sudo rm -rf /mnt/z
./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./base/txos/init/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./base/txos/boot/fat/x86fboot.asm
	mkdir -p ./bin
	nasm -f bin ./base/txos/boot/fat/x86fboot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./base/txos/init/kernel.asm
	mkdir -p ./build
	nasm -f elf -g ./base/txos/init/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./base/txos/init/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./base/txos/init/kernel.c -o ./build/kernel.o

./build/loader.o: ./base/txos/init/loader.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./base/txos/init/loader.c -o ./build/loader.o

./build/user.asm.o: ./base/txos/init/user.asm
	nasm -f elf -g ./base/txos/init/user.asm -o ./build/user.asm.o

./build/graphics.o: ./base/txos/ke/graphics/graphics.h
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./base/txos/ke/graphics/graphics.c -o ./build/graphics.o

./build/bug.o: ./base/txos/ke/bug.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./base/txos/ke/bug.c -o ./build/bug.o

./build/idt/idt.asm.o: ./base/txos/ke/idt/idt.asm
	mkdir -p ./build/idt
	nasm -f elf -g ./base/txos/ke/idt/idt.asm -o ./build/idt/idt.asm.o

./build/gdt/gdt.o: ./base/txos/ke/gdt/gdt.c
	mkdir -p ./build/gdt
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/gdt $(FLAGS) -std=gnu99 -c ./base/txos/ke/gdt/gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./base/txos/ke/gdt/gdt.asm
	nasm -f elf -g ./base/txos/ke/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/idt/idt.o: ./base/txos/ke/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/idt $(FLAGS) -std=gnu99 -c ./base/txos/ke/idt/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./base/txos/ke/memory/memory.c
	mkdir -p ./build/memory
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/memory.c -o ./build/memory/memory.o

./build/task/process.o: ./base/txos/ke/task/process.c
	mkdir -p -p ./build/task
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/task $(FLAGS) -std=gnu99 -c ./base/txos/ke/task/process.c -o ./build/task/process.o

./build/task/task.o: ./base/txos/ke/task/task.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/task $(FLAGS) -std=gnu99 -c ./base/txos/ke/task/task.c -o ./build/task/task.o

./build/task/tss.asm.o: ./base/txos/ke/task/tss.asm
	nasm -f elf -g ./base/txos/ke/task/tss.asm -o ./build/task/tss.asm.o

./build/io/io.asm.o: ./base/txos/ke/io/io.asm
	mkdir -p ./build/io
	nasm -f elf -g ./base/txos/ke/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./base/txos/ke/memory/heap/heap.c
	mkdir -p ./build/memory/heap
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory/heap $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./base/txos/ke/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory/heap $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

./build/memory/paging/paging.o: ./base/txos/ke/memory/paging/paging.c
	mkdir -p ./build/memory/paging
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory/paging $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./base/txos/ke/memory/paging/paging.asm
	nasm -f elf -g ./base/txos/ke/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./base/txos/ke/disk/disk.c
	mkdir -p ./build/disk
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/disk $(FLAGS) -std=gnu99 -c ./base/txos/ke/disk/disk.c -o ./build/disk/disk.o

./build/disk/streamer.o:./base/txos/ke/disk/streamer.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/disk $(FLAGS) -std=gnu99 -c ./base/txos/ke/disk/streamer.c -o ./build/disk/streamer.o

./build/fs/fat/fat16.o: ./base/txos/ke/fs/fat/fat16.c
	mkdir -p ./build/fs/fat
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/fs -I./src/fat $(FLAGS) -std=gnu99 -c ./base/txos/ke/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/fs/file.o: ./base/txos/ke/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/fs $(FLAGS) -std=gnu99 -c ./base/txos/ke/fs/file.c -o ./build/fs/file.o

./build/fs/pparser.o: ./base/txos/ke/fs/pparser.c
	mkdir -p ./build/fs
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/fs $(FLAGS) -std=gnu99 -c ./base/txos/ke/fs/pparser.c -o ./build/fs/pparser.o

./build/string/string.o: ./base/txos/ke/string/string.c
	mkdir -p ./build/string
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/string $(FLAGS) -std=gnu99 -c ./base/txos/ke/string/string.c -o ./build/string/string.o

clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ./build
	rm -rf *.exe
	rm -rf *.dll
