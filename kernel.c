#include "libc/stdio.h"
#include "libc/ata.h"

void kmain(void) {
    clear_screen();
    printf("Reading sector 0 from disk...\n\n");

    uint8_t buffer[512];
    ata_read_sector(0, buffer);

    printf("First bytes: ");
    for (int i = 0; i < 20; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n\n");

    printf("First 20 bytes (hex):\n");
    for (int i = 0; i < 20; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
}

