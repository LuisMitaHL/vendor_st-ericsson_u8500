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

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <debug.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <reg.h>
#include <kernel/thread.h>
#include <arch/ops.h>
#include <platform/timer.h>
#include <dev/keys.h>
#include <target.h>

#include "bootimg.h"
#include "app.h"
#include "toc.h"
#include "mmc_if.h"
#include "target_config.h"        /* db8500_l2_clean_invalidate_lock/enable */
#include "cspsa_fp.h"
#include "tiny_env.h"
#if ENABLE_FASTBOOT
#include "fastboot.h"
#endif
#include "bass_app.h"
#include "abx500.h"
#include "reboot.h"
#include "fs_sd_wrapper.h"
#include "crashdump.h"
#include "abx500_vibrator.h"

#include "boot.h"
#include "boottime.h"
#include "machineid.h"

#ifdef ENABLE_TEXT
#include "text.h"
#endif

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

const unsigned char CurrentDate[] = __DATE__ " " __TIME__ " LK:"LKVERSION  " BASS_APP:" BASSAPVERSION " ";


static        struct atag               *params; /* used to point at the current tag */
static        t_lk_mode_type            lk_mode;

#define tag_next(t) ((struct atag *)((uint32_t *)(t) + (t)->hdr.size))
#define tag_size(type)  ((sizeof(struct atag_header) + sizeof(struct type)) >> 2)


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

extern void l2cc_enable(void);


static void setup_core_tag(void * address);
static void setup_initrd2_tag(uint32_t start, uint32_t size);
static void setup_mem_tag(uint32_t start, uint32_t len);
static void setup_cmdline_tag(const char * line);
static void setup_boottime_tags(void);
static void setup_end_tag(void);
static int setup_tags(void *parameters, boot_img_hdr *hdr);
static int load_from_mmc(const char                *partition,
                         boot_src_info_t           *boot_info,
                         bass_signed_header_info_t *sec_hdr_info);
static int load_from_ram(boot_src_info_t *boot_info);
static int load_signed_from_mmc(const char *partition, boot_src_info_t *boot_info);
static t_lk_mode_type get_lk_mode(void);
static int secure_boot_enabled(void);
static int stop_auto_boot (void);
static void steboot_close(void);
static int start_itp(void);
#if ENABLE_FASTBOOT
static void handle_fastboot(void);
#endif
static void handle_recovery(void);
static void clear_misc(void);
static int does_toc_exist(char *toc_name);
static t_lk_flash_status check_flash(char *toc_name);


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/* Application init function */
void steboot_init(const struct app_descriptor *app)
{
    bool exit_loop = false;
    t_lk_flash_status toc_result  = LK_FLASH_STATUS_NOT_INITIATED;
    t_lk_flash_status data_result = LK_FLASH_STATUS_NOT_INITIATED;

    printf("LK version: %s\n", CurrentDate);

    dprintf(INFO,"STE Boot Init\n");

    boottime_tag("run_vibr");

/* Vibrator not yet available for ab8540 */
#if CONFIG_ENABLE_VIBRATOR
    run_vibrator(VIBRATOR_TIME_IN_MS, VIBRATOR_INTENSE);
#endif /* CONFIG_ENABLE_VIBRATOR */

    lk_mode = get_lk_mode();
    if (!is_env_loaded() &&
        (LK_MODE_NORMAL == lk_mode || LK_MODE_RECOVERY == lk_mode)) {
        dprintf(CRITICAL, "Env not loaded! Could not continue!\n");
        return;
    }

    while (!exit_loop) {
        switch (lk_mode) {
            case LK_MODE_NORMAL:
                if (!stop_auto_boot()) {
                    toc_result = check_flash(BOOT_NAME);
                    if (toc_result == LK_FLASH_STATUS_DATA_OK ||
                        toc_result == LK_FLASH_STATUS_BOOT_SYST_OK_NO_RECO) {
                        data_result = boot_from_flash(BOOT_NAME);
                        if (data_result == LK_FLASH_STATUS_SECURITY_ERROR ||
                            data_result == LK_FLASH_STATUS_ITP_ERROR) {
                            exit_loop =true;
                        } else if ((data_result == LK_FLASH_STATUS_NO_HEADER ||
                                    data_result == LK_FLASH_STATUS_NO_RAMDISK) &&
                                    toc_result == LK_FLASH_STATUS_DATA_OK) {
                            lk_mode = LK_MODE_RECOVERY;
                        } else {
#if ENABLE_FASTBOOT
                            lk_mode = LK_MODE_FASTBOOT;
#else
                            exit_loop =true;
#endif
                        }
                    } else if (toc_result == LK_FLASH_STATUS_NO_BOOT_OR_SYST) {
                        lk_mode = LK_MODE_RECOVERY;
                    } else {
#if ENABLE_FASTBOOT
                        lk_mode = LK_MODE_FASTBOOT;
#else
                        exit_loop =true;
#endif
                    }
                } else {
                    exit_loop =true;
                }
                break;
            case LK_MODE_FASTBOOT:
#if ENABLE_FASTBOOT
                handle_fastboot();
                exit_loop =true;
#else
                exit_loop =true;
#endif
                break;
            case LK_MODE_RECOVERY:
                boottime_tag("reco");
                if (toc_result == LK_FLASH_STATUS_NOT_INITIATED) {
                    toc_result = check_flash(RECO_NAME);
                }
                if (toc_result == LK_FLASH_STATUS_DATA_OK ||
                    toc_result == LK_FLASH_STATUS_NO_BOOT_OR_SYST) {
                    data_result = boot_from_flash(RECO_NAME);
                    if (data_result == LK_FLASH_STATUS_SECURITY_ERROR ||
                        data_result == LK_FLASH_STATUS_ITP_ERROR) {
                        exit_loop =true;
                    } else if (data_result == LK_FLASH_STATUS_NO_HEADER ||
                               data_result == LK_FLASH_STATUS_NO_RAMDISK) {
#if ENABLE_FASTBOOT
                        lk_mode = LK_MODE_FASTBOOT;
#else
                        exit_loop =true;
#endif
                    }
                } else {
#if ENABLE_FASTBOOT
                    lk_mode = LK_MODE_FASTBOOT;
#else
                    exit_loop =true;
#endif
                }
                break;
            case LK_MODE_CRASHDUMP:
                handle_crashdump(DUMP_TO_SD);
                exit_loop = true;
                break;
        }
    }
}


/* LK mode handlers */

t_lk_flash_status boot_from_flash(char *boot_name)
{
    struct toc_entry            *toc_list;
    struct toc_entry            *toc_ent;
    uint32_t                    buf[BOOT_HDR_SIZE / sizeof(uint32_t)];
    struct boot_img_hdr         boot_hdr;
    boot_src_info_t             boot_src_info;
    bass_return_code            sec_result;
    bass_signed_header_info_t   sec_hdr_info;
#if BOOTLOADER_START_MODEM
    uint32_t                    cspsa_val;
    struct tee_product_config  product_config;
#endif
    int                         boot_header_result;
#ifdef ENABLE_TEXT
    char text[80];
#endif

    dprintf (INFO, "boot_from_flash: load \"%s\"\n", boot_name);

#if BOOTLOADER_START_MODEM
    sec_result = get_product_config(&product_config);

    /* Only load modem for Linux boot
     * and product is not 9500 */
    if ((0 == strcmp (boot_name, BOOT_NAME)) &&
        (BASS_RC_SUCCESS == sec_result) &&
        (PRODUCT_ID_9500 != product_config.product_id)) {

        dprintf (INFO, "PRODUCT_ID %d\n", product_config.product_id);

        /* - Check CSPSA Fast Param 0xfffffffc for modem/Kernel/ITP load
        *   conditions */
        if (cspsa_fp_read(CSPSA_FAST_PARAM_BOOT, &cspsa_val)) {
            dprintf(INFO, "boot_from_flash: cspsa_fp_read could not read CSPSA_FAST_PARAM_BOOT\n");
            /* As default load kernel and modem */
            cspsa_val = (CSPSA_LOAD_KERNEL | CSPSA_LOAD_MODEM);
        } else {
            dprintf(INFO, "boot_from_flash: cspsa_fp_read CSPSA_FAST_PARAM_BOOT=%08X\n", cspsa_val);
        }

        if (cspsa_val & CSPSA_LOAD_MODEM) {
            dprintf(INFO, "Start modem\n");
            if (start_modem() != 0x0) {
                dprintf(CRITICAL, "boot_from_flash. Could not start the modem\n");
            }
        }

        /* A CSPSA fast-parameter can indicate whether the modem should be
         * booted in normal or ITP mode. This is passed into the kernel to
         * be exported to userspace in order to be used by modem utilities.
         */
        if (cspsa_val & CSPSA_LOAD_MODEM_ITP) {
            create_env("modem_boot_type", "itp", true);
            dprintf(INFO, "modem boot is itp\n");
        } else {
            create_env("modem_boot_type", "normal", true);
            dprintf(INFO, "modem boot is normal\n");
        }

        if (cspsa_val & CSPSA_LOAD_ITP) {
            dprintf(INFO, "Start ITP\n");
            if (start_itp() != 0x0) { /* No return from this function if OK */
                dprintf(CRITICAL, "boot_from_flash: Could not start the ITP\n");
                return LK_FLASH_STATUS_ITP_ERROR;
            }
        }
    }
#endif

    /* Locate requested partition */
    toc_list = toc_addr_get();
    toc_ent = toc_find_id(toc_list, boot_name);

     if (toc_ent == (struct toc_entry *)~0) {
         printf("%s image not found\n", boot_name);
     }

    /* Load header */
    if (-1 == toc_load_part(boot_name, (uint32_t)buf, 0, BOOT_HDR_SIZE)) {
#ifdef ENABLE_TEXT
        sprintf (text, "FAILED TO LOAD %s", boot_name);
        text_draw (0, 0, text);
#endif
        dprintf (CRITICAL, "boot_from_flash: failed to read boot_img header\n");
        return LK_FLASH_STATUS_NO_HEADER;
    }

#if 0
	/* Test to manipulate security signed header */
	buf[12] ^= 0x01;
#endif

    /* Always verify header */
    dprintf (INFO, "boot_from_flash: verify header\n");
    sec_result = verify_signedheader((uint8_t*) buf,
                                     BOOT_HDR_SIZE,
                                     BASS_PL_TYPE_APE_NW_CODE,
                                     &sec_hdr_info);
    if (BASS_RC_SUCCESS != sec_result) {
#ifdef ENABLE_TEXT
        sprintf (text, "VERIFY HEADER FAIL (%d) %s", (int)sec_result, boot_name);
        text_draw (0, 0, text);
#endif
        dprintf(CRITICAL, "boot_from_flash: verify_signedheader failed, returned %d\n",
               (int) sec_result);
        return LK_FLASH_STATUS_SECURITY_ERROR;
    }

    dprintf (INFO, "boot_from_flash: size_of_signed_header 0x%08x\n", sec_hdr_info.size_of_signed_header);

    /* Parse boot image header */
    boot_header_result = buf_to_image_hdr (&boot_hdr, (uint32_t*)(buf + sec_hdr_info.size_of_signed_header/sizeof(uint32_t)));
    if (0 != boot_header_result) {
        dprintf (CRITICAL, "boot_from_flash: INVALID BOOT IMAGE\n");
        if (ANDROID_HEADER_ERROR == boot_header_result) {
            return LK_FLASH_STATUS_NO_HEADER;
        } else {
            return LK_FLASH_STATUS_NO_RAMDISK;
        }
    }
    print_image_hdr (&boot_hdr);

    /* Start Linux */
    boot_src_info.hdr = &boot_hdr;
    boot_src_info.buf = NULL;

    if (load_from_mmc(boot_name, &boot_src_info, &sec_hdr_info)) {
#ifdef ENABLE_TEXT
        sprintf (text, "LOAD PAYLOAD FAIL %s", boot_name);
        text_draw (0, 0, text);
#endif
        dprintf (CRITICAL, "ERROR! Failed to load boot image payload\n");
        return LK_FLASH_STATUS_SECURITY_ERROR;
    }

    if (0 == strcmp (boot_name, RECO_NAME)) {
        dprintf(INFO, "Setting recovery environment variable\n");
        create_env("bootcmd", "run emmcbootrec", true);
    }

#ifdef ENABLE_ARB
    handle_arb_tables();
#endif

    start_linux(&boot_src_info); /* Does not return if OK*/

    return LK_FLASH_STATUS_SECURITY_ERROR;
}


#if ENABLE_FASTBOOT
static void handle_fastboot(void)
{
    int rc;
    dprintf(INFO, "handle_fastboot\n");
#ifdef ENABLE_TEXT
    char text[80];
#endif


    if (secure_boot_enabled()) {
#ifdef ENABLE_TEXT
        text_draw (0, 0, "fastboot: Only allowed in R&D");
#endif
        dprintf(CRITICAL, "fastboot: Only allowed in R&D\n");
        return;
    }
    /* Init and run fastboot */
    rc = fastboot_init();
    if (rc) {
#ifdef ENABLE_TEXT
        sprintf(text, "fastboot: init failed, ret=%d", rc);
        text_draw (0, 0, text);
#endif
        dprintf(CRITICAL, "fastboot: init failed, ret=%d\n", rc);
        return;
    }
    dprintf(INFO, "fastboot: init done\n");
    return;
}
#endif



/* Start Linux kernel.
 * Kernel and ramdisk must already be in correct RAM location
 */
int start_linux(boot_src_info_t *boot_info)
{
    unsigned int machine;
    void (*theKernel)(int zero, int arch, uint32_t params);
    uint32_t parm_at;

    if (!boot_info) {
        printf("No boot info param!\n");
        return -1;
    }

    /* Check boot image header, set up by steboot_init or fastboot*/
    if (!boot_info->hdr) {
        printf("No valid boot image!\n");
        return -1;
    }

    machine = get_machine_id ();
    if (MACH_TYPE_INVALID == machine) {
        dprintf (CRITICAL, "start_linux: Machine type not correct!\n");
        return -1;
    }

    /* TODO: Confirm this location! Dynamic? */
    parm_at = DRAM_BASE + 0x100;

    boottime_tag("start_kernel");

    /* Set up kernel parameters */
    if (-1 == setup_tags((void *)parm_at, boot_info->hdr)) {    /* sets up parameters */
        dprintf (INFO, "start_linux: failed to setup environment\n");
        return -1;
    }

    /* if second_size != 0: jump to second_addr
     * else: jump to kernel_addr */
    if (boot_info->hdr->second_size > 0) {
        /* TODO: Same prototype for second? */
        theKernel = (void (*)(int, int, uint32_t))boot_info->hdr->second_addr;
    } else {
        theKernel = (void (*)(int, int, uint32_t))boot_info->hdr->kernel_addr;
    }

    /* finalize LK job */
    steboot_close();

    /* stop irq */
    enter_critical_section();

    /* Disable L1 caches */
    arch_disable_cache(UCACHE);

    /*
     * Cannot disable L2 in NS mode, furthermore ux500 Linux expects
     * an enabled L2 cache.
     * NOTE: must be a clean and invalidate, invalidate alone is not
     * sufficient.
     * Second NOTE: ... really? Seems OK anyway!
     * L2 cache lock is also needed to avoid L2 cache use during kernel decompression
     */
    db8500_l2_clean_invalidate_lock();

#if ARM_WITH_MMU
    arch_disable_mmu();
#endif
    /* ux500 Linux expects it, so make sure it is enabled */
    /* db8500_l2_enable();*/  /* TODO: Doesn't seem so? */

    dprintf(INFO, "starting kernel at 0x%08x, machine %d\n", boot_info->hdr->kernel_addr, machine);

    /* jump to kernel with register set */
    theKernel(0, machine, parm_at);
    /* NOTREACHED */

    return -1;
}


#if BOOTLOADER_START_MODEM
int start_modem(void)
{
    struct toc_entry        *toc_list;
    struct toc_entry        *toc_ent;
    uint32_t                ab8500_cutid = ab8500_chip_id_read();

    /* Locate MODEM partition */
    toc_list = toc_addr_get();
    toc_ent = toc_find_id(toc_list, MODEM_NAME);

    if (toc_ent == (struct toc_entry *)~0) {
        printf("%s image not found\n", MODEM_NAME);
        return -1;
    }

    /* Load MODEM */
    toc_ent = toc_load_id(MODEM_NAME, toc_ent->loadaddr);
    if (toc_ent == (struct toc_entry *)~0) {
        printf("Could not load %s\n", MODEM_NAME);
        return -1;
    }

    /* Check that IPL exists */
    toc_ent = toc_find_id(toc_list, IPL_NAME);

    if (toc_ent == (struct toc_entry *)~0) {
        printf("%s image not found\n", IPL_NAME);
        return -1;
    }

    /* Disable L1 caches */
    arch_disable_cache(UCACHE);

    /*
     * Cannot disable L2 in NS mode, furthermore ux500 Linux expects
     * an enabled L2 cache.
     * NOTE: must be a clean and invalidate, invalidate alone is not
     * sufficient.
     * L2 cache lock is also needed to avoid L2 cache use during kernel decompression
     */
    db8500_l2_clean_invalidate_lock();

#if ARM_WITH_MMU
    arch_disable_mmu();
#endif

    /* Let Security handle the rest of the modem start */
    dprintf(INFO, "Calling Security with ab8500_cutid %08X\n", ab8500_cutid);
    if (BASS_RC_SUCCESS != bass_u8500_verify_start_modem(ab8500_cutid)) {
        printf("Security could not start the modem\n");
        return -1;
    }
    dprintf(INFO, "Modem is now started by Security\n");

    return 0;
}


static int start_itp(void)
{
    struct toc_entry            *toc_list;
    struct toc_entry            *toc_ent;
    uint32_t                    *itp_data;
    bass_return_code            sec_result;
    bass_signed_header_info_t   sec_hdr_info;
    uint32_t                    len;
    bass_payload_type_t         pltype = BASS_PL_TYPE_ITP;
    /* Locate ITP partition */
    toc_list = toc_addr_get();
    toc_ent = toc_find_id(toc_list, ITP_NAME);

    if (toc_ent == (struct toc_entry *)~0) {
        printf("start_itp: %s image not found\n", ITP_NAME);
        return -1;
    }

    dprintf(INFO, "start_itp: %s found loadaddr %08X size %08X\n", ITP_NAME,
            toc_ent->loadaddr, toc_ent->size);

    /* For href500 the load address is 0x0 that leads to NULL pointer problem
     * for MMC driver and probably also ISSW if ITP is signed. Because of that
     * ITP is loaded to a temporary buffer and after all checks cpoied to the
     * correct location.
     */
    len = ((toc_ent->size / 512) + 1) * 512;
    itp_data = (uint32_t*) malloc(len);
    if (NULL == itp_data) {
        printf("start_itp: Could not allocate data for %s\n", ITP_NAME);
        return -1;
    }

    dprintf (INFO, "itp_data 0x%08x, size 0x%08x\n", (uint32_t)itp_data, len);

        /* Load ITP */
    toc_ent = toc_load_id(ITP_NAME, (uint32_t) itp_data);
    if (toc_ent == (struct toc_entry *)~0) {
        printf("start_itp: Could not load %s\n", ITP_NAME);
        free(itp_data);
        return -1;
    }

    /* Always verify header */
    sec_result = verify_signedheader((uint8_t*) itp_data,
                                     toc_ent->size,
                                     pltype,
                                     &sec_hdr_info);
    if (BASS_RC_SUCCESS != sec_result) {
        printf("start_itp: verify_signedheader failed, returned %d\n", (int) sec_result);
        free(itp_data);
        return -1;
    }

    sec_result = bass_check_payload_hash(&sec_hdr_info.ht, &pltype,
                                         (void*) ((uint32_t)itp_data + sec_hdr_info.size_of_signed_header),
                                         sec_hdr_info.pl_size,
                                         (void*) sec_hdr_info.ehash.value,
                                         SHA256_HASH_SIZE);
    if (BASS_RC_SUCCESS != sec_result) {
        printf("start_itp: bass_check_payload_hash failed with %d\n", (int) sec_result);
        free(itp_data);
        return -1;
    }
    dprintf (INFO, "start_itp: Payload verified successfully\n");

    /* Copy to the correct load address */
    dprintf(INFO, "start_itp: Copy itp %d bytes data from %08X to loadaddr %08X\n",
            toc_ent->size - sec_hdr_info.size_of_signed_header,
            (uint32_t) (itp_data + (sec_hdr_info.size_of_signed_header / sizeof(uint32_t))),
            toc_ent->loadaddr);
    memcpy((uint32_t*) toc_ent->loadaddr,
            (itp_data + (sec_hdr_info.size_of_signed_header / sizeof(uint32_t))),
            toc_ent->size - sec_hdr_info.size_of_signed_header);
    dprintf(INFO, "Copied itp_data to loadaddr %08X\n", toc_ent->loadaddr);
    free(itp_data);

    /* finalize LK job */
    steboot_close();

    /* stop irq */
    enter_critical_section();

    /* Disable L1 caches */
    arch_disable_cache(UCACHE);

    /*
     * Cannot disable L2 in NS mode, furthermore ux500 Linux expects
     * an enabled L2 cache.
     * NOTE: must be a clean and invalidate, invalidate alone is not
     * sufficient.
     * L2 cache lock is also needed to avoid L2 cache use during kernel decompression
     */
    db8500_l2_clean_invalidate_lock();

#if ARM_WITH_MMU
    arch_disable_mmu();
#endif
    /* Jump to ITP */
    ((void (*)(void))toc_ent->loadaddr)();

    return 0;
}
#endif


int buf_to_image_hdr (struct boot_img_hdr *img_hdr, uint32_t *buf)
{
    uint32_t ii;
    unsigned char   magic[BOOT_MAGIC_SIZE + 1];

    ii = 0;

    memcpy (img_hdr->magic, (void*)(buf + ii), BOOT_MAGIC_SIZE);
    ii += BOOT_MAGIC_SIZE / sizeof(uint32_t);

    memcpy (magic, img_hdr->magic, BOOT_MAGIC_SIZE);
    magic[BOOT_MAGIC_SIZE] = 0;
    dprintf (INFO, "buf_to_img_hdr: magic \"%s\"\n", magic);

    if (0 != memcmp(img_hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
        dprintf (INFO, "INVALID BOOT IMAGE HEADER!\n");
        return ANDROID_HEADER_ERROR;
    }

    memcpy (&img_hdr->kernel_size, (void*)(buf + ii), sizeof(unsigned));  /* size in bytes */
    ii += 1;

    memcpy (&img_hdr->kernel_addr, (void*)(buf + ii), sizeof(unsigned));  /* physical load addr */
    ii += 1;

    memcpy (&img_hdr->ramdisk_size, (void*)(buf + ii), sizeof(unsigned)); /* size in bytes */
    if (0 == img_hdr->ramdisk_size) {
        dprintf (INFO, "RAMDISK SIZE 0!\n");
        if (does_toc_exist(ROOTFS_NAME))
            dprintf(INFO, "%s exists in flash, LBP mode detected\n", ROOTFS_NAME);
        else
            return RAMDISK_SIZE_ERROR;
    }
    ii += 1;

    memcpy (&img_hdr->ramdisk_addr, (void*)(buf + ii), sizeof(unsigned)); /* physical load addr */
    ii += 1;

    memcpy (&img_hdr->second_size, (void*)(buf + ii), sizeof(unsigned));  /* size in bytes */
    ii += 1;

    memcpy (&img_hdr->second_addr, (void*)(buf + ii), sizeof(unsigned));  /* physical load addr */
    ii += 1;

    memcpy (&img_hdr->tags_addr, (void*)(buf + ii), sizeof(unsigned));    /* physical addr for kernel tags */
    ii += 1;

    memcpy (&img_hdr->page_size, (void*)(buf + ii), sizeof(unsigned));    /* flash page size we assume */
    ii += 1;

    memcpy (img_hdr->unused, (void*)(buf + ii), sizeof(unsigned) * 2);    /* future expansion: should be 0 */
    ii += 2;

    memcpy (img_hdr->name, (void*)(buf + ii), BOOT_NAME_SIZE); /* asciiz product name */
    ii += BOOT_NAME_SIZE / sizeof(uint32_t);

    memcpy (img_hdr->cmdline, (void*)(buf + ii), BOOT_ARGS_SIZE);
    ii += BOOT_ARGS_SIZE / sizeof(uint32_t);

    memcpy (img_hdr->id, (void*)(buf + ii), sizeof(unsigned) * 8); /* timestamp / checksum / sha1 / etc */

    return 0;
}


void print_image_hdr (struct boot_img_hdr *hdr)
{
    unsigned char   magic[BOOT_MAGIC_SIZE + 1];
    unsigned char   name[BOOT_NAME_SIZE + 1]; /* asciiz product name */
    unsigned char   cmdline[BOOT_ARGS_SIZE + 1];

    if(!hdr) {
        printf("print_image_hdr(): bad image header\n");
        return;
    }

    memcpy (magic, hdr->magic, BOOT_MAGIC_SIZE);
    magic[BOOT_MAGIC_SIZE] = 0;

    memcpy (name, hdr->name, BOOT_NAME_SIZE);
    name[BOOT_NAME_SIZE] = 0;

    memcpy (cmdline, hdr->cmdline, BOOT_ARGS_SIZE);
    cmdline[BOOT_ARGS_SIZE] = 0;

    dprintf (INFO, "BOOT image header:\n");
    dprintf (INFO, "magic        \"%s\"\n", magic);
    dprintf (INFO, "kernel_size  0x%08x\n", hdr->kernel_size);
    dprintf (INFO, "kernel_addr  0x%08x\n", hdr->kernel_addr);
    dprintf (INFO, "ramdisk_size 0x%08x\n", hdr->ramdisk_size);
    dprintf (INFO, "ramdisk_addr 0x%08x\n", hdr->ramdisk_addr);
    dprintf (INFO, "second_size  0x%08x\n", hdr->second_size);
    dprintf (INFO, "second_addr  0x%08x\n", hdr->second_addr);
    dprintf (INFO, "tags_addr    0x%08x\n", hdr->tags_addr);
    dprintf (INFO, "page_size    0x%08x\n", hdr->page_size);
    dprintf (INFO, "name         \"%s\"\n", name);
    dprintf (INFO, "cmdline      \"%s\"\n", cmdline);
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/* This function is called prior to exit the LK */
static void steboot_close(void)
{
    /* deactivate ab8500 */
    ab8500_dev_close();
}


static int load_from_mmc(const char                *partition,
                         boot_src_info_t           *boot_info,
                         bass_signed_header_info_t *sec_hdr_info)
{
    uint32_t                offset;
    int32_t                 addr_check;
    uint32_t                start_addr;
    bass_return_code        bass_result;
    bass_payload_type_t     pltype = BASS_PL_TYPE_APE_NW_CODE;
    /* Verify the partition payload */
    addr_check = (int32_t) (boot_info->hdr->kernel_addr - boot_info->hdr->page_size);
    if (addr_check < 0x0) {
        printf("load_from_mmc: Could not load to an address < 0, cannot verify payload\n");
        return -1;
    }
    offset = 0;
    start_addr = boot_info->hdr->kernel_addr - boot_info->hdr->page_size;
    dprintf (INFO, "load_from_mmc: Will load data to address %08X\n", start_addr);

    boottime_tag("load_kernel_start");
    if ((struct toc_entry *)~0 == toc_load_id(partition, start_addr)) {
        dprintf (INFO, "load_from_mmc: failed to read boot_img kernel\n");
        return -1;
    }

#if 0
	/* Test to manipulate security signed payload */
	start_addr += 5;
#endif

    boottime_tag("load_kernel_end");

    bass_result = bass_check_payload_hash(&sec_hdr_info->ht, &pltype,
                                         (void*) (start_addr + sec_hdr_info->size_of_signed_header),
                                         sec_hdr_info->pl_size,
                                         (void*) sec_hdr_info->ehash.value,
                                         SHA256_HASH_SIZE);
    if (BASS_RC_SUCCESS != bass_result) {
        printf("load_from_mmc: bass_check_payload_hash failed with %d\n", (int) bass_result);
        return -1;
    }
    dprintf (INFO, "load_from_mmc: Payload verified\n");
    boottime_tag("payload_verified");

    /* The Security header has moved the kernel forward the length of it */
    boot_info->hdr->kernel_addr += sec_hdr_info->size_of_signed_header;
    /* Copy the ramdisk part of boot image to the correct location */
    start_addr = boot_info->hdr->kernel_addr + (((boot_info->hdr->kernel_size +
                 (boot_info->hdr->page_size -1))/ boot_info->hdr->page_size) *
                 boot_info->hdr->page_size);
    dprintf (INFO, "load_from_mmc: Will copy data from address %08X to address %08X\n",
             start_addr, boot_info->hdr->ramdisk_addr);
    memcpy((void*) boot_info->hdr->ramdisk_addr, (void*) start_addr,
            boot_info->hdr->ramdisk_size);

    /* Copy second part of boot */
    if (boot_info->hdr->second_size > 0) {
        start_addr += (((boot_info->hdr->ramdisk_size + (boot_info->hdr->page_size -1))/
                        boot_info->hdr->page_size) * boot_info->hdr->page_size);
        dprintf (INFO, "load_from_mmc: Will copy data from address %08X to address %08X\n",
                start_addr, boot_info->hdr->second_addr);
        memcpy((void*) boot_info->hdr->second_addr, (void*) start_addr,
                boot_info->hdr->second_size);
    }
    return 0;
}


/*
 * ATAG handling
 */

static void setup_core_tag(void * address)
{
    params = (struct atag *)address;    /* start at given address */

    params->hdr.tag = ATAG_CORE;    /* start with the core tag */
    params->hdr.size = tag_size(atag_core); /* size the tag */

    params->u.core.flags = 0;
    params->u.core.pagesize = 0;
    params->u.core.rootdev = 0; /* typically overidden from cmdline */

    params = tag_next(params);  /* move pointer to next tag */
}


void setup_initrd2_tag(uint32_t start, uint32_t size)
{
    params->hdr.tag = ATAG_INITRD2;     /* Initrd2 tag */
    params->hdr.size = tag_size(atag_initrd2);  /* size tag */

    dprintf (INFO, "ATAG_INITRD2 start 0x%08x, size 0x%08x\n", start, size);

    params->u.initrd2.start = start;    /* physical start */
    params->u.initrd2.size = size;      /* compressed ramdisk size */

    params = tag_next(params);      /* move pointer to next tag */
}


static void setup_mem_tag(uint32_t start, uint32_t len)
{
    params->hdr.tag = ATAG_MEM; /* Memory tag */
    params->hdr.size = tag_size(atag_mem);  /* size tag */

    params->u.mem.start = start;    /* Start of memory area (phys addr) */
    params->u.mem.size = len;   /* Length of area */

    params = tag_next(params);  /* move pointer to next tag */
}


static void setup_cmdline_tag(const char * line)
{
    int linelen = strlen(line);

    if (!linelen)
        return; /* do not insert a tag for an empty commandline */

    params->hdr.tag = ATAG_CMDLINE;     /* Commandline tag */
    params->hdr.size = (sizeof(struct atag_header) + linelen + 1 + 4) >> 2;

    strcpy(params->u.cmdline.cmdline, line); /* commandline into tag */

    params = tag_next(params);      /* move pointer to next tag */
}

static void setup_boottime_tags(void)
{
#ifdef CONFIG_BOOTTIME
    unsigned int i;
    struct boottime_entry *b;

    params->hdr.tag = ATAG_BOOTTIME;
    params->hdr.size = tag_size(atag_boottime);

    params->u.boottime.idle = boottime_idle_get();
    params->u.boottime.total = boottime_idle_done();

    for (i = 0; i < BOOTTIME_MAX; i++) {
        b = boottime_get_entry(i);
        if (b == NULL)
            break;

        dprintf (INFO, "BT: %s %d\n", b->name, b->time);

        params->u.boottime.entry[i].time = b->time;
        strncpy((char *)params->u.boottime.entry[i].name,
            (char *)b->name, BOOTTIME_MAX_NAME_LEN);
        params->u.boottime.entry[i].name[BOOTTIME_MAX_NAME_LEN - 1] = '\0';

    }

    params->u.boottime.num = i;

    params = tag_next(params);
#endif
}

static void setup_end_tag(void)
{
    params->hdr.tag = ATAG_NONE;        /* Empty tag ends list */
    params->hdr.size = 0;           /* zero length */
}


static int setup_tags(void *parameters, boot_img_hdr *hdr)
{
#if WITH_TINY_ENV
    char *env_cmdline;
#endif
    char *default_cmdline =
        "cachepolicy=writealloc noinitrd hwtoolonuart= rdinit=init"
        "init=init board_id=1 crashkernel=crashkernel=1M@0x5600000 "
        "logo.nologo startup_graphics=1 mpcore_wdt.mpcore_margin=359 "
        "androidboot.console=ttyAMA2 root=/dev/ram0 rw  mem=96M@0 "
        "mem_mtrace=15M@96M mem_mshared=1M@111M mem_modem=16M@112M "
        "mali.mali_mem=32M@128M mem=56M@160M hwmem=167M@216M mem_issw=1M@383M "
        "mem=128M@384M console=ttyAMA2,115200n8"
        "blkdevparts=mmcblk0:16384@147456,516096@163840,2048@1024,262144@679936,385024@942080,8@81912 rootwait";
    char *cmdline;
    char *sum_cmd;
    size_t len;

    dprintf(INFO, "setup tags at 0x%08lx\n", (unsigned long)parameters);
    setup_core_tag(parameters);             /* TODO: Clarify this! */
    setup_mem_tag(DRAM_BASE, 0x20000000);   /* 512 at 0x00000000 */

    /* Get ramdisk parameters from boot.img header */
    setup_initrd2_tag(hdr->ramdisk_addr, hdr->ramdisk_size);

    /* Assemble commandline */
#if WITH_TINY_ENV
    env_cmdline = tenv_env_to_cmdline ();
    if (env_cmdline) {
        cmdline = env_cmdline;
    } else {
        dprintf (INFO, "No valid ENV, fall back to default cmdline\n");
        cmdline = default_cmdline;
    }
#else
    cmdline = default_cmdline;
#endif

    len = strlen (cmdline) + 1;
    if ((char*)hdr->cmdline) {
        len += strlen((char*)hdr->cmdline) + 2;
    }

    sum_cmd = (char*)malloc(len);
    if (sum_cmd) {
        /* Concatenate strings */
        strcpy (sum_cmd, cmdline);
        if ((char*)hdr->cmdline) {
            strcat (sum_cmd, " ");
            strcat (sum_cmd, (char*)hdr->cmdline);
        }
        setup_cmdline_tag(sum_cmd);
        free (sum_cmd);
    } else {
        /* Default to hardcoded cmdline */
        setup_cmdline_tag(default_cmdline);
    }

    setup_boottime_tags();

    setup_end_tag();            /* end of tags */

#if WITH_TINY_ENV
    if (env_cmdline) {
        free (env_cmdline);
    }
#endif

    return 0;
}


static int stop_auto_boot ()
{
    int         stop_boot = 0;
#if (WITH_LIB_CONSOLE && !OPTIMIZED_TIME)
    char        c;
    uint32_t    loop;

    boottime_tag("autoboot_delay");

    /* Wait for one second to allow user to enter LK commandline */
    printf("Press any key to stop autoboot...\n");

    for (loop = 0; loop < 10; loop++) {
        if (dgetc(&c, 0) < 0) {
            thread_sleep(100); /* ms */
        } else  {
            stop_boot = 1;
            break;
        }
    }

#endif

#ifdef ENABLE_TEXT
    if (stop_boot) {
        text_draw (0, 0, "LK console ACTIVE");
    }
#endif

    return stop_boot;
}


static t_lk_mode_type get_lk_mode(void)
{
    uint32_t            reset_reason;
    uint32_t            *misc_data;
    struct toc_entry    *toc_list;
    struct toc_entry    *toc_ent;
    struct misc_message *misc_cmd = NULL;

    /* Check if reboot reason is SW_RESET_RECOVERY */
    reset_reason = readl(PRCM_RESET_STATUS);
    dprintf(INFO, "reset_reason %08X\n", reset_reason);
    if (PRCM_A9_SOFTWARE_RESET == reset_reason) {
        dprintf(INFO, "SW reset\n");
        if (SW_RESET_RECOVERY ==
            readw(U8500_PRCMU_TCDM_BASE + PRCM_SW_RST_REASON)) {
            dprintf(INFO, "SW_RESET_RECOVERY\n");
            clear_misc();
            return LK_MODE_RECOVERY;
        }
    }

    /* Check magic keys for Recovery and Fastboot */
    if (keys_get_state(KEY_HOME) || keys_get_state(KEY_VOLUMEUP)) {
        dprintf(INFO, "Magic key recovery pressed\n");
        clear_misc();
        return LK_MODE_RECOVERY;
    }
#if ENABLE_FASTBOOT
    if (keys_get_state(KEY_BACK) || keys_get_state(KEY_VOLUMEDOWN)) {
        dprintf(INFO, "Magic key fastboot pressed\n");
        return LK_MODE_FASTBOOT;
    }
#endif

    /* Check if it is a reboot after a crash */
    if ( ((readl(U8500_PRCMU_TCDM_BASE + PRCM_SW_RST_REASON) & 0x0000ffff) == SW_RESET_CRASH) ||
         ((readl(PRCM_RESET_STATUS)  & PRCM_A9_CPU0_WATCHDOG_RESET) == PRCM_A9_CPU0_WATCHDOG_RESET) ||
         ((readl(PRCM_RESET_STATUS)  & PRCM_A9_CPU1_WATCHDOG_RESET) == PRCM_A9_CPU1_WATCHDOG_RESET)) {
        return LK_MODE_CRASHDUMP;
    }

    /* MISC partition handling */
    toc_list = toc_addr_get();
    toc_ent = toc_find_id(toc_list, MISC_NAME);
    if (toc_ent != (struct toc_entry *)~0) {
        dprintf(INFO, "%s size %08X\n", MISC_NAME, toc_ent->size);
        misc_data = (uint32_t*) malloc(toc_ent->size);
        if (NULL != misc_data) {
            toc_ent = toc_load_id(MISC_NAME, (uint32_t) misc_data);
            if (toc_ent == (struct toc_entry *)~0) {
                dprintf(INFO, "Could not load %s\n", MISC_NAME);
            } else {
                misc_cmd = (struct misc_message*)
                           malloc(sizeof(struct misc_message));
                if (NULL != misc_cmd) {
                    memcpy(misc_cmd, misc_data, sizeof(struct misc_message));
                    if (strcmp(misc_cmd->command, "boot-recovery") == 0x0) {
                        free(misc_cmd);
                        free(misc_data);
                        dprintf(INFO, "Found data in the %s partition\n",
                                MISC_NAME);
                        return LK_MODE_RECOVERY;
                    }
                }
            }
            free(misc_data);
        } else {
            dprintf(INFO, "Could not allocate data for %s\n", MISC_NAME);
        }
    } else {
        dprintf(INFO, "%s image not found\n", MISC_NAME);
    }

    /* Check if it is a boot mode for the fastboot */
#if ENABLE_FASTBOOT
    if (PRCM_A9_SOFTWARE_RESET == reset_reason) {
        dprintf(INFO, "SW reset\n");
        if (SW_RESET_FASTBOOT ==
            readw(U8500_PRCMU_TCDM_BASE + PRCM_SW_RST_REASON)) {
            dprintf(INFO, "SW_RESET_FASTBOOT\n");
            return LK_MODE_FASTBOOT;
        }
    }
#endif

    return LK_MODE_NORMAL;
}


static int secure_boot_enabled(void)
{
    struct tee_product_config   config;
    bass_return_code            sec_result;

    /* Read the product configuration */
    sec_result = get_product_config(&config);
    if (BASS_RC_SUCCESS != sec_result) {
        printf("get_product_config failed, returned %d\n",
               (int) sec_result);
        return 1;
    }
    dprintf(INFO, "product_id %08X rt_flags %08X fuse_flags %08X\n",
            config.product_id, config.rt_flags, config.fuse_flags);


    if ((config.rt_flags & TEE_RT_FLAGS_SECURE_BOOT) ==
        TEE_RT_FLAGS_SECURE_BOOT) {
        return 1;
    }

    return 0;
}


static void clear_misc(void)
{
    struct toc_entry *toc_list;
    struct toc_entry *toc_ent;
    uint32_t         *misc_data;

    toc_list = toc_addr_get();
    toc_ent = toc_find_id(toc_list, MISC_NAME);
    if (toc_ent != (struct toc_entry *)~0) {
        dprintf(INFO, "%s size %08X\n", MISC_NAME, toc_ent->size);
        misc_data = (uint32_t*) malloc(toc_ent->size);
        if (NULL != misc_data) {
            memset((void*) misc_data, 0x00, toc_ent->size);
            if (toc_save_id(MISC_NAME, (uint32_t) misc_data, toc_ent->size)) {
                dprintf(INFO, "Could not save %s\n", MISC_NAME);
            }
            free(misc_data);
        } else {
            dprintf(INFO, "Could not allocate data for %s\n", MISC_NAME);
        }
    } else {
        dprintf(INFO, "%s image not found\n", MISC_NAME);
    }
}


#ifdef ENABLE_ARB
static bass_return_code read_arb_tables_from_secword(
    struct bass_app_arb_table_info  *arbinfo)
{
    bass_return_code ret_code;

    if (arbinfo == NULL) {
        dprintf(INFO, "Input parameter of arbinfo is NULL\n");
        ret_code = BASS_RC_FAILURE;
        goto function_exit;
    }
    arbinfo->toc_id1_size = 0x0;
    arbinfo->toc_id2_size = 0x0;
    arbinfo->table1_size = 0x0;
    arbinfo->table2_size = 0x0;
    arbinfo->toc_id1 = NULL;
    arbinfo->toc_id2 = NULL;
    arbinfo->table1 = NULL;
    arbinfo->table2 = NULL;

    ret_code = bass_get_arb_table_info(true, arbinfo);
    if (ret_code != BASS_RC_SUCCESS) {
        dprintf(INFO, "Fail to get arb table info\n");
        goto function_exit;
    }
    dprintf(INFO, "ret_code of first call =%d\n", ret_code);

    arbinfo->toc_id1 = (char *)malloc(arbinfo->toc_id1_size);
    if (arbinfo->toc_id1 == NULL) {
        dprintf(INFO, "Could not allocate data for arbinfo->toc_id1\n");
        ret_code = BASS_RC_FAILURE;
        goto function_exit;
    }
    arbinfo->toc_id2 =  (char *)malloc(arbinfo->toc_id2_size);
    if (arbinfo->toc_id2 == NULL) {
        dprintf(INFO, "Could not allocate data for arbinfo->toc_id2\n");
        ret_code = BASS_RC_FAILURE;
        goto function_exit;
    }
    arbinfo->table1 = (uint8_t *)malloc(arbinfo->table1_size);
    if (arbinfo->table1 == NULL) {
        dprintf(INFO, "Could not allocate data for arbinfo->table1\n");
        ret_code = BASS_RC_FAILURE;
        goto function_exit;
    }
    arbinfo->table2 =  (uint8_t *)malloc(arbinfo->table2_size);
    if (arbinfo->table2 == NULL) {
        dprintf(INFO, "Could not allocate data for arbinfo->table2\n");
        ret_code = BASS_RC_FAILURE;
        goto function_exit;
    }

    memset((void *) arbinfo->toc_id1, 0x00, arbinfo->toc_id1_size);
    memset((void *) arbinfo->toc_id2, 0x00, arbinfo->toc_id2_size);
    memset((void *) arbinfo->table1, 0x00, arbinfo->table1_size);
    memset((void *) arbinfo->table2, 0x00, arbinfo->table2_size);

    ret_code = bass_get_arb_table_info(false, arbinfo);
    dprintf(INFO, "ret_code of second call =%d\n", ret_code);

function_exit:
    return ret_code;
}


static void save_arb_tables_to_mmc(struct bass_app_arb_table_info *arbinfo)
{
    struct toc_entry        *toc_list;
    struct toc_entry        *toc_ent1;
    struct toc_entry        *toc_ent2;
    uint8_t    *arb1_data;
    uint8_t    *arb2_data;
    uint32_t    new_ent1_length;
    uint32_t    new_ent2_length;

    if (arbinfo == NULL) {
        dprintf(INFO, "arbinfo is NULL, not saved\n");
        goto function_exit;
    }

    /* get TOC size */
    toc_list = toc_addr_get();
    toc_ent1 = toc_find_id(toc_list, arbinfo->toc_id1);
    if (toc_ent1 == (struct toc_entry *)~0) {
        dprintf(INFO, "%s not found\n", arbinfo->toc_id1);
        goto function_exit;
    }
    if (toc_ent1->size < arbinfo->table1_size) {
        new_ent1_length = arbinfo->table1_size;
    } else {
        new_ent1_length = toc_ent1->size;
    }
    arb1_data = (uint8_t *) malloc(new_ent1_length);
    if (arb1_data == NULL) {
        dprintf(INFO, "Could not allocate data for arb1_data\n");
        goto function_exit;
    }
    memset(arb1_data, 0, new_ent1_length);

    toc_ent2 = toc_find_id(toc_list, arbinfo->toc_id2);
    if (toc_ent2 == (struct toc_entry *)~0) {
        dprintf(INFO, "%s not found\n", arbinfo->toc_id2);
        goto function_exit;
    }
    if (toc_ent2->size < arbinfo->table2_size) {
        new_ent2_length = arbinfo->table2_size;
    } else {
        new_ent2_length = toc_ent2->size;
    }
    arb2_data = (uint8_t *) malloc(new_ent2_length);
    if (arb2_data == NULL) {
        dprintf(INFO, "Could not allocate data for arb2_data\n");
        goto function_exit;
    }
    memset(arb2_data, 0, new_ent2_length);

    if ((arbinfo->table1_size != 0) && (arbinfo->table1 != NULL)) {
        dprintf(INFO, "Saving %s\n", arbinfo->toc_id1);
        memcpy(arb1_data, arbinfo->table1, arbinfo->table1_size);

        if (toc_save_id(arbinfo->toc_id1, (uint32_t)arb1_data,
            new_ent1_length)) {
            dprintf(INFO, "Could not save %s\n", arbinfo->toc_id1);
        }
        dprintf(INFO, "%s is saved\n", arbinfo->toc_id1);
    } else {
        dprintf(INFO, "arbinfo->table1 no saved\n");
    }

    if ((arbinfo->table2_size != 0) && (arbinfo->table2 != NULL)) {
        dprintf(INFO, "Saving %s\n", arbinfo->toc_id2);
        memcpy(arb2_data, arbinfo->table2, arbinfo->table2_size);
        if (toc_save_id(arbinfo->toc_id2, (uint32_t)arb2_data,
            new_ent2_length)) {
            dprintf(INFO, "Could not save %s\n", arbinfo->toc_id2);
        }
        dprintf(INFO, "%s is saved\n", arbinfo->toc_id2);
    } else {
        dprintf(INFO, "arbinfo->table2 no saved\n");
    }

function_exit:
    if (arb1_data)
        free(arb1_data);
    if (arb2_data)
        free(arb2_data);
    return;
}


void handle_arb_tables(void)
{
    bass_return_code result;
    struct bass_app_arb_table_info  *arbinfo = NULL;

    dprintf(INFO, "Checking if ARB tables need to be updated\n");
    arbinfo = (struct bass_app_arb_table_info  *)malloc(
        sizeof(struct bass_app_arb_table_info));
    if (arbinfo == NULL) {
        dprintf(INFO, "Could not allocate data for arbinfo\n");
        goto function_exit;
    }
    arbinfo->toc_id1_size = 0x0;
    arbinfo->toc_id2_size = 0x0;
    arbinfo->table1_size = 0x0;
    arbinfo->table2_size = 0x0;
    arbinfo->toc_id1 = NULL;
    arbinfo->toc_id2 = NULL;
    arbinfo->table1 = NULL;
    arbinfo->table2 = NULL;

    result = read_arb_tables_from_secword(arbinfo);
    dprintf(INFO, "Result of read_arb_tables_from_secword =%d\n", result);

    if (result == BASS_RC_SUCCESS) {
        save_arb_tables_to_mmc(arbinfo);
    } else {
        dprintf(INFO, "No need to update ARB tables\n");
    }
    dprintf(INFO, "Result of handle_arb_tabls =%d\n", result);

function_exit:
    if (arbinfo->toc_id1)
        free(arbinfo->toc_id1);
    if (arbinfo->toc_id2)
        free(arbinfo->toc_id2);
    if (arbinfo->table1)
        free(arbinfo->table1);
    if (arbinfo->table2)
        free(arbinfo->table2);
    if (arbinfo)
        free(arbinfo);
    return;
}
#endif


static int does_toc_exist(char *toc_name)
{
    struct toc_entry *toc_list;
    struct toc_entry *toc_ent;

    toc_list = toc_addr_get();
    toc_ent = toc_find_id(toc_list, toc_name);
    if (toc_ent == (struct toc_entry *)~0) {
        dprintf(INFO, "%s image not found\n", toc_name);
        return 0;
    }
    dprintf(INFO, "%s size %08X\n", toc_name, toc_ent->size);
    if (0x00 == toc_ent->size) {
        dprintf(INFO, "%s image found but is empty\n", toc_name);
        return 0;
    }

    return 1;
}


static t_lk_flash_status check_flash(char *toc_name)
{
    if (0 == strcmp (toc_name, BOOT_NAME)) {
        if (does_toc_exist(BOOT_NAME) && does_toc_exist(SYST_NAME)) {
            if (does_toc_exist(RECO_NAME)) {
                dprintf(INFO, "All images exists in flash\n");
                return LK_FLASH_STATUS_DATA_OK;
            } else {
                dprintf(INFO, "No %s in flash\n", RECO_NAME);
                return LK_FLASH_STATUS_BOOT_SYST_OK_NO_RECO;
            }
        } else {
            dprintf(INFO, "%s or %s is missing\n", BOOT_NAME, SYST_NAME);
            if (does_toc_exist(RECO_NAME)) {
                dprintf(INFO, "%s exists in flash\n", RECO_NAME);
                return LK_FLASH_STATUS_NO_BOOT_OR_SYST;
            } else {
                dprintf(INFO, "%s, %s and %s is missing\n",
                  BOOT_NAME, SYST_NAME, RECO_NAME);
                if (does_toc_exist(ROOTFS_NAME)) {
                    dprintf(INFO, "%s exists in flash, LBP mode detected\n", ROOTFS_NAME);
                    return LK_FLASH_STATUS_DATA_OK;
                } else
                    return LK_FLASH_STATUS_NO_RECO;
            }
        }
    }

    if (does_toc_exist(RECO_NAME)) {
        dprintf(INFO, "%s exists in flash\n", RECO_NAME);
        return LK_FLASH_STATUS_DATA_OK;
    } else {
        dprintf(INFO, "No %s exists in flash\n", RECO_NAME);
        return LK_FLASH_STATUS_NO_RECO;
    }
}


#if defined(ENABLE_FEATURE_BUILD_HBTS)
void hbts_init(const struct app_descriptor *app)
{
    printf("HBTS Init\n");

    printf("LK version: %s\n", CurrentDate);

/* Vibrator not yet available for AB8540 */
#if CONFIG_ENABLE_VIBRATOR
    run_vibrator(300, VIBRATOR_INTENSE);
#endif /* CONFIG_ENABLE_VIBRATOR */
}
#endif


APP_START(aboot)
#if !defined(ENABLE_FEATURE_BUILD_HBTS)
    .init = steboot_init,
#else
    .init = hbts_init,
#endif
APP_END

#if WITH_LIB_CONSOLE

#include <lib/console.h>
#include <stdio.h>

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
static int cmd_start_linux (int argc, const cmd_args *argv);
#if ENABLE_FASTBOOT
static int cmd_start_fastboot (int argc, const cmd_args *argv);
#endif
#endif
static int cmd_version (int argc, const cmd_args *argv);

STATIC_COMMAND_START
#if !defined(ENABLE_FEATURE_BUILD_HBTS)
        { "bootu", "boot linux", &cmd_start_linux },
#if ENABLE_FASTBOOT
        { "fastboot", "fastboot cmd processor", &cmd_start_fastboot },
#endif
#endif
        { "version", "lk version", &cmd_version },
STATIC_COMMAND_END(boot);

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
static int cmd_start_linux (int argc, const cmd_args *argv)
{
    if (!is_env_loaded()) {
        dprintf(CRITICAL, "Env not loaded! Could not continue!\n");
        return -1;
    }

    boot_from_flash(BOOT_NAME);
    /* Normally no return */
    return 0;
}

#if ENABLE_FASTBOOT
static int cmd_start_fastboot (int argc, const cmd_args *argv)
{
    handle_fastboot();
    return 0;
}
#endif
#endif /* !ENABLE_FEATURE_BUILD_HBTS */

static int cmd_version (int argc, const cmd_args *argv)
{
    printf("LK version: %s\n", CurrentDate);
    return 0;
}


#endif /* WITH_LIB_CONSOLE */

