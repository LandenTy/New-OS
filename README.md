# New-OS

This is a hobbyist operating system written in x86 Assembly. It builds a bootable ISO image using `nasm` and `grub`, and runs in `QEMU`.\
Tested on **Linux Mint**, but works on any **Ubuntu-based system**.

---

## 🛠 Dependencies

Install the following packages:

```bash
sudo apt update
sudo apt install build-essential nasm grub-pc-bin xorriso qemu-system
```

---

## 📦 Build Instructions

To build the OS:

```bash
make
```

This will assemble your bootloader and kernel and generate a bootable ISO named `os.iso`.

---

## Running the OS

To launch the OS in QEMU:

```bash
make run
```

This will boot the ISO in a virtual machine using `qemu-system-i386`.

---

## Preview

Here's a preview of the OS running in QEMU:

![OS Preview](images/OS.png)

Loading the HDD (Fat32 Partitioned):

![OS Preview](images/FAT32_START.png)

---

## Notes

- Uses **GRUB multiboot** for ISO creation
- Targets **BIOS** (not UEFI)
- Kernel is optional in early stages; bootloader alone is fine
- I've started working on implementing a FAT32 Hard Disk Support
- Will begin working on Optical Media soon
