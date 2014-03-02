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
 * On ST-Ericsson Ux500 platforms exist 3 ways of partitioning information:
 * . kernel command line (blkdev_parts)
 * . TOC
 * . MBR
 *
 * The kernel command line requires a not mainlined ST-Ericsson extension in
 * kernel/fs/partitions: blkdev_parts.c
 * Example: blkdevparts=mmcblk0:655360@101376,256@0,2048@1024
 * Entities are 512 byte blocks.
 * The meaning of the partitions depends on how it is mounted in Linux.
 * Under Android the partitions might (and have) a different meaning than
 * in the ST-Ericsson GLK. So it is impossible to derive Android Fastboot
 * partition names from this kind of partition information.
 * It would be different if there was a name as in:
 * blkdevparts=mmcblk0:655360@101376(system),256@0(foo),2048@1024(bar)
 *
 * The TOC is needed by the boot ROM to load the ISSW (Initial Secure SW). The
 * ISSW needs the TOC to load the x-loader.  Now ST-Ericsson could switch to
 * MBR/GPT for the rest of the system, but decided not to do so. Consequently
 * all partition information should be in the TOC.
 * Any partition which offset or size exceeds 4 GiB cannot be in the TOC, since
 * the TOC offsets and size information are in bytes, not blocks.
 * (Workaround: introduce a flag in the TOC entry flags field to indicate block
 * addresses.)
 *
 * The MBR is the most common way for MMC devices in mainline Linux.
 * On Ux500 it is currently only used on external SD cards.
 * ST-Ericsson flash tools to not support the MBR, only TOC.
 * It was used on eMMC before the TOC was applied for the Linux partitions as
 * well. During that period the boot loader, U-Boot, wrote a fixed MBR.
 *
 * Conclusion: currently we can only use partition information from the TOC,
 * which might not match the blkdevparts parameter, which has precedence and no
 * name information.
 * Furthermore we cannot boot a mainline Linux system from eMMC without an
 * MBR/GPT.
 */

/*
 * NB: In this file all partitions count from 0.
 */

#include <stdlib.h>
#include <string.h>

#include <mmc_if.h>
#include <toc.h>
#include <partition_parser.h>

static int write_sparse_image (mmc_properties_t *pdev,
                               void *source,
                               unsigned len,
                               uint32_t start);
static uint8_t *parse_sparse_header (uint8_t *data, sparse_hdr_t *hdr);
static uint8_t *parse_chunk_header (uint8_t *data, sparse_chunk_hdr_t *hdr);


/* kernel partitions */
struct partition_entry partitions[PARTITIONS_MAX];
static unsigned int partitions_cnt = 0;
/* fastboot partitions */
fb_partition_entry_t fb_partitions[PARTITIONS_MAX];
static unsigned int fb_partitions_cnt = 0;
static unsigned int fb_partitions_idx;
/* fastboot protected partitions */
#define FB_PROTECTED_PART_NUM	8
static const char *fb_protected_partitions[FB_PROTECTED_PART_NUM] = {
		"ISSW"  ,
		"CSPSA0",
		"ARB_TBL1",
		"ARB_TBL2",
		"X-LOADER",
		"MEM_INIT",
		"PWR_MGT",
		"SUBTOC"
	};

/* fastboot boot partitions */
#define FB_BOOT_PART_NUM	2
static const char *fb_boot_partitions[FB_BOOT_PART_NUM] = {
		"boot",
		"recovery"
};

/* fastboot filesystem partitions */
#define FB_FILESYS_PART_NUM	5
static const char *fb_filesys_partitions[FB_FILESYS_PART_NUM] = {
		"misc",
		"modemfs",
		"system",
		"cache",
		"userdata"
};

typedef enum {
	PARTITION_TYPE_UNIFIED,
	PARTITION_TYPE_FASTBOOT
} partition_type_t;

/*
 * If a matching TOC entry exists, its name will be used, otherwise names
 * below.
 */
const char *mbr_part2name[16] = {
	"mbr_p1", "mbr_p2", "mbr_p3", "mbr_p4", "mbr_p5", "mbr_p6", "mbr_p7", "mbr_p8",
	"mbr_p9", "mbr_p10", "mbr_p11", "mbr_p12", "mbr_p13", "mbr_p14", "mbr_p15", "mbr_p16"
};
const char *cmdline_part2name[16] = {
	"mmcblk0p1", "mmcblk0p2", "mmcblk0p3", "mmcblk0p4", "mmcblk0p5", "mmcblk0p6", "mmcblk0p7", "mmcblk0p8",
	"mmcblk0p9", "mmcblk0p10", "mmcblk0p11", "mmcblk0p12", "mmcblk0p13", "mmcblk0p14", "mmcblk0p15", "mmcblk0p16"
};

static int gpt_exists = 0; /* set during MBR parsing */

static inline uint32_t le32_to_uint32(uint8_t *le32)
{
    return ((le32[3] << 24) + (le32[2] << 16) + (le32[1] << 8) + le32[0]);
}

static int partition_check_mbr(const unsigned char *mbr_buf)
{
	if ((mbr_buf[0x1fe] != 0x55) || (mbr_buf[0x1ff] != 0xaa)) {
		dprintf(INFO, "MBR signature does not match\n");
		return -1;
	}

	return 0;
}

/* XXX: move to toc_common.c */
/*
 * Find a range (start, start + size) match in the TOC table.
 * Offset and size are in bytes.
 * Returns pointer to internal TOC table entry upon success, otherwise ~0.
 */
static struct toc_entry *toc_find_range(struct toc_entry *toc, uint32_t offset,
		uint32_t size)
{
	unsigned int i;
	uint32_t n_entries;
	uint64_t offset_b;

	n_entries = toc_n_entries_get();

	/* Find matching <offset,size> entry */
	for (i = 0; i < n_entries; i++) {
		if ((toc[i].flags & 0x0000ffff) == FLAGS_LBA) {
			offset_b = toc[i].offset * 512;
		} else {
			offset_b = toc[i].offset;
		}
		if (offset_b == offset) {
			dprintf(SPEW, "partition_parser: found offset match: "
					"id %s, offs 0x%08x, size 0x%08x\n",
					toc[i].id, toc[i].offset, toc[i].size);
			if (toc[i].size != size) {
				dprintf(CRITICAL, "partition_parser: "
					"toc[%s].size 0x%08x does not match "
					"look-up size 0x%08x\n",
					toc[i].id, toc[i].size, size);
				return (struct toc_entry *)~0;
			}
			return &toc[i];
		}
	}

	return (struct toc_entry *)~0;
}


/*
 * Add partition info found by parsing the boot command line to the common
 * partition table. Try to find a match in the TOC and then use that name.
 */
static int partition_add_cmdline(unsigned int part_nbr, uint32_t lba_start,
		uint32_t lba_length)
{
	struct toc_entry *toce;
	struct toc_entry *toc;

	if (partitions_cnt == PARTITIONS_MAX)
		return -1;

	/*
	 * Find matching toc entry.
	 * Assumption: toc entry begins on block boundary
	 */
	toc = toc_addr_get();
	toce = toc_find_range(toc, lba_start * 512, lba_length * 512);
	if (toce == (struct toc_entry *)~0) {
		dprintf(SPEW, "partition_add_cmdline: "
				"no matching toc found for partition#%d\n",
				partitions_cnt);
		strcpy((char *)partitions[partitions_cnt].name,
			cmdline_part2name[part_nbr]);
	} else {
		memcpy(partitions[partitions_cnt].name, toce->id,
				TOC_ID_LENGTH);
	}
	partitions[partitions_cnt].type = 0; /* XXX: invent a type */
	partitions[partitions_cnt].offset = lba_start * 512;
	partitions[partitions_cnt].size = lba_length * 512;
	partitions_cnt++;

	return 0;
}


/*
 * Add partition info found by parsing the MBR to the common partition table.
 * Try to find a match in the TOC and then use that name.
 */
static int partition_add_mbr(unsigned int ptn, uint32_t lba_start,
		uint32_t lba_length, unsigned int ptype)
{
	struct toc_entry *toce;
	struct toc_entry *toc;

	if (partitions_cnt == PARTITIONS_MAX)
		return -1;

	/*
	 * Find matching toc entry.
	 * Assumption: toc entry begins on block boundary
	 */
	toc = toc_addr_get();
	toce = toc_find_range(toc, lba_start * 512, lba_length * 512);
	if (toce == (struct toc_entry *)~0) {
		dprintf(SPEW,
		"partition_add_mbr: no matching toc found for partition#%d\n",
			ptn);
		strcpy((char *)partitions[partitions_cnt].name,
			mbr_part2name[ptn]);
	} else {
		memcpy(partitions[partitions_cnt].name, toce->id,
				TOC_ID_LENGTH);
	}

	partitions[partitions_cnt].type = ptype;
	partitions[partitions_cnt].offset = lba_start * 512;
	partitions[partitions_cnt].size = lba_length * 512;
	partitions_cnt++;
	return 0;
}

/*
 * Read the raw MBR from the block device, parse its info and add the
 * partitions found to the unified partition table.
 * XXX: EBR currently not supported
 */
static int partition_read_mbr(mmc_properties_t *mmc_dev)
{
	int i;
	int rc;
	unsigned char mbr_buf[512];
	unsigned long count;
	unsigned long blkoffs;
	unsigned long blkcnt;
	struct mbr_part *pt;
	unsigned int ptype;

	/*
	 * MBR is always at block 0
	 */
	blkoffs = 0;
	blkcnt = 1;
	count = mmc_read(mmc_dev, blkoffs, blkcnt, (void *)mbr_buf);
	if (count != MMC_OK) {
		dprintf(INFO, "partition_read_mbr: mmc_bread failed: rc %ld\n",
				count);
		return -1;
	}

	rc = partition_check_mbr(mbr_buf);
	if (rc)
		return rc;

	/*
	 * copy MBR partition information into our partition table
	 */
	pt = (struct mbr_part *)(mbr_buf + MBR_PART_TBL_OFFS);
	for (i = 0; i < 4; i++, pt++) {
		uint32_t lba_start;
		uint32_t lba_length;

		ptype = pt->part_type;
		if (ptype == MBR_TYPE_PROTECTED) {
			gpt_exists = 1;
			return rc;
		}

		/* check for invalid MBR partition type */
		if (ptype == 0)
			continue;

		lba_start = le32_to_uint32(pt->lba_start);
		lba_length = le32_to_uint32(pt->lba_length);
		rc = partition_add_mbr(i, lba_start, lba_length, ptype);
		if (rc)
			return rc;
	}

	return rc;
}

static int partition_read_gpt(mmc_properties_t *mmc)
{
	(void)mmc;

	return -1;
}

/*
 * read TOC, MBR or GPT partition table info into common unified partition
 * table.
 *
 * XXX: remove mmc dependencies when bio framework is added and use block
 * device descriptor instead.
 */
int partition_read_table(mmc_properties_t *mmc)
{

	int rc;
	unsigned int i;
	struct toc_entry *toc;
	uint32_t n_entries;

	n_entries = toc_n_entries_get();

	/* copy toc (main toc and sub-toc) */
	toc = toc_addr_get();
	for (i = 0; i < n_entries; i++) {
		if (memcmp(toc[i].id, TOC_ID_NO_ENTRY, TOC_ID_LENGTH) != 0) {
			partitions[partitions_cnt].type = 0;
			if ((toc[i].flags & 0x0000ffff) == FLAGS_LBA) {
				partitions[partitions_cnt].offset = toc[i].offset * 512;
				partitions[partitions_cnt].size = toc[i].size * 512;
			} else {
				partitions[partitions_cnt].offset = toc[i].offset;
				partitions[partitions_cnt].size = toc[i].size;
			}
			memcpy(partitions[partitions_cnt].name, toc[i].id,
				TOC_ID_LENGTH);
			partitions_cnt++;
		}
	}

	rc = partition_read_mbr(mmc);
	if (rc)
		dprintf(INFO, "no MBR found\n");

	/* partition_read_mbr sets as side-effect gpt_exists */
	if (gpt_exists) {
		rc = partition_read_gpt(mmc);
		if (rc)
			dprintf(CRITICAL, "error reading GPT\n");
	}

	return rc;
}


/*
 * Parse Linux cmdline for blkdevparts (ST-Ericsson specific) and
 * add partitions defined by that to common partition table.
 * blkdev_name is always mmcblk0 for HREF eMMC.
 * Example: blkdevparts=mmcblk0:655360@101376,256@0,2048@1024
 * Entities are 512 byte blocks.
 */

/* Parse for a matching blkdev-id and return pointer to partdef */
static const char *parse_blkdev_id(const char *cmdline, const char *blkdev_name)
{
        unsigned int blkdev_id_len;
        const char *p, *blkdev_id;

        /* Start parsing for a matching blkdev-id */
        p = blkdev_id = cmdline;
        while (blkdev_id != NULL) {

                /* Find the end of the blkdev-id string */
                p = strchr(blkdev_id, ':');
                if (p == NULL)
                        return NULL;

                /* Check if we found a matching blkdev-id */
                blkdev_id_len = p - blkdev_id;
                if (strlen(blkdev_name) == blkdev_id_len) {
                        if (strncmp(blkdev_name, blkdev_id, blkdev_id_len) == 0)
                                return p;
                }

                /* Move to next blkdev-id string if there is one */
                blkdev_id = strchr(p, ';');
                if (blkdev_id != NULL)
                        blkdev_id++;
        }
        return NULL;
}

static int parse_partdef(const char **part, unsigned int part_nbr)
{
        uint64_t size, offset;
        const char *p = *part;

        /* Skip the beginning "," or ":" */
        p++;

        /* Fetch and verify size from partdef */
        size = (uint64_t)atoul(p);
	p = strchr(p, '@');
        if ((size == 0) || (p == NULL))
                return 0;

        /* Skip the "@" */
        p++;

        /* Fetch offset from partdef and check if there are more parts */
        offset = (uint64_t)atoul(p);
	p = strchr(p, ',');
        if (*p == ',')
                *part = p;
        else
                *part = NULL;

        /* Add partition to common partitions table */
        dprintf(SPEW, "blkdevparts: partition: size=%llu, offset=%llu\n",
                (unsigned long long) size,
                (unsigned long long) offset);

        partition_add_cmdline(part_nbr, offset, size);

        return 1;
}

/*
 * returns number of partitions found in "....blkdev_name:<partdef>..."
 */
int parse_blkdev_parts(const char *cmdline, const char *blkdev_name)
{
        const char *partdef;
        int part_nbr = 0;

        /* Find partdef */
        partdef = parse_blkdev_id(cmdline, blkdev_name);
	if (partdef == NULL) {
		dprintf(INFO, "blkdev %s not found\n", blkdev_name);
		return 0;
	}

        /* Add parts */
        while (partdef != NULL) {
                /* Find next part and add it to state */
                if (!parse_partdef(&partdef, part_nbr))
                        return 0;
                part_nbr++;
        }
        return part_nbr;
}

int partition_parse_cmdline(const char *cmdline)
{
	int rc;
	const char *p;
	const char *parameter = "blkdevparts=";

	p = strstr(cmdline, parameter);
	if (p == NULL) {
		dprintf(INFO, "cmdline: %s parameter not found\n", parameter);
		return -1;
	}
	p += strlen(parameter);
	rc = parse_blkdev_parts(p, "mmcblk0");
	if (rc == 0) {
		dprintf(INFO, "cmdline: no partitions found, %s\n", cmdline);
		return -1;
	}
	return 0;
}

static void partition_entry_update(fb_partition_entry_t *pentry, uint32_t new_size)
{
	if(pentry->toc_size == new_size)
		return;

	pentry->toc_size = new_size;
	toc_update(pentry->flags & FB_PART_LOCATION_SUBTOC ? 1 : 0,
		pentry->name,new_size);
}

fb_partition_entry_t *partition_entry_find(const char *name)
{
	unsigned int i;

	for (i = 0; i < fb_partitions_cnt; i++) {
		if(!strcmp((char*)fb_partitions[i].name, name))
			return &fb_partitions[i];
	}
	return (fb_partition_entry_t *)NULL;
}

fb_partition_entry_t *partition_entry_find_first(void)
{
	fb_partitions_idx = 1;
	return fb_partitions_cnt ? &fb_partitions[0] : NULL;
}

fb_partition_entry_t *partition_entry_find_next(void)
{
	return fb_partitions_idx < fb_partitions_cnt ?
		&fb_partitions[fb_partitions_idx++] : NULL;
}

int partition_entry_erase(fb_partition_entry_t *pentry)
{
	mmc_properties_t *pdev;
	uint32_t b_start, b_count;

	pdev = toc_device_properties_get();
	if (!pdev) {
		dprintf(CRITICAL, "partition_entry_erase(): device not ready\n");
		return -1;
	}

	b_start = pentry->offset / pdev->write_bl_len;
	b_count = pentry->size   / pdev->write_bl_len;

	if ((pentry->offset  % pdev->write_bl_len)
		|| (pentry->size % pdev->write_bl_len)) {
		dprintf(CRITICAL, "partition_entry_erase(): partition is not aligned to device block size\n");
		return -1;
	}

	dprintf(INFO, "partition_entry_erase(): blk start=0x%08x count=0x%08x\n",b_start,b_count);
	if(MMC_OK != mmc_erase(pdev,b_start,b_count))
		return -1;

	partition_entry_update(pentry,0);
	return 0;
}

static bool fb_part_is_protected(char *id)
{
	unsigned len;
	unsigned i = FB_PROTECTED_PART_NUM-1;
	do {
		len = strlen(fb_protected_partitions[i]);
		if(!memcmp(fb_protected_partitions[i],id,len))
			return true;
	} while (i--);
	return false;
}

static void fb_part_sort(void)
{
	fb_partition_entry_t tmp;
	unsigned i, j, pos;

	for(i = 0; i < (fb_partitions_cnt-1); i++) {
		pos = i;
		for(j = i+1; j < fb_partitions_cnt; j++) {
			if(fb_partitions[pos].offset > fb_partitions[j].offset)
				pos = j;
		}
		if(pos != i) {
			tmp = fb_partitions[i];
			fb_partitions[i] = fb_partitions[pos];
			fb_partitions[pos] = tmp;
		}
	}
}

static bool fb_part_is_boot(char *id)
{
	unsigned i = FB_BOOT_PART_NUM - 1;
	do {
		if(!strcmp(id,fb_boot_partitions[i]))
			return true;
	} while(i--);
	return false;
}

static bool fb_part_is_filesys(char *id)
{
	unsigned i = FB_FILESYS_PART_NUM - 1;
	do {
		if(!strcmp(id,fb_filesys_partitions[i]))
			return true;
	} while(i--);
	return false;
}

void fb_partitions_init(void)
{
	mmc_properties_t *pdev;
	struct toc_entry *t_entry;
	unsigned i, j;

	fb_partitions_cnt = 0;

	pdev = toc_device_properties_get();
	if (!pdev) {
		dprintf(CRITICAL, "fb_partitions_init(): device not ready\n");
		return;
	}

	/* build partitions based on toc content */
	i = 0;
	t_entry = toc_find_first();
	do {
		if (memcmp(t_entry->id, TOC_ID_NO_ENTRY, TOC_ID_LENGTH)) {
			if ((t_entry->flags & 0x0000ffff) == FLAGS_LBA) {
				if (t_entry->offset * 512 > UINT_MAX || t_entry->size * 512 > UINT_MAX) {
					dprintf(INFO, "fb_partitions_init(): Skipping TOC \"%s\": out of range", t_entry->id);
					i++;
					t_entry = toc_find_next();
					if (!t_entry)
						break;
					continue;
				}
				fb_partitions[fb_partitions_cnt].offset = t_entry->offset * 512;
				fb_partitions[fb_partitions_cnt].toc_size = t_entry->size * 512;
			} else {
				fb_partitions[fb_partitions_cnt].offset = t_entry->offset;
				fb_partitions[fb_partitions_cnt].toc_size = t_entry->size;
			}
			memcpy(fb_partitions[fb_partitions_cnt].name, t_entry->id,TOC_ID_LENGTH);
			fb_partitions[fb_partitions_cnt].name[TOC_ID_LENGTH-1] = 0;
			fb_partitions[fb_partitions_cnt].flags = FB_PART_FLAGS_RESET_VALUE;
			if(i >= SUBTOC_START_IDX)
				fb_partitions[fb_partitions_cnt].flags |= FB_PART_LOCATION_SUBTOC;
			if(fb_part_is_boot(fb_partitions[fb_partitions_cnt].name))
				fb_partitions[fb_partitions_cnt].flags |= FB_PART_BOOT;
			if(fb_part_is_filesys(fb_partitions[fb_partitions_cnt].name))
				fb_partitions[fb_partitions_cnt].flags |= FB_PART_FILESYS;
			if(++fb_partitions_cnt >= PARTITIONS_MAX)
				break;
		}
		i++;
	} while(NULL != (t_entry = toc_find_next()));

	if(!fb_partitions_cnt)
		return;

	/* sort partitions based on offset field */
	fb_part_sort();

	/* calculate partition entry size */
	for(i = 0; i < (fb_partitions_cnt - 1); i++) {
		fb_partitions[i].size = fb_partitions[i+1].offset - fb_partitions[i].offset;
	}
	fb_partitions[fb_partitions_cnt-1].size =
	pdev->device_size - fb_partitions[fb_partitions_cnt-1].offset;

	/* exclude protected partitions */
	for(i = j = 0; i < fb_partitions_cnt; i++) {
		if(!fb_part_is_protected(fb_partitions[i].name)) {
			fb_partitions[j] = fb_partitions[i];
			j++;
		}
	}
	fb_partitions_cnt -= (i-j);
}


uint32_t get_partition_size (char *name)
{
	unsigned int ii;

	for (ii = 0; ii < fb_partitions_cnt; ii++) {
		if (0 == strcmp (fb_partitions[ii].name, name)) {
			return fb_partitions[ii].size;
		}
	}

	return 0;
}


int partition_entry_write(fb_partition_entry_t *pentry, void *source, unsigned len)
{
	mmc_properties_t *pdev;
	uint32_t b_start, b_count, b_len;

	pdev = toc_device_properties_get();
	if (!pdev) {
		dprintf(CRITICAL, "partition_entry_write(): device not ready\n");
		return -1;
	}

	b_start = pentry->offset / pdev->write_bl_len;
	b_count = pentry->size   / pdev->write_bl_len;

	if ((pentry->offset  % pdev->write_bl_len)
		|| (pentry->size % pdev->write_bl_len)) {
		dprintf(CRITICAL, "partition_entry_write(): partition is not aligned to device block size\n");
		return -1;
	}

	b_len = len / pdev->write_bl_len;

	if (len % pdev->write_bl_len)
		b_len++;

	if (b_count < b_len) {
		dprintf(CRITICAL, "partition_entry_write(): not enough space\n");
		return -1;
	}

	/* Check if sparse FS image */
	if ((pentry->flags & FB_PART_FILESYS) &&
		(SPARSE_MAGIC == *(uint32_t*)source))  {
		/* Sparse formatted, write in chunks */
		if (0 != write_sparse_image(pdev, source, len, b_start))
			return -1;
	} else {
		/* Raw binary data*/
		dprintf(INFO, "partition_entry_write(): blk start=0x%08x blocks=0x%08x\n",b_start,b_len);
		if(MMC_OK != mmc_write(pdev,b_start,b_len,source))
			return -1;
	}

	if (!(pentry->flags & FB_PART_FILESYS)) {
		partition_entry_update(pentry,len);
		return 0;
	}

	if(!pentry->toc_size) {
		len = pentry->size;
		partition_entry_update(pentry,len);
	}
	return 0;
}

unsigned partition_find_biggest(unsigned max_limit)
{
	unsigned i, rc;
	for(i = 0, rc = 0; i < fb_partitions_cnt; i++)
		if ((rc < fb_partitions[i].size) && (fb_partitions[i].size < max_limit))
			rc = fb_partitions[i].size;
	return rc;
}

/* Write sparse image */
static int write_sparse_image (mmc_properties_t *pdev,
                               void *source,
                               unsigned len,
                               uint32_t start)
{
    uint8_t            *ptr;
    uint32_t            offset;
    uint32_t            n_mmc_blocks;
    sparse_hdr_t        hdr;
    sparse_chunk_hdr_t  chnk;

    ptr = (uint8_t*)source;
    ptr = parse_sparse_header (ptr, &hdr);
    if (hdr.magic == SPARSE_MAGIC) {
        dprintf (INFO, "SPARSE_MAGIC found\n");
        dprintf (SPEW, "magic 0x%08x\n"\
                "major 0x%04x\n"\
                "minor 0x%04x\n"\
                "ff1 0x%04x\n"\
                "ff2 0x%04x\n"\
                "block_size 0x%08x\n"\
                "tot_blocks 0x%08x\n"\
                "tot_chunks 0x%08x\n"\
                "crc32 0x%08x\n",
                hdr.magic, hdr.major, hdr.minor, hdr.ff1, hdr.ff2,
                hdr.block_size, hdr.tot_blocks, hdr.tot_chunks, hdr.crc32);

        /* Start by erasing area to whipe any data left in empty areas */
        dprintf (INFO, "Erasing 0x%08x blocks @ block 0x%08x\n", len / pdev->read_bl_len, start);
        if (MMC_OK != mmc_erase (pdev, start, len / pdev->read_bl_len)) {
            dprintf (CRITICAL, "Sparse ERASE failed\n");
            return -1;
        }

        offset = start; /* Offset in number of mmc blocks */
        while (((uint8_t*)source + len) > ptr) {
            ptr = parse_chunk_header (ptr, &chnk);
            dprintf (SPEW, "chunk_type 0x%04x\nreserved   0x%04x\n"\
                    "chunk_blocks   0x%08x\ntot_bytes      0x%08x\n",
                    chnk.chunk_type, chnk.reserved,
                    chnk.chunk_blocks, chnk.tot_bytes);

            n_mmc_blocks = (hdr.block_size * chnk.chunk_blocks) / pdev->read_bl_len;
            if (SPARSE_CHUNK_TYPE_RAW == chnk.chunk_type) {
                dprintf (INFO, "Writing sparse chunk 0x%08x bytes @ 0x%08x\n",
                        hdr.block_size * chnk.chunk_blocks, offset * pdev->read_bl_len);
                if(MMC_OK != mmc_write (pdev, offset, n_mmc_blocks, (uint32_t*)ptr))
                    return -1;
            }
            offset += n_mmc_blocks;
            ptr += chnk.tot_bytes - sizeof (sparse_chunk_hdr_t);
        }

        return 0;
    }
    return -1;
}


static uint8_t *parse_sparse_header (uint8_t *data, sparse_hdr_t *hdr)
{
    if (!data || !hdr) {
        return NULL;
    }

    memcpy (&hdr->magic, data, sizeof (uint32_t));

    /* Check for valid header */
    if (hdr->magic != SPARSE_MAGIC) {
        return NULL;
    }

    data += sizeof (uint32_t);
    memcpy (&hdr->major, data, sizeof (uint16_t));
    data += sizeof (uint16_t);
    memcpy (&hdr->minor, data, sizeof (uint16_t));
    data += sizeof (uint16_t);
    memcpy (&hdr->ff1, data, sizeof (uint16_t));
    data += sizeof (uint16_t);
    memcpy (&hdr->ff2, data, sizeof (uint16_t));
    data += sizeof (uint16_t);
    memcpy (&hdr->block_size, data, sizeof (uint32_t));
    data += sizeof (uint32_t);
    memcpy (&hdr->tot_blocks, data, sizeof (uint32_t));
    data += sizeof (uint32_t);
    memcpy (&hdr->tot_chunks, data, sizeof (uint32_t));
    data += sizeof (uint32_t);
    memcpy (&hdr->crc32, data, sizeof (uint32_t));
    data += sizeof (uint32_t);

    return data;
}


static uint8_t *parse_chunk_header (uint8_t *data, sparse_chunk_hdr_t *hdr)
{
    if (!data || !hdr) {
        return NULL;
    }

    memcpy (&hdr->chunk_type, data, sizeof (uint16_t));
    /* Check for valid chunk header */
    switch (hdr->chunk_type) {
    case SPARSE_CHUNK_TYPE_RAW:
    case SPARSE_CHUNK_TYPE_DC:
    case SPARSE_CHUNK_TYPE_FILL:
    case SPARSE_CHUNK_TYPE_CRC:
        break;
    default:
        return NULL;
    }

    data += sizeof (uint16_t);
    memcpy (&hdr->reserved, data, sizeof (uint16_t));
    data += sizeof (uint16_t);
    memcpy (&hdr->chunk_blocks, data, sizeof (uint32_t));
    data += sizeof (uint32_t);
    memcpy (&hdr->tot_bytes, data, sizeof (uint32_t));
    data += sizeof (uint32_t);

    return data;
}


#if !defined(ENABLE_FEATURE_BUILD_HBTS)
#if WITH_LIB_CONSOLE

#include <lib/console.h>
#include <stdio.h>
#include <toc.h>

static int cmd_partitions(int argc, const cmd_args *argv);
static int cmd_fb_partitions(int argc, const cmd_args *argv);

STATIC_COMMAND_START
        { "partitions",    "print unified partition table", &cmd_partitions },
        { "fb_partitions", "print fastboot partition table", &cmd_fb_partitions },
STATIC_COMMAND_END(partition);

static void partition_print(partition_type_t p_type)
{
	unsigned int i;

	switch(p_type) {
		case PARTITION_TYPE_UNIFIED:
			printf("ptn      Name   Type  First_lba  Size(bl)"
			"  Offset(bytes) Size(bytes)\n");
			for (i = 0; i < partitions_cnt; i++) {
				printf("%2d:%12s 0x%02x 0x%08llx 0x%08llx  0x%08llx 0x%08llx\n",
				i, partitions[i].name,
				partitions[i].type,
				partitions[i].offset / 512, partitions[i].size / 512,
				partitions[i].offset,
				partitions[i].size);
			}
			break;
		case PARTITION_TYPE_FASTBOOT:
			printf("ptn      Name     Offset(bytes)   Size(bytes)    TOC Size(bytes)  Flags\n");
			for (i = 0; i < fb_partitions_cnt; i++) {
				printf("%2d:%12s   0x%08x      0x%08x     0x%08x       0x%08x\n",
				i, fb_partitions[i].name,  fb_partitions[i].offset,
				fb_partitions[i].size,
				fb_partitions[i].toc_size,
				fb_partitions[i].flags);
			}
			break;
		default:
			return;
	}
}

static int cmd_partitions(int argc, const cmd_args *argv)
{

	(void)argc;
	(void)argv;

	partition_print(PARTITION_TYPE_UNIFIED);

	return 0;
}

static int cmd_fb_partitions(int argc, const cmd_args *argv)
{

	(void)argc;
	(void)argv;

	partition_print(PARTITION_TYPE_FASTBOOT);

	return 0;
}

#endif /* WITH_LIB_CONSOLE */
#endif /* ENABLE_FEATURE_BUILD_HBTS */
