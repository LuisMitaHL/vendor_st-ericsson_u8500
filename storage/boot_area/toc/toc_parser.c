/************************************************************************
 *                                                                      *
 *  Copyright (C) 2011 ST-Ericsson SA                                   *
 *                                                                      *
 *  This software is released under the terms of the BSD-style          *
 *  license accompanying TOC.                                           *
 *                                                                      *
 *  Author: Peter Nessrup <peter.nessrup@stericsson.com>                *
 *  Author: Mikael Larsson <mikael.xt.larsson@stericsson.com>           *
 *                                                                      *
 ************************************************************************/

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include "toc_parser.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define TOC_HANDLE_MAGIC	(0x70C0CA70)
#define SUPPORTED_SECTOR_SIZE	512
#define TOC_MAGIC		"ISSW"
#define TOC_ID_NO_ENTRY		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define TOC_MAX_ENTRIES		(SUPPORTED_SECTOR_SIZE / sizeof(tocparser_toc_entry_t))

/* Some customers use less than 4 TOC copies */
#ifndef MAX_NUM_TOCS
#define MAX_NUM_TOCS		4
#endif

#define TOC_BOOT_IMAGE_SIZE	0x20000

/* Some customers have other stuff at address 0 */
#ifndef TOC_ROOT_OFFSET
#define TOC_ROOT_OFFSET		0
#endif

#define NO_TOC			-1
#define TOC_ID_LENGTH		12

#define WRITE_BUFF_SIZE		(SUPPORTED_SECTOR_SIZE * 4)

/* Mask for the values present in flags field */
#define FLAGS_MASK		0x0000FFFF

/* Sub TOC identifier in the 'flags' field is 'ST' */
#define FLAGS_SUB_TOC		0x00005354

/* Partition Entry indentifier in the 'flags' field is 'PE' */
#define FLAGS_PARTITION		0x00005045

/* The offset is in LBA (512 B) rather than bytes, 'LB' */
#define FLAGS_LBA		0x00004C42

/* Part of the flags field contain the device number */
#define FLAGS_DEVICE_NUMBER_SHIFT	(16)
#define FLAGS_DEVICE_NUMBER_MASK	(0x3 << FLAGS_DEVICE_NUMBER_SHIFT)

#ifndef TOCPARSER_DEV
#define TOCPARSER_DEV		"/dev/mmcblk0"
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define TP_DBG(x)	if (debug) x

#define TP_DBG_FUNC() TP_DBG(printf("%s()\n", __FUNCTION__))

#define ROUND_CEIL(a, b) ((((a) + (b) - 1) / (b)) * (b))
#define ROUND_FLOOR(a, b) (((a) / (b)) * (b))

typedef struct {
	uint8_t  id[TOC_ID_LENGTH];
	uint32_t num_items;
	uint32_t location;
	uint32_t cached_section;
	tocparser_toc_entry_t cache[TOC_MAX_ENTRIES];
} tocparser_subtoc_t;

struct tocparser_handle_t {
	uint32_t magic;
	tocparser_toc_entry_t main_toc[TOC_MAX_ENTRIES];
	tocparser_subtoc_t subtoc;
	int fd;
	int toc_location;
	uint8_t *boot_block_buffer;
	uint8_t write_buff[WRITE_BUFF_SIZE];
	tocparser_mode_t mode;
};

static uint32_t debug;

static int tocparser_main_toc_remove_entry(tocparser_toc_entry_t *main_toc,
					   tocparser_toc_entry_t *entry);

static int is_toc(tocparser_toc_entry_t *toc)
{
	/* Check for TOC MAGIC */
	TP_DBG_FUNC();

	return memcmp(toc->id, TOC_MAGIC, sizeof(TOC_MAGIC));
}

/* Returns Bytes, performs LBA conversion if needed */
static uint64_t get_offset(tocparser_toc_entry_t *toc)
{
	if ((toc->flags & FLAGS_MASK) == FLAGS_LBA)
		return (uint64_t) toc->offset * SUPPORTED_SECTOR_SIZE;
	else
		return (uint64_t) toc->offset;

}

static uint64_t get_size(tocparser_toc_entry_t *toc)
{
	if ((toc->flags & FLAGS_MASK) == FLAGS_LBA)
		return (uint64_t) toc->size * SUPPORTED_SECTOR_SIZE;
	else
		return (uint64_t) toc->size;

}

/* Sets Bytes or LBA, performs conversion if needed */
static void set_offset(tocparser_toc_entry_t *toc, uint64_t offset)
{
	if ((toc->flags & FLAGS_MASK) == FLAGS_LBA)
		toc->offset = (uint32_t) offset / SUPPORTED_SECTOR_SIZE;
	else
		toc->offset = (uint32_t) offset;

}

static void set_size(tocparser_toc_entry_t *toc, uint64_t size)
{
	if ((toc->flags & FLAGS_MASK) == FLAGS_LBA)
		toc->size = (uint32_t) size / SUPPORTED_SECTOR_SIZE;
	else
		toc->size = (uint32_t) size;

}

/**
 * tocparser_recalc_entry_offsets() Recalculate the relative offsets in the toc
 *				    to absolute offsets given to the client of
 *				    the library.
 *
 * @param [in] handle	Pointer to our running state.
 * @param [in] toc	Pointer to the main toc.
 */
static void tocparser_recalc_entry_offsets(tocparser_handle_t *handle,
					   tocparser_toc_entry_t *toc)
{
	uint32_t i;

	TP_DBG_FUNC();
	/*
	 * Recalculate the offsets that are outside of the boot image (0x20000)
	 * since they need to be absolute and not relative
	 */
	if (handle->toc_location == 0)
		return;

	for (i = 0; i < TOC_MAX_ENTRIES; i++, toc++) {
		/*
		 * Only do this for offsets outside the boot area and not
		 * on empty entries
		 */
		if (memcmp(toc->id,
			   TOC_ID_NO_ENTRY,
			   TOC_ID_LENGTH) == 0) {
			TP_DBG(printf("Empty TOC entry\n"));
			continue;
		}

		if (get_offset(toc) >= TOC_BOOT_IMAGE_SIZE) {
			TP_DBG(printf("Recalculating offset on TOCEntry: %s\n",
					toc->id));
			set_offset(toc, get_offset(toc) + handle->toc_location);
		}
	}
}

/**
 * tocparser_generate_toc_sector() Generate a toc containing relative offsets,
 *				   that can be written to flash.
 *
 * @param [in] target_toc_location	The offset this toc sector will be
 *					written to.
 * @param [in] from_toc			The main toc (with absolute offsets) we
 *					want to base the toc sector on.
 * @param [in/out] out_toc		Buffer where the toc sector (with
 *					relative offsets) will be created.
 */
static void tocparser_generate_toc_sector(int target_toc_location,
					  tocparser_toc_entry_t *from_toc,
					  tocparser_toc_entry_t *out_toc)
{
	uint32_t i;
	TP_DBG_FUNC();

	for (i = 0; i < TOC_MAX_ENTRIES; i++, from_toc++, out_toc++) {
		*out_toc = *from_toc;

		/*
		 * Only do this for offsets outside the boot area and not
		 * on empty entries
		 */
		if (memcmp(out_toc->id,
			    TOC_ID_NO_ENTRY,
			    TOC_ID_LENGTH) == 0) {
			TP_DBG(printf("Empty TOC entry\n"));
			continue;
		}

		if (get_offset(out_toc) >= TOC_BOOT_IMAGE_SIZE) {
			TP_DBG(printf("Recalculating offset on TOCEntry: %s\n",
					out_toc->id));
			set_offset(out_toc, get_offset(out_toc) - target_toc_location);
		}
	}
}

/**
 * tocparser_boot_block_load() Load the boot block for the active toc into ram.
 *
 * @param [in] handle	Pointer to our running state.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_load(tocparser_handle_t *handle)
{
	TP_DBG_FUNC();

	if (handle->boot_block_buffer != NULL) {
		TP_DBG(printf("Boot block buffer present, do nothing...\n"));
		return 0;
	}

	TP_DBG(printf("No boot block buffer present, loading...\n"));

	handle->boot_block_buffer = malloc(TOC_BOOT_IMAGE_SIZE);

	if (lseek(handle->fd, handle->toc_location, SEEK_SET) !=
		handle->toc_location)
		goto fail;

	if (read(handle->fd, handle->boot_block_buffer,
			TOC_BOOT_IMAGE_SIZE) !=
			TOC_BOOT_IMAGE_SIZE) {
		TP_DBG(printf("Failed to read boot block into ram\n"));
		goto fail;
	}

	/* Clear the first sector of the buffer containing the toc.
	 * This part of the buffer is later used to invalidate the
	 * boot block we are updating so that the boot rom will not
	 * try to start from a half-written boot block
	 */
	memset(handle->boot_block_buffer, 0, SUPPORTED_SECTOR_SIZE);

	return 0;

fail:
	if (handle->boot_block_buffer) {
		free(handle->boot_block_buffer);
		handle->boot_block_buffer = NULL;
	}
	return 1;
}

/**
 * tocparser_subtoc_cache_read() Read the correct sector for the subtoc
 *				 into the cache.
 *
 * @param [in] fd		The fd for our device.
 * @param [in] entry_num	Entry number in the subtoc we are interrested in
 * @param [in] subtoc		Pointer to the subtoc structure.
 * @return Returns 0 on success.
 */
static int tocparser_subtoc_cache_read(int fd,
				       const int entry_num,
				       tocparser_subtoc_t *subtoc)
{
	TP_DBG_FUNC();

	if ((entry_num/TOC_MAX_ENTRIES) != subtoc->cached_section) {
		/* The entry is not in the cached section */
		off64_t offset = (entry_num / TOC_MAX_ENTRIES) *
				SUPPORTED_SECTOR_SIZE;

		if (lseek64(fd, subtoc->location + offset, SEEK_SET) !=
		    (off64_t) (subtoc->location + offset))
			return 1;

		if (read(fd,
			  &subtoc->cache,
			  SUPPORTED_SECTOR_SIZE) == SUPPORTED_SECTOR_SIZE) {
			TP_DBG(printf("Sub TOC entries read into cache\n"));
			/* New cached section available */
			subtoc->cached_section = entry_num / TOC_MAX_ENTRIES;
		} else
			return 1;
	}
	return 0;
}

/**
 * tocparser_get_subtoc_entry_num() The an entry from a specific offset in the
 *				    subtoc.
 *
 * @param [in] fd	The fd for our device.
 * @param [in] num	Entry number in the subtoc we are interrested in
 * @param [in] subtoc	Pointer to the subtoc structure.
 * @return Returns the requested entry or NULL if not found.
 */
static tocparser_toc_entry_t *tocparser_get_subtoc_entry_num(int fd,
						   const uint32_t num,
						   tocparser_subtoc_t *subtoc)
{
	TP_DBG_FUNC();

	if (num >= subtoc->num_items)
		return NULL;

	if (tocparser_subtoc_cache_read(fd, num, subtoc) == 0)
		/*
		 * Now we have the correct cache for the requested entry
		 * Return the requested entry
		 */
		return &subtoc->cache[num % TOC_MAX_ENTRIES];

	TP_DBG(printf("Sub TOC entry not found\n"));
	return NULL;
}

/**
 * tocparser_get_subtoc_entry_id() Retreive an entry and its sequence
 *				...number (optional) with a specific id from the
 *				   subtoc.
 *
 * @param [in] fd		The fd for our device.
 * @param [in] toc_id		NULL terminated string containing the name.
 * @param [in] partition_entry	Controls whether to look for a TOC entry that is
 *				a partition entry or an image entry.
 * @param [in] subtoc		Pointer to the subtoc structure.
 * @param [out] entry_num   Sequence number of the entry. NULL if not used.
 * @return Returns the requested entry or NULL if not found.
 */
static tocparser_toc_entry_t *tocparser_get_subtoc_entry_id(int fd,
						  const char *toc_id,
						  bool partition_entry,
						  tocparser_subtoc_t *subtoc,
						  uint32_t *entry_num)
{
	tocparser_toc_entry_t *toc_entry;
	uint8_t toc_id_buf[TOC_ID_LENGTH];
	TP_DBG_FUNC();

	if (subtoc->num_items > 0) {
		uint32_t i;

		/*
		 * Use a tmp buffer to compare the incoming string with the
		 * toc entry id, as it is not a null terminated string in the
		 * toc and we always want to compare all bytes. This to not make
		 * a false match of a subset of the toc entry id
		 */
		memset(toc_id_buf, 0, TOC_ID_LENGTH);
		memcpy(toc_id_buf, toc_id, MIN(strlen(toc_id), TOC_ID_LENGTH));

		for (i = 0; i < subtoc->num_items; i++) {
			toc_entry = tocparser_get_subtoc_entry_num(fd,
								   i,
								   subtoc);
			if (toc_entry == NULL)
				break;

			if (!partition_entry) {
				if ((toc_entry->flags & FLAGS_MASK) ==
				    FLAGS_PARTITION)
					continue;
			}
			else if ((toc_entry->flags & FLAGS_MASK) !=
				 FLAGS_PARTITION) {
				continue;
			}

			if (memcmp(toc_id_buf,
				   toc_entry->id,
				   TOC_ID_LENGTH) == 0) {
				TP_DBG(printf("%s found.\n", toc_id));
				if (entry_num)
					*entry_num = i;
				return toc_entry;
			}
		}
	}

	TP_DBG(printf("Sub TOC entry not found\n"));
	return NULL;
}

/**
 * tocparser_get_entry_subtoc() Get the entry describing the subtoc from
 *				the given toc.
 *
 * @param [in] toc	Pointer to the toc structure.
 * @return Returns pointer to the subtoc entry, NULL on failure.
 */
static tocparser_toc_entry_t *tocparser_get_entry_subtoc(
						tocparser_toc_entry_t *toc)
{
	uint32_t i;

	TP_DBG_FUNC();

	/* Find sub TOC entry */
	for (i = 0; i < TOC_MAX_ENTRIES; i++, toc++) {
		/* Check the flags for a sub TOC */
		if ((toc->flags & FLAGS_MASK) == FLAGS_SUB_TOC)
			return toc;

		if (memcmp(toc->id,
			    TOC_ID_NO_ENTRY,
			    TOC_ID_LENGTH) == 0)
			/*
			 * Don't iterate the rest of the entries
			 * if an empty entry was found
			 */
			break;
	}

	TP_DBG(printf("sub TOC not found!\n"));
	return NULL;
}

/**
 * tocparser_get_entry() Get a specific toc entry, this function will
 *			 first search the toc and then the subtoc for
 *			 the requested entry.
 *
 * @param [in] fd		The fd for our device.
 * @param [in] toc_id		NULL terminated string containing the name.
 * @param [in] partition_entry	Controls whether to look for a TOC entry that is
 *				a partition entry or an image entry.
 * @param [in] toc		Main toc to search.
 * @param [in] subtoc		Subtoc to search if we did not find the entry
 *				in the main toc.
 * @param [out] in_sub_toc	True if the entry is present in the subtoc.
 * @return Returns pointer to the toc entry, NULL on failure.
 */
static tocparser_toc_entry_t *tocparser_get_entry(int fd,
					const char *toc_id,
					bool partition_entry,
					tocparser_toc_entry_t *toc,
					tocparser_subtoc_t *subtoc,
					bool *in_sub_toc)
{
	uint32_t i;
	uint8_t toc_id_buf[TOC_ID_LENGTH];

	TP_DBG_FUNC();

	if (in_sub_toc)
		*in_sub_toc = false;

	/*
	 * Use a tmp buffer to compare the incoming string with the
	 * toc entry id, as it is not a null terminated string in the
	 * toc and we always want to compare all bytes. This to not make
	 * a false match of a subset of the toc entry id
	 */
	memset(toc_id_buf, 0, TOC_ID_LENGTH);
	memcpy(toc_id_buf, toc_id, MIN(strlen(toc_id), TOC_ID_LENGTH));

	/* Find TOC entry */
	for (i = 0; i < TOC_MAX_ENTRIES; i++, toc++) {
		/*
		 * Don't iterate the rest of the entries
		 * if an empty entry was found
		 */
		if (memcmp(toc->id,
			    TOC_ID_NO_ENTRY,
			    TOC_ID_LENGTH) == 0)
			break;

		if (!partition_entry) {
			if ((toc->flags & FLAGS_MASK) == FLAGS_PARTITION)
				continue;
		}
		else if ((toc->flags & FLAGS_MASK) != FLAGS_PARTITION) {
			continue;
		}

		if (memcmp(toc_id_buf, toc->id, TOC_ID_LENGTH) == 0) {
			TP_DBG(printf("%s found.\n", toc_id));
			return toc;
		}
	}

	/* Now the item is either in the subtoc or not present. If it is not
	 * present then the 'in_sub_toc'  out value will have no effect.
	 */
	if (in_sub_toc)
		*in_sub_toc = true;

	/* Check if the Id can be found in the sub TOC */
	return tocparser_get_subtoc_entry_id(fd,
					     toc_id,
					     partition_entry,
					     subtoc,
					     NULL);
}

/**
 * tocparser_print_entry() Print a given toc entry
 *
 * @param [in] toc_entry	Entry to print.
 */
static void tocparser_print_entry(tocparser_toc_entry_t *toc_entry)
{
	char buf[TOC_ID_LENGTH + 1];

	if (toc_entry) {
		printf("0x%08x ", toc_entry->offset);
		if ((toc_entry->flags & FLAGS_MASK) == FLAGS_LBA)
			printf("x %4d B ", SUPPORTED_SECTOR_SIZE);
		else
			printf("B        ");
		printf("0x%08x ", toc_entry->size);
		if ((toc_entry->flags & FLAGS_MASK) == FLAGS_LBA)
			printf("x %4d B ", SUPPORTED_SECTOR_SIZE);
		else
			printf("B        ");
		printf("0x%08x ", toc_entry->flags);
		printf("0x%08x ", toc_entry->align);
		printf("0x%08x ", toc_entry->loadaddr);
		memcpy(buf, &toc_entry->id, TOC_ID_LENGTH);
		buf[TOC_ID_LENGTH] = 0;
		printf("\"%s\"\n", buf);
	}
}

/**
 * tocparser_print_subtoc() Print all of the entries in the subtoc
 *
 * @param [in] handle	Handle to our running state.
 */
static void tocparser_print_subtoc(tocparser_handle_t *handle)
{
	uint32_t i = 0;

	if (handle->subtoc.num_items <= 0)
		return;

	printf("Printing Sub TOC entries - pointed out by: %s\n",
		handle->subtoc.id);
	printf("Offset              Size                Flags      Align      LoadAddr   ID\n");
	for (i = 0; i < handle->subtoc.num_items; i++)
		tocparser_print_entry(
			tocparser_get_subtoc_entry_num(handle->fd, i,
						       &handle->subtoc));
}

/**
 * tocparser_print() Print all of the entries in the toc and subtoc
 *
 * @param [in] handle	Handle to our running state.
 */
static int tocparser_print(tocparser_handle_t *handle)
{
	uint32_t i = 0;

	TP_DBG_FUNC();

	if (handle->toc_location == NO_TOC) {
		TP_DBG(printf("TOC doesn't exist!\n"));
		return 1;
	}
	/* Print the whole TOC */
	printf("Printing TOC at %x\n", handle->toc_location);
	printf("Offset              Size                Flags      Align      LoadAddr   ID\n");
	for (i = 0; i < TOC_MAX_ENTRIES; i++) {
		if (memcmp(handle->main_toc[i].id,
			   TOC_ID_NO_ENTRY,
			   TOC_ID_LENGTH) != 0)
			/* Don't print empty TOC entries */
			tocparser_print_entry(&handle->main_toc[i]);
	}
	tocparser_print_subtoc(handle);

	return 0;
}

/**
 * tocparser_read_subtoc() Read the sub toc from the device.
 *
 * @param [in] handle	Pointer to our running state.
 */
static int tocparser_read_subtoc(tocparser_handle_t *handle)
{
	tocparser_toc_entry_t *subtoc_entry, *toc_entry_in_subtoc;
	off64_t offset;
	uint32_t n;

	TP_DBG_FUNC();

	/* Reset the sub TOC struct first of all */
	memset(handle->subtoc.id, 0, sizeof(handle->subtoc.id));
	handle->subtoc.num_items = 0;
	memset(handle->subtoc.cache, 0xFF, sizeof(handle->subtoc.cache));
	handle->subtoc.cached_section = 0;

	/* Try to get the sub TOC entry in the root TOC */
	subtoc_entry = tocparser_get_entry_subtoc(handle->main_toc);

	if (subtoc_entry == NULL) {
		TP_DBG(printf("No sub TOC found\n"));
		return 0;
	}

	/* We found a sub TOC in the root TOC, initialize the struct */
	TP_DBG(printf("A sub TOC exists\n"));

	offset = subtoc_entry->offset;
	if (lseek64(handle->fd, offset, SEEK_SET) != offset) {
		TP_DBG(printf("Error seeking to the sub TOC!\n"));
		return -1;
	}

	n = read(handle->fd,
	          handle->subtoc.cache,
		  SUPPORTED_SECTOR_SIZE);

	if (n != SUPPORTED_SECTOR_SIZE) {
		TP_DBG(printf("Error trying to read the sub TOC!\n"));
		return -1;
	}

	TP_DBG(printf("sub TOC read OK\n"));

	/* Fill in the rest of the struct */
	memcpy(handle->subtoc.id, subtoc_entry->id,
		sizeof(handle->subtoc.id));
	/* TOC entries in the subtoc */
	handle->subtoc.num_items =
			subtoc_entry->size / (sizeof(tocparser_toc_entry_t));
	/* Location of the subtoc in blocks */
	handle->subtoc.location = subtoc_entry->offset;

	/* In case subtoc_entry->size is hard-coded to be the maximum size */
	n = 0;
	do {
		toc_entry_in_subtoc = tocparser_get_subtoc_entry_num(handle->fd,
							n, &handle->subtoc);
		n++;
	} while (toc_entry_in_subtoc && memcmp(toc_entry_in_subtoc->id,
					TOC_ID_NO_ENTRY, TOC_ID_LENGTH));

	if (handle->subtoc.num_items != n - 1)
		TP_DBG(printf("num_items = %d, "
			      "but there were only %d entries\n",
			      handle->subtoc.num_items, n - 1));
	handle->subtoc.num_items = n - 1;

	return 0;
}

/**
 * tocparser_read() Read the toc from the device. This function will also read
 *		    the sub toc.
 *
 * @param [in] handle	Pointer to our running state.
 * @return Returns 0 on success.
 */
static int tocparser_read(tocparser_handle_t *handle)
{
	uint32_t i = 0;

	TP_DBG_FUNC();

	do {
		off_t offset = TOC_ROOT_OFFSET + i * TOC_BOOT_IMAGE_SIZE;

		TP_DBG(printf("TOC: #%d\n", i + 1));

		/* Read up what should be the TOC */
		if (lseek(handle->fd, offset, SEEK_SET) != offset) {
			TP_DBG(printf("Failed to seek to toc\n"));
			return 1;
		}

		if (read(handle->fd,
			  handle->main_toc,
			  SUPPORTED_SECTOR_SIZE) != SUPPORTED_SECTOR_SIZE) {
			TP_DBG(printf("Failed to read main toc!\n"));
			return 1;
		}

		/* Search for TOC identifier to validate the TOC */
		if (is_toc(&handle->main_toc[0]) == 0) {
			handle->toc_location = offset;
			TP_DBG(printf("TOC found, TOC location: %x.\n",
				      handle->toc_location));
		}
		i++;
	} while ((handle->toc_location == NO_TOC) && (i < MAX_NUM_TOCS));

	if (handle->toc_location == NO_TOC) {
		TP_DBG(printf("Could not find any toc\n"));
		return -1;
	}

	/* We found a valid TOC */
	tocparser_recalc_entry_offsets(handle, &handle->main_toc[0]);
	TP_DBG(printf("TOC now exists\n"));
	/* Try to find a sub TOC as well */
	if (tocparser_read_subtoc(handle)) {
		TP_DBG(printf("Failed to read subtoc!\n"));
		return -1;
	}

	return 0;
}

/**
 * tocparser_sync() Sync data buffers for the device of file we are handling.
 *
 * @param [in] handle	Pointer to our running state.
 * @return Returns 0 on success.
 */
static int tocparser_sync(tocparser_handle_t *handle)
{
	int err;
	struct stat buff;

	TP_DBG_FUNC();

	if ((err = fstat(handle->fd, &buff))) {
		TP_DBG(printf("fstat call on fd failed with errno = %d\n",
				errno));
		return -1;
	}

	if (S_ISBLK(buff.st_mode)) {
		sync();
	}
	else {
		if ((err = fsync(handle->fd))) {
			TP_DBG(printf("fsync call on fd failed with errno"
					" = %d\n", errno));
			return -1;
		}
	}

	return 0;
}

/**
 * tocparser_main_toc_get_first_last_entry_in_range() Find either the first or
 *						     the last entry in a given
 *						     range.
 *
 * @param [in] main_toc		Pointer to the main toc table.
 * @param [in] startaddr	Start of the range.
 * @param [in] endaddr		End of the range (first byte not part of the
 *				range).
 * @param [in] find_last	True if we are looking for the last entry in the
 *				range, if not, then we are looking for the first
 *				entry in the range.
 * @return Returns the first/last entry found in range or NULL if no entry was
 *	   found.
 */
static tocparser_toc_entry_t *tocparser_main_toc_get_first_last_entry_in_range(
						tocparser_toc_entry_t *main_toc,
						uint64_t startaddr,
						uint64_t endaddr,
						bool find_last)
{
	uint32_t i;
	tocparser_toc_entry_t *first_entry = NULL;
	tocparser_toc_entry_t *last_entry = NULL;

	TP_DBG_FUNC();

	TP_DBG(printf("Criteria: startaddr = 0x%08llx, endaddr = 0x%08llx, "
		      "find_last = %u\n", startaddr, endaddr, find_last));

	/* Find TOC entry */
	for (i = 0; i < TOC_MAX_ENTRIES; i++, main_toc++) {
		/*
		 * Don't iterate the rest of the entries
		 * if an empty entry was found
		 */
		if (memcmp(main_toc->id,
			    TOC_ID_NO_ENTRY,
			    TOC_ID_LENGTH) == 0)
			break;

		if ((main_toc->flags & FLAGS_MASK) == FLAGS_PARTITION)
			continue;

		if (get_offset(main_toc) < startaddr ||
		    get_offset(main_toc) >= endaddr) {
			TP_DBG(printf("Ignoring entry %u, startaddr = 0x%08llx, "
			      "endaddr = 0x%08llx\n", i, get_offset(main_toc),
			      get_offset(main_toc) + get_size(main_toc)));
			continue;
		}

		if (first_entry == NULL ||
		    get_offset(main_toc) < get_offset(first_entry)) {
			first_entry = main_toc;
		}

		if (last_entry == NULL ||
		    get_offset(main_toc) > get_offset(last_entry)) {
			last_entry = main_toc;
		}

		if (last_entry == main_toc ||
		    first_entry == main_toc)
			TP_DBG(printf("Selecting entry %u, startaddr = 0x%08llx, "
				      "endaddr = 0x%08llx\n", i, get_offset(main_toc),
				       get_offset(main_toc) + get_size(main_toc)));
	}

	return find_last ? last_entry : first_entry;
}

/**
 * tocparser_main_toc_add_entry() Add a toc entry to the main toc. Entries will
 *				  be inserted first of least depending on input.
 *
 * @param [in] main_toc		Pointer to the main toc.
 * @param [in] entry		The entry we want to insert.
 * @param [in/out] position	Position where the entry should be inserted.
 *                          If equal to TOC_MAX_ENTRIES the entry will be
 *                          inserted in the first free entry and the position
 *                          will be returned in this variable.
 *
 * @return Returns 0 on success.
 */
static int tocparser_main_toc_add_entry(tocparser_toc_entry_t *main_toc,
					tocparser_toc_entry_t *entry,
					uint8_t *position)
{
	uint32_t first_free;
	uint32_t i = 0;

	TP_DBG_FUNC();

	/* There should be only one Sub TOC. If there is existing one remove it */
	if ((entry->flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
		for (i = 0; i < TOC_MAX_ENTRIES; i++) {
			if ((main_toc[i].flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
				if (tocparser_main_toc_remove_entry(main_toc, &main_toc[i])) {
					TP_DBG(printf("Failed to remove entry from main toc\n"));
					return -1;
				}
				break;
			}
		}
	}

	for (first_free = 0; first_free < TOC_MAX_ENTRIES; first_free++) {
		if (!memcmp(main_toc[first_free].id,
				TOC_ID_NO_ENTRY,
				TOC_ID_LENGTH))
		break;
	}

	if (first_free == TOC_MAX_ENTRIES) {
		TP_DBG(printf("No more space for new entries!\n"));
		return -1;
	}

	if (*position == TOC_MAX_ENTRIES) {
		TP_DBG(printf("Inserting last at position %d\n", first_free));
		main_toc[first_free] = *entry;
		*position = first_free;
		first_free++;
		if (first_free < TOC_MAX_ENTRIES) {
			memcpy(main_toc[first_free].id,
				TOC_ID_NO_ENTRY,
				TOC_ID_LENGTH);
		}
	}
	else {
		TP_DBG(printf("Inserting at position %d\n", *position));
		memmove(&main_toc[*position + 1],
			&main_toc[*position],
			SUPPORTED_SECTOR_SIZE - ((*position + 1) * sizeof(tocparser_toc_entry_t)));
		main_toc[*position] = *entry;
	}

	return 0;
}

/**
 * tocparser_boot_block_rewrite_specific() Rewrite one of the boot blocks at the
 *					   given offset.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] offset		The offset where we want to rewrite the boot
 *				block.
 * @param [in] only_toc		If true we will only rewrite the toc and not the
 *				data content of the boot block.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_rewrite_specific(tocparser_handle_t *handle,
						off_t offset,
						bool only_toc)
{
	tocparser_toc_entry_t out_toc[TOC_MAX_ENTRIES];
	TP_DBG_FUNC();

	if (!only_toc) {
		if (lseek(handle->fd, offset, SEEK_SET) != offset) {
			TP_DBG(printf("Failed to seek to toc\n"));
			return 1;
		}

		/* First sector in boot_block_buffer contains all 0, making this
		 * boot block invalid while we are updating it
		 */
		if (write(handle->fd,
			  handle->boot_block_buffer,
			  SUPPORTED_SECTOR_SIZE) !=
				SUPPORTED_SECTOR_SIZE)
			return 1;

		if (tocparser_sync(handle))
			return 1;

		/* Write the data part of the boot block */
		if (write(handle->fd,
			  handle->boot_block_buffer + SUPPORTED_SECTOR_SIZE,
			  TOC_BOOT_IMAGE_SIZE - SUPPORTED_SECTOR_SIZE) !=
				TOC_BOOT_IMAGE_SIZE - SUPPORTED_SECTOR_SIZE) {
			TP_DBG(printf("Writing boot block content failed\n"));
			return 1;
		}

		if (tocparser_sync(handle)) {
			TP_DBG(printf("Syncing boot block content failed\n"));
			return 1;
		}
	}

	tocparser_generate_toc_sector(offset,
			handle->main_toc,
			out_toc);

	/* Write the toc to the boot block */
	if (lseek(handle->fd, offset, SEEK_SET) != offset) {
		TP_DBG(printf("Failed to seek to toc\n"));
		return 1;
	}

	if (write(handle->fd,
		  out_toc,
		  SUPPORTED_SECTOR_SIZE) != SUPPORTED_SECTOR_SIZE) {
		TP_DBG(printf("Failed to write new toc\n"));
		return 1;
	}

	if (tocparser_sync(handle)) {
		TP_DBG(printf("Failed to sync new toc\n"));
		return 1;
	}

	return 0;
}

/**
 * tocparser_boot_block_rewrite() Rewrite all of the boot blocks. We do not know
 *				  the states of the other tocs except the one we
 *				  read from flash, update that one last in an
 *				  attemt to make sure that there is at least
 *				  one valid toc.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] only_toc		If true we will only rewrite the toc and not the
 *				data content of the boot blocks.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_rewrite(tocparser_handle_t *handle,
					bool only_toc)
{
	off_t offset;
	uint32_t i = 0;
	TP_DBG_FUNC();

	for (i = 0; i < MAX_NUM_TOCS; i++) {
		offset = TOC_ROOT_OFFSET + i * TOC_BOOT_IMAGE_SIZE;

		if (offset == handle->toc_location)
			continue;

		TP_DBG(printf("TOC: #%d (0x%08x)\n", i + 1, (uint32_t) offset));

		if (tocparser_boot_block_rewrite_specific(handle,
							  offset,
							  only_toc)) {
			TP_DBG(printf("Re-write failed\n"));
			return 1;
		}
	}

	TP_DBG(printf("TOC: (0x%08x)\n", (uint32_t) handle->toc_location));
	if (tocparser_boot_block_rewrite_specific(handle,
						  handle->toc_location,
						  only_toc))
		return 1;

	return 0;
}

/**
 * tocparser_boot_block_make_space_for_issw() Move the content of the boot block
 *					      to make space for ISSW as it needs
 *					      to be located at the start of the
 *					      boot block. Also update the
 *					      offsets for moved entries. Caller
 *					      needs to make shure that there is
 *					      enough space to fit the issw.
 *
 * @param [in] handle	Handle to our running state.
 * @param [in] main_toc	Buffer containing the main toc.
 * @param [in] issw	Entry describing the ISSW.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_make_space_for_issw(tocparser_handle_t *handle,
						tocparser_toc_entry_t *main_toc,
						tocparser_toc_entry_t *issw)
{
	uint32_t i;
	uint32_t issw_size = ROUND_CEIL(issw->size, SUPPORTED_SECTOR_SIZE);
	TP_DBG_FUNC();

	TP_DBG(printf("issw_size = %u\n",issw_size));

	memmove(handle->boot_block_buffer + SUPPORTED_SECTOR_SIZE + issw_size,
		handle->boot_block_buffer + SUPPORTED_SECTOR_SIZE,
		TOC_BOOT_IMAGE_SIZE - SUPPORTED_SECTOR_SIZE - issw_size);

	/* Increment all TOC entry offsets within the boot blocks to accomodate
	 * for ISSW
	 */
	for (i = 0; i < TOC_MAX_ENTRIES; i++, main_toc++) {
		if (!memcmp(main_toc->id, TOC_ID_NO_ENTRY, TOC_ID_LENGTH))
			continue;

		if (get_offset(main_toc) < TOC_BOOT_IMAGE_SIZE) {
			TP_DBG(printf("Entry will be moved, offset 0x%08llx, "
					"size = 0x%08llx\n", get_offset(main_toc),
					get_size(main_toc)));
			set_offset(main_toc, get_offset(main_toc) + issw_size);
		}
	}

	return 0;
}

/**
 * tocparser_boot_block_insert() This method will insert an entry (and it's
 *				 data) in the boot block. This method will
 *				 check if the entry we are about to insert
 *				 is ISSW. In that case we will insert it
 *				 at the start of the boot block (just after
 *				 the toc).
 *
 * @param [in] handle	Handle to our running state.
 * @param [in] entry	The entry we want to insert.
 * @param [in] data	The data content for the entry we are trying to insert.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_insert(tocparser_handle_t *handle,
				       tocparser_toc_entry_t *entry,
				       const uint8_t *data)
{
	uint32_t insert_location = SUPPORTED_SECTOR_SIZE;
	uint8_t position = TOC_MAX_ENTRIES;

	TP_DBG_FUNC();

	if (memcmp(entry->id, TOC_MAGIC, sizeof(TOC_MAGIC))) {
		tocparser_toc_entry_t *last = tocparser_main_toc_get_first_last_entry_in_range(
				&handle->main_toc[0],
				SUPPORTED_SECTOR_SIZE,
				TOC_BOOT_IMAGE_SIZE,
				true);

		if (last) {
			TP_DBG(printf("Could fetch entry in boot block,"
						" insert after\n"));
			if (get_offset(last) > UINT_MAX)
				return 1;
			insert_location = (uint32_t) get_offset(last) + get_size(last);
			insert_location = ROUND_CEIL(insert_location,
					SUPPORTED_SECTOR_SIZE);
		}

		if (insert_location + get_size(entry) > TOC_BOOT_IMAGE_SIZE) {
			TP_DBG(printf("Error: out of bounds, location+size = "
					"0x%08llx\n",
					(uint64_t) insert_location + get_size(entry)));
			return 1;
		}
	}
	else {
		if (tocparser_boot_block_make_space_for_issw(handle,
							&handle->main_toc[0],
							entry))
			return 1;
		/* ISSW TOC, set toc position to 0 */
		position = 0;
	}

	set_offset(entry, insert_location);

	TP_DBG(printf("insert location = 0x%08x\n", insert_location));

	if (tocparser_main_toc_add_entry(&handle->main_toc[0], entry, &position))
		return 1;

	memcpy(handle->boot_block_buffer + insert_location, data,
						(uint32_t) get_size(entry));
	if (get_size(entry) % SUPPORTED_SECTOR_SIZE) {
		uint8_t *buf = handle->boot_block_buffer +
				insert_location +
				(uint32_t) get_size(entry);

		memset(buf,
		       0xFF,
		       SUPPORTED_SECTOR_SIZE -
				(uint32_t) get_size(entry) % SUPPORTED_SECTOR_SIZE);
	}

	return 0;
}

/**
 * tocparser_boot_block_pack() Pack the content of the boot block.
 *			       Align all of the moved entries on
 *			       SUPPORTED_SECTOR_SIZE bounderies.
 *
 * @param [in] handle	Handle to our running state.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_pack(tocparser_handle_t *handle)
{
	tocparser_toc_entry_t *entry;
	uint32_t new_start_location = SUPPORTED_SECTOR_SIZE;

	TP_DBG_FUNC();

	entry = tocparser_main_toc_get_first_last_entry_in_range(
					&handle->main_toc[0],
					SUPPORTED_SECTOR_SIZE,
					TOC_BOOT_IMAGE_SIZE,
					false);

	while (entry != NULL) {
		if (new_start_location < get_offset(entry)) {
			TP_DBG(printf("Moving entry at 0x%08llx, size: 0x%08llx"
				      " to 0x%08x", get_offset(entry),
				      get_size(entry), new_start_location));

			memmove(handle->boot_block_buffer + new_start_location,
				handle->boot_block_buffer + (uint32_t) get_offset(entry),
				(uint32_t) get_size(entry));

			memset(handle->boot_block_buffer + new_start_location +
				(uint32_t) get_size(entry),
				0xFF,
				(uint32_t) get_offset(entry) - new_start_location);

			set_offset(entry, new_start_location);
		}

		new_start_location = (uint32_t) get_offset(entry) +
						(uint32_t) get_size(entry);
		new_start_location = ROUND_CEIL(new_start_location,
						SUPPORTED_SECTOR_SIZE);

		entry = tocparser_main_toc_get_first_last_entry_in_range(
						&handle->main_toc[0],
						get_offset(entry) + get_size(entry),
						TOC_BOOT_IMAGE_SIZE,
						false);
	}

	return 0;
}

/**
 * tocparser_boot_block_calc_free_space() Calculate the amount of free space in
 *					  boot block. This function will take
 *					  space consumed by re-alignment of
 *					  content into account.
 *
 * @param [in] main_toc		Pointer to the main toc.
 * @param [in] exlude		Entry that will be excluded when calulating the
 *				amount of space consumed by content in the boot
 *				block. This is likely an entry that is to be
 *				replaced.
 * @param [out] free_space	Amount of free space left in the boot block.
 * @return Returns 0 on success.
 */
static int tocparser_boot_block_calc_free_space(tocparser_toc_entry_t *main_toc,
						tocparser_toc_entry_t *exclude,
						uint32_t *free_space) {
	uint32_t int_free_space = TOC_BOOT_IMAGE_SIZE - SUPPORTED_SECTOR_SIZE;
	tocparser_toc_entry_t *entry;

	TP_DBG_FUNC();

	entry = tocparser_main_toc_get_first_last_entry_in_range(
					main_toc,
					SUPPORTED_SECTOR_SIZE,
					TOC_BOOT_IMAGE_SIZE,
					false);
	while (entry != NULL) {
		if (entry != exclude) {
			uint32_t space_consumed = (uint32_t) get_size(entry);
			space_consumed = ROUND_CEIL(space_consumed,
						    SUPPORTED_SECTOR_SIZE);
			TP_DBG(printf("Found entry, offset: 0x%08llx,"
				      " size: 0x%08llx, space consumed: 0x%08x\n",
				      get_offset(entry),
				      get_size(entry),
				      space_consumed));

			/* As we are ceiling the values with
			 * SUPPORTED_SECTOR_SIZE, if the boot area contains alot
			 * of smal sections we might consume more space then is
			 * present in the boot block. '
			 */
			if (space_consumed > int_free_space) {
				TP_DBG(printf("No free space left, aborting."));
				int_free_space = 0;
				break;
			}
			int_free_space -= space_consumed;
		}

		entry = tocparser_main_toc_get_first_last_entry_in_range(
						main_toc,
						get_offset(entry) + get_size(entry),
						TOC_BOOT_IMAGE_SIZE,
						false);
	}

	TP_DBG(printf("free space = %u\n", int_free_space));

	*free_space = int_free_space;
	return 0;

}

/**
 * tocparser_subtoc_cache_write() Write the cached sector of the subtoc.
 *
 * @param [in] fd	FD for the device we want to write to.
 * @param [in] subtoc	The subtoc we want to write.
 * @return Returns 0 on success.
 */
static int tocparser_subtoc_cache_write(int fd,
				        tocparser_subtoc_t *subtoc)
{
	off64_t offset;

	TP_DBG_FUNC();

	offset = subtoc->location +
		 subtoc->cached_section * SUPPORTED_SECTOR_SIZE;

	if (lseek64(fd, offset, SEEK_SET) != offset) {
		TP_DBG(printf("failed to seek to sub TOC\n"));
		return 1;
	}

	if (write(fd,
		  &subtoc->cache,
		  SUPPORTED_SECTOR_SIZE) != SUPPORTED_SECTOR_SIZE) {
		TP_DBG(printf("failed to write sub TOC\n"));
		return 1;
	}

	TP_DBG(printf("Sub TOC cache written\n"));
	return 0;
}

/**
 * tocparser_set_entry() Update the values of a toc entry.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] toc_id		Entry we want to update.
 * @param [in] toc		Pointer to the main toc.
 * @param [in] subtoc		Pointer to the subtoc.
 * @param [in] size		Pointer to variable containing the new
 *				size, if this is NULL the old value
 *				will be kept.
 * @param [in] loadaddr		Pointer to variable containing the new
 *				loadaddr, if this is NULL the old value
 *				will be kept.
 * @return Returns 0 on success.
 */
static int tocparser_set_entry(tocparser_handle_t *handle,
			       const char *toc_id,
			       tocparser_toc_entry_t *toc,
			       tocparser_subtoc_t *subtoc,
			       uint32_t *size,
			       uint32_t *loadaddr)
{
	TP_DBG_FUNC();

	bool in_sub_toc;
	tocparser_toc_entry_t *toc_entry;

	toc_entry = tocparser_get_entry(handle->fd, toc_id, false,
					toc,
					subtoc,
					&in_sub_toc);

	if (toc_entry != NULL) {
		int err;

		if (size)
			set_size(toc_entry, *size);
		if (loadaddr)
			toc_entry->loadaddr = *loadaddr;

		/* Power will not failed from this point and
		 * onwards as promised by the project.
		 *
		 * It has been stated that this is good enough (tm) for eMMC.
		 */

		if (in_sub_toc) {
			if ((toc_entry->flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
				TP_DBG(printf("Error: SUBTOC located inside"
						" of SUBTOC\n"));
				err = -1;
			}
			else
				err = tocparser_subtoc_cache_write(handle->fd,
							   &handle->subtoc);
		} else {
			err = tocparser_boot_block_rewrite(handle, true);
		}

		if (!err)
			err = tocparser_sync(handle);

		/* Now it is ok for power to fail again
		 * (at least from the toc parser point of
		 * view).
		 */

		if (err)
			return -1;

		return 0;
	}

	TP_DBG(printf("TOC Entry: %s not found!\n", toc_id));

	return -1;
}

/**
 * tocparser_add_subtoc_entry() Add sub toc entry to subtoc in handle.
 *
 * @param [in] handle   Handle to our running state..
 * @param [in] toc      toc entry for subtoc
 * @return Returns 0 on success.
 */
static int tocparser_add_subtoc_entry(tocparser_handle_t *handle,
					tocparser_toc_entry_t *toc)
{
	uint8_t entry_no = TOC_MAX_ENTRIES;
	tocparser_toc_entry_t *subtoc = NULL;
	uint32_t i = 0;

	TP_DBG_FUNC();

	/* First check if there is SUB TOC in the main TOC */
	for (i = 0; i < TOC_MAX_ENTRIES; i++) {
		if ((handle->main_toc[i].flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
			subtoc = &handle->main_toc[i];
			break;
		}
	}
	if (!subtoc)
		return -1;

	/* Cache the section in which the subtoc should be inserted */
	if (tocparser_subtoc_cache_read(handle->fd, handle->subtoc.num_items, &handle->subtoc))
		return -1;

	/* Calculate the first empty subtoc position in the cached section*/
	entry_no = handle->subtoc.num_items % TOC_MAX_ENTRIES;

	/* Insert the new subtoc entry in the cached section */
	handle->subtoc.cache[entry_no] = *toc;

	/* Write the cached subtoc section on flash */
	if (tocparser_subtoc_cache_write(handle->fd, &handle->subtoc))
		return -1;

	handle->subtoc.num_items += 1;

	/* Increase the size of the SUB TOC entry */
	subtoc->size += sizeof(tocparser_toc_entry_t);

	return 0;
}


/**
 * tocparser_inittoc() Init the in-ram toc structure by reading it from flash.
 *
 * @param [in] handle		Handle to our running state.
 * @return Returns 0 on success.
 */
static int tocparser_inittoc(tocparser_handle_t *handle)
{
	TP_DBG_FUNC();


	if (handle->toc_location == NO_TOC) {
		/* Read TOC to see if we have a valid TOC */
		if (tocparser_read(handle) == 0)
			return 0;
	} else
		/*
		 * We are trying to initialize the TOC again
		 * for the same device
		 */
		return 0;

	/* No valid TOC found */
	return 1;
}

/**
 * tocparser_main_toc_remove_entry() Remove an entry located in the main toc.
 *
 * @param [in] main_toc main toc.
 * @param [in] entry	Entry to remove
 * @return Returns 0 on success.
 */
static int tocparser_main_toc_remove_entry(tocparser_toc_entry_t *main_toc,
					   tocparser_toc_entry_t *entry)
{
	TP_DBG_FUNC();

	memmove(&entry[0],
		&entry[1],
		SUPPORTED_SECTOR_SIZE - ((uint32_t)&entry[1] - (uint32_t)&main_toc[0]));

	memcpy(main_toc[TOC_MAX_ENTRIES - 1].id,
	       TOC_ID_NO_ENTRY,
	       TOC_ID_LENGTH);

	return 0;
}

/**
 * tocparser_sub_toc_remove_entry() Remove an entry located in the sub toc.
 *
 * @param [in] handle	Handle to our running state.
 * @param [in] entry	Entry to be removed
 * @return Returns 0 on success.
 */
static int tocparser_sub_toc_remove_entry(tocparser_handle_t *handle,
					tocparser_toc_entry_t *entry)
{
	uint32_t i = 0;
	uint8_t entry_num_in_section = TOC_MAX_ENTRIES;
	tocparser_toc_entry_t first_entry_previous;
	tocparser_toc_entry_t last_entry_next;
	uint32_t last_section = 0;
	uint32_t cached_section = 0;
	tocparser_toc_entry_t *subtoc_entry = NULL;
	uint32_t subtoc_size = 0;

	TP_DBG_FUNC();

	/* Check if there is at least one entry in the SUBTOC */
	if (handle->subtoc.num_items == 0)
		return -1;

	/* Try to find the entry by it's ID and then remove it */

	if (!tocparser_get_subtoc_entry_id( handle->fd,
				(const char *)entry->id,
				false,
				(tocparser_subtoc_t *)&handle->subtoc,
				&i))
		return -1;

	entry_num_in_section = i % TOC_MAX_ENTRIES;

	/* Remove entry */
	if (entry_num_in_section < (TOC_MAX_ENTRIES - 1)) {
		memmove(&handle->subtoc.cache[entry_num_in_section],
			&handle->subtoc.cache[entry_num_in_section + 1],
			SUPPORTED_SECTOR_SIZE - ((entry_num_in_section + 1) * sizeof(tocparser_toc_entry_t)));
	}

	memcpy(handle->subtoc.cache[TOC_MAX_ENTRIES - 1].id,
		TOC_ID_NO_ENTRY,
		TOC_ID_LENGTH);

	if ((tocparser_subtoc_cache_write(handle->fd, &handle->subtoc) != 0))
		return -1;

	/* Shift the entries in all the rest of the sections */
	last_section = handle->subtoc.num_items / TOC_MAX_ENTRIES;
	cached_section = handle->subtoc.cached_section;

	/* Check if the current section is the last one */
	if (last_section != cached_section) {
		/* Go through all the rest of the sections, shift the entries back for 1 entry and write them on flash */
		i = last_section;
		do {
			if (tocparser_subtoc_cache_read(handle->fd, (i * TOC_MAX_ENTRIES), &handle->subtoc) != 0)
				return -1;

			if (i != cached_section) {
				/* Save the first entry from current section */
				first_entry_previous = handle->subtoc.cache[0];

				/* Shift the entries for one position */
				memmove(&handle->subtoc.cache[0], &handle->subtoc.cache[1], (SUPPORTED_SECTOR_SIZE - sizeof(tocparser_toc_entry_t)));
			}

			if (i != last_section) {
				/* Write the first entry form the previous section as last in the current one */
				handle->subtoc.cache[TOC_MAX_ENTRIES - 1] = last_entry_next;
			} else {
				memcpy(handle->subtoc.cache[TOC_MAX_ENTRIES - 1].id,
					TOC_ID_NO_ENTRY,
					TOC_ID_LENGTH);
			}
			last_entry_next = first_entry_previous;

			/* Write current section on flash */
			if ((tocparser_subtoc_cache_write(handle->fd, &handle->subtoc) != 0))
				return -1;

			i-= 1;

		} while (i != cached_section - 1);
	}

	handle->subtoc.num_items -= 1;

	/* Now reduce the size parameter of the SUB TOC in the main TOC */

	subtoc_entry = tocparser_get_entry_subtoc(handle->main_toc);

	if (subtoc_entry == NULL) {
		TP_DBG(printf("No sub TOC found\n"));
		return 0;
	}

	if (subtoc_entry->size < sizeof(tocparser_toc_entry_t))
		return -1;

	subtoc_size = subtoc_entry->size - sizeof(tocparser_toc_entry_t);

	if (tocparser_set_entry(handle, (const char *)subtoc_entry->id,
				(tocparser_toc_entry_t *)&handle->main_toc[0],
				(tocparser_subtoc_t *)&handle->subtoc,
				&subtoc_size,
				NULL)) {
		TP_DBG(printf("Failed to update TOC Entry: %s!\n", (const char *)subtoc_entry->id));
		return -1;
	}

	return 0;
}
/**
 * tocparser_write_partition_content_from_fd() Update the data content of a toc
 *					       entry. Data is fetched from a
 *					       file.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] filedescriptor	FD containing the new data for the entry.
 * @param [in] size		Size of the data to write.
 * @return Returns  0 on success.
 */
static int tocparser_write_partition_content_from_fd(tocparser_handle_t *handle,
						     int filedescriptor,
						     uint32_t size)
{
	uint32_t written_size = 0;
	uint8_t *buff = &handle->write_buff[0];

	TP_DBG_FUNC();

	TP_DBG(printf("Bytes to write: %u\n", size));

	while(written_size < size) {
		uint32_t write_chunk_size;
		uint32_t chunk_size = size - written_size;

		chunk_size = MIN(chunk_size, WRITE_BUFF_SIZE);

		write_chunk_size = ROUND_CEIL(chunk_size,
					      SUPPORTED_SECTOR_SIZE);

		if (write_chunk_size > chunk_size)
			memset(buff + chunk_size,
			       0xFF,
			       write_chunk_size - chunk_size);

		TP_DBG(printf("Reading %u bytes\n", chunk_size));
		if (read(filedescriptor, buff, chunk_size) !=
				(off_t) chunk_size) {
			TP_DBG(printf("Failed to read\n"));
			return 1;
		}

		TP_DBG(printf("Writing %u bytes\n", write_chunk_size));
		if (write(handle->fd, buff, write_chunk_size) !=
				(off_t) write_chunk_size) {
			TP_DBG(printf("Failed to write\n"));
			return -1;
		}

		written_size += chunk_size;

		TP_DBG(printf("Written bytes: %u\n", written_size));
	}

	if (tocparser_sync(handle))
		return 1;

	return 0;
}

/**
 * tocparser_write_partition_content_from_buffer() Update the data content of a
 *						   toc entry. Data is fetched
 *						   from an in-ram buffer.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] data		Buffer containing the new data.
 * @param [in] size		Size of the data to write.
 * @return Returns  0 on success.
 */
static int tocparser_write_partition_content_from_buffer(tocparser_handle_t *handle,
							 uint8_t *data,
							 uint32_t size)
{
	uint8_t *buff = &handle->write_buff[0];
	uint32_t chunk_size = ROUND_FLOOR(size, SUPPORTED_SECTOR_SIZE);

	if (chunk_size != 0 &&
	    write(handle->fd, data, chunk_size) !=
			(off_t) chunk_size) {
		return 1;
	}

	if (chunk_size != size) {
		memset(buff, 0xFF, SUPPORTED_SECTOR_SIZE);
		memcpy(buff, data + chunk_size, size - chunk_size);
		if (write(handle->fd, buff, SUPPORTED_SECTOR_SIZE) !=
				SUPPORTED_SECTOR_SIZE) {
			return 1;
		}
	}

	return 0;
}

/**
 * tocparser_write_partition_content() Update the data content of a toc entry.
 *				       This call will not change the content of
 *				       the toc itself.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] target		Entry we want to update.
 * @param [in] data		Buffer containing the new data, can be NULL. In
 *				that case the filedescriptor is used to read the
 *				new data of the entry.
 * @param [in] filedescriptor   FD containing the new data if data ptr is NULL.
 * @param [in] size		Size of the data to write.
 * @return Returns  0 on success.
 */
static int tocparser_write_partition_content(tocparser_handle_t *handle,
					     tocparser_toc_entry_t *target,
					     uint8_t *data,
					     int filedescriptor,
					     uint32_t size)
{
	TP_DBG_FUNC();
	off64_t offset = get_offset(target);

	if (lseek64(handle->fd, offset, SEEK_SET) != offset)
		return 1;

	if (data) {
		if (tocparser_write_partition_content_from_buffer(handle,
								 data,
								 size))
			return 1;
	}
	else {
		if (tocparser_write_partition_content_from_fd(handle,
							     filedescriptor,
							     size))
			return 1;
	}

	if (tocparser_sync(handle))
		return 1;

	return 0;
}

/**
 * tocparser_normal_write_toc_entry() Update the content of a toc entry where
 *				      the data is not located in the boot block.
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] toc_id		Id of the toc entry we want to update.
 * @param [in] target		Entry we want to update.
 * @param [in] data		Buffer containing the new data, can be NULL. In
 *				that case the filedescriptor is used to read the
 *				new data of the entry.
 * @param [in] filedescriptor   FD containing the new data if data ptr is NULL.
 * @param [in] size		Size of the data to write.
 * @param [in] loadaddr		Pointer to variable containing the new
 *				loadaddr, if this is NULL the old value
 *				will be kept.
 * @return Returns TOCPARSER_OK on success.
 */
static tocparser_error_code_t tocparser_normal_write_toc_entry(tocparser_handle_t *handle,
							       const char *toc_id,
							       tocparser_toc_entry_t *target,
							       uint8_t *data,
							       int filedescriptor,
							       uint32_t size,
							       uint32_t *loadaddr)
{
	TP_DBG_FUNC();

	//When we read the part entry our target ptr might become invalid.
	tocparser_toc_entry_t temp_target = *target;
	tocparser_toc_entry_t *part;

	if (get_offset(target) % SUPPORTED_SECTOR_SIZE)
		return TOCPARSER_ERR_INVALID_TOC; /* Bad alignment! */

	if ((target->flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
		TP_DBG(printf("Error: Trying to write SUBTOC.\n"));
		return TOCPARSER_WRITE_FAILED;
	}

	part = tocparser_get_entry(handle->fd, toc_id, true,
			(tocparser_toc_entry_t *)&handle->main_toc[0],
			(tocparser_subtoc_t *)&handle->subtoc,
			NULL);

	if ((part && size > get_size(part)) ||
	    (!part && size > get_size(&temp_target))) {
			TP_DBG(printf("Error: Not enough space\n"));
			return TOCPARSER_ERR_OUT_OF_SPACE;
	}

	if (tocparser_write_partition_content(handle,
						&temp_target,
						data,
						filedescriptor,
						size)) {
		TP_DBG(printf("Error: Failed to write content\n"));
		return TOCPARSER_WRITE_FAILED;
	}

	if(tocparser_set_entry(handle,
				toc_id,
				&handle->main_toc[0],
				&handle->subtoc,
				&size,
				loadaddr)) {
		TP_DBG(printf("Error: Failed to set new entry data\n"));
		return TOCPARSER_SET_ENTRY_FAILED;
	}

	return TOCPARSER_OK;
}

/**
 * tocparser_boot_block_write_toc_entry() Update the content of a
 *					  toc entry where the data
 *					  is located in the boot block
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] target		Entry we want to update.
 * @param [in] data		Buffer containing the new data.
 * @param [in] size		Size of the data to write.
 * @param [in] loadaddr		Pointer to variable containing the new
 *				loadaddr, if this is NULL the old value
 *				will be kept.
 * @return Returns the requested entry of NULL if not found.
 */
static tocparser_error_code_t tocparser_boot_block_write_toc_entry(
						tocparser_handle_t *handle,
						tocparser_toc_entry_t *target,
						uint8_t *data,
						uint32_t size,
						uint32_t *loadaddr)
{
	tocparser_toc_entry_t temp;
	uint32_t free_space;

	TP_DBG_FUNC();

	if (tocparser_boot_block_load(handle))
		return TOCPARSER_READ_FAILED;

	if (tocparser_boot_block_calc_free_space(&handle->main_toc[0],
							target,
							&free_space)) {
		TP_DBG(printf("Failed to calculate free space\n"));
		return TOCPARSER_ERR_OUT_OF_SPACE;
	}

	if (free_space < size) {
		TP_DBG(printf("Not enough free space in the boot block.\n"));
		return TOCPARSER_ERR_OUT_OF_SPACE;
	}

	memset(handle->boot_block_buffer + (uint32_t) get_offset(target), 0xFF,
						(uint32_t) get_size(target));

	temp = *target;

	/* After this call the target pointer is invalid */
	if (tocparser_main_toc_remove_entry(&handle->main_toc[0], target)) {
		TP_DBG(printf("Failed to remove entry\n"));
		return TOCPARSER_SET_ENTRY_FAILED;
	}

	target = NULL;

	if (tocparser_boot_block_pack(handle)) {
		TP_DBG(printf("Failed to pack boot block\n"));
		return TOCPARSER_SET_ENTRY_FAILED;
	}

	if (loadaddr)
		temp.loadaddr = *loadaddr;

	set_size(&temp, size);

	if (tocparser_boot_block_insert(handle, &temp, data))
		return TOCPARSER_WRITE_FAILED;

	if (tocparser_boot_block_rewrite(handle, false))
		return TOCPARSER_WRITE_FAILED;

	return TOCPARSER_OK;
}

/**
 * tocparser_boot_block_write_toc_entry_from_fd() Update the content of a
 *						  toc entry where the data
 *						  is located in the boot block
 *
 * @param [in] handle		Handle to our running state.
 * @param [in] target		Entry we want to update.
 * @param [in] filedescriptor	FD for file containing the new data.
 * @param [in] size		Size of the data to write.
 * @param [in] loadaddr		Pointer to variable containing the new
 *				loadaddr, if this is NULL the old value
 *				will be kept.
 * @return Returns the requested entry of NULL if not found.
 */
static tocparser_error_code_t tocparser_boot_block_write_toc_entry_from_fd(
						tocparser_handle_t *handle,
						tocparser_toc_entry_t *target,
						int filedescriptor,
						uint32_t size,
						uint32_t *loadaddr)
{
	uint8_t *data = malloc(size);
	tocparser_error_code_t result;
	TP_DBG_FUNC();

	if (!data) {
		result = TOCPARSER_OUT_OF_MEMORY;
		goto exit;
	}

	if (read(filedescriptor, data, size) != (off_t) size) {
		result = TOCPARSER_READ_FAILED;
		goto exit;
	}

	result = tocparser_boot_block_write_toc_entry(handle,
						      target,
						      data,
						      size,
						      loadaddr);

exit:
	if (data)
		free(data);
	return result;
}

void tocparser_print_toc(tocparser_handle_t *handle)
{
	TP_DBG_FUNC();

	if (tocparser_print(handle) != 0)
		TP_DBG(printf("Failed to print TOC\n"));
}

tocparser_error_code_t tocparser_set_toc_entry(tocparser_handle_t *handle,
					       const char *toc_id,
					       uint32_t *size,
					       uint32_t *loadaddr)
{
	TP_DBG_FUNC();

	if (!handle || (handle->magic != TOC_HANDLE_MAGIC)) {
		TP_DBG(printf("tocparser_set_toc_entry: not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	if (handle->mode != TOCPARSER_READWRITE) {
		TP_DBG(printf("tocparser_set_toc_entry: read only\n"));
		return TOCPARSER_ERR_READONLY;
	}

	if (tocparser_set_entry(handle, toc_id,
				(tocparser_toc_entry_t *)&handle->main_toc[0],
				(tocparser_subtoc_t *)&handle->subtoc,
				size,
				loadaddr)) {
		TP_DBG(printf("Failed to update TOC Entry: %s!\n", toc_id));
		return TOCPARSER_SET_ENTRY_FAILED;
	}

	return TOCPARSER_OK;
}

tocparser_error_code_t tocparser_add_toc_entry(tocparser_handle_t *handle,
					tocparser_toc_entry_t *toc,
					tocparser_toc_entry_t *subtoc,
					uint8_t *toc_position)
{
	uint8_t position = TOC_MAX_ENTRIES;
	tocparser_error_code_t result;

	TP_DBG_FUNC();

	if (!handle || (handle->magic != TOC_HANDLE_MAGIC)) {
		TP_DBG(printf("tocparser_add_toc_entry: not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	if ((handle->mode != TOCPARSER_EMPTY_TOC) || ((toc == NULL) && (subtoc == NULL))) {
		TP_DBG(printf("tocparser_add_toc_entry: invalid param\n"));
		return TOCPARSER_ERR_INVALID_PARAM;
	}

	if (handle->toc_location == NO_TOC) {
		/* In this case the only valid entry is TOC_MAGIC */
		if (toc == NULL)
			return TOCPARSER_ERR_INVALID_PARAM;

		if (is_toc(toc) == 0) {
			handle->toc_location = TOC_ROOT_OFFSET;
			TP_DBG(printf("tocparser_add_toc_entry: TOC created, TOC location: 0x%x.\n", handle->toc_location));
		} else {
			return TOCPARSER_ERR_INVALID_PARAM;
		}
	}

	if (toc_position != NULL)
		position  = *toc_position;

	if (toc != NULL) {
		/* New TOC entry */
		if ((toc->flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
			/* New SUBTOC. The size should be 0 since all the sections are empty */
			if (toc->size > 0)
				return TOCPARSER_ERR_INVALID_PARAM;
		}

		result = tocparser_remove_toc_entry(handle, (const char *)toc->id);
		if ((result != TOCPARSER_OK) && (result != TOCPARSER_ENTRY_NOT_FOUND)) {
			TP_DBG(printf("Failed to remove entry\n"));
			return TOCPARSER_SET_ENTRY_FAILED;
		}

		if (0 != tocparser_main_toc_add_entry(&handle->main_toc[0], toc, &position))
			return TOCPARSER_ADD_ENTRY_FAILED;

		/* If the entry is SUBTOC then update the handle */
		if ((toc->flags & FLAGS_MASK) == FLAGS_SUB_TOC) {
			if (tocparser_read_subtoc(handle)) {
				TP_DBG(printf("tocparser_add_toc_entry: Failed to read subtoc!\n"));
				return TOCPARSER_READ_FAILED;
			}
		}
	}
	if (subtoc != NULL) {
		/* New SUBTOC entry */
		result = tocparser_remove_toc_entry(handle, (const char *)subtoc->id);
		if ((result != TOCPARSER_OK) && (result != TOCPARSER_ENTRY_NOT_FOUND))  {
			TP_DBG(printf("Failed to remove entry\n"));
			return TOCPARSER_SET_ENTRY_FAILED;
		}

		if (tocparser_add_subtoc_entry(handle, subtoc) != 0)
			return TOCPARSER_ADD_ENTRY_FAILED;
	}

	return TOCPARSER_OK;
}

tocparser_error_code_t tocparser_remove_toc_entry(
		tocparser_handle_t *handle,
		const char *toc_id)
{
	tocparser_toc_entry_t *target;
	bool in_sub_toc;

	TP_DBG_FUNC();

	if (!handle || handle->magic != TOC_HANDLE_MAGIC) {
		TP_DBG(printf("Error: Not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	if (handle->mode == TOCPARSER_READONLY) {
		TP_DBG(printf("Error: Read only\n"));
		return TOCPARSER_ERR_READONLY;
	}

	target = tocparser_get_entry(handle->fd, toc_id, false,
			(tocparser_toc_entry_t *)&handle->main_toc[0],
			(tocparser_subtoc_t *)&handle->subtoc,
			&in_sub_toc);

	if (target == NULL) {
		return TOCPARSER_ENTRY_NOT_FOUND;
	}


	if (in_sub_toc) {
		TP_DBG(printf("Entry in subtoc \n"));
		if (tocparser_sub_toc_remove_entry(handle, target)) {
			TP_DBG(printf("Failed to remove entry from subtoc\n"));
			return TOCPARSER_SET_ENTRY_FAILED;
		}
	} else
		if (tocparser_main_toc_remove_entry(&handle->main_toc[0], target)) {
			TP_DBG(printf("Failed to remove entry from main toc\n"));
			return TOCPARSER_SET_ENTRY_FAILED;
		}

	return TOCPARSER_OK;
}

tocparser_error_code_t tocparser_write_boot_and_toc(tocparser_handle_t *handle)
{
	TP_DBG_FUNC();

	if (!handle || (handle->magic != TOC_HANDLE_MAGIC)) {
		TP_DBG(printf("tocparser_write_boot_and_toc: not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	if (handle->mode != TOCPARSER_EMPTY_TOC) {
		TP_DBG(printf("tocparser_write_boot_and_toc: invalid param\n"));
		return TOCPARSER_ERR_INVALID_PARAM;
	}

	if (handle->toc_location == NO_TOC)
		return TOCPARSER_NOT_INITIALIZED;

	if (tocparser_boot_block_load(handle))
		return TOCPARSER_READ_FAILED;

	if (tocparser_boot_block_rewrite(handle, false)) {
		TP_DBG(printf("tocparser_write_boot_and_toc: write failed\n"));
		return TOCPARSER_WRITE_FAILED;
	}

	if (tocparser_sync(handle)) {
		TP_DBG(printf("tocparser_write_boot_and_toc: write failed\n"));
		return TOCPARSER_WRITE_FAILED;
	}

	return TOCPARSER_OK;
}

tocparser_error_code_t tocparser_get_toc_entry_64(tocparser_handle_t *handle,
							  const char *toc_id,
							  uint64_t *offset,
							  uint64_t *size,
							  uint32_t *loadaddr)
{
	tocparser_toc_entry_t *toc_entry;

	TP_DBG_FUNC();

	if (!handle || (handle->magic != TOC_HANDLE_MAGIC)) {
		TP_DBG(printf("tocparser_get_toc_entry: not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	toc_entry = tocparser_get_entry(handle->fd, toc_id, false,
				(tocparser_toc_entry_t *)&handle->main_toc[0],
				(tocparser_subtoc_t *)&handle->subtoc,
				NULL);

	if (toc_entry != NULL) {
		*offset = get_offset(toc_entry);
		*size = get_size(toc_entry);
		*loadaddr = toc_entry->loadaddr;
		return TOCPARSER_OK;
	}

	TP_DBG(printf("TOC Entry: %s not found!\n", toc_id));

	return TOCPARSER_ENTRY_NOT_FOUND;
}

tocparser_error_code_t tocparser_get_toc_entry(tocparser_handle_t *handle,
					       const char *toc_id,
					       uint32_t *offset,
					       uint32_t *size,
					       uint32_t *loadaddr)
{
	uint64_t int_offset;
	uint64_t int_size;
	tocparser_error_code_t ret;

	ret = tocparser_get_toc_entry_64(handle, toc_id, &int_offset, &int_size,
						loadaddr);
	if ((int_size > UINT_MAX || int_offset > UINT_MAX) && (ret == TOCPARSER_OK))
		return TOCPARSER_OUT_OF_MEMORY;
	*offset = (uint32_t) int_offset;
	*size = (uint32_t) int_size;
	return ret;
}

tocparser_error_code_t tocparser_get_toc_partition_64(tocparser_handle_t *handle,
							      const char *toc_id,
							      uint64_t *offset,
							      uint64_t *size)
{
	tocparser_toc_entry_t *toc_entry;

	TP_DBG_FUNC();

	if (!handle || handle->magic != TOC_HANDLE_MAGIC) {
		TP_DBG(printf("tocparser_get_toc_entry: not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	toc_entry = tocparser_get_entry(handle->fd, toc_id, true,
				(tocparser_toc_entry_t *)&handle->main_toc[0],
				(tocparser_subtoc_t *)&handle->subtoc,
				NULL);

	if (toc_entry == NULL) {
		TP_DBG(printf("TOC Entry: %s not found!\n", toc_id));
		return TOCPARSER_ENTRY_NOT_FOUND;
	}

	if (offset)
		*offset = get_offset(toc_entry);
	if (size)
		*size = get_size(toc_entry);

	return TOCPARSER_OK;
}

tocparser_error_code_t tocparser_get_toc_partition(tocparser_handle_t *handle,
					           const char *toc_id,
					           uint32_t *offset,
					           uint32_t *size)
{
	uint64_t int_offset;
	uint64_t int_size;
	tocparser_error_code_t ret;

	ret = tocparser_get_toc_partition_64(handle, toc_id, &int_offset,
					      &int_size);
	if (int_size > UINT_MAX || int_offset > UINT_MAX)
		return TOCPARSER_OUT_OF_MEMORY;
	*offset = (uint32_t) int_offset;
	*size = (uint32_t) int_size;
	return ret;
}


tocparser_error_code_t tocparser_load_toc_entry_64(tocparser_handle_t *handle,
						   const char *toc_id,
						   uint64_t offset,
						   uint32_t size,
						   uint32_t loadaddr)
{
	uint64_t int_offset;
	uint64_t int_size;
	uint32_t int_loadaddr;
	uint32_t n;

	TP_DBG_FUNC();

	if (!handle || handle->magic != TOC_HANDLE_MAGIC)
		return TOCPARSER_NOT_INITIALIZED;

	if (tocparser_get_toc_entry_64(handle, toc_id, &int_offset, &int_size,
				    &int_loadaddr)) {
		TP_DBG(printf("tocparser: get_entry_info_toc failed\n"));
		return TOCPARSER_GET_ENTRY_FAILED;
	}

	if (int_size > UINT_MAX)
		return TOCPARSER_OUT_OF_MEMORY;

	int_offset += offset;

	if (size != 0)
		int_size = size;

	if (loadaddr != 0)
		int_loadaddr = loadaddr;

	TP_DBG(printf("tocparser: int_offset:0x%llX int_size:0x%X "
		      "int_loadaddr:0x%X\n", int_offset, (uint32_t) int_size,
		      int_loadaddr));

	if (lseek64(handle->fd, (off64_t) int_offset, SEEK_SET) != (off64_t) int_offset) {
		TP_DBG(printf("tocparser: Failed to seek\n"));
		return TOCPARSER_SEEK_FAILED;
	}

	n = read(handle->fd,
		 (uint8_t *)int_loadaddr,
		 (uint32_t)int_size);

	if (n != int_size) {
		TP_DBG(printf("tocparser: Failed to load %s!\n", toc_id));
		return TOCPARSER_READ_FAILED;
	}

	return TOCPARSER_OK;
}


tocparser_error_code_t tocparser_load_toc_entry(tocparser_handle_t *handle,
						const char *toc_id,
						uint32_t offset,
						uint32_t size,
						uint32_t loadaddr)
{

	return tocparser_load_toc_entry_64(handle, toc_id, (uint64_t) offset,
					   size, loadaddr);
}


tocparser_error_code_t tocparser_write_toc_entry_from_fd(
		tocparser_handle_t *handle,
		const char *toc_id,
		int filedescriptor,
		uint32_t size,
		uint32_t *loadaddr)
{
	tocparser_toc_entry_t *target;
	bool in_sub_toc;

	TP_DBG_FUNC();

	if (!handle || handle->magic != TOC_HANDLE_MAGIC) {
		TP_DBG(printf("not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	if (handle->mode != TOCPARSER_READWRITE) {
		TP_DBG(printf("read only\n"));
		return TOCPARSER_ERR_READONLY;
	}

	target = tocparser_get_entry(handle->fd, toc_id, false,
			(tocparser_toc_entry_t *)&handle->main_toc[0],
			(tocparser_subtoc_t *)&handle->subtoc,
			&in_sub_toc);

	if (target == NULL)
		return TOCPARSER_ENTRY_NOT_FOUND;

	if (get_offset(target) < TOC_BOOT_IMAGE_SIZE) {
		if (in_sub_toc) {
			TP_DBG(printf("Invalid toc, entry in subtoc describes"
					"data in the boot block\n"));
			return TOCPARSER_ERR_INVALID_TOC;
		}

		return tocparser_boot_block_write_toc_entry_from_fd(handle,
							    target,
							    filedescriptor,
							    size,
							    loadaddr);
	}

	return tocparser_normal_write_toc_entry(handle,
		toc_id,
		target,
		NULL,
		filedescriptor,
		size,
		loadaddr);
}

tocparser_error_code_t tocparser_write_toc_entry_from_buffer(
		tocparser_handle_t *handle,
		const char *toc_id,
		uint8_t *data,
		uint32_t size,
		uint32_t *loadaddr)
{
	tocparser_toc_entry_t *target;
	bool in_sub_toc;

	TP_DBG_FUNC();

	if (!handle || handle->magic != TOC_HANDLE_MAGIC) {
		TP_DBG(printf("Error: Not initialized\n"));
		return TOCPARSER_NOT_INITIALIZED;
	}

	if ((handle->mode != TOCPARSER_READWRITE) && (handle->mode != TOCPARSER_EMPTY_TOC)) {
		TP_DBG(printf("Error: Read only\n"));
		return TOCPARSER_ERR_READONLY;
	}

	if (size == 0) {
		TP_DBG(printf("Error: Size is zero"));
	        return TOCPARSER_ERR_INVALID_PARAM;
	}

	target = tocparser_get_entry(handle->fd, toc_id, false,
			(tocparser_toc_entry_t *)&handle->main_toc[0],
			(tocparser_subtoc_t *)&handle->subtoc,
			&in_sub_toc);

	if (target == NULL)
		return TOCPARSER_ENTRY_NOT_FOUND;

	if (get_offset(target) < TOC_BOOT_IMAGE_SIZE) {
		TP_DBG(printf("Entry located in the boot blocks\n"));

		if (in_sub_toc) {
			TP_DBG(printf("Error: Entry in subtoc describes "
					"data in the boot blocks"));
			return TOCPARSER_ERR_INVALID_TOC;
		}

		return tocparser_boot_block_write_toc_entry(handle,
							    target,
							    data,
							    size,
							    loadaddr);
	}

	return tocparser_normal_write_toc_entry(handle,
		toc_id,
		target,
		data,
		-1,
		size,
		loadaddr);

}



tocparser_error_code_t tocparser_init(char *dev,
				      tocparser_mode_t mode,
				      tocparser_handle_t **handle)
{
	char *int_dev = TOCPARSER_DEV;
	char *toc_env;
	tocparser_handle_t *local_handle = NULL;

	TP_DBG_FUNC();

	if (handle == NULL ||
		(mode != TOCPARSER_READONLY &&
		mode != TOCPARSER_READWRITE &&
		mode != TOCPARSER_EMPTY_TOC))
		return TOCPARSER_ERR_INVALID_PARAM;

	*handle = NULL;

	local_handle = malloc(sizeof(tocparser_handle_t));

	if (local_handle == NULL)
		return TOCPARSER_OUT_OF_MEMORY;

	memset(local_handle, 0, sizeof(*local_handle));

	/* get debug env */
	toc_env = getenv("TOCPARSER_DEBUG");
	if (toc_env)
		/* debug env exist */
		debug = strtoul(toc_env, NULL, 10);
	else
		debug = 0;

	if (dev != NULL)
		int_dev = dev;

	local_handle->mode = mode;
	if (mode == TOCPARSER_READONLY)
		local_handle->fd = open(int_dev, O_RDONLY);
	else
		local_handle->fd = open(int_dev, O_RDWR);

	if (local_handle->fd < 0) {
		free(local_handle);
		return TOCPARSER_OPEN_FAILED;
	}

	local_handle->toc_location = NO_TOC;
	local_handle->subtoc.num_items = 0;

	if (tocparser_inittoc(local_handle) && (mode != TOCPARSER_EMPTY_TOC)) {
		free(local_handle);
		return TOCPARSER_INIT_FAILED;
	}

	TP_DBG(
	if (mode == TOCPARSER_EMPTY_TOC)
		printf("Initialized empty TOC\n")
	);

	local_handle->magic = TOC_HANDLE_MAGIC;

	*handle = local_handle;

	return TOCPARSER_OK;
}

tocparser_error_code_t tocparser_uninit(tocparser_handle_t *handle)
{
	tocparser_error_code_t error = TOCPARSER_OK;

	TP_DBG_FUNC();

	if ((handle != NULL) && (handle->fd >= 0))
		if (close(handle->fd))
			error = TOCPARSER_CLOSE_DEV_FAILED;

	if (handle != NULL && handle->boot_block_buffer != NULL) {
		free(handle->boot_block_buffer);
		handle->boot_block_buffer = NULL;
	}

	if (handle != NULL) {
		free(handle);
		handle = NULL;
	}

	return error;
}


#define TOCERR2STR(error) case (error): return #error;
const char *tocparser_err2str(tocparser_error_code_t error)
{
	switch (error) {
		TOCERR2STR(TOCPARSER_OK)
		TOCERR2STR(TOCPARSER_NOT_INITIALIZED)
		TOCERR2STR(TOCPARSER_OUT_OF_MEMORY)
		TOCERR2STR(TOCPARSER_DEVICE_OPEN_FAILED)
		TOCERR2STR(TOCPARSER_OPEN_FAILED)
		TOCERR2STR(TOCPARSER_INIT_FAILED)
		TOCERR2STR(TOCPARSER_READ_FAILED)
		TOCERR2STR(TOCPARSER_SEEK_FAILED)
		TOCERR2STR(TOCPARSER_WRITE_FAILED)
		TOCERR2STR(TOCPARSER_ENTRY_NOT_FOUND)
		TOCERR2STR(TOCPARSER_GET_ENTRY_FAILED)
		TOCERR2STR(TOCPARSER_SET_ENTRY_FAILED)
		TOCERR2STR(TOCPARSER_CLOSE_DEV_FAILED)
		TOCERR2STR(TOCPARSER_ERR_INVALID_PARAM)
		TOCERR2STR(TOCPARSER_ERR_READONLY)
		TOCERR2STR(TOCPARSER_ERR_INVALID_TOC)
		TOCERR2STR(TOCPARSER_ERR_OUT_OF_SPACE)
		TOCERR2STR(TOCPARSER_ADD_ENTRY_FAILED)
	default:
		return "UNKNOWN";
	}
}
#undef TOCERR2STR

