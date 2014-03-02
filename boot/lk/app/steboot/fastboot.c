/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <string.h>
#include <stdlib.h>
#include <kernel/thread.h>
#include <kernel/event.h>
#include <dev/udc.h>
#include "usb_wrapper.h"
#include "bootimg.h"
#include "tiny_env.h"
#include "partition_parser.h"
#include "bass_app.h"
#include "platform.h"
#include "reboot.h"
#include "boot.h"
#include "bass_app.h"
#include "target.h"


#define MAX_RSP_SIZE			64
#define MAX_CMD_SIZE			64
#define FB_VERSION				"1.0"
#define BOOTLDR_VERSION			"1.0"
#define EVER					;;
#define CMD_BUF_LEN				1024
#define CMD_BUF_ALIGN			1024
#define AP_DIE_ID_LEN			12
#define MODEL_ID_NONE			" "
#define MODEL_ID_A				"A"
#define MODEL_ID_B				"B"
#define MODEL_ID_C				"C"
#define MODEL_ID_E				"E"
#define MODEL_ID_F				"F"
#define MODEL_ID_H				"H"
#define MEGABYTE				(1u << 20u)
#define FASTBOOT_POOL_ALIGN		MEGABYTE
#define REBOOT_CMD_SUFFIX		"-bootloader"

typedef enum {
	FB_CMD_DOWNLOAD = 0,
	FB_CMD_GETVAR,
	FB_CMD_BOOT,
	FB_CMD_ERASE,
	FB_CMD_FLASH,
	FB_CMD_REBOOT,
	FB_CMD_CONTINUE,
	FB_CMD_LAST
} e_cmd_idx;

typedef struct {
	e_cmd_idx	idx;
	void		(*handler)(const char *arg);
	const char	*prefix;
} x_fb_cmd_type;

typedef enum {
	FB_VAR_VERSION = 0,
	FB_VAR_AP_DIE_ID,
	FB_VAR_VERSION_BASEBAND,
	FB_VAR_MAX_DNLD_SIZE,
	FB_VAR_VERSION_BOOTLDR,
	FB_VAR_SERIALNO,
	FB_VAR_PRODUCT,
	FB_VAR_MODEL_ID,
	FB_VAR_SECURE,
	FB_VAR_LAST
} e_var_idx;

typedef struct {
	e_var_idx	idx;
	const char 	*prefix;
} x_fb_var_type;

static void cmd_download(const char *arg);
static void cmd_getvar(const char *arg);
static void cmd_boot(const char *arg);
static void cmd_erase(const char *arg);
static void cmd_flash(const char *arg);
static void cmd_reboot(const char *arg);
static void cmd_continue(const char *arg);

static x_fb_cmd_type fb_cmd_list[] = {
 { FB_CMD_DOWNLOAD, cmd_download, "download:" },
 { FB_CMD_GETVAR,   cmd_getvar,   "getvar:"   },
 { FB_CMD_BOOT,     cmd_boot,     "boot"      },
 { FB_CMD_ERASE,    cmd_erase,    "erase:"    },
 { FB_CMD_FLASH,    cmd_flash,    "flash:"    },
 { FB_CMD_REBOOT,   cmd_reboot,   "reboot"    },
 { FB_CMD_CONTINUE, cmd_continue, "continue"  }
};

static x_fb_var_type fb_var_list[] = {
	{ FB_VAR_VERSION,          "version"            },
	{ FB_VAR_AP_DIE_ID,        "apdieid"            },
	{ FB_VAR_VERSION_BASEBAND, "version-baseband"   },
	{ FB_VAR_MAX_DNLD_SIZE,    "max-download-size"  },
	{ FB_VAR_VERSION_BOOTLDR,  "version-bootloader" },
	{ FB_VAR_SERIALNO,         "serialno"           },
	{ FB_VAR_PRODUCT,          "product"            },
	{ FB_VAR_MODEL_ID,         "model-id"           },
	{ FB_VAR_SECURE,           "secure"             }
};

static const char *product_id[] = {
	"Unknown" ,
	"8400"    ,
	"8500B"   ,
	"9500"    ,
	"5500"    ,
	"7400"    ,
	"8500C"   ,
	"8500A"   ,
	"8500E"   ,
	"8520F"   ,
	"8520H"   ,
	"9540"    ,
	"9500C"   ,
	"8500F"   ,
	"8540APE" ,
	"8540XMIP",
	"8520E",
	"8520J"
};

static const char *secure[] = {
	"R&D",
	"Product"
};

static event_t usb_online;
static char *cmd_buf;
static const char *product_id_str;
static const char *secure_str;
static const char *model_id_str;

static void *download_base;
static unsigned download_max;
static unsigned download_size;
static bool usb_ready_flag;

static int load_from_ram(boot_src_info_t *boot_info);

/* todo: give lk strtoul and nuke this */
static unsigned hex2unsigned(const char *x)
{
    unsigned n = 0;

    while(*x) {
        switch(*x) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = (n << 4) | (*x - '0');
            break;
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
            n = (n << 4) | (*x - 'a' + 10);
            break;
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
            n = (n << 4) | (*x - 'A' + 10);
            break;
        default:
            return n;
        }
        x++;
    }

    return n;
}

static const char * mem2str(void *mem_src, unsigned src_len)
{
	const char hex2char[] = {
		'0','1','2','3','4','5','6','7',
		'8','9','a','b','c','d','e','f'};
	char *src, *dst;

	if (!src_len || ((src_len<<1u) >=CMD_BUF_LEN))
		return NULL;

	src = (char *)mem_src;
	dst = cmd_buf;

	do {
		*dst++ = hex2char[*src & 0x0fu];
		*dst++ = hex2char[(*src++ >> 4u) & 0x0fu];
	} while(--src_len);

	*dst = 0;
	return cmd_buf;
}

static void fastboot_ack(const char *code, const char *reason)
{
	char response[MAX_RSP_SIZE];

	if (reason == 0)
		reason = "";

	snprintf(response, MAX_RSP_SIZE, "%s%s", code, reason);

	dprintf(INFO,"fastboot: ack: %s\n", response);
	usb_write(response, strlen(response));
}

static void fastboot_info(const char *reason)
{
	char response[MAX_RSP_SIZE];

	if (reason == 0)
		return;

	snprintf(response, MAX_RSP_SIZE, "INFO%s", reason);

	dprintf(INFO,"fastboot: info: %s\n", response);
	usb_write(response, strlen(response));
}

static void fastboot_fail(const char *reason)
{
	fastboot_ack("FAIL", reason);
}

static void fastboot_okay(const char *info)
{
	fastboot_ack("OKAY", info);
}

static void cmd_download(const char *arg)
{
	char response[MAX_RSP_SIZE];
	unsigned len = hex2unsigned(arg);
	int r;

	dprintf(INFO,"fastboot: cmd_download(), len=%d, maxsize=%d\n", len, download_max);

	download_size = 0;
	if (len > download_max) {
		fastboot_fail("data too large");
		return;
	}

	sprintf(response,"DATA%08x", len);
	if (usb_write(response, strlen(response)) < 0) {
		return;
	}

	/* clear whole buffer */
	memset(download_base, 0, download_max);

	r = usb_read(download_base, len);
	if ((r < 0) || ((unsigned) r != len))
		return;

	download_size = len;
	fastboot_okay("");
}

static void getvar_apdieid(void)
{
	uint8_t id_buf[AP_DIE_ID_LEN];
	bass_return_code b_rc;

	b_rc = bass_get_die_id(id_buf,AP_DIE_ID_LEN);
	if(b_rc != BASS_RC_SUCCESS) {
		fastboot_fail("internal error");
		dprintf(CRITICAL,"fastboot: bass_get_die_id(), b_rc=%d\n",b_rc);
		return ;
	}
	fastboot_okay(mem2str(id_buf,AP_DIE_ID_LEN));
}

static int product_cfg_get(void)
{
	struct tee_product_config prod_cfg;
	bass_return_code b_rc;

	product_id_str = product_id[PRODUCT_ID_UNKNOWN];
	secure_str     = secure[0];
	model_id_str   = MODEL_ID_NONE;

	/* Read the product configuration */
	b_rc = get_product_config(&prod_cfg);
	if (BASS_RC_SUCCESS != b_rc) {
		dprintf(CRITICAL, "get_product_config failed, ret=%d\n",(int)b_rc);
		return -1;
	}

	switch (prod_cfg.product_id) {
		case PRODUCT_ID_8500B:
			model_id_str = MODEL_ID_B;
			break;
		case PRODUCT_ID_8500C:
		case PRODUCT_ID_9500C:
			model_id_str = MODEL_ID_C;
			break;
		case PRODUCT_ID_8500A:
		case PRODUCT_ID_9540:
		case PRODUCT_ID_8540APE:
		case PRODUCT_ID_8540XMIP:
			model_id_str = MODEL_ID_A;
			break;
		case PRODUCT_ID_8500E:
			model_id_str = MODEL_ID_E;
			break;
		case PRODUCT_ID_8520F:
			model_id_str = MODEL_ID_F;
			break;
		case PRODUCT_ID_8520H:
			model_id_str = MODEL_ID_H;
			break;
		default:
			dprintf(INFO, "product_cfg_get: wrong product id=%d\n",prod_cfg.product_id);
			prod_cfg.product_id = PRODUCT_ID_UNKNOWN;
			break;
	}

	product_id_str = product_id[prod_cfg.product_id];
	secure_str = secure[prod_cfg.fuse_flags & TEE_FUSE_FLAGS_IS_PRODCHIP ? 1 : 0];

	return 0;
}

static void fb_partitions_print(void)
{
	fb_partition_entry_t *pentry;
	fastboot_info("Partition not found. Supported partitions:");
	pentry = partition_entry_find_first();
	while(pentry) {
		fastboot_info(pentry->name);
		pentry = partition_entry_find_next();
	}
}

static void fb_vars_print(void)
{
	int i;
	fastboot_info("Variable not found. Supported variables:");
	for(i = FB_VAR_VERSION; i < FB_VAR_LAST; i++)
		fastboot_info(fb_var_list[i].prefix);
}

static void cmd_getvar(const char *arg)
{
	e_var_idx var_idx;
	dprintf(INFO,"fastboot: cmd_getvar(), var=%s\n",arg);

	for(var_idx = FB_VAR_VERSION; var_idx < FB_VAR_LAST; var_idx++) {
		if(!strcmp(fb_var_list[var_idx].prefix, arg))
		switch(fb_var_list[var_idx].idx) {
			case FB_VAR_VERSION:
				fastboot_okay(FB_VERSION);
				return;
			case FB_VAR_AP_DIE_ID:
			case FB_VAR_VERSION_BASEBAND:
				getvar_apdieid();
				return;
			case FB_VAR_MAX_DNLD_SIZE:
				sprintf(cmd_buf,"%u",download_max);
				fastboot_okay(cmd_buf);
				return;
			case FB_VAR_VERSION_BOOTLDR:
				fastboot_okay(BOOTLDR_VERSION);
				return;
			case FB_VAR_SERIALNO:
				fastboot_okay(get_asic_id_str ());
				return;
			case FB_VAR_PRODUCT:
				fastboot_okay(product_id_str);
				return;
			case FB_VAR_MODEL_ID:
				fastboot_okay(model_id_str);
				return;
			case FB_VAR_SECURE:
				fastboot_okay(secure_str);
				return;
			default:
			break;
		}
	}
	fb_vars_print();
	fastboot_fail("unknown variable");
}

static int is_valid_boot_image(struct boot_img_hdr* img_hdr)
{
	bass_return_code			sec_result;
	bass_signed_header_info_t	sec_hdr_info;
	int rc;
	bass_payload_type_t pltype = BASS_PL_TYPE_APE_NW_CODE;

	dprintf (INFO, "fastboot: %s\n",__func__);

	/* Always verify header */
	sec_result = verify_signedheader(
		(uint8_t*)download_base, BOOT_HDR_SIZE,
		pltype, &sec_hdr_info);

	if (BASS_RC_SUCCESS != sec_result) {
		dprintf(CRITICAL, "fastboot: verify_signedheader failed, ret=%d\n",
			(int)sec_result);
		return 0;
	}
	dprintf (INFO, "fastboot: size_of_signed_header 0x%08x\n",
		sec_hdr_info.size_of_signed_header);


	rc = buf_to_image_hdr(img_hdr, (uint32_t*)(download_base +
		(sec_hdr_info.size_of_signed_header/sizeof(uint32_t))));
	if (rc) {
		dprintf(CRITICAL,"fastboot: buf_to_image_hdr failed, ret=%d\n",rc);
		return 0;
	}
	print_image_hdr(img_hdr);

	sec_result = bass_check_payload_hash(&sec_hdr_info.ht, &pltype,
		(void*) (download_base + sec_hdr_info.size_of_signed_header),
		sec_hdr_info.pl_size, (void*) sec_hdr_info.ehash.value, SHA256_HASH_SIZE);
	if (BASS_RC_SUCCESS != sec_result) {
		dprintf(CRITICAL,"fastboot: bass_check_payload_hash failed, ret=%d\n",
			(int)sec_result);
		return 0;
	}
	dprintf (INFO, "fastboot: %s: OK\n",__func__);

	return 1;
}

static void cmd_boot(const char *arg)
{
	struct boot_img_hdr			img_hdr;
	boot_src_info_t 			boot_info;

	dprintf(INFO,"fastboot: cmd_boot()\n");

	/* Check if env variables is loaded */
	if(!is_env_loaded()) {
		fastboot_fail("env variables not loaded");
		return;
	}

	/* Always verify image */
	if(!is_valid_boot_image(&img_hdr)) {
		fastboot_fail("image verification failed");
		return;
	}

#if BOOTLOADER_START_MODEM
	/* Start modem */
	if (0 != start_modem()) {
		fastboot_fail ("Failed to start modem");
		return;
	}
#endif

	/* put response to host */
	fastboot_okay("");

	/* Start linux */
	boot_info.hdr = &img_hdr;
	boot_info.buf = download_base;
	dprintf(INFO,"fastboot: starting linux...\n");

	/* Copy images to correct adresses */
	(void)load_from_ram(&boot_info);

	/* no return from here */
	start_linux(&boot_info);
}

static void cmd_continue(const char *arg)
{
	fb_partition_entry_t *pentry;

	dprintf(INFO,"fastboot: cmd_continue()\n");
	/* Check if env variables is loaded */
	if(!is_env_loaded()) {
		fastboot_fail("env variables not loaded");
		return;
	}

	/* find partition in toc */
	pentry = partition_entry_find(SYST_NAME);
	if (!pentry) {
		fb_partitions_print();
		fastboot_fail("unknown partition name");
		return;
	}

	if (0 == pentry->toc_size) {
		dprintf (INFO, "%s: system partition is empty\n", __func__);
		fastboot_fail ("system partition empty, aborting");
		return;
	}

	/* put response to host */
	fastboot_okay("");
	boot_from_flash(BOOT_NAME);
}

static void cmd_erase(const char *arg)
{
	fb_partition_entry_t *pentry;
	int rc;

	dprintf(INFO,"fastboot: cmd_erase(), partition=%s\n", arg);

	/* find partition in toc */
	pentry = partition_entry_find(arg);
	if(!pentry) {
		fb_partitions_print();
		fastboot_fail("unknown partition name");
		return;
	}

	/* erase content of selected partition */
	rc = partition_entry_erase(pentry);
	if (rc) {
		fastboot_fail("failed to erase partition");
		return;
	}

	fastboot_okay("");
}

static void cmd_flash(const char *arg)
{
	fb_partition_entry_t *pentry;
	struct boot_img_hdr img_hdr;
	int res1, res2;

	dprintf(INFO,"fastboot: cmd_flash(), partition=%s, data size=%u\n",
			arg, download_size);

	/* check if we have data to flash */
	if (!download_size) {
		fastboot_fail("no data to flash");
		return;
	}

	/* find partition in toc */
	pentry = partition_entry_find(arg);
	if (!pentry) {
		fb_partitions_print();
		fastboot_fail("unknown partition name");
		return;
	}

	/* check if image is allowed to flash into desired partition */
	res1 = pentry->flags & FB_PART_BOOT ? 1 : 0;
	res2 = is_valid_boot_image(&img_hdr);
	dprintf(INFO,"fastboot: cmd_flash(), boot_partition=%d, boot_image=%d\n", res1, res2);

	if (res1 ^ res2) {
		fastboot_fail("image vs partition matching failed");
		return;
	}

	/* write data to partition */
	if (partition_entry_write(pentry,download_base,download_size)) {
		fastboot_fail("failed to write partition");
		return;
	}

	fastboot_okay("");
}

static void cmd_reboot(const char *arg)
{
	uint16_t reboot_reason;
	dprintf(INFO,"fastboot: cmd_reboot() %s\n", arg);

	/* this is "reboot" cmd */
	if(*arg == '\0')
		reboot_reason = SW_RESET_NORMAL;
	else
		/* this is "reboot-bootloader" cmd */
		if(!strcmp(arg,REBOOT_CMD_SUFFIX))
			reboot_reason = SW_RESET_FASTBOOT;
		else {
			/* unknown cmd */
			fastboot_fail("unknown command");
			return;
		}

	fastboot_okay("");
	platform_reboot(reboot_reason);
}

static void fastboot_command_loop(void)
{
	e_cmd_idx cmd_idx;
	int rc, len;

	dprintf(INFO,"fastboot_command_loop()\n");
	for(EVER) {
		rc = usb_read(cmd_buf, MAX_CMD_SIZE);
		if(rc < 0)
			break;
		if(rc == 0)
			continue;
		cmd_buf[rc] = 0;
		dprintf(INFO,"fastboot: cmd buf: %s\n", cmd_buf);

		for(cmd_idx = FB_CMD_DOWNLOAD; cmd_idx < FB_CMD_LAST; cmd_idx++) {
			len = strlen(fb_cmd_list[cmd_idx].prefix);
			rc = strncmp(cmd_buf,fb_cmd_list[cmd_idx].prefix,len);
			if(rc)
				continue;
			/* call cmd handler */
			fb_cmd_list[cmd_idx].handler(cmd_buf+len);
			break;
		}
		if(rc)
			fastboot_fail("unknown command");
	}
	dprintf(INFO,"fastboot: USB link down\n");
}

static void fastboot_run(void)
{
	for (EVER) {
		event_wait(&usb_online);
		fastboot_command_loop();
	}
}

static void fastboot_notify(struct udc_gadget *gadget, unsigned event)
{
	if (event == UDC_EVENT_ONLINE) {
		dprintf(ALWAYS, "USB is connected and ready for transfer\n");
		usb_ready_flag = true;
		event_signal(&usb_online, 0);
	}
	if (event == UDC_EVENT_OFFLINE) {
		dprintf(ALWAYS, "USB is disconnected\n");
		usb_ready_flag = false;
	}
}

static struct udc_gadget fastboot_gadget = {
	.notify		= fastboot_notify,
	.ifc_string	= "Fastboot",
};

static struct udc_device usbfastboot_device = {
	.vendor_id	= 0x18d1,
	.product_id	= 0xD00D,
	.version_id	= 0x0100,
	.manufacturer	= "ST-Ericsson",
	.product	= "Fastboot device",
	.serialno	= NULL, // Set later by usb_wrapper to asic_id_str
};

static bool fastboot_malloc()
{
	unsigned psize, malloc_size;
	psize = 0xffffffffu;

	download_base = NULL;
	download_max = 0;

	do {
		/* get next largest partition size */
		psize = partition_find_biggest(psize);
		/* round malloc size to the nearest largest MB */
		malloc_size = (psize & (MEGABYTE-1)) ?
			(psize + MEGABYTE) & ~(MEGABYTE-1) : psize;
		if (malloc_size) {
			dprintf(INFO, "fastboot_malloc(): trying allocate %d MB\n",malloc_size >> 20u);
			download_base = memalign(FASTBOOT_POOL_ALIGN, malloc_size);
			if (download_base) {
				dprintf(INFO, "fastboot_malloc(): done!\n");
				download_max = malloc_size;
				break;
			} else {
				dprintf(INFO, "fastboot_malloc(): failed!\n");
			}
		}
	} while(psize);

	return (download_max != 0);
}

int fastboot_init(void)
{
	int rc;

	dprintf(INFO, "fastboot_init()\n");
	if (!fastboot_malloc()) {
		dprintf(CRITICAL, "fastboot_init(): download_base alloc failed\n");
		return -1;
	}

	cmd_buf = memalign(CMD_BUF_ALIGN, CMD_BUF_LEN);
	if (!cmd_buf) {
		dprintf(CRITICAL, "fastboot_init(): cmd_buf alloc failed\n");
		return -1;
	}

	/* initialize USB device */
	event_init(&usb_online, 0, EVENT_FLAG_AUTOUNSIGNAL);
	rc = usbdev_init(&usbfastboot_device, &fastboot_gadget);
	if (rc) {
		dprintf(CRITICAL, "fastboot_init(): usbdev_init() failed\n");
		return rc;
	}

	/* init product ids */
	rc = product_cfg_get();
	if (rc) {
		dprintf(CRITICAL, "fastboot_init(): product_cfg_get() failed\n");
		return rc;
	}

	/*
	 * Call fastboot command processor loop
	 * directly and stay there for a while.
	 */
	fastboot_run();

	free(download_base);
	free(cmd_buf);
	return 0;
}


static int load_from_ram(boot_src_info_t *boot_info)
{
    uint32_t                offset_kernel;
    uint32_t                offset_ramdsk;
    uint32_t                offset_second;

    dprintf (INFO, "boot_from_ram()\n");

    /* calculate src offsets */
    offset_kernel = boot_info->hdr->page_size;     /* KERNEL comes right after header, 1 page */
    offset_ramdsk = offset_kernel + (((boot_info->hdr->kernel_size + (boot_info->hdr->page_size -1))
                    / boot_info->hdr->page_size) * boot_info->hdr->page_size);
    offset_second = offset_ramdsk + (((boot_info->hdr->ramdisk_size + (boot_info->hdr->page_size -1))
                     / boot_info->hdr->page_size) * boot_info->hdr->page_size);

    offset_kernel += (uint32_t)boot_info->buf;
    offset_ramdsk += (uint32_t)boot_info->buf;
    offset_second += (uint32_t)boot_info->buf;

    /* Load kernel part of BOOT */
    memcpy((void *)boot_info->hdr->kernel_addr, (void *)offset_kernel, boot_info->hdr->kernel_size);
    /* Load ramdisk part of BOOT */
    dprintf (INFO, "load ramdisk from boot + 0x%08x to 0x%08x\n", offset_ramdsk, boot_info->hdr->ramdisk_addr);
    memcpy((void *)boot_info->hdr->ramdisk_addr, (void *)offset_ramdsk, boot_info->hdr->ramdisk_size);
    /* Load second part of BOOT */
    if (boot_info->hdr->second_size > 0) {
        dprintf (INFO, "load second from boot + 0x%08x to 0x%08x\n", offset_second, boot_info->hdr->second_addr);
        memcpy((void *)boot_info->hdr->second_addr, (void *)offset_second, boot_info->hdr->second_size);
    }
    return 0;
}

