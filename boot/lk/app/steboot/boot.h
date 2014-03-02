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
 * ATAG code taken from
 * http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html
 * Appendix B.
 * That code is released under BSD license.
 * Copyright (c) 2004 Vincent Sanders
 */

#ifndef _BOOT_H
#define _BOOT_H

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#ifndef BOOT_NAME
#define BOOT_NAME "boot"
#endif

#ifndef ITP_NAME
#define ITP_NAME   "ITP"
#endif

#ifndef MODEM_NAME
#define MODEM_NAME "MODEM"
#endif

#ifndef IPL_NAME
#define IPL_NAME   "IPL"
#endif

#ifndef MISC_NAME
#define MISC_NAME  "misc"
#endif

#ifndef RECO_NAME
#define RECO_NAME  "recovery"
#endif

#ifndef SYST_NAME
#define SYST_NAME  "system"
#endif

#ifndef ARB1_NAME
#define ARB1_NAME  "ARB_TBL1"
#endif

#ifndef ARB2_NAME
#define ARB2_NAME  "ARB_TBL2"
#endif

#ifndef ROOTFS_NAME
#define ROOTFS_NAME  "ROOTFS"
#endif

#define BOOT_HDR_SIZE   0x800

#define DRAM_BASE 0x00000000

/* CSPSA Fast Parameter */
#define CSPSA_FAST_PARAM_BOOT 0x00000000

/* CSPSA Fast Parameter Boot values */
#define CSPSA_LOAD_KERNEL    0x00000001
#define CSPSA_LOAD_MODEM     0x00000002
#define CSPSA_LOAD_ITP       0x00000004
#define CSPSA_LOAD_MODEM_ITP 0x00000008

/* Vibrator */
#define VIBRATOR_TIME_IN_MS 30
#define VIBRATOR_INTENSE    75

/* Boot header errors */
#define ANDROID_HEADER_ERROR -1
#define RAMDISK_SIZE_ERROR   -2

/* list of possible tags */
#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009
#define ATAG_BOOTTIME   0x41000403

/* structures for each atag */
struct atag_header {
    uint32_t size; /* length of tag in words including this header */
    uint32_t tag;  /* tag type */
};

struct atag_core {
    uint32_t flags;
    uint32_t pagesize;
    uint32_t rootdev;
};

struct atag_mem {
    uint32_t size;
    uint32_t start;
};

struct atag_initrd2 {
    uint32_t start;
    uint32_t size;
};

struct atag_serialnr {
    uint32_t low;
    uint32_t high;
};

struct atag_revision {
    uint32_t rev;
};

struct atag_cmdline {
    char cmdline[1];
};

#define BOOTTIME_MAX 10
#define BOOTTIME_MAX_NAME_LEN 64

struct boottime_entry {
	uint32_t time; /* in us */
	uint8_t  name[BOOTTIME_MAX_NAME_LEN];
};

struct atag_boottime {
	struct boottime_entry entry[BOOTTIME_MAX];
	uint32_t idle;  /* in us */
	uint32_t total; /* in us */
	uint8_t  num;
};

struct atag {
    struct atag_header hdr;
    union {
        struct atag_core        core;
        struct atag_mem         mem;
        struct atag_initrd2     initrd2;
        struct atag_serialnr    serialnr;
        struct atag_revision    revision;
        struct atag_cmdline     cmdline;
        struct atag_boottime    boottime;
    } u;
};


struct misc_message {
    char command[32];
    char status[32];
    char recovery[1024];
};

typedef enum {
    LK_MODE_NORMAL,
    LK_MODE_FASTBOOT,
    LK_MODE_RECOVERY,
    LK_MODE_CRASHDUMP
} t_lk_mode_type;

typedef enum {
    LK_FLASH_STATUS_NOT_INITIATED,
    LK_FLASH_STATUS_DATA_OK,
    LK_FLASH_STATUS_BOOT_SYST_OK_NO_RECO,
    LK_FLASH_STATUS_NO_BOOT_OR_SYST,
    LK_FLASH_STATUS_NO_RECO,
    LK_FLASH_STATUS_NO_HEADER,
    LK_FLASH_STATUS_NO_RAMDISK,
    LK_FLASH_STATUS_SECURITY_ERROR,
    LK_FLASH_STATUS_ITP_ERROR
} t_lk_flash_status;


#define NORMAL_BOOT   0
#define RECOVERY_BOOT 1


/*******************************************************************************
 * Declaration of external functions
 ******************************************************************************/

t_lk_flash_status boot_from_flash(char *boot_name);
int start_linux(boot_src_info_t *boot_info);
int start_modem (void);
int buf_to_image_hdr (struct boot_img_hdr *img_hdr, uint32_t *buf);
void print_image_hdr (struct boot_img_hdr *hdr);
void handle_arb_tables(void);


#endif // _BOOT_H
