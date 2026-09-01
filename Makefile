init_build_env:
	mkdir -p work

.ONESHELL:
build_asm:
	cd work
# build bootloader
	nasm -f bin -o bootloader.raw ../src/bootloader.asm
# write 512 bytes of zeros into iso file
	dd if=/dev/zero of=boot.iso bs=512 count=1
# copy bootloader into iso
	dd if=bootloader.raw of=boot.iso seek=0 count=1 conv=notrunc
# copy in boot signature
	echo -n -e "\x55\xAA" | dd of=boot.iso obs=1 seek=510 count=1 conv=notrunc

build: init_build_env build_asm
	#cd work && \
	#gcc ../src/main.c

run: build
	work/a.out

test: build
	qemu-system-x86_64 -m 128 -drive format=raw,file=work/boot.iso -d int -no-reboot -no-shutdown

all: run