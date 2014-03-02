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

#include <debug.h>
#include <string.h>
#include <stdlib.h>

#include "timer.h"
#include "tiny_env.h"
#include "target.h"
#include "target_config.h"
#include "ff.h"
#include "crashdump.h"
#ifdef ENABLE_TEXT
#include "text.h"
#endif

#define CRASHDUMP_DIR_NAME  ""
#define DUMP_FILE_PATTERN_EXT   ".elf"
#define DUMP_FILE_PATTERN_PREFIX    "dump_"
#define DUMP_FILE_PATTERN_FULL_LEN  12
#define DUMP_FILE_PATTERN_SHORT_LEN 8
#define DUMP_FILE_PATTERN_PREFIX_LEN    5
#define DUMP_FILE_PATTERN_SUFFIX_LEN    3
#define DUMP_FILE_NUM_MAX   999
#define FULL_PATH_LEN   256
#define PT_NOTE 4
#define PT_LOAD 1
#define PT_NULL 0

typedef struct {
    uint32_t size;
    uint32_t start;
} mem_region_t;

/* ELF32 header */
typedef struct {
    unsigned char   ident[16];  /* ELF Identification */
    unsigned short  type;   /* object file type */
    unsigned short  machine;    /* machine */
    unsigned int    version;    /* object file version */
    unsigned int    entry;  /* virtual entry point */
    unsigned int    phoff;  /* program header table offset */
    unsigned int    shoff;  /* section header table offset */
    unsigned int    flags;  /* processor-specific flags */
    unsigned short  ehsize; /* ELF header size */
    unsigned short  phentsize;  /* program header entry size */
    unsigned short  phnum;  /* number of program header entries */
    unsigned short  shentsize;  /* section header entry size */
    unsigned short  shnum;  /* number of section header entries */
    unsigned short  shstrndx;   /* offset of "section header string table" */
} elf32_hdr_t;

/* Program Header */
typedef struct {
    unsigned int    type;   /* segment type */
    unsigned int    offset; /* segment offset */
    unsigned int    vaddr;  /* virtual address of segment */
    unsigned int    paddr;  /* physical address - ignored? */
    unsigned int    filesz; /* number of bytes in file for seg. */
    unsigned int    memsz;  /* number of bytes in mem. for seg. */
    unsigned int    flags;  /* flags */
    unsigned int    align;  /* memory alignment */
} elf32_program_hdr_t;

static mem_region_t* mem_regions;
static uint32_t nr_regions;
static char elf_fname[DUMP_FILE_PATTERN_FULL_LEN+1];
static char full_path[FULL_PATH_LEN];

static FIL     filehandle;
static FATFS   fs;

static bool is_digit(char c)
{
    return ((c >='0') && (c <='9'));
}

static void to_lower(char *str)
{
    char *ptr = str;

    while (*ptr) {
        if((*ptr >='A') && (*ptr <='Z'))
            *ptr = (*ptr) | 0x20;
            ptr++;
    }
}

static uint32_t dec_str_to_unit32(char** x)
{
    uint32_t val = 0;
    char *t = *x;

    dprintf(INFO, "t:%s\n", t);
    while (*t != '\0') {
        if (*t >= '0' && *t <= '9') {
            val = 10 * val + (*t - '0');
        } else {
             switch (*t) {
                case 'G': val *= 1024;
                    // Fall through
                case 'M': val *= 1024;
                    // Fall through
                case 'K': val *= 1024;
                    break;
                case '@': t++;
                    *x = t;
                    return val;
                case ' ': *x = t;
                    return val;
                default:  *x = t;
                    return val;
            }
        }
        t++;
        dprintf(SPEW, "t:%s val: %d\n", t, val);
    }
    *x = t;
    return val;
}

static int parse_memory_regions(void)
{
    char*   memargdata = NULL;
    char*   tmp;
    uint32_t    handled_regions = 0;

    memargdata = tenv_getval("memargs");
    if (NULL == memargdata) {
        dprintf(CRITICAL,"parse_memory_regions: no memory regions to dump\n");
        return -1;
    }
    dprintf(INFO, "memargs:%s\n", memargdata);
    dprintf(INFO, "memargs length:%d\n", strlen(memargdata));

    tmp = memargdata;
    while (*tmp != '\0') {
        dprintf(SPEW, "tmp:%s\n", tmp);
        if ((strncmp(tmp, "mem=", 4)==0) || (strncmp(tmp, "mem_issw=", 9)==0)) {
            nr_regions++;
        }
        while (*tmp != ' ' && *tmp != '\0') {
            tmp++;
        }
        while (*tmp == ' ' && *tmp != '\0') {
            tmp++;
        }
    }

    /* Add one entry to hold SOC_AVS_BACKUPRAM */
    nr_regions += 1;

    dprintf(INFO, "Number of regions to dump is %d\n", nr_regions);

    mem_regions = malloc(nr_regions * sizeof(mem_region_t));
    if (NULL == mem_regions) {
        dprintf(CRITICAL,"parse_memory_regions: failed to allocate mem_regions array\n");
        return -2;
    }

    while (*memargdata != '\0') {
        dprintf(SPEW, "memargdata:%s\n", memargdata);
        if (strncmp(memargdata, "mem=", 4)==0) {
            memargdata += 4;
            mem_regions[handled_regions].size = dec_str_to_unit32(&memargdata);
            mem_regions[handled_regions].start = dec_str_to_unit32(&memargdata);
            dprintf(SPEW, "mem_regions[%d].size = %08X\n", handled_regions,
                    mem_regions[handled_regions].size);
            dprintf(SPEW, "mem_regions[%d].start = %08X\n", handled_regions,
                    mem_regions[handled_regions].start);
            handled_regions++;
        } else if (strncmp(memargdata, "mem_issw=", 9)==0) {
            uint32_t mem_issw_size;
            memargdata += 9;
            mem_issw_size = dec_str_to_unit32(&memargdata);
            if (mem_issw_size > 0x100000)
                mem_issw_size = 0x100000;
            mem_regions[handled_regions].size = mem_issw_size;
            mem_regions[handled_regions].start = dec_str_to_unit32(&memargdata);
            dprintf(SPEW, "mem_regions[%d].size = %08X\n", handled_regions,
                    mem_regions[handled_regions].size);
            dprintf(SPEW, "mem_regions[%d].start = %08X\n", handled_regions,
                    mem_regions[handled_regions].start);
            handled_regions++;
        }
        while (*memargdata != ' ' && *memargdata != '\0') {
            memargdata++;
        }
        while (*memargdata == ' ' && *memargdata != '\0') {
            memargdata++;
        }
    }

    /* Add SOC_AVS_BACKUPRAM */
    mem_regions[handled_regions].size = SOC_AVS_BACKUPRAM_LEN;
    mem_regions[handled_regions].start = SOC_AVS_BACKUPRAM;

    dprintf(SPEW, "mem_regions[%d].size = %08X\n", handled_regions,
                    mem_regions[handled_regions].size);

    dprintf (INFO, "parse_memory_regions: done\n");
    return 0;
}

static int init_fs(void)
{
    if(!mmc_sd_detected()) {
        printf ("Please insert SD card\n");
#ifdef ENABLE_TEXT
        text_draw (0, 0, "Please insert SD-card");
#endif
        while (!mmc_sd_detected()) {
            printf (".");
            udelay (1000000);
        }
    }

    (void)f_mount (0, &fs);

    return 0;
}

static int open_fs(void)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;

    i = 0; /* file number counter */
    /* Parse crashdump dir */
    res = f_opendir(&dir, CRASHDUMP_DIR_NAME);
    dprintf (INFO, "%s: f_opendir(%s) = %d\n", __func__, CRASHDUMP_DIR_NAME, res);
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
                break;  /* Break on error or end of dir */
            fn = fno.fname;

            if (!(fno.fattrib & AM_DIR)) {
                to_lower (fn);
                if (strstr (fn, DUMP_FILE_PATTERN_PREFIX) &&
                    strstr (fn, DUMP_FILE_PATTERN_EXT)) {
                    dprintf(INFO, "Found %s\n", fn);
                    i++;    /* file name pattern match, increase counter */
                }
            }
        }
    }

    dprintf (INFO, "%s: res %d, i %d\n", __func__, res, i);
    if (res == FR_OK) {
        if (i <= DUMP_FILE_NUM_MAX) {
            /* Generate filename dump_nnn.elf */
            sprintf(elf_fname,"%s%03d%s",DUMP_FILE_PATTERN_PREFIX,i,DUMP_FILE_PATTERN_EXT);

            /* Full path */
            sprintf(full_path,"%s/%s",CRASHDUMP_DIR_NAME,elf_fname);
            printf("%s: Dump to file: %s\n", __func__, full_path);

            res = f_open (&filehandle, full_path, FA_WRITE | FA_CREATE_ALWAYS);
            if (res != FR_OK) {
                return -5;
            }
        } else {
            return -4;
        }
    } else {
        return -1;
    }

    return 0;
}

static void close_fs(void)
{
    FRESULT res;

    /* Close file */
    res = f_close (&filehandle);
    dprintf (INFO, "%s (%d): res %d\n", __func__, __LINE__, res);

    /* Unmount FS */
    res = f_mount (0, NULL);
    dprintf (INFO, "%s (%d): res %d\n", __func__, __LINE__, res);
}

/*
 * Check program header and segment
 * Truncate note segments.
 * Return segment size.
 */
static uint32_t check_phdr(elf32_program_hdr_t *phdr)
{
    uint32_t i;
    uint32_t *note;

    if (phdr->type == PT_NOTE) {
        /* see Linux kernel/kexec.c:append_elf_note() */
        note = (uint32_t *)(phdr->paddr);
        for (i = 0; i < phdr->filesz>>2u;) {
            if (note[i] == 0 && note[i+1] == 0 && note[i+2] == 0)
                return i<<2u;
            i += 3 + (note[i] + 3) / 4 + (note[i+1] + 3) / 4;
        }
    }
    return phdr->filesz;
}

static elf32_hdr_t *create_elfhdr(mem_region_t *regions, unsigned int region_no)
{
    elf32_hdr_t *ehdr;
    elf32_program_hdr_t *phdr;
    unsigned int hdr_size;
    unsigned int i;

    hdr_size = sizeof(*ehdr)+(region_no+1)*sizeof(*phdr);
    ehdr = malloc(hdr_size);
    if(ehdr == NULL) {
        return NULL;
    }
    memset(ehdr, 0, hdr_size);

    ehdr->ident[0] = 0x7f;
    ehdr->ident[1] = 'E';
    ehdr->ident[2] = 'L';
    ehdr->ident[3] = 'F';
    ehdr->ident[4] = 1;    /* 32 Bit format object*/
    ehdr->ident[5] = 1;    /* Little-Endian */
    ehdr->ident[6] = 1;
    ehdr->ident[7] = 0;    /* No extension */
    ehdr->ident[8] = 0;
    ehdr->type = 4;
    ehdr->machine = 40;
    ehdr->version = 1;
    ehdr->entry = 0x0;
    ehdr->phoff = sizeof(*ehdr);
    ehdr->shoff = 0x0;
    ehdr->flags = 0x0;
    ehdr->ehsize = sizeof(*ehdr);
    ehdr->phentsize = sizeof(*phdr);
    ehdr->phnum = region_no+1;
    ehdr->shentsize = 0;
    ehdr->shnum = 0;
    ehdr->shstrndx = 0;

    phdr = (elf32_program_hdr_t*)(ehdr + 1);
    phdr[0].type = PT_NOTE;

    for (i=0; i<region_no; i++) {
        phdr[i+1].type   = PT_LOAD;
        phdr[i+1].vaddr  = 0xc0000000 + regions[i].start;
        phdr[i+1].paddr  = regions[i].start;
        phdr[i+1].filesz = regions[i].size;
        phdr[i+1].memsz  = regions[i].size;
        phdr[i+1].flags  = 7;
    }
    return ehdr;
}

static int write_elf(elf32_hdr_t *elfhdr_addr)
{
    elf32_hdr_t *oldhdr = elfhdr_addr;
    elf32_hdr_t *ehdr;
    elf32_program_hdr_t *phdr;
    uint32_t i;
    uint32_t offset;
    uint32_t tot;
    uint32_t phdr_cnt;
    uint32_t notes_cnt = 0;
    uint32_t save;
    uint32_t len;
    int      rc;
    uint32_t bw;

    offset = oldhdr->ehsize + oldhdr->phentsize * oldhdr->phnum;
    ehdr = (elf32_hdr_t *) malloc(offset);
    if (ehdr == NULL) {
        dprintf(CRITICAL,"write_elf: ELF header malloc error\n");
        return -1;
    }
    memcpy(ehdr, oldhdr, offset);

    /*
     * check program header entries and update length
     * for merged PT_NOTE segments
     */
    tot = 0;
    phdr_cnt = ehdr->phnum;
    printf("write_elf: phdr_cnt=%d\n", phdr_cnt);
    for (i = 0; i < phdr_cnt; i++) {
        phdr = (elf32_program_hdr_t *) ((char *) ehdr + ehdr->ehsize +
            i * ehdr->phentsize);
        len = check_phdr(phdr);
        dprintf(INFO,"write_elf: prog hdr %d: %x ad %x len %x adjusted to %x\n",
            i, (uint32_t) phdr, phdr->paddr, phdr->filesz, len);
        phdr->filesz = len;
        phdr->memsz = len;
        if (phdr->type == PT_NOTE) {    /* note segment */
            tot += len;
            notes_cnt++;
        }
    }
    dprintf(INFO,"write_elf: length of %d note segments: %x\n", notes_cnt, tot);

    /*
     * all PT_NOTE segments have been merged into one.
     * Update ELF Header accordingly
     */
    ehdr->phnum = phdr_cnt - notes_cnt + 1;

    /* write elf header */
    rc = f_write (&filehandle, (void*)ehdr, ehdr->ehsize, &bw);
    if((rc != FR_OK) || (bw != (uint32_t)ehdr->ehsize)) {
        free (ehdr);
        dprintf(CRITICAL, "write_elf: ehdr write error %d\n",rc);
        return -1;
    }

    /* write program headers */
    offset = ehdr->ehsize + ehdr->phentsize * ehdr->phnum;
    printf("write_elf: write Phdr: phdr_cnt=%d\n", phdr_cnt);
    for (i = 0; i < phdr_cnt; i++) {
        phdr = (elf32_program_hdr_t *) ((char *)ehdr + ehdr->ehsize +
            i * ehdr->phentsize);
        save = phdr->filesz;
        if (i == 0) {
            phdr->filesz = tot;
            phdr->memsz = tot;
        } else if (phdr->type == PT_NOTE) /* note segment */
            continue;
        phdr->offset = offset;
        dprintf(INFO,"write_elf: prog hdr %d: %x ad %x len %x off %x\n",
            i, (uint32_t)phdr, phdr->paddr, phdr->filesz, phdr->offset);
        offset += phdr->filesz;

        rc = f_write (&filehandle, (void *)phdr,ehdr->phentsize, &bw);
        if((rc != FR_OK) || (bw != (uint32_t)ehdr->phentsize)) {
            dprintf(CRITICAL,"write_elf: phdr write error %d\n",rc);
            free(ehdr);
            return -1;
        }
        phdr->filesz = save;
        phdr->memsz = save;
    }

    /* write segments */
    printf("write_elf: write segments...\n");
    for(i = 0; i < phdr_cnt; i++) {
        phdr = (elf32_program_hdr_t *) ((char *)ehdr + ehdr->ehsize +
            i * ehdr->phentsize);
        if (phdr->type > PT_NULL) {
            printf ("write_elf: write segment %d, %d bytes\n", i, phdr->filesz);
            rc = f_write (&filehandle, (void *)phdr->paddr,phdr->filesz, &bw);
            if((rc != FR_OK) || (bw != (uint32_t)phdr->filesz)) {
                dprintf(CRITICAL,"write_elf: memory segment write error %d\n",rc);
                free(ehdr);
                return -1;
            }
        }
    }
    dprintf(INFO,"write_elf: done\n");
    free(ehdr);
    return 0;
}

static int create_crashdump(void)
{
    elf32_hdr_t *elf_header;

    elf_header = create_elfhdr(mem_regions,nr_regions);
    if (NULL == elf_header) {
        dprintf(CRITICAL,"create_crashdump: create_elfhdr failed\n");
        return -1;
    }

    return write_elf(elf_header);
}

static int handle_crashdump_sd(void)
{
    int rc;

    /* get memory regions to dump */
    nr_regions = 0;
    rc = parse_memory_regions();
    dprintf(INFO, "crashdump_sd: parse_memory_regions: %d \n",rc);
    if(rc)
        return rc;
    if(!nr_regions) {
        dprintf(INFO, "crashdump_sd: no memregions to dump\n");
        return -1;
    }

    /* init fs */
    rc = init_fs();
    dprintf(INFO, "crashdump_sd: init_fs: %d \n",rc);
    if(rc) {
        free(mem_regions);
        return rc;
    }

    /* open file for crashdump */
    rc = open_fs();
    dprintf(INFO, "crashdump_sd: file_open: %d \n",rc);
    if(rc) {
        free(mem_regions);
        return rc;
    }

    /* create crashdump */
#ifdef ENABLE_TEXT
    text_draw (0, 0, "Writing to SD-card");
#endif
    rc = create_crashdump();
    dprintf(INFO, "crashdump_sd: create_crashdump: %d \n",rc);
    free(mem_regions);

    /* close file system */
    close_fs();
    return rc;
}

static int handle_crashdump_usb(void)
{
    dprintf(INFO, "crashdump_usb: not supported yet\n");
    return -1;
}

int handle_crashdump(crashdump_device_t dump_dev)
{
    int rc;
#ifdef ENABLE_TEXT
    char buf [80];
#endif

    printf ("Crash handling: Trying to save crash data to SD card\n");

#ifdef ENABLE_TEXT
    text_draw (0, 0, "CRASH HANDLER ACTIVE");
#endif

    switch(dump_dev) {
        case DUMP_TO_SD:
            rc = handle_crashdump_sd();
            break;
        case DUMP_TO_USB:
            rc = handle_crashdump_usb();
            break;
        default:
            dprintf(CRITICAL, "handle_crashdump: unknown device: %d \n",dump_dev);
            rc = -1;
            break;
    }
#ifdef ENABLE_TEXT
    sprintf (buf, "CRASH HANDLER DONE rc = %d", rc);
    text_draw (0, 0, buf);
#endif
    return rc;
}
