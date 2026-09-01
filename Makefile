init_build_env:
	rm -r work
	mkdir -p work

.ONESHELL:
build_c:
	cd work
	i686-elf-gcc -m32 -ffreestanding -c ../src/kernel.c -o kernel.o
	strip --strip-all kernel.o
	i686-elf-ld -melf_i386 -o kernel.bin -Ttext 0x1000 --oformat binary kernel.o

.ONESHELL:
build_asm: build_c
	cd work
# build bootloader
	nasm -f bin -o bootloader_unbootable.bin ../src/bootloader.asm
# set up bootloader.bin to be bootable
	dd if=/dev/zero of=bootloader.bin bs=1 count=510 conv=notrunc
	echo -e -n "\x55\xAA" | dd of=bootloader.bin oflag=append conv=notrunc
# copy in bootloader code
	dd if=bootloader_unbootable.bin of=bootloader.bin conv=notrunc
# concatenate C executable
	cat bootloader.bin kernel.bin > boot.iso
# add on 128 sectors worth of zeroes so I don't have to worry about sector counts
	dd if=/dev/zero of=boot.iso bs=1 count=65536 oflag=append conv=notrunc

build: init_build_env build_asm
	#cd work && \
	#gcc ../src/main.c

run: build
	work/a.out

test: build
	qemu-system-x86_64 -m 128 -drive format=raw,file=work/boot.iso -d int -no-reboot -no-shutdown

debug: build
	qemu-system-x86_64 -s -S -m 128 -drive format=raw,file=work/boot.iso -d int -no-reboot -no-shutdown

all: run