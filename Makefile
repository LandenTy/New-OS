CFLAGS = -m32 -ffreestanding -Wall -Wextra

OBJS = boot.o kernel.o libc/stdio.o libc/string.o libc/ata.o

all: myos.iso

boot.o: boot.asm
	nasm -f elf32 boot.asm -o boot.o

kernel.o: kernel.c
	gcc $(CFLAGS) -c kernel.c -o kernel.o

libc/stdio.o: libc/stdio.c
	gcc $(CFLAGS) -c libc/stdio.c -o libc/stdio.o

libc/string.o: libc/string.c
	gcc $(CFLAGS) -c libc/string.c -o libc/string.o

kernel: $(OBJS) linker.ld
	ld -m elf_i386 -T linker.ld -o kernel $(OBJS)

iso/boot/kernel: kernel
	mkdir -p iso/boot
	cp kernel iso/boot/kernel

myos.iso: iso/boot/kernel iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

clean:
	rm -f *.o libc/*.o kernel myos.iso
	rm -rf iso/boot/kernel

run: myos.iso
	qemu-system-i386 -cdrom myos.iso -hda disk.img -boot d

