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

#ifndef _TOC_H
#define _TOC_H

#include <stdint.h>
#include <mmc_if.h>

#define TOC_BLK_SIZE	512
#define TOC_MAGIC		"ISSW"
#define TOC_ID_NO_ENTRY	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define TOC_MAIN_ENTRIES	(TOC_BLK_SIZE/sizeof(struct toc_entry))
#define TOC_ENTRY_SIZE	32
#define TOC_ENTRIES_PER_BLOCK	(TOC_BLK_SIZE/TOC_ENTRY_SIZE)
#define MAX_NUM_TOCS		4
#define TOC_BOOT_IMAGE_SIZE	0x20000
#define NO_TOC			-1
#define TOC_ID_LENGTH		12
#define SUBTOC_START_IDX	TOC_MAIN_ENTRIES

/* Sub TOC identifier in the 'flags' field is 'ST' */
#define FLAGS_SUB_TOC		0x00005354

/* Partition Entry indentifier in the 'flags' field is 'PE' */
#define FLAGS_PARTITION		0x00005045

/* The offset is in LBA (512 B) rather than bytes, 'LB' */
#define FLAGS_LBA			0x00004C42


/* copy from boot ROM into eSRAM */
#define TOC_COPY_ADDRESS	0x40022000

struct mmc;

struct toc_entry {
	uint32_t offset;
	uint32_t size;
	uint32_t flags;
	uint32_t align;
	uint32_t loadaddr;
	uint8_t  id[TOC_ID_LENGTH];
};

struct toc_entry *toc_addr_get(void);
uint32_t toc_n_entries_get(void);
struct toc_entry *toc_find_first(void);
struct toc_entry *toc_find_next(void);
struct toc_entry *toc_load_id(const char *id, uint32_t loadaddr);
int toc_save_id(const char *id, uint32_t data, uint32_t length);
void toc_print(struct toc_entry *toc);
struct toc_entry *toc_find_id(struct toc_entry *toc, const char *id);
void toc_recalc_entry_offsets(struct toc_entry *toc, long reloff);
int is_toc(struct toc_entry *toc);
void toc_update(int location, char *toc_id, uint32_t new_size);

int toc_load_part(const char *id,
                  uint32_t dest,
                  uint32_t offset,
                  uint32_t size);

int toc_init_mmc(mmc_properties_t *mmc);
mmc_properties_t *toc_device_properties_get(void);

#endif /* _TOC_H */
