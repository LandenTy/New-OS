#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#define SECTOR_SIZE 512

typedef struct {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t num_fats;
    uint32_t fat_size_32;
    uint32_t root_cluster;
} bpb_t;

typedef struct dir_entry {
    char name[11];
    uint8_t attr;
    uint8_t nt_reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} dir_entry_t;

// Global BPB instance (defined in fat32.c)
extern bpb_t bpb;

int fat32_init(void);
int fat32_read_file(const char *filename, uint8_t *buffer, uint32_t buffer_size);
void fat32_list_root(void);
void fat32_read_cluster(uint32_t cluster, uint8_t *buffer);
uint32_t fat32_next_cluster(uint32_t current_cluster);

#endif

