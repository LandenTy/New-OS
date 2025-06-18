#include "fat32.h"
#include "../libc/ata.h"
#include "../libc/stdio.h"
#include <stdint.h>
#include <string.h> // for memcmp

bpb_t bpb;  // global BPB struct

static uint32_t fat_start_lba;
static uint32_t cluster_start_lba;
static uint8_t sector[SECTOR_SIZE];
static uint8_t cluster_buf[SECTOR_SIZE * 8]; // adjust if needed

// Convert cluster number to LBA
static uint32_t cluster_to_lba(uint32_t cluster) {
    return cluster_start_lba + (cluster - 2) * bpb.sectors_per_cluster;
}

// Read a cluster (all sectors in cluster)
void fat32_read_cluster(uint32_t cluster, uint8_t *buffer) {
    uint32_t first_sector = cluster_to_lba(cluster);
    for (uint32_t i = 0; i < bpb.sectors_per_cluster; i++) {
        ata_read_sector(first_sector + i, buffer + i * SECTOR_SIZE);
    }
}

// Read next cluster number from FAT
uint32_t fat32_next_cluster(uint32_t current_cluster) {
    uint32_t fat_offset = current_cluster * 4;
    uint32_t fat_sector = fat_start_lba + (fat_offset / SECTOR_SIZE);
    uint32_t entry_offset = fat_offset % SECTOR_SIZE;

    ata_read_sector(fat_sector, sector);

    uint32_t next_cluster = *(uint32_t *)(sector + entry_offset);
    return next_cluster & 0x0FFFFFFF;  // mask upper 4 bits as per FAT32 spec
}

// Initialize FAT32 by reading boot sector
int fat32_init(void) {
    ata_read_sector(0, sector);

    if (sector[0] != 0xEB && sector[0] != 0xE9) {
        printf("Invalid boot sector signature\n");
        return 0;
    }

    bpb.bytes_per_sector = sector[11] | (sector[12] << 8);
    if (bpb.bytes_per_sector != 512) {
        printf("Unsupported bytes per sector: %d\n", bpb.bytes_per_sector);
        return 0;
    }

    bpb.sectors_per_cluster = sector[13];
    bpb.reserved_sector_count = sector[14] | (sector[15] << 8);
    bpb.num_fats = sector[16];
    bpb.fat_size_32 = sector[36] | (sector[37] << 8) | (sector[38] << 16) | (sector[39] << 24);
    bpb.root_cluster = sector[44] | (sector[45] << 8) | (sector[46] << 16) | (sector[47] << 24);

    fat_start_lba = bpb.reserved_sector_count;
    cluster_start_lba = fat_start_lba + bpb.num_fats * bpb.fat_size_32;

    printf("FAT32 init: sectors_per_cluster=%d, reserved=%d, fats=%d, sectors_per_fat=%u, root_cluster=%u\n",
           bpb.sectors_per_cluster, bpb.reserved_sector_count, bpb.num_fats, bpb.fat_size_32, bpb.root_cluster);

    return 1;
}

// Read a file from root directory (simple, only root, no subdirs)
int fat32_read_file(const char *filename, uint8_t *buffer, uint32_t buffer_size) {
    uint32_t cluster = bpb.root_cluster;

    while (cluster < 0x0FFFFFF8) {
        fat32_read_cluster(cluster, cluster_buf);

        for (int offset = 0; offset < bpb.sectors_per_cluster * SECTOR_SIZE; offset += 32) {
            dir_entry_t *entry = (dir_entry_t *)(cluster_buf + offset);

            if (entry->name[0] == 0x00) {
                return 0; // No more entries
            }
            if (entry->name[0] == 0xE5) {
                continue; // Deleted entry
            }
            if (entry->attr & 0x08) {
                continue; // Volume label
            }
            if (memcmp(entry->name, filename, 11) == 0) {
                uint32_t file_cluster = ((uint32_t)entry->first_cluster_high << 16) | entry->first_cluster_low;
                uint32_t file_size = entry->file_size;

                printf("Found file: %.11s cluster=%u size=%u\n", filename, file_cluster, file_size);

                uint32_t bytes_read = 0;
                uint32_t curr_cluster = file_cluster;

                while (curr_cluster < 0x0FFFFFF8 && curr_cluster != 0) {
                    for (uint32_t s = 0; s < bpb.sectors_per_cluster; s++) {
                        if (bytes_read >= buffer_size) return 1; // Prevent buffer overflow

                        ata_read_sector(cluster_to_lba(curr_cluster) + s, buffer + bytes_read);
                        bytes_read += SECTOR_SIZE;

                        if (bytes_read >= file_size) {
                            return 1; // Done reading
                        }
                    }

                    curr_cluster = fat32_next_cluster(curr_cluster);
                }
                return 1;
            }
        }

        cluster = fat32_next_cluster(cluster);
    }

    return 0; // Not found
}

// List root directory files
void fat32_list_root(void) {
    uint32_t cluster = bpb.root_cluster;

    do {
        fat32_read_cluster(cluster, cluster_buf);

        for (int i = 0; i < bpb.sectors_per_cluster * SECTOR_SIZE; i += 32) {
            dir_entry_t *entry = (dir_entry_t *)(cluster_buf + i);

            if (entry->name[0] == 0x00) return;  // no more entries
            if (entry->name[0] == 0xE5) continue; // deleted entry
            if ((entry->attr & 0x0F) == 0x0F) continue; // skip long file names

            // Print 8.3 filename
            for (int j = 0; j < 8; j++) {
                if (entry->name[j] == ' ') break;
                putchar(entry->name[j]);
            }
            putchar('.');
            for (int j = 8; j < 11; j++) {
                if (entry->name[j] == ' ') break;
                putchar(entry->name[j]);
            }
            putchar('\n');
        }

        cluster = fat32_next_cluster(cluster);
    } while (cluster < 0x0FFFFFF8);
}

