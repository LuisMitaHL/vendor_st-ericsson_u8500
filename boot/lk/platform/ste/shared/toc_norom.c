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

/*
 * TOC routines using the MMC driver.
 */

#include <debug.h>

#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include <kernel/thread.h>
#include <reg.h>

#include <mmc_if.h>
#include <toc.h>
#include <platform/interrupts.h>
#include <platform/irqs.h>
//#include <mmc.h>

/*
 * Reserve space for one subtoc with TOC_MAIN_ENTRIES as well. subtoc has same
 * format as TOC.
 */
static struct toc_entry *toc_array = NULL;
static uint32_t n_toc_entries = 0;
static mmc_properties_t *toc_mmc = NULL;
static unsigned toc_find_idx;
static unsigned toc_map[MAX_NUM_TOCS];
static struct toc_entry toc_copyspace[TOC_MAIN_ENTRIES];
static uint32_t subtoc_offset_bl;

static int toc_load_entry_mmc(struct toc_entry *toc_entry, uint32_t loadaddr)
{
	unsigned long blkcnt;
	uint32_t offset_bl;
	int status;

	if (loadaddr == (uint32_t)~0)
		loadaddr = toc_entry->loadaddr;

	dprintf(INFO, "EMMC toc load: size %d, loadaddr 0x%08x\n",
			toc_entry->size, loadaddr);
	if ((toc_entry->flags & 0x0000ffff) == FLAGS_LBA) {
		blkcnt = toc_entry->size;
	} else {
		blkcnt = (toc_entry->size + 511) / 512;
	}

	if ((toc_entry->flags & 0x0000ffff) == FLAGS_LBA) {
		offset_bl = toc_entry->offset;
	} else {
		offset_bl = toc_entry->offset / 512;
	}
	status = mmc_read (toc_mmc, offset_bl, blkcnt, (uint32_t*)loadaddr);
	if (status != MMC_OK) {
		dprintf(INFO, "toc_load_entry_mmc: mmc_bread failed: rc %d\n",
				status);
		return -1;
	}

	dprintf(INFO, "toc_load_entry_mmc: loaded %ld blks\n", blkcnt);
	return 0;
}

static int toc_save_entry_mmc(struct toc_entry *toc_entry, uint32_t data, uint32_t length)
{
	unsigned long blkcnt;
	int status;
	uint32_t offset_bl;
	uint32_t size_b;

	size_b = toc_entry->size;
	if ((toc_entry->flags & 0x0000ffff) == FLAGS_LBA)
		size_b *= 512;

	if ((length == (uint32_t)~0) || (length > size_b))
		length = size_b;

	dprintf(INFO, "EMMC toc save: size %d, data 0x%08x\n",
			size_b, data);
	blkcnt = (length + 511) / 512;

	/* XXX: xloader is not block aligned! */
	if ((toc_entry->flags & 0x0000ffff) == FLAGS_LBA) {
		offset_bl = toc_entry->offset;
	} else {
		offset_bl = toc_entry->offset / 512;
	}
	status = mmc_write (toc_mmc, offset_bl, blkcnt, (uint32_t*)data);
	if (status != MMC_OK) {
		dprintf(INFO, "toc_save_entry_mmc: mmc_write failed: rc %d\n",
				status);
		return -1;
	}

	dprintf(INFO, "toc_save_entry_mmc: saved %ld blks\n", blkcnt);
	return 0;
}


/*
 * Determine N SUBTOC entries, rounded up to nearest 512 byte boundary
 */
static int toc_get_n_subtoc_entries (struct toc_entry *toc)
{
	unsigned int i;
	int entries = 0;

	/* Find sub TOC entry */
	for (i = 0; i < TOC_MAIN_ENTRIES; i++, toc++) {
		/*
		 * Check the flags for a sub TOC
		 * NB: some designer didn't know the difference between
		 * a flag and magic value :-(
		 */
		if ((toc->flags & 0x0000FFFF) == FLAGS_SUB_TOC) {

			/* Determine number of blocks used by SUBTOC */
			entries = toc->size / TOC_BLK_SIZE;
			entries += (toc->size % TOC_BLK_SIZE) == 0 ? 0 : 1;
			entries = (entries * TOC_BLK_SIZE) / sizeof(struct toc_entry);
			dprintf (INFO, "SUBTOC contains %d entries\n", entries);

			/* Setup global pointer */
			subtoc_offset_bl = toc->offset / 512;
			break;
		}

		if (memcmp(toc->id, TOC_ID_NO_ENTRY, TOC_ID_LENGTH) == 0) {
			/*
			 * Don't iterate the rest of the entries
			 * if an empty entry was found
			 */
			break;
		}
	}

	return entries;
}


/*
 * find subtoc offset in main TOC and load subtoc
 */
static int toc_read_subtoc_mmc(mmc_properties_t *mmc, int n_blocks,
		struct toc_entry *subtoc)
{
	int status;

	dprintf (INFO, "reading %d blocks to subtoc\n", n_blocks);
	status = mmc_read (mmc, subtoc_offset_bl, n_blocks, (uint32_t*)subtoc);
	if (MMC_OK != status) {
		dprintf(INFO, "toc_read_subtoc_mmc: mmc_bread failed: "
				"rc %d\n", status);
		return -1;
	}

	return 0;
}

static int toc_read_mmc(mmc_properties_t *mmc)
{
	int rc = 0;
	unsigned long toc_blkaddr;
	unsigned int i;
	int subtoc_entries;
	int toc_num = -1;
	struct toc_entry toc_arr[TOC_MAIN_ENTRIES];
	struct toc_entry *toc = &toc_arr[0];
	int status;

	/*
	 * The first toc might be overwritten by the MBR, so go through
	 * all 4 TOCs until we found a valid one.
	 * There is now CRC, so we assume it is a valid one, if the first entry
	 * looks ok.
	 */
	for (i = 0; i < MAX_NUM_TOCS; i++) {
		toc_blkaddr = i * TOC_BOOT_IMAGE_SIZE / 512;
		status = mmc_read (mmc, toc_blkaddr, 1, (uint32_t*)toc);
		if (MMC_OK != status) {
			dprintf(CRITICAL, "toc_read_mmc: mmc_bread failed: "
					"rc %d\n", status);
			return -1;
		}
		if (is_toc(toc)) {
			if(toc_num == -1)
				toc_num = i;
			toc_map[i] = 1;
		} else
			toc_map[i] = 0;
		dprintf(INFO,"toc_read_mmc: toc_map[%d]=%d\n",i,toc_map[i]);
	}

	if (toc_num == -1) {
		dprintf(CRITICAL,"toc_read_mmc: no valid toc found\n");
		return -1;
	}

	/* re-load the first found toc */
	toc_blkaddr = toc_num * TOC_BOOT_IMAGE_SIZE / 512;
	status = mmc_read (mmc, toc_blkaddr, 1, (uint32_t*)toc);
	if (MMC_OK != status) {
		dprintf(CRITICAL, "toc_read_mmc: mmc_bread failed: "
				"rc %d\n", status);
		return -1;
	}

	/*
	 * We found a valid TOC.
	 * If toc is not at 0, i.e. not the first one, we must adjust the
	 * offsets, so toc_entry->offset can be used directly.
	 */
	if (toc_num != 0) {
		toc_recalc_entry_offsets(toc, toc_blkaddr * 512);
		dprintf(INFO, "toc_read_mmc: will use toc nummer %d\n",
				toc_num);
	}

	/* Determine SUBTOC length */
	subtoc_entries = toc_get_n_subtoc_entries (toc);

	/* Allocate space for global TOC + SUBTOC vector */
	n_toc_entries = TOC_MAIN_ENTRIES + subtoc_entries;
	dprintf (INFO, "Allocating 0x%08x for toc\n", n_toc_entries  * sizeof(struct toc_entry));
	toc_array = (struct toc_entry*)malloc (n_toc_entries * sizeof(struct toc_entry));
	if (NULL == toc_array) {
		dprintf (CRITICAL, "Failed to allocate toc vector!\n");
		return -1;
	}

	/* Copy TOC to global vector */
	memcpy (toc_array, toc, TOC_MAIN_ENTRIES * sizeof(struct toc_entry));

	/* Read SUBTOC to global vector */
	/* subtoc offsets are already absolute */
	if (subtoc_entries > 0) {
		rc = toc_read_subtoc_mmc(mmc, subtoc_entries/TOC_ENTRIES_PER_BLOCK, &toc_array[SUBTOC_START_IDX]);
	}

	return rc;
}


struct toc_entry *toc_addr_get(void)
{
	return &toc_array[0];
}

uint32_t toc_n_entries_get(void)
{
	return n_toc_entries;
}

struct toc_entry *toc_find_first(void)
{
	toc_find_idx = 1;
	return toc_addr_get();
}

struct toc_entry *toc_find_next(void)
{
	return (toc_find_idx < n_toc_entries) ?
		&toc_array[toc_find_idx++] :
		(struct toc_entry *)NULL;
}

/*
 * load TOC into memory
 */
int toc_init_mmc(mmc_properties_t *mmc)
{
	toc_mmc = mmc;
	return toc_read_mmc(mmc);
}


/*
 * Load toc entry by id. This routine is called for loading the Linux kernel
 * and other images.
 * Returns pointer to toc_entry, ~0 on error.
 */
struct toc_entry *toc_load_id(const char *id, uint32_t loadaddr)
{
	struct toc_entry *toc_entry;

	toc_entry = toc_find_id(&toc_array[0], id);
	if (toc_entry == (struct toc_entry *)~0)
		return toc_entry;

	/*
	 * XXX: determine boot i/f. If booted from USB the must use boot ROM
	 * TOC in eSRAM.
	 */

	toc_load_entry_mmc(toc_entry, loadaddr);

	return toc_entry;
}

/*
 * Save toc entry by id. This routine is called for images.
 * Returns 0, -1 on error.
 */
int toc_save_id(const char *id, uint32_t data, uint32_t length)
{
	struct toc_entry *toc_entry;

	toc_entry = toc_find_id(&toc_array[0], id);
	if (toc_entry == (struct toc_entry *)~0)
		return -1;

	toc_save_entry_mmc(toc_entry, data, length);

	return 0;
}


/*
 * Load part of TOC data.
 * - Locate id
 * - read size bytes from toc_entry->offset + offset
 * - store at dest
 *
 * Returns 0, -1 on error
 */
int toc_load_part(const char *id,
                  uint32_t dest,
                  uint32_t offset,
                  uint32_t size)
{
	unsigned long 		blkcnt;
	int 				status;
	struct toc_entry	*toc_entry;
	uint32_t			offset_bl;
	uint32_t	size_b;

	toc_entry = toc_find_id(&toc_array[0], id);
	if (toc_entry == (struct toc_entry *)~0)
		return -1;

	if ((toc_entry->flags & 0x0000ffff) == FLAGS_LBA) {
		size_b = toc_entry->size * 512;
	} else {
		size_b = toc_entry->size;
	}

	/* Don't read outside of partition! */
	if ((size + offset) > size_b) {
		dprintf(INFO, "toc_load_part: invalid size 0x%08x + 0x%08x > 0x%08x\n", offset, size, size_b);
		return -1;
	}

	blkcnt = (size + 511) / 512;
	if ((toc_entry->flags & 0x0000ffff) == FLAGS_LBA) {
		offset_bl = toc_entry->offset + (offset / 512);
	} else {
		offset_bl = (toc_entry->offset + offset) / 512;
	}

	status = mmc_read (toc_mmc, offset_bl, blkcnt, (uint32_t*)dest);
	if (status != MMC_OK) {
		dprintf(INFO, "toc_load_part: mmc_bread failed: rc %d\n",
				status);
		return -1;
	}

	dprintf(INFO, "toc_load_part: loaded %ld blks\n", blkcnt);
	return 0;

}

mmc_properties_t *toc_device_properties_get(void)
{
	return toc_mmc;
}

static bool toc_entry_update(char *toc_id, struct toc_entry *toc, uint32_t size)
{
	unsigned i;
	bool found;

	for(i = 0, found = false; i < TOC_MAIN_ENTRIES; i++) {
		if(!strcmp(toc_id, (char *)toc[i].id)) {
			if ((toc[i].flags & 0x0000ffff) == FLAGS_LBA) {
				toc[i].size = (size + 511) / 512;
			} else {
				toc[i].size = size;
			}
			found = true;
			break;
		}
	}
	return found;
}

/*
 * Update an entry within TOC or subTOC with new TOC entry size.
 * All copies of TOC on device are updated.
 * Parameters:
 * - location: 0 - in toc, 1 - in subtoc
 * - toc_id: name of TOC entry
 * - new_size: new TOC entry size
 */
void toc_update(int location, char *toc_id, uint32_t new_size)
{
	int rc;
	unsigned i;
	uint32_t toc_offset_bl;
	uint32_t n_wr_blocks;

	dprintf(INFO,"%s: location=%d id=%s, new_size=0x%08x\n",
		__func__,location,toc_id,new_size);

	switch(location) {
		case 0:
			/* find and update entry in toc array*/
			if(toc_entry_update(toc_id,toc_array, new_size)) {
				dprintf(INFO,"%s: entry found in TOC\n",__func__);
				/* update all toc copies on the media */
				for(i = 0, toc_offset_bl = 0; i < MAX_NUM_TOCS; i++) {
					if(toc_map[i]) {
						rc = mmc_read(toc_mmc, toc_offset_bl, 1, (uint32_t*)toc_copyspace);
						if(MMC_OK != rc) {
							dprintf(CRITICAL,"%s: toc read failed: %d\n",
							__func__,rc);
							continue;
						}
						toc_entry_update(toc_id,toc_copyspace,new_size);
						rc = mmc_write(toc_mmc, toc_offset_bl, 1, (uint32_t*)toc_copyspace);
						if(MMC_OK != rc) {
							dprintf(CRITICAL,"%s: toc write failed: %d\n",
							__func__,rc);
							continue;
						}
						dprintf(INFO,"%s: toc %d updated\n",__func__,i);
					}
					toc_offset_bl += (TOC_BOOT_IMAGE_SIZE/512);
				}
				return;
			}
			break;
		case 1:
			/* find and update entry in sub toc */
			if(toc_entry_update(toc_id,&toc_array[SUBTOC_START_IDX], new_size)) {
				dprintf(INFO,"%s: entry found in sub TOC\n",__func__);

				/* write updated subtoc to the media */
				n_wr_blocks = (n_toc_entries - TOC_MAIN_ENTRIES) / TOC_ENTRIES_PER_BLOCK;
				rc = mmc_write(toc_mmc, subtoc_offset_bl, n_wr_blocks, (uint32_t*)&toc_array[SUBTOC_START_IDX]);
				if(MMC_OK == rc)
					dprintf(INFO,"%s: subtoc updated\n",__func__);
				else
					dprintf(CRITICAL,"%s: subtoc update failed: %d\n",__func__,rc);
				return;
			}
			break;
	}
	dprintf(CRITICAL,"%s: entry not found\n",__func__);
}
