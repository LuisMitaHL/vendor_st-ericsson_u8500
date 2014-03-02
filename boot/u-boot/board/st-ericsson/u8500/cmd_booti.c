/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <bzlib.h>
#include <environment.h>
#include <asm/byteorder.h>

#include "bootimg.h"

/*cmd_boot.c*/
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#if defined (CONFIG_SETUP_MEMORY_TAGS) 	|| \
	defined (CONFIG_CMDLINE_TAG) 	|| \
	defined (CONFIG_INITRD_TAG) 	|| \
	defined (CONFIG_SERIAL_TAG) 	|| \
	defined (CONFIG_REVISION_TAG) 	|| \
	defined (CONFIG_VFD) 		|| \
	defined (CONFIG_LCD)
static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
					ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined (CONFIG_VFD) || defined (CONFIG_LCD)
static void setup_videolfb_tag (gd_t *gd);
# endif

static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)        show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

#ifdef CONFIG_U8500
extern int sec_bridge_verify_kernel_image(u32 *img_addr);
#endif

extern image_header_t header;   /* from cmd_bootm.c */

void do_bootimg_linux (boot_img_hdr *bootimg_header_data)
{
	ulong initrd_start, initrd_end;
	void (*theKernel)(int zero, int arch, uint params);
	bd_t *bd = gd->bd;
#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv ("bootargs");
#endif

	theKernel = (void (*)(int, int, uint))(bootimg_header_data->kernel_addr);

	initrd_start =  bootimg_header_data->ramdisk_addr;;
	initrd_end = initrd_start + bootimg_header_data->ramdisk_size;

#if defined (CONFIG_SETUP_MEMORY_TAGS) 	|| \
	defined (CONFIG_CMDLINE_TAG) 	|| \
	defined (CONFIG_INITRD_TAG) 	|| \
	defined (CONFIG_SERIAL_TAG) 	|| \
	defined (CONFIG_REVISION_TAG) 	|| \
	defined (CONFIG_LCD) 		|| \
	defined (CONFIG_VFD)
	setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
        setup_serial_tag (&params);
#endif
#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
	if (initrd_start && initrd_end)
		setup_initrd_tag (bd, initrd_start, initrd_end);
#endif
#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
	setup_videolfb_tag ((gd_t *) gd);
#endif
	setup_end_tag (bd);
#endif

	/* we assume that the kernel is in place */
	printf ("\nStarting kernel ... %ld\n\n", bd->bi_arch_number);

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif

	cleanup_before_linux ();

	theKernel (0, bd->bi_arch_number, bd->bi_boot_params);
}

#if defined (CONFIG_SETUP_MEMORY_TAGS) 	|| \
	defined (CONFIG_CMDLINE_TAG) 	|| \
	defined (CONFIG_INITRD_TAG) 	|| \
	defined (CONFIG_SERIAL_TAG) 	|| \
	defined (CONFIG_REVISION_TAG) 	|| \
	defined (CONFIG_LCD) 		|| \
	defined (CONFIG_VFD)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;
	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */

static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	* use its default command line.
	*/
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
	(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}

#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	* ramdisk can be found. ATAG_RDIMG is a better name, actually.
	*/
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */


#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
extern ulong calc_fbsize (void);
static void setup_videolfb_tag (gd_t *gd)
{
	/* An ATAG_VIDEOLFB node tells the kernel where and how large
	 * the framebuffer for video was allocated (among other things).
	 * Note that a _physical_ address is passed !
	 *
	 * We only use it to pass the address and size, the other entries
	 * in the tag_videolfb are not of interest.
	 */
	params->hdr.tag = ATAG_VIDEOLFB;
	params->hdr.size = tag_size (tag_videolfb);

	params->u.videolfb.lfb_base = (u32) gd->fb_base;
	/* Fb size is calculated according to parameters for our panel
	*/
	params->u.videolfb.lfb_size = calc_fbsize();

	params = tag_next (params);
}
#endif /* CONFIG_VFD || CONFIG_LCD */

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#endif  /* CONFIG_REVISION_TAG */

static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

void
bootimg_print_image_hdr (boot_img_hdr *hdr)
{
#ifdef DEBUG
	int i;
	printf ("Image magic: %s\n", hdr->magic);
	printf ("kernel_size: 0x%x\n", hdr->kernel_size);
	printf ("kernel_addr: 0x%x\n", hdr->kernel_addr);
	printf ("rdisk_size:  0x%x\n", hdr->ramdisk_size);
	printf ("rdisk_addr:  0x%x\n", hdr->ramdisk_addr);
	printf ("second_size: 0x%x\n", hdr->second_size);
	printf ("second_addr: 0x%x\n", hdr->second_addr);
	printf ("tags_addr:   0x%x\n", hdr->tags_addr);
	printf ("page_size:   0x%x\n", hdr->page_size);
	printf ("name: %s\n", hdr->name);
	printf ("cmdline: %s\n", hdr->cmdline);

	for (i=0;i<8;i++)
		printf ("id[%d]:   0x%x\n", i, hdr->id[i]);
#endif
}

boot_img_hdr bootimg_header_data;

int do_booti (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong addr;
	unsigned remainder;

	addr = simple_strtoul(argv[1], NULL, 16);

#ifdef CONFIG_U8500
	if (sec_bridge_verify_kernel_image ((u32*)&addr))
		addr = 0;
#endif

#ifdef DEBUG
	printf ("## bootimg.img @ %08lx ...\n", addr);
#endif

	memmove (&bootimg_header_data, (char *)addr, sizeof(boot_img_hdr));

	/* print copied header */
	bootimg_print_image_hdr((boot_img_hdr *)&bootimg_header_data);

	if (strncmp((char *)(bootimg_header_data.magic),BOOT_MAGIC, 8)) {
		puts ("Bad Magic Number\n");
		goto err;
	}

	addr += bootimg_header_data.page_size;
#ifdef DEBUG
	printf ("Copying kernel from  [0x%08x] len[0x%08x] to [0x%08x]...\n",
		(unsigned int)addr,
		(unsigned int)bootimg_header_data.kernel_size,
		(unsigned int)bootimg_header_data.kernel_addr);
#endif
	/* Kernel moving */
	memmove ((void *)(bootimg_header_data.kernel_addr), (void *)addr,
				bootimg_header_data.kernel_size);

	/* Next whole page after end of kernel */
	addr += bootimg_header_data.kernel_size;
	remainder = bootimg_header_data.kernel_size %
			bootimg_header_data.page_size;
	if (remainder)
		addr += bootimg_header_data.page_size - remainder;

#ifdef DEBUG
	printf ("Copying ramdisk from [0x%08x] len[0x%08x] to [0x%08x]...\n",
		(unsigned int)addr,
		(unsigned int)bootimg_header_data.ramdisk_size,
		(unsigned int)bootimg_header_data.ramdisk_addr);
#endif
	/* Ramdisk moving */
	memmove ((void *)(bootimg_header_data.ramdisk_addr), (void *)addr,
				bootimg_header_data.ramdisk_size);

	do_bootimg_linux(&bootimg_header_data);

err:
	puts ("\n## Control returned to monitor - resetting...\n");
	do_reset(cmdtp, flag, argc, argv);

	return 1;
}

U_BOOT_CMD(
	booti, 2, 1, do_booti,
	"boot android bootimg from memory",
	"<addr>\n - boot application image stored in memory\n"
	"\t'addr' should be the address of boot image which is zImage+ramdisk.img"
);
