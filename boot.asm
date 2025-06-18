; boot.asm
section .multiboot
align 4
    dd 0x1BADB002         ; magic
    dd 0x0                ; flags
    dd -0x1BADB002        ; checksum

section .text
global _start
extern kmain               ; <-- tell NASM the symbol is in another file

_start:
    cli
    mov esp, 0x90000       ; set up stack
    call kmain             ; call kernel
    hlt
