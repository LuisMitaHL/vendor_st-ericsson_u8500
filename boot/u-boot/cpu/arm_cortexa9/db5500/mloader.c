/*
 * Code ported from mloader in ST-Ericsson Linux kernel.
 *
 * Original Linux authors:
 * Copyright (C) ST-Ericsson SA 2010
 * Authors: Paer-Olof Haakansson <par-olof.hakansson@stericsson.com>
 *          Martin Persson <martin.persson@stericsson.com>
 *
 * Ported to U-boot by:
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Mikael Larsson <mikael.xt.larsson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL) version 2
 */
#include <common.h>
#include <malloc.h>
#include <part.h>
#include <elf.h>
#include <asm/arch/tee.h>

/* ST-Ericsson specific ELF segment type for PT_HASH. */
#define ELF_SEGMENT_HASH		0x656D7001
#define ELF_SEGMENT_HASH_SIZE		0x20
/*
 * ST-Ericsson specific ELF segment type for PT_LM_SIGNATURE.
 * This segment contains the COPS header of the ELF file.
 */
#define ELF_SEGMENT_LM_SIGNATURE	0x656D7000
#define LM_MAGIC			"OSE Configuration"
#define LM_MAGIC_SIZE			(sizeof(LM_MAGIC) - 1)

#define LOAD_SEGMENT			0x00000001
#define DONT_LOAD_SEGMENT		0xFFFFFFFF

#define MLOADER_ESUCCESS 0

#define ELF_SEGMENT_IS_EXEC(x)		((x) & PF_X)
#define ELF_SEGMENT_IS_WRITABLE(x)	((x) & PF_W)
#define ELF_SEGMENT_IS_READABLE(x)	((x) & PF_R)

/* Struct to hold information for one segment in the ELF file */
struct segment {
	struct mcore_segment_descr mcore_segment;
	void *mmap_virt_addr;
	void *mmap_addr_pg_aligned;
	u32 mmap_aligned_offset;
	u32 mmap_aligned_mapsize;
};

/* Holds modem related memory ranges */
struct modem_mem_data {
	void *start;
	void *end;
	u32 total_size;
	u32 shared_size;
	u32 private_size;
};

struct mcore_elf_data {
	struct elfhdr *header;
	void *signature_ptr;/*  = NULL; */
	Elf32_Phdr *pheader;/*  = NULL; */
	struct segment *segments_ptr;/*  = NULL; */
	u32 nbr_segments;/*  = 0; */
	u32 nbr_hashes;/*  = 0; */
	void *addr;/*  = NULL; */
	void *base_addr;/*  = NULL; */
};

struct issw_data {
	u32 required_issw_size;
	void *base_addr;
	void *current_issw_addr;
	struct access_image_descr *acc_image_descr;
};

/* mloader_cmp_addr() is used when sorting segments with qsort() */
static int mloader_cmp_addr(const void *p1,
			 const void *p2)
{
	return (int)(((struct segment *)p1)->mcore_segment.segment) -
		(int)(((struct segment *)p2)->mcore_segment.segment);
}

static u32 find_hash_for_segment(struct segment *segments_ptr,
					  void *hash_for_physaddr,
					  void *hash_store_addr,
					  u32 number_headers)
{
	u32 i;

	debug("find_hash_for_segment\n");

	/* Compare load address of segment with hash address */
	for (i = 0; i < number_headers; i++) {
		if (segments_ptr->mcore_segment.segment ==
		    hash_for_physaddr) {
			segments_ptr->mcore_segment.hash = hash_store_addr;
			/* There is a matching hash */
			debug("find_hash_for_segment: Found matching hash\n");
			return MLOADER_ESUCCESS;
		}
		segments_ptr++;
	}

	/* No match segment/hash */
	debug("find_hash_for_segment: No match\n");
	return 1;
}

/*
 * Used to copy the segments that were processed to a region
 * set-up for passing to ISSW
 */
static void copy_contents_of_segment_list(
	struct mcore_segment_descr *mcore_segment_descr_ptr,
	struct segment *segments_ptr,
	u32 number_headers)
{
	u32 i;

	debug("copy_contents_of_segment_list\n");

	for (i = 0; i < number_headers; i++) {
		if (segments_ptr->mcore_segment.size != 0) {
			memcpy(mcore_segment_descr_ptr,
			       &segments_ptr->mcore_segment,
			       sizeof(struct mcore_segment_descr));
			mcore_segment_descr_ptr++;
		}
		segments_ptr++;
	}

}

/*
 * The ISSW data must be stored in modem private memory to make sure
 * that the data is not compromised.
 * This function looks for space in that area before the loaded ELF,
 * between its segments and after the ELF
 */
static u32 find_space_for_issw_data(struct mcore_elf_data *elf,
				    struct modem_mem_data *modem,
				    struct issw_data *issw)
{
	u32 base_addr;
	int i;
	u32 this_seg;
	u32 next_seg;
	u32 pagesize = malloc_getpagesize;

	size_t this_seg_size;
	struct segment *current_segment = elf->segments_ptr;
	u8 space_found = 0;
	u8 lowest_segment_checked = 0;

	issw->base_addr = NULL;

	for (i = 0; i < (elf->header->e_phnum - 1); i++) {

		this_seg = (u32)current_segment->mcore_segment.segment;
		this_seg_size = current_segment->mcore_segment.size;
		next_seg = (u32)((current_segment) + 1)->mcore_segment.segment;

		debug("find_space_for_issw_data: this_seg:0x%x"
		      "this_seg_size:0x%x next_seg:0x%x\n", this_seg,
		      this_seg_size, next_seg);

		/* Any space between SECR_DDRMODLOW (modem memory start) and
		 * lowest placed loaded segment?
		 */
		if (!lowest_segment_checked && (this_seg_size > 0)) {
			lowest_segment_checked = 1;
			base_addr = ALIGN((u32)modem->start +
					       modem->shared_size, pagesize);

			if (this_seg - base_addr >
			    issw->required_issw_size) {
				space_found = 1;
				break;
			}
		}

		/* Any space between this segment and next? */
		/* Possibly there is space...* align address to check */
		base_addr = ALIGN((u32)this_seg + this_seg_size, pagesize);

		/* Any space between aligned address and next segment */
		if ((base_addr < next_seg) &&
		    (next_seg - base_addr > issw->required_issw_size)) {
			space_found = 1;
			break;
		}
		current_segment++;
	}

	if (!space_found) {
		/* Search last segment --> mloader parameters end */
		current_segment = ((elf->segments_ptr) +
				   elf->header->e_phnum - 1);

		base_addr = ALIGN(((u32)current_segment->mcore_segment.segment +
				  (u32)current_segment->mcore_segment.size),
				  pagesize);

		if ((u32)modem->end - base_addr >
		    issw->required_issw_size) {
			space_found = 1;
		}
	}

	if (!space_found) {
		printf("mloader: Could not find area to store ISSW data\n");
		return -1;
	}

	issw->base_addr = (void *)base_addr;
	return MLOADER_ESUCCESS;
}

#define ADD_CURRENT_SEGMENT() \
			((elf->segments_ptr)+i)->mcore_segment.segment = \
			(void *)elf->pheader[i].p_paddr; \
			((elf->segments_ptr)+i)->mcore_segment.size = \
			(u32)elf->pheader[i].p_filesz; \
			elf->nbr_segments++; \
			this_segment = LOAD_SEGMENT;

static int load_segments(struct mcore_elf_data *elf,
			 struct modem_mem_data *modem,
			 struct issw_data *issw)
{
	int this_segment;
	u32 i;
	u32 signature_size = 0;
	u32 total_hash_size = 0;

	debug("load_segments\n");

	elf->nbr_segments = 0;

	/* Loop through program headers, load code, data and LMCONF */
	for (i = 0; i < elf->header->e_phnum; i++) {

		/* Don't load unknown segment types... */
		this_segment = DONT_LOAD_SEGMENT;

		if (ELF_SEGMENT_IS_EXEC(elf->pheader[i].p_flags)) {
			/* CODE */
			ADD_CURRENT_SEGMENT();
		} else if (ELF_SEGMENT_IS_WRITABLE(elf->pheader[i].p_flags) &&
			   ELF_SEGMENT_IS_READABLE(elf->pheader[i].p_flags)) {
			/* DATA */
			ADD_CURRENT_SEGMENT();
		} else if (ELF_SEGMENT_IS_READABLE(elf->pheader[i].p_flags)) {
			/* LMCONF */
			char *temp_str;

			temp_str = (char *)elf->pheader[i].p_offset;
			if (memcmp(temp_str, LM_MAGIC,
						LM_MAGIC_SIZE) == 0) {
				ADD_CURRENT_SEGMENT();
			}
		} else if (elf->pheader[i].p_type == ELF_SEGMENT_LM_SIGNATURE)
			/* LM_SIGNATURE */
			/* Save size */
			signature_size = elf->pheader[i].p_filesz;
		else if (elf->pheader[i].p_type == ELF_SEGMENT_HASH) {
			/* HASH */
			/* Save size */
			if (elf->pheader[i].p_filesz != ELF_SEGMENT_HASH_SIZE) {
				printf("load_segments: invalid hash size\n");
				return 1;
			}
			total_hash_size += elf->pheader[i].p_filesz;
		}

		if (this_segment == LOAD_SEGMENT) {

			if (!((elf->pheader[i].p_paddr >=
			    (u32)(modem->end - modem->private_size + 1)) &&
			    ((elf->pheader[i].p_paddr +
						elf->pheader[i].p_filesz) <=
			       (u32)modem->end))) {
				printf("mloader: Tried to load ELF segment at "
				       "illegal address. Addr: 0x%08x\n",
				       elf->pheader[i].p_paddr);
				return 1;
			}

			memcpy((void *)elf->pheader[i].p_paddr,
			       (void *)(CONFIG_SYS_LOAD_ADDR+
					elf->pheader[i].p_offset),
				elf->pheader[i].p_filesz);
		}
	}

	/*
	 * Add all sizes, make 32 bit placement aligment for all
	 * parts except hashes which are known to be 0x20 bytes each
	 */
	issw->required_issw_size =
		ALIGN(signature_size, 4) +
		total_hash_size +
		ALIGN((u32)sizeof(struct access_image_descr), 4) +
		ALIGN((u32)(elf->nbr_segments *
			    sizeof(struct mcore_segment_descr)), 4) +
		ALIGN((u32)sizeof(struct elfhdr), 4) +
		ALIGN((u32)(elf->header->e_phnum * sizeof(Elf32_Phdr)), 4);

	return MLOADER_ESUCCESS;
}

static int load_hashes_to_issw_area(struct mcore_elf_data *elf,
				    struct issw_data *issw)
{
	int this_segment;
	u32 hash_found;
	u32 i;

	elf->signature_ptr = NULL;

	/* Now find hashes... */
	for (i = 0; i < elf->header->e_phnum; i++) {

		/* Don't load unknown segment types... */
		this_segment = DONT_LOAD_SEGMENT;

		/* HASH */
		if (elf->pheader[i].p_type == ELF_SEGMENT_HASH) {

			hash_found =
				find_hash_for_segment(elf->segments_ptr,
					(void *)elf->pheader[i].p_paddr,
					issw->current_issw_addr,
					elf->header->e_phnum);

			if (hash_found == MLOADER_ESUCCESS) {
				this_segment = LOAD_SEGMENT;
				elf->nbr_hashes++;
			} else {
				printf("mloader: Potential error in hash "
				       "segment.\n");
			}
		}

		/* LM_SIGNATURE */
		if (elf->pheader[i].p_type == ELF_SEGMENT_LM_SIGNATURE) {

			elf->signature_ptr = (void *)issw->current_issw_addr;
			this_segment = LOAD_SEGMENT;
		}

		if (this_segment == LOAD_SEGMENT) {

			memcpy(issw->current_issw_addr,
			       (void *)(CONFIG_SYS_LOAD_ADDR +
					elf->pheader[i].p_offset),
				elf->pheader[i].p_filesz);

			issw->current_issw_addr += elf->pheader[i].p_filesz;
			issw->current_issw_addr =
				(void *)ALIGN((u32)issw->current_issw_addr, 4);
		}
	}

#ifdef MLOADER_CHECK_SIGNING
	if (elf->nbr_hashes != elf->nbr_segments) {
		printf("mloader: ELF segment signing problem, "
		       "not all segments are signed.\n");
		return -1;
	}
#endif

	return 0;
}

static int transfer_to_issw_data_area(struct mcore_elf_data *elf,
				      struct issw_data *issw)
{
	struct mcore_segment_descr *mcore_segment;
	int retval;


	issw->current_issw_addr = issw->base_addr;

	/* Now find hashes... */
	retval = load_hashes_to_issw_area(elf, issw);

	if (retval)
		return retval;

	/* Copy ELF header to modem private memory to be accessed by ISSW */
	memcpy((void *)((u32)issw->current_issw_addr), elf->header,
			sizeof(struct elfhdr));

	issw->acc_image_descr =
		(struct access_image_descr *)malloc(issw->required_issw_size);

	issw->acc_image_descr->elf_hdr = (void *)issw->current_issw_addr;
	issw->current_issw_addr += sizeof(struct elfhdr);
	issw->current_issw_addr =
				(void *)ALIGN((u32)issw->current_issw_addr, 4);

	/* Copy program header table to modem private memory */
	memcpy((void *)((u32)issw->current_issw_addr), elf->pheader,
	       sizeof(Elf32_Phdr) * elf->header->e_phnum);

	issw->acc_image_descr->pgm_hdr_tbl = (void *)issw->current_issw_addr;

	issw->current_issw_addr += sizeof(Elf32_Phdr) * elf->header->e_phnum;
	issw->current_issw_addr =
				(void *)ALIGN((u32)issw->current_issw_addr, 4);

	issw->acc_image_descr->signature = elf->signature_ptr;
	issw->acc_image_descr->nbr_segment = elf->nbr_segments;

	mcore_segment =
		(struct mcore_segment_descr *)&(issw->acc_image_descr->descr);

	copy_contents_of_segment_list(mcore_segment,
				      elf->segments_ptr,
				      elf->header->e_phnum);

	return 0;
}

int mloader_load_modem(block_dev_desc_t *block_dev)
{
	struct modem_mem_data modem_mem;
	struct mcore_elf_data elf;
	struct issw_data issw;
	int retval = 0;

	debug("\nmloader_load_modem\n");

	elf.segments_ptr = NULL;
	issw.acc_image_descr = NULL;

	/* Load modem into default loadaddress */
	if (toc_load_toc_entry(block_dev, CONFIG_MLOADER_TOC_MODEM_NAME, 0, 0,
			       CONFIG_SYS_LOAD_ADDR)) {
		printf("mloader: not possible to load modem\n");
		retval = -1;
		goto exit;
	}

	elf.header = (struct elfhdr *)CONFIG_SYS_LOAD_ADDR;

	if (!IS_ELF(*elf.header)) {
		printf("mloader: Illegal ELF magic\n");
		retval = -1;
		goto exit;
	}

	/* Allocate buffer for program header table. */
	elf.pheader = (Elf32_Phdr *)(CONFIG_SYS_LOAD_ADDR +
						elf.header->e_phoff);

	modem_mem.start = (void *)CONFIG_MODEM_MEM_START;
	modem_mem.total_size = CONFIG_MODEM_MEM_TOTAL_SIZE;
	modem_mem.shared_size = CONFIG_MODEM_MEM_SHARED_SIZE;

	modem_mem.private_size = modem_mem.total_size - modem_mem.shared_size;

	modem_mem.end = (void *)((u32)modem_mem.start +
						modem_mem.total_size - 1);

	elf.segments_ptr = calloc(sizeof(struct segment) * elf.header->e_phnum,
				  1);

	if (elf.segments_ptr == NULL)
		return -1;

	retval = load_segments(&elf,
			       &modem_mem,
			       &issw);

	if (retval != MLOADER_ESUCCESS)
		goto exit;

	qsort(elf.segments_ptr,
	      elf.header->e_phnum,
	      sizeof(struct segment),
	      mloader_cmp_addr);

	/*
	 * Now that the segments are sorted, search for space in the modem
	 * private memory area to store the ISSW data
	 */
	retval = find_space_for_issw_data(&elf,
					  &modem_mem,
					  &issw);

	if (retval != MLOADER_ESUCCESS)
		goto exit;

	transfer_to_issw_data_area(&elf, &issw);

	/*
	 * Call service offered by ISSW to verify ELF
	 * and start the modem side.
	 */
	retval = verify_start_modem(issw.acc_image_descr);

	if (retval)
		printf("mloader: verify and start modem failed. "
		       "errorcode:%d\n", retval);

exit:
	if (elf.segments_ptr != NULL)
		free(elf.segments_ptr);

	if (issw.acc_image_descr != NULL)
		free(issw.acc_image_descr);

	return retval;
}
