#include "ata.h"

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void insw(uint16_t port, void* addr, uint32_t count) {
    asm volatile ("rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

#define ATA_DATA       0x1F0
#define ATA_ERROR      0x1F1
#define ATA_SECCOUNT0  0x1F2
#define ATA_LBA0       0x1F3
#define ATA_LBA1       0x1F4
#define ATA_LBA2       0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7

void ata_wait() {
    while (inb(ATA_STATUS) & 0x80); // Wait for BSY to clear
    while (!(inb(ATA_STATUS) & 0x08)); // Wait for DRQ to set
    int timeout = 1000000;
    while ((inb(ATA_STATUS) & 0x80) && --timeout); // Wait for BSY to clear
    timeout = 1000000;
    while (!(inb(ATA_STATUS) & 0x08) && --timeout); // Wait for DRQ to set

    if (timeout == 0) {
        // Optional: signal error or print message here
    }
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F)); // 0xE0 = master, LBA mode
    outb(ATA_SECCOUNT0, 1); // Read 1 sector
    outb(ATA_LBA0, lba & 0xFF);
    outb(ATA_LBA1, (lba >> 8) & 0xFF);
    outb(ATA_LBA2, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, 0x20); // Read with retry

    ata_wait();
    insw(ATA_DATA, buffer, 256); // 256 words = 512 bytes
}

