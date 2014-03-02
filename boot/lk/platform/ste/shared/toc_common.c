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
 * Common TOC code.
 */

#include <debug.h>

#include <string.h>
#include <stdio.h>

#include <kernel/thread.h>
#include <reg.h>

#include <toc.h>

//#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

int is_toc(struct toc_entry *toc)
{
	/* Check for TOC MAGIC */
	return !memcmp(toc->id, TOC_MAGIC, sizeof(TOC_MAGIC));
}

struct toc_entry *toc_find_id(struct toc_entry *toc, const char *toc_id)
{
	unsigned int i;
	uint32_t n_entries;
	uint64_t offset_b;

	n_entries = toc_n_entries_get();

	/* Find TOC entry */
	for (i = 0; i < n_entries; i++) {
		if (strncmp((char *)toc_id, (char *)toc[i].id, TOC_ID_LENGTH)
				== 0) {
			if ((toc[i].flags & 0x0000ffff) == FLAGS_LBA) {
				offset_b = toc[i].offset * 512;
			} else {
				offset_b = toc[i].offset;
			}
			dprintf(INFO, "found toc_id %s at 0x%08llx\n",
					toc_id, offset_b);
			return &toc[i];
		}
	}

	return (struct toc_entry *)~0;
}


void toc_recalc_entry_offsets(struct toc_entry *toc, long reloff)
{
	unsigned int i;
	uint64_t offset_b;

	/*
	 * Recalculate the offsets that are outside of the boot image (0x20000)
	 * since they need to be absolute and not relative.
	 */
	for (i = 0; i < TOC_MAIN_ENTRIES; i++, toc++) {
		/*
		 * Only do this for offsets outside the boot area and not
		 * on empty entries
		 */
		if (memcmp(toc->id, TOC_ID_NO_ENTRY, TOC_ID_LENGTH) == 0) {
			/* Empty TOC entry */
			continue;
		}

		if ((toc->flags & 0x0000ffff) == FLAGS_LBA) {
			offset_b = toc->offset * 512;
		} else {
			offset_b = toc->offset;
		}

		if (offset_b >= TOC_BOOT_IMAGE_SIZE) {
			dprintf(SPEW, "Recalculating offset on TOCEntry: %s\n",
			      toc->id);
			offset_b += reloff;
		}

		if ((toc->flags & 0x0000ffff) == FLAGS_LBA) {
			toc->offset = (offset_b + 511) / 512;
		} else {
			toc->offset = offset_b;
		}
	}
}

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
#if WITH_LIB_CONSOLE

#include <lib/console.h>
#include <stdio.h>
#include <toc.h>

static int cmd_tocprint(int argc, const cmd_args *argv);
static int cmd_tocload(int argc, const cmd_args *argv);

STATIC_COMMAND_START
        { "tocprint", "print TOC ", &cmd_tocprint },
        { "tocload", "load TOC entry ", &cmd_tocload },
STATIC_COMMAND_END(toc);

static void toc_print_entry(struct toc_entry *toc_entry)
{
	char buf[TOC_ID_LENGTH + 1];

	if (toc_entry) {
		printf("0x%08lx ", toc_entry->offset);
		printf("0x%08lx ", toc_entry->size);
		printf("0x%08lx ", toc_entry->flags);
		printf("0x%08lx ", toc_entry->align);
		printf("0x%08lx ", toc_entry->loadaddr);
		memcpy(buf, &toc_entry->id, TOC_ID_LENGTH);
		buf[TOC_ID_LENGTH] = 0;
		printf("\"%s\"\n", buf);
	}
}

void toc_print(struct toc_entry *toc)
{
	unsigned int i;
	uint32_t n_entries;

	n_entries = toc_n_entries_get();

	/* Print the whole TOC */
	printf("Printing internal TOC (mem addr %p)\n", toc);
	printf("Offset     Size       Flags      Align      LoadAddr   ID\n");
	for (i = 0; i < n_entries; i++) {
		if (memcmp(toc[i].id, TOC_ID_NO_ENTRY, TOC_ID_LENGTH) != 0)
			/* Don't print empty TOC entries */
			toc_print_entry(&toc[i]);
	}
}


static int cmd_tocprint(int argc, const cmd_args *argv)
{
	int i;
	struct toc_entry *toc_entry;
	struct toc_entry *toc = toc_addr_get();

	if (argc == 1) {
		toc_print(toc);
		return 0;
	}
        for (i = 1; i < argc; i++) {
		toc_entry = toc_find_id(toc, argv[i].str);
		if (toc_entry != (struct toc_entry *)~0)
			toc_print_entry(toc_entry);
	}
        return 0;
}

static int cmd_tocload(int argc, const cmd_args *argv)
{
	struct toc_entry *toc_entry;
	uint32_t loadaddr;

	if (argc != 2 && argc != 3) {
		printf("%s: <name> [loadaddr]\n", argv[0].str);
		return -1;
	}
	if (argc == 3)
		loadaddr = argv[2].u;
	else
		loadaddr = ~0;	/* use toc_entry field */
	toc_entry = toc_load_id(argv[1].str, loadaddr);

	if (toc_entry == (struct toc_entry *)~0) {
		printf("\nloading %s failed\n", argv[1].str);
	} else {
		if (loadaddr == (uint32_t)~0)
			loadaddr = toc_entry->loadaddr;
		printf("loaded img to 0x%08x\n", loadaddr);
	}

        return 0;
}
#endif /* WITH_LIB_CONSOLE */
#endif /* ENABLE_FEATURE_BUILD_HBTS */
