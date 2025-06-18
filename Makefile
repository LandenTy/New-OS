CFLAGS = -m32 -ffreestanding -Wall -Wextra
OBJS = boot.o kernel.o libc/stdio.o libc/ata.o libc/string.o fs/fat32.o

all: myos.iso

boot.o: boot.asm
	nasm -f elf32 boot.asm -o boot.o

kernel.o: kernel.c
	gcc $(CFLAGS) -c kernel.c -o kernel.o

libc/stdio.o: libc/stdio.c
	gcc $(CFLAGS) -c libc/stdio.c -o libc/stdio.o

libc/string.o: libc/string.c
	gcc $(CFLAGS) -c libc/string.c -o libc/string.o

libc/ata.o: libc/ata.c libc/ata.h
	gcc $(CFLAGS) -c libc/ata.c -o libc/ata.o

fs/fat32.o: fs/fat32.c fs/fat32.h
	gcc $(CFLAGS) -c fs/fat32.c -o fs/fat32.o

kernel: $(OBJS) linker.ld
	ld -m elf_i386 -T linker.ld -o kernel $(OBJS)

iso/boot/kernel: kernel
	mkdir -p iso/boot
	cp kernel iso/boot/kernel

myos.iso: iso/boot/kernel iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

clean:
	rm -f *.o libc/*.o fs/*.o kernel myos.iso

run: myos.iso
	qemu-system-i386 -cdrom myos.iso -hda disk.img -boot d

