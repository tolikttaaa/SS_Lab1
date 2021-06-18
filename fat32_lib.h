//
// Created by ttaaa on 4/22/21.
//

#ifndef SYSTEM_SOFTWARE_FAT32_LIB_H
#define SYSTEM_SOFTWARE_FAT32_LIB_H

struct fat_BS {
    unsigned char boot_jmp[3];
    unsigned char oem_name[8];

    // BPB
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors_count;
    unsigned char table_count;               // always 2
    unsigned short root_entry_count;         // for FAT32 is 0
    unsigned short small_sectors_count;      // for FAT32 is 0
    unsigned char media_descriptor;
    unsigned short sectors_per_fat_small;    // for FAT32 is 0
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sector_count;
    unsigned int large_sectors_count;
    unsigned int sectors_per_fat_large;
    unsigned short extended_flags;
    unsigned short fs_version;
    unsigned int root_cluster_number;
    unsigned short fs_info_sector_number;
    unsigned short backup_boot_sector;
    unsigned char reserved_0[12];

    // BPB Extended
    unsigned char physical_drive_number;
    unsigned char reserved_1;                // for FAT32 is 0
    unsigned char extended_boot_signature;
    unsigned int volume_serial_number;
    unsigned char volume_label[11];
    unsigned char system_id[8];
}__attribute__((packed));

struct fs_info {
    unsigned int lead_signature;
    unsigned char reserved_0[480];
    unsigned int signature;
    unsigned int last_free_cluster;
    unsigned int available;
    unsigned char reserved_1[12];
    unsigned int tail_signature;
}__attribute__((packed));

struct dir_value {
    unsigned char *filename;
    unsigned char type;
    unsigned int first_cluster;
    unsigned int size;
    void *next;
};

struct dir_entry {
    unsigned char file_name[8];
    unsigned char extension[3];
    unsigned char attributes;
    unsigned char reserved;
    unsigned char crt_time_ms;
    unsigned short crt_time;
    unsigned short crt_date;
    unsigned short last_date_access;
    unsigned short high_cluster_num;
    unsigned short last_mod_time;
    unsigned short last_mod_date;
    unsigned short low_cluster_num;
    unsigned int file_size;
}__attribute__((packed));

struct long_filename {
    unsigned char id;
    unsigned char name0_4[10];
    unsigned char attribute;
    unsigned char long_entry_type;
    unsigned char checksum;
    unsigned char name5_10[12];
    unsigned short always_zero;
    unsigned char name11_12[4];
}__attribute__((packed));

struct partition_value {
    int device_fd;
    unsigned int cluster_size;
    unsigned int first_data_sector;
    unsigned int active_cluster;
    struct fat_BS *fat_boot;
    struct fs_info *fs_info;
};

unsigned int read_file_cluster(struct partition_value *part, unsigned int cluster, char *buf);

struct dir_value *init_dir_value(struct dir_entry *entry, unsigned char *filename);
void destroy_dir_value(struct dir_value *dir_val);
struct dir_value *read_dir(unsigned int first_cluster, struct partition_value *value);
int change_dir(struct partition_value *value, const unsigned char *dir_name);

struct partition_value *open_partition(const char *partition);
void close_partition(struct partition_value *part);

#endif //SYSTEM_SOFTWARE_FAT32_LIB_H
