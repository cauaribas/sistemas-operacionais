#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>
#include <stdio.h>

#define DIR_FREE_ENTRY 0xE5

#define DIR_ATTR_READONLY 1 << 0 /* file is read only */
#define DIR_ATTR_HIDDEN 1 << 1 /* file is hidden */
#define DIR_ATTR_SYSTEM 1 << 2 /* system file (also hidden) */
#define DIR_ATTR_VOLUMEID 1 << 3 /* special entry containing disk volume lable */
#define DIR_ATTR_DIRECTORY 1 << 4 /* describes a subdirectory */
#define DIR_ATTR_ARCHIVE 1 << 5 /*  archive flag (always set when file is modified */
#define DIR_ATTR_LFN 0xf /* not used */

#define SIG 0xAA55 /* boot sector signature -- sector is executable */

#pragma pack(push, 1)
struct fat_dir {
	unsigned char name[11]; /* Short name + file extension */
	uint8_t attr; /* file attributes */
	uint8_t ntres; /* reserved for Windows NT, Set value to 0 when a file is created. */
	uint8_t creation_stamp; /* milisecond timestamp at file creation time */
	uint16_t creation_time; /* time file was created */
	uint16_t ctreation_date; /* date file was created */
	uint16_t last_access_date; /* last access date (last read/written) */
	uint16_t reserved_fat32; /* reserved for fat32 */
	uint16_t last_write_time; /* time of last write */
	uint16_t last_write_date; /* date of last write */
	uint16_t starting_cluster; /* starting cluster */
	uint32_t file_size; /* 32-bit */
};

/* Boot Sector and BPB
 * Located at the first sector of the volume in the reserved region.
 * AKA as the boot sector, reserved sector or even the "0th" sector.
 */
struct fat_bpb {
    uint8_t jmp_instruction[3];      /* Jump instruction to the bootstrap code */
    unsigned char oem_id[8];         /* OEM ID: name of the operating system that formatted the volume */

    uint16_t bytes_p_sect;           /* bytes per sector */
    uint8_t sector_p_clust;          /* sectors per cluster */
    uint16_t reserved_sect;          /* reserved sectors at the beginning of the volume */
    uint8_t n_fat;                   /* number of copies of the File Allocation Table (FAT) */
    uint16_t root_entry_count;       /* number of entries in the root directory (zero in FAT32) */
    uint16_t total_sectors_16;       /* total number of sectors (if zero, use total_sectors_32) */
    uint8_t media_desc;              /* media descriptor */
    uint16_t sect_per_fat_16;        /* sectors per FAT (if zero, use sect_per_fat_32) */

    uint16_t sect_per_track;         /* sectors per track */
    uint16_t number_of_heads;        /* number of heads */
    uint32_t hidden_sects;           /* hidden sectors */
    uint32_t total_sectors_32;       /* total number of sectors (used if total_sectors_16 is zero) */
    uint32_t sect_per_fat_32;        /* sectors per FAT in FAT32 */

    uint16_t ext_flags;              /* extended flags */
    uint16_t fs_version;             /* file system version */
    uint32_t root_cluster;           /* starting cluster of the root directory in FAT32 */
    uint16_t fs_info;                /* file system information sector */
    uint16_t backup_boot_sector;     /* backup boot sector */

    uint8_t reserved[12];            /* reserved for future use */
    uint8_t drive_number;            /* drive number */
    uint8_t reserved1;               /* reserved (must be zero) */
    uint8_t boot_signature;          /* boot signature (0x29 if the next three fields are valid) */
    uint32_t volume_id;              /* volume ID (serial number) */
    unsigned char volume_label[11];  /* volume label (name of the volume) */
    unsigned char fs_type[8];        /* file system type (e.g., "FAT32") */
};
/*
 * NOTE - Modificação
 * Motivo: IDE avisou de pragma não terminado
 * Diff: ø → #pragma pack(pop, 1)
 */
#pragma pack(pop)

int read_bytes(FILE *, unsigned int, void *, unsigned int);
void rfat(FILE *, struct fat_bpb *);

/* prototypes for calculating fat stuff */
uint32_t bpb_faddress(struct fat_bpb *);
uint32_t bpb_froot_addr(struct fat_bpb *);
uint32_t bpb_fdata_addr(struct fat_bpb *);
uint32_t bpb_fdata_sector_count(struct fat_bpb *);
uint32_t bpb_fdata_cluster_count(struct fat_bpb* bpb);

///

#define FAT16STR_SIZE       11
#define FAT16STR_SIZE_WNULL 12

#define RB_ERROR -1
#define RB_OK     0

#define FAT16_EOF_LO 0xfff8
#define FAT16_EOF_HI 0xffff

#endif
