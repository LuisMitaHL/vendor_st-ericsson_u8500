/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _PARTITION_PARSER_H
#define _PARTITION_PARSER_H

#include <stdint.h>
#include "mmc_if.h"


/* Sparse format constants */
#define SPARSE_MAGIC                (uint32_t)0xed26ff3a
#define SPARSE_CHUNK_TYPE_RAW       0xCAC1
#define SPARSE_CHUNK_TYPE_DC        0xCAC3
#define SPARSE_CHUNK_TYPE_FILL      0xCAC2
#define SPARSE_CHUNK_TYPE_CRC       0xCAC4


/* Sparse format header */
typedef struct sparse_hdr_st {
    uint32_t    magic;          /* 32 bit magic: 0xed26ff3a */
    uint16_t    major;          /* 16 bit major version (0x1) - reject images with higher major versions */
    uint16_t    minor;          /* 16 bit minor version (0x0) - allow images with higher minor versions */
    uint16_t    ff1;            /* 16 bit 28 bytes for first revision of the file format */
    uint16_t    ff2;            /* 16 bit 12 bytes for first revision of the file format */
    uint32_t    block_size;     /* 32 bit block size in bytes must be a multiple of 4 (4096) */
    uint32_t    tot_blocks;     /* 32 bit total blocks in the non-sparse output image */
    uint32_t    tot_chunks;     /* 32 bit total chunks in the sparse input image */
    uint32_t    crc32;          /* 32 bit image checksum CRC32 checksum of the original data, counting "don't care" chunks */
} sparse_hdr_t;


/* Sparse format chunk header */
typedef struct sparse_chunk_hdr_st {
    uint16_t    chunk_type;     /* 16 bit chunk type: */
    uint16_t    reserved;       /* 16 bits reserved (write as 0, ignore on read) */
    uint32_t    chunk_blocks;   /* 32 bit chunk size in blocks in output image */
    uint32_t    tot_bytes;      /* 32 bit total size in bytes */
} sparse_chunk_hdr_t;


/*
 * common partition table to hold TOC, MBR and GPT information
 */

#define PARTITIONS_MAX        32
#define PARTITIONS_NAMELEN    72    /* GPT: 36 UTF-16LE entries */
#define FB_PARTITION_LEN      12

/*
 * Unified toc, mbr and gpt entry types.
 * Since the toc entries are in bytes we must use byte offsets.
 */
struct partition_entry
{
    unsigned int type;              /* (mbr) partition type */
    unsigned long long offset;      /* in bytes */
    unsigned long long size;        /* in bytes */
    unsigned char name[72];         /* 36 UTF-16LE entries */
#if 0
    /* GPT support it currently not implemented and these entries are unused */
    unsigned char type_guid[16];    /* gpt partition type guid */
    unsigned char unique_partition_guid[16];    /* gpt uid */
    unsigned long long attributes;
#endif
};

typedef struct {
    uint32_t offset;
    uint32_t size;
    uint32_t toc_size;
    uint32_t flags;
    char name[FB_PARTITION_LEN];
} fb_partition_entry_t;

/* bit flag values for the FB partitions */
#define FB_PART_FLAGS_RESET_VALUE    0
#define FB_PART_LOCATION_SUBTOC      0x00000001
#define FB_PART_BOOT                 0x00000002
#define FB_PART_FILESYS              0x00000004

int partition_read_table(mmc_properties_t *mmc);
int parse_blkdev_parts(const char *cmdline, const char *blkdev_name);
int partition_parse_cmdline(const char *);
fb_partition_entry_t *partition_entry_find(const char *);
fb_partition_entry_t *partition_entry_find_first(void);
fb_partition_entry_t *partition_entry_find_next(void);
int partition_entry_erase(fb_partition_entry_t *pentry);
int partition_entry_write(fb_partition_entry_t *pentry, void *source, unsigned len);
void fb_partitions_init(void);
unsigned partition_find_biggest(unsigned max_limit);
uint32_t get_partition_size (char *name);

/* MBR see http://en.wikipedia.org/wiki/Master_boot_record */
#define MBR_PART_TBL_OFFS    0x1be
#define MBR_TYPE_PROTECTED    0xee
struct mbr_part {
    uint8_t status;
    uint8_t start_head;
    uint8_t start_sector;    /* sector in 5-0; bits 9-8 of cyl in bits 7-6 */
    uint8_t start_cyl;
    uint8_t part_type;
    uint8_t end_head;
    uint8_t end_sector;
    uint8_t end_cyl;
    uint8_t lba_start[4];    /* starting sector counting from 0, LE format */
    uint8_t lba_length[4];   /* number of sectors in partition, LE format */
};

#endif /* _PARTITION_PARSER_H */
