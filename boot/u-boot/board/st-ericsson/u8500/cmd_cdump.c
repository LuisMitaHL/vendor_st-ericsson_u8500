/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Spjalle Joelson for ST-Ericsson
 * Licensed under GPLv2.
 *
 * Cleanups and comments: Michael Brandt <michael.brandt@stericsson.com>
 */

/* cmd_cdump.c - crash dump commands, require FAT write support */

#include <common.h>
#include <command.h>
#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif
#include "malloc.h"
#include <mmc.h>
#include <asm/io.h>

#include <asm/setup.h>
#include <elf.h>
#include <fat.h>
#include <asm/arch/hardware.h>
#include <exports.h>
#include <linux/ctype.h>
#ifdef CONFIG_LCD
#include "mcde_display.h"
#include <lcd.h>
#endif

#define TWD_WDOG_LOAD			0x20
#define TWD_WDOG_CONTROL		0x28
#define TWD_WDOG_DISABLE		0x34

#define MAX_PREFIX 1024

/* SoC settings AVS */
#define SOC_AVS_BACKUPRAM               0x80151c30
#define SOC_AVS_BACKUPRAM_LEN           32

extern int restarted(void);

#if defined(CONFIG_LCD)
extern void lcd_setfgcolor (int color);
extern void lcd_setbgcolor (int color);
#endif

/*
 * Note: with the Rockbox FAT support, the file path must be an absolute path,
 * i.e. with leading /.
 */
static char crash_filename[20];

static void kick_mpcore_wdt(void)
{
	unsigned long mpcore_wdt_loadreg;

	mpcore_wdt_loadreg = readl(U8500_TWD_BASE + TWD_WDOG_LOAD);
	/*
	 * According to the linux mpcore_wdt driver a different value needs to
	 * be written to the load register every time
	 */
	mpcore_wdt_loadreg = mpcore_wdt_loadreg ^ 0x1;
	writel(mpcore_wdt_loadreg, U8500_TWD_BASE + TWD_WDOG_LOAD);
}

void stop_mpcore_wdt(void)
{
	writel(0x12345678, U8500_TWD_BASE + TWD_WDOG_DISABLE);
	writel(0x87654321, U8500_TWD_BASE + TWD_WDOG_DISABLE);
	writel(0x0, U8500_TWD_BASE + TWD_WDOG_CONTROL);
}

/*
 * Check ELF header
 */
#if defined(ENG_VERSION)
static int check_elfhdr(Elf32_Ehdr *elfhdr_addr)
{
	unsigned char *elfhdr = (unsigned char *) elfhdr_addr;

	/* check ELF core image header MAGIC */
	if (memcmp(elfhdr, ELFMAG, SELFMAG) != 0)
		return 1;

	/* check that this is ELF32 */
	if (elfhdr[EI_CLASS] == ELFCLASS32)
		return 0;

	return 1;
}
#endif

/*
 * Write a chunk
 */
static int write_chunk(int fd, void *addr, size_t count)
{
	size_t bytes;

	bytes = write(fd, addr, count);
	if (bytes != count) {
		printf("write error\n");
		close(fd);
		return -1;
	}
	return 0;
}

/*
 * Write a big chunk with 'progress' indicator '.' for every MiB
 */
static int write_big_chunk(int fd, void *addr, size_t count)
{
	unsigned char *a = addr;
	size_t total = 0;

	if (count < 0x100000)
		return write_chunk(fd, addr, count);
	/* if large chunk then print dot to show progress */
	while (total < count) {
		size_t bytes = count - total;

		kick_mpcore_wdt();

		if (bytes > 0x100000)
			bytes = 0x100000;
		if (write_chunk(fd, a, bytes))
			return -1;
		putc('.');
		total += bytes;
		a += bytes;
	}
	putc('\n');
	return 0;
}

/*
 * Open the dump file for writing. Create if it not exists.
 * Note that with the Rockbox FAT support, the file path must be an absolute
 * path, i.e. with leading /.
 */
static int open_create(const char *filename)
{
	int fd;

	fd = open(filename, O_WRONLY | O_CREAT);
	if (fd < 0)
		printf("%s open error\n", filename);
	return fd;
}

/*
 * Check program header and segment
 * Truncate note segments.
 * Return segment size.
 */
static u32 check_phdr(Elf32_Phdr *proghdr)
{
	u32 i;
	u32 *note;
	Elf32_Phdr *phdr = proghdr;

	if (phdr->p_type == PT_NOTE) {
		/* see Linux kernel/kexec.c:append_elf_note() */
		note = (u32 *)(phdr->p_paddr);
		for (i = 0; i < phdr->p_filesz/4;) {
			if (note[i] == 0 && note[i+1] == 0 && note[i+2] == 0)
				return i*4;
			i += 3 + (note[i] + 3) / 4 + (note[i+1] + 3) / 4;
		}
	}

	return phdr->p_filesz;
}

/*
 * Dump crash to file
 */
static int write_elf(Elf32_Ehdr *elfhdr_addr, int fd)
{
	Elf32_Ehdr *oldhdr = elfhdr_addr;
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	u32 i;
	u32 offset;
	u32 tot;
	u32 phdr_cnt;
	u32 notes_cnt = 0;
	u32 save;
	u32 len;

	offset = oldhdr->e_ehsize + oldhdr->e_phentsize * oldhdr->e_phnum;
	ehdr = (Elf32_Ehdr *) malloc(offset);
	if (ehdr == NULL) {
		debug("elf header alloc error\n");
		return -1;
	}
	memcpy(ehdr, oldhdr, offset);

	/*
	 * check program header entries and update length
	 * for merged PT_NOTE segments
	 */
	tot = 0;
	phdr_cnt = ehdr->e_phnum;
	debug("phdr_cnt=%d\n", phdr_cnt);
	for (i = 0; i < phdr_cnt; i++) {
		phdr = (Elf32_Phdr *) ((char *) ehdr + ehdr->e_ehsize +
				       i * ehdr->e_phentsize);
		len = check_phdr(phdr);
		debug("prog hdr %d: %x ad %x len %x adjusted to %x\n",
		      i, (u32) phdr, phdr->p_paddr, phdr->p_filesz, len);
		phdr->p_filesz = len;
		phdr->p_memsz = len;
		if (phdr->p_type == PT_NOTE) {	/* note segment */
			tot += len;
			notes_cnt++;
		}
	}
	debug("Length of %d note segments: %x\n", notes_cnt, tot);

	/*
	 * all PT_NOTE segments have been merged into one.
	 * Update ELF Header accordingly
	 */
	ehdr->e_phnum = phdr_cnt - notes_cnt + 1;

	/* write elf header into file on sdcard */
	if (write_chunk(fd, ehdr, (size_t) ehdr->e_ehsize)) {
		free(ehdr);
		return -1;
	}

	/* write program headers into file on sdcard */
	offset = ehdr->e_ehsize + ehdr->e_phentsize * ehdr->e_phnum;
	debug("Write Phdr: proghdr_cnt=%d\n", phdr_cnt);
	for (i = 0; i < phdr_cnt; i++) {
		phdr = (Elf32_Phdr *) ((char *)ehdr + ehdr->e_ehsize +
				       i * ehdr->e_phentsize);
		save = phdr->p_filesz;
		if (i == 0) {
			phdr->p_filesz = tot;
			phdr->p_memsz = tot;
		} else if (phdr->p_type == PT_NOTE) /* note segment */
			continue;
		phdr->p_offset = offset;
		debug("prog hdr %d: %x ad %x len %x off %x\n",
		       i, (u32) phdr, phdr->p_paddr, phdr->p_filesz,
		       phdr->p_offset);
		offset += phdr->p_filesz;
		if (write_chunk(fd, (void *) phdr, (size_t)
				ehdr->e_phentsize)) {
			free(ehdr);
			return -1;
		}
		phdr->p_filesz = save;
		phdr->p_memsz = save;
	}

	/* write segments into file on sdcard */
	debug("write segments...\n");
	for (i = 0; i < phdr_cnt; i++) {
		phdr = (Elf32_Phdr *) ((char *) ehdr + ehdr->e_ehsize +
				       i * ehdr->e_phentsize);
		if (phdr->p_type > PT_NULL) {
			if (write_big_chunk(fd, (void *) phdr->p_paddr,
							 phdr->p_filesz)) {
				free(ehdr);
				return -1;
			}
		}
	}

	free(ehdr);
	return 0;
}

/*
 * Dump crash to file
 */
static int dump_elf(Elf32_Ehdr *elfhdr_addr, char *filename)
{
	int fd;
	int rc;

	printf("Crash dump to %s\n", filename);
	fd = open_create(filename);
	printf("open_create return %d\n", fd);
	if (fd < 0)
		return 1;
	rc = write_elf(elfhdr_addr, fd);
	close(fd);

	return rc;
}

/*
 * Wait for MMC/SD card to be inserted
 */
static int wait_for_mmc(void)
{
	struct mmc *mmc;

	mmc = find_mmc_device(CONFIG_SD_FAT_DEV_NUM);
	if (!mmc) {
		printf("MMC device %d not found\n", CONFIG_SD_FAT_DEV_NUM);
		return 1;
	}
	printf("Insert MMC/SD card or press ctrl-c to abort\n");
	while (mmc_init_silent(mmc) != 0) {
		kick_mpcore_wdt();
		printf("Insert MMC/SD card or press ctrl-c to abort\n");
		putc('.');
		udelay(500000);
		/* check for ctrl-c to abort... */
		if (ctrlc()) {
			puts("Abort\n");
			return -1;
		}
	}
	return 0;
}

#if defined(ENG_VERSION)
/*
 * Find kexec/kdump ATAG command line
 */
static char *get_atag_cmdline(void)
{
	ulong atag_offset = 0x1000; /* 4k offset from memory start */
	ulong offset = 0x8000;      /* 32k offset from memory start */
	/*
	 * Get pointer to ATAG area, somewhere below U-boot image.
	 * Above values are hard coded in the kexec-tools.
	 */
	u32 * atags = (u32 *)(_armboot_start - offset + atag_offset);
	u32 i = 0;

	/*
	 * ATAG command line: \0 terminated string.
	 * The list ends with an ATAG_NONE node.
	 */
	for (i = 0; (atags[i] != 0) && (atags[i+1] != ATAG_NONE);
			i += atags[i]) {

		if (atags[i+1] == ATAG_CMDLINE)
			return (char *) &atags[i+2];
		/* sanity check before checking next ATAG */
		if (atags[i] > (offset - atag_offset) / sizeof(u32) - i)
			return NULL;
		if ((atags[i] + i) < i) /* cannot step backwards */
			return NULL;
	}

	return NULL;
}

/*
 * Find out where the kdump elf header is.
 */
static Elf32_Ehdr *get_elfhdr_addr(void)
{
	const char elfcorehdr[] = "elfcorehdr=";
	char *cmd;
	char *atag_cmdline = get_atag_cmdline();

	if (atag_cmdline != NULL) {
		cmd = strstr(atag_cmdline, elfcorehdr);
		if (cmd != NULL) {
			cmd += strlen(elfcorehdr);
			return (Elf32_Ehdr *) simple_strtoul(cmd, NULL, 16);
		}
	}
	return NULL;
}
#endif

/*
 * Find then next dump file name to ensure we do not
 * overwrite an old core dump.
 */
static int find_dump_file_name(void)
{
	int fd;
	int prefix = 0;

	do {
		prefix++;
		sprintf(crash_filename,"/cdump_%d.elf",prefix);
		fd = open(crash_filename, O_WRONLY);
		if(fd >= 0)
			close(fd);
	} while (fd >= 0 && prefix < MAX_PREFIX);

	return prefix;
}

/*
 * Dump crash to file (typically FAT file on SD/MMC).
 */
static int crashdump(Elf32_Ehdr *elfhdr_addr)
{
	int rc;
	block_dev_desc_t *dev_desc=NULL;

	rc = wait_for_mmc();
	if (rc == 0) {
		dev_desc = get_dev("mmc", CONFIG_SD_FAT_DEV_NUM);	/* mmc 1 */
		rc = fat_register_device(dev_desc, CONFIG_SD_FAT_PART_NUM); /* part 1 */
		if (rc != 0) {
			printf("crashdump: fat_register_device failed %d\n",
					rc);
			return -1;
		}

		if(find_dump_file_name() < MAX_PREFIX)
			rc = dump_elf(elfhdr_addr, crash_filename);
		else
			printf("Number of dumps have reached maximum on SD card: %d\n", MAX_PREFIX);
	}

	if (rc != 0)
		printf("crashdump: error writing dump to %s\n", crash_filename);

	return rc;
}

#if defined(ENG_VERSION)
static int reboot_at_crash(void)
{
	char *reboot_env_val;
	/*
	 * Magic parameter passed on kernel command line by kexec/kdump
	 * overrides u-boot environment.
	 */
	if (strstr(get_atag_cmdline(), "reboot_at_crash") != NULL)
		return 1;
	reboot_env_val = getenv("crash_reboot");
	if ((reboot_env_val != NULL) && strcmp(reboot_env_val, "no") == 0)
		return 0;
	return 1;
}
#endif

typedef struct range {
	unsigned long start;
	unsigned long size;
} range_t;

/*
 * Parse nn[KMG]@ss[KMG]
 */
static void suffixed_addr_toul(char *s, range_t *r)
{
	char *end_ptr;
	unsigned long suffix_mult;

	r->size = simple_strtoul(s, &end_ptr, 0);
	suffix_mult = 1;
	switch (*end_ptr) {
	case 'G':
		suffix_mult *= 1024;
	case 'M':
		suffix_mult *= 1024;
	case 'K':
		suffix_mult *= 1024;
		end_ptr++;
		break;
	}
	r->size *= suffix_mult;

	if (*end_ptr != '@') {
		r->start = 0;
		return;
	}

	s = end_ptr + 1;
	r->start = simple_strtoul(s, &end_ptr, 0);
	suffix_mult = 1;
	switch (*end_ptr) {
	case 'G':
		suffix_mult *= 1024;
	case 'M':
		suffix_mult *= 1024;
	case 'K':
		suffix_mult *= 1024;
		end_ptr++;
		break;
	}
	r->start *= suffix_mult;
	return;
}

Elf32_Ehdr *create_elfhdr(range_t *regions, unsigned int region_no) {
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	unsigned int hdr_size;
	unsigned int i;

	hdr_size = sizeof(*ehdr)+(region_no+1)*sizeof(*phdr);
	ehdr = malloc(hdr_size);
	if(ehdr == NULL) {
		return NULL;
	}

	memset(ehdr, 0, hdr_size);

	ehdr->e_ident[EI_MAG0] = ELFMAG0;
	ehdr->e_ident[EI_MAG1] = ELFMAG1;
	ehdr->e_ident[EI_MAG2] = ELFMAG2;
	ehdr->e_ident[EI_MAG3] = ELFMAG3;
	ehdr->e_ident[EI_CLASS] = ELFCLASS32;
	ehdr->e_ident[EI_DATA] = ELFDATA2LSB;
	ehdr->e_ident[EI_VERSION] = 1;
	ehdr->e_ident[EI_OSABI] = ELFOSABI_NONE;
	ehdr->e_ident[EI_ABIVERSION] = ELFABIVERSION;
	ehdr->e_type = ET_CORE;
	ehdr->e_machine = EM_ARM;
	ehdr->e_version = EV_CURRENT;
	ehdr->e_entry = 0x0;
	ehdr->e_phoff = sizeof(*ehdr);
	ehdr->e_shoff = 0x0;
	ehdr->e_flags = 0x0;
	ehdr->e_ehsize = sizeof(*ehdr);
	ehdr->e_phentsize = sizeof(*phdr);
	ehdr->e_phnum = region_no+1;
	ehdr->e_shentsize = 0;
	ehdr->e_shnum = 0;
	ehdr->e_shstrndx = 0;

	phdr = (Elf32_Phdr*)(ehdr + 1);
	phdr[0].p_type = PT_NOTE;

	for (i=0; i<region_no; i++) {
		phdr[i+1].p_type = PT_LOAD;
		phdr[i+1].p_vaddr = 0xc0000000 + regions[i].start;
		phdr[i+1].p_paddr = regions[i].start;
		phdr[i+1].p_filesz = regions[i].size;
		phdr[i+1].p_memsz = regions[i].size;
		phdr[i+1].p_flags = PF_X | PF_W | PF_R;
	}
	return ehdr;
}

static size_t match_region(const char *p)
{
	const char *regions[] = {
		"mem=",
		"mem_issw=",
		"mem_trace="
	};
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(regions); i++) {
		size_t len = strlen(regions[i]);

		if (strncmp(p, regions[i], len) == 0)
			return len;
	}

	return 0;
}

int dump_regions_from_environment(void)
{
	Elf32_Ehdr *elfhdr_addr;
	int rc = 0;
	char *orig_bootargs;
	char *bootargs;
	char *p;
	char *memargs_cmd;
	char *memargs;
	int nuf_ph;
	range_t ck = {0, 0};
	range_t *regions;
	char *crashkernel;
	unsigned int i;
	unsigned int region_no;

	/*
	  Right now getenv("crashkernel") has a value on the form
	      "crashkernel=nn[KMG]@ss[KMG]"
	  but this is expanded into
	      "crashkernel=${crashkernel}"
	  when constructing the kernel commandline, amounting to:
	      "crashkernel=crashkernel=nn[KMG]@ss[KMG]"
	  This might get fixed so this code can handle both.
	 */
	crashkernel = getenv("crashkernel");
	if(crashkernel == NULL) {
		debug("checkcrash: no crashkernel environment variable\n");
		rc = 1;
		goto out5;
	}
	crashkernel = strdup(crashkernel);
	if(crashkernel == NULL) {
		debug("checkcrash: could not strdup crashkernel variable\n");
		rc = 1;
		goto out5;
	}
	p = strchr(crashkernel, '=');
	if (p != NULL) {
		p++;
		suffixed_addr_toul(p, &ck);
	}
	else {
		suffixed_addr_toul(crashkernel, &ck);
	}

	/* Make copy of bootargs, if it exists */
	orig_bootargs = getenv("bootargs");
	if(orig_bootargs != NULL) {
		orig_bootargs = strdup(orig_bootargs);
		if(orig_bootargs == NULL) {
			debug("checkcrash: could not strdup bootargs variable to reset to\n");
			rc = 1;
			goto out4;
		}
	}

	/* Unset bootargs */
	if (setenv("bootargs", "") != 0) {
		debug("checkcrash: could not unset bootargs variable\n");
		rc = 1;
		goto out3;
	}

	/* Run memargs to set bootargs */
	memargs_cmd = getenv("memargs");
	if(memargs_cmd == NULL) {
		debug("checkcrash: no memargs_cmd environment variable\n");
		rc = 1;
		goto out3;
	}

#ifndef CONFIG_SYS_HUSH_PARSER
	if (run_command(memargs_cmd, 0) < 0) {
		debug("checkcrash: failed to run memargs\n");
		rc = 1;
		goto out3;
	}
#else
	if (parse_string_outer(memargs_cmd,
			       FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0) {
		debug("checkcrash: failed to run memargs\n");
		rc = 1;
		goto out3;
	}
#endif

	memargs = bootargs = strdup(getenv("bootargs"));
	if(bootargs == NULL) {
		debug("checkcrash: could not strdup bootargs variable to manipulate it\n");
		rc = 1;
		goto out3;
	}

	/* Restore bootargs */
	if (orig_bootargs && setenv("bootargs", orig_bootargs)) {
		debug("checkcrash: could not reset bootargs variable\n");
		rc = 1;
		goto out2;
	}

	/* Calculate the number of regions */
	nuf_ph = 1; // One extra since the crashkernel is likely to split one of the regions
	p = memargs;
	while (*p != '\0') {
		if (match_region(p)) {nuf_ph++;}
		while (!isspace(*p)) {
			p++;
		}
		while (isspace(*p)) {
			p++;
		}
	}

	/* Add one entry for SOC_AVS_BACKUPRAM */
	nuf_ph += 1;

	printf ("nuf_ph %d\n", nuf_ph);
	regions = malloc(nuf_ph*sizeof(*regions));
	if (regions == NULL) {
		debug("checkcrash: could not allocate array of ranges\n");
		rc = 1;
		goto out2;
	}

	region_no = 0;
	while (memargs != NULL) {
		size_t len;

		p = strsep(&memargs, " \t");

		len = match_region(p);
		if (!len)
			continue;

		p += len;
		suffixed_addr_toul(p, &regions[region_no]);
		if (ck.start==(regions[region_no].start) &&
		    (ck.start+ck.size)==(regions[region_no].start+regions[region_no].size)) {
			/* Skip */
		}
		else if (ck.start==(regions[region_no].start) &&
		    (ck.start+ck.size)<(regions[region_no].start+regions[region_no].size)) {
			/* Skip beginning */
			regions[region_no].start = ck.start+ck.size;
			regions[region_no].size -= ck.size;
		}
		else if (ck.start>(regions[region_no].start) &&
		    (ck.start+ck.size)==(regions[region_no].start+regions[region_no].size)) {
			/* Skip end */
			regions[region_no].size -= ck.size;
		}
		else if (ck.start>=(regions[region_no].start) &&
		    (ck.start+ck.size)<=(regions[region_no].start+regions[region_no].size)) {
			/* Split */
			regions[region_no+1].start = ck.start + ck.size;
			regions[region_no+1].size = (regions[region_no].start + regions[region_no].size) -
				regions[region_no+1].start;
			regions[region_no].size = ck.start - regions[region_no].start;
			region_no++;
		}
		region_no++;
	}

	/* Add SOC_AVS_BACKUPRAM */
	regions[region_no].start = SOC_AVS_BACKUPRAM;
	regions[region_no].size = SOC_AVS_BACKUPRAM_LEN;
	region_no++;

	for (i=0;i<region_no;i++) {
		debug("%lu placed at %lu\n", regions[i].size, regions[i].start);
	}

	elfhdr_addr = create_elfhdr(regions, region_no);
	if (elfhdr_addr == NULL) {
		debug("Could not create elfhdr\n");
		rc = 1;
		goto out1;
	}

	if (crashdump(elfhdr_addr) != 0) {
		rc = 1;
	}

	free(elfhdr_addr);
out1:
	free(regions);
out2:
	free(bootargs);
out3:
	free(orig_bootargs);
out4:
	free(crashkernel);
out5:
	return rc;
}

/*
 * Dump crash to file (typically FAT file on SD/MMC).
 */
static int do_checkcrash(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int rc = 0;
#if defined(ENG_VERSION)
	Elf32_Ehdr *elfhdr_addr;
	char *crash_env_val;

	crash_env_val = getenv("crash_dump");
	if ((crash_env_val != NULL) && strcmp(crash_env_val, "yes") == 0) {
		rc = -1;
		elfhdr_addr = get_elfhdr_addr();
		if (elfhdr_addr != NULL)
			rc = check_elfhdr(elfhdr_addr);
		if (rc == 0) {
			printf("crash dump elf header found."
			       " Dumping to card...\n");
			rc = crashdump(elfhdr_addr);
			if (rc != 0)
				printf("checkcrash: "
				       "error writing dump from %x to %s\n",
				       (u32) elfhdr_addr, crash_filename);
		}
		else {
			printf("crash dump elf header not found. Will try to create one"
			       " Dumping to card...\n");
			rc = dump_regions_from_environment();
		}
	}
	stop_mpcore_wdt();

	/*
	 * For some reason we can't reboot into the linux kernel when coming
	 * from there using kexec. On the other hand, we can't reset through the
	 * PRCMU when we are here because of a watchdog reset.
	 * For now let's
	 * 1) Reset through PRCMU when coming from kexec
	 * 2) Continue booting when coming from a watchdog reset
	 */
	if (!reboot_at_crash())
		setenv("bootdelay", "-1");
	else
#endif
		if (restarted())
			do_reset();

	return rc;
}

U_BOOT_CMD(
	checkcrash,	1,	0,	do_checkcrash,
	"check ATAGS from crash and dump to file",
	"    - dump crash info to file and stop autoboot\n"
);
