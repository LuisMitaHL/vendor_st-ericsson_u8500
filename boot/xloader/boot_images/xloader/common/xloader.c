/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "xloader.h"
#include "xl_drivers.h"
#include "xp70_memmap.h"
#include "gitversion.h"
#include "build_date.h"
#include "xl_drivers.h"
#include "xl_l1_cache_mmu_mgmt.h"
#include "nomadik_registers.h"
#include "ab8500.h"
#include <bass_app.h>

#ifdef TEE_TEST
#include "tee_test.h"
#endif

#include "isswapi.h"
#include "crtmem.h"
#include "init_gcc.h"
#include "float.h"

#include "xloader_recordids.h"

#if (__STN_9540 > 0)
extern int linker_xloader_stack_top;
#endif

/*
 * bin is a pointer to SoC settings binary in memory
 * bin_len is the length of SoC settings binary in memory
 * boot_status is the status of the boot.
 */
typedef uint32_t (*soc_setting_exec)(const uint8_t *bin, size_t bin_len,
        uint32_t boot_status);

#define SOC_SETTINGS_VERSION    2

#define SOC_SETTINGS_EXEC      2
#if (__STN_9540 > 0)
#undef SOC_SETTINGS_EXEC
#define SOC_SETTINGS_EXEC      3
#endif

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

#ifdef MACH_L8540
#define        ASIC_ID_STRING "8540 A0"
#elif (__STN_9540 == 10)
#define        ASIC_ID_STRING "9540 A0"
#elif (__STN_8500 == 20)
#define        ASIC_ID_STRING "8500 B0"
#else
#define ASIC_ID_STRING  "unknown ux500"
#endif

#ifdef WAIT_DEBUG
#define DEBUG_WAIT_LOOP \
    do {                         \
        XLPUTS("Wait debug...\n");       \
        {                    \
            volatile int spin = 0x1;     \
            while (spin == 0x1) {        \
                ;            \
            }                \
        }                    \
    } while (0)
#else
#define DEBUG_WAIT_LOOP
#endif

#define XP70_DATA_MBFFC     0xFFC   /* prcm_pwr_state */
#define XP70_DATA_MBFE8     0xFE8   /* prcm_header */
#define XP70_DATA_MBFDC     0xFDC   /* prcm_reqmb0 */

#define MAX_TRANSFER_MMC    0x01FFFE00

/* *** MUST be aligned with value defined in mem init */
#define PWR_MNGT_STARTED    0x1

#define PAYLOAD_OFFSET  0x4
#define BACKUP_ACT_PERIPH_POS 0x14

/* Round up the even multiple of size, size has to be a multiple of 2 */
#define ROUNDUP(v, size) (((v) + (size - 1)) & ~(size - 1))

#define SDMMC_CLK_MGT_REG (PRCMU_REG_START_ADDR + 0x24)
#define BACKUPRAM_ROM_DEBUG_ADDR (BACKUP_RAM_START_ADDR + 0x3FC)
#define MMC_BLOCK_ID        0x20
#define SDMMC_CLK_50MHZ     0x130
#define SDMMC_CLK_100MHZ    0x128

/*
 * Note that buffer length is number of 32bit words!
 */
#define BUFFER_LENGTH       (1024 / sizeof(uint32_t))

/* Both types of records share the size-offset */
#define RECORD_SIZE_OFFSET  1
/*
 * Permanent engineering mode.
 * For readability we introduce defines when handling
 * the buffer-offsets for the size and data offset.
 * The record-data is padded to make an even 4-byte data.
 */
#define ENG_DATA_SIZE_OFFSET    2
#define ENG_DATA_OFFSET     3
/*
 * Secure profile data.
 * Fewer set data is specified for these records.
 */
#define SEC_DATA_OFFSET     2

#define ENG_MAX_RECORD_LENGTH 256
#define SEC_MAX_RECORD_LENGTH 256

/* Only look through the first 50kb of the boot-partition */
#define MAX_BOOT_OFFSET     (50 * 1024)

/*------------------------------------------------------------------------
 * Private Functions
 *----------------------------------------------------------------------*/
static t_uint32 xloader(struct data_collection *data,
            struct toc_data *toc_data);

static t_uint32 core1_loop(void);

static void init_data(t_pub_rom_loader_info *loader_info,
              struct data_collection *data,
              struct toc_data *toc_data);

static t_uint32 find_executable(const t_uint8 *bin, t_uint32 bin_len,
            soc_setting_exec *exec);
static t_uint32 execute_soc_settings(const t_uint8 *bin, t_uint32 bin_len,
            t_uint32 boot_status);

/*------------------------------------------------------------------------
 * Public variables declaration
 *----------------------------------------------------------------------*/

/* No debug printouts if Xloader loaded over UART, disturbs flash loader */
t_bool boot_print_enable = FALSE;

int main(void)
{

    static struct data_collection data;
    static struct toc_data toc_data;

    t_pub_rom_loader_info *loader_info;
    t_uint32 status;

    /* Get pointer on Rom shared data structure, transmitted via ISSW */
    loader_info = (t_pub_rom_loader_info *) r0_save;

    XLPRINTF_INIT(((t_boot_trace_info *)
               loader_info->pdebugInfo)->Bit.DbgBootIf);

    XLPUTS(gitinfo);
    XLPRINTF("Built: %s\n", build_date);

    init_data(loader_info, &data, &toc_data);

    XLPRINTF("Configured for: %s\n", ASIC_ID_STRING);

    XLPRINTF("Running on chip ID: %x\n", *data.rom_cut_id);

    DEBUG_WAIT_LOOP;

    status = xloader(&data, &toc_data);

    XLPUTS("xloader returned!\n");

    xldbg_send_trace(loader_info, (t_address) (&status), sizeof(status));

    /* If no NORMAL Appli or error, Xloader loops here
     * The user can connect to debugger and then load her/his own appli...
     * Good Luck, job of Xloader is finished now....
     * Wake up Core1 and make it loop in Non secure
     */
    ROM_SHARED_FUNC_WAKEUP_CPU1((t_address)&core1_loop, data.jmp_table);
#ifdef ENABLE_SYSTEM_RESET
    if ((data.indication.scenario & MASK_DEBUG_INFO) ==
        FLASHING_INDICATION) {
        XLPUTS("reseting ...\n");
        issw_api_reset();
        while (1)
            ;
    }
#else
    XLPUTS("Wait forever...\n");

    while (1)
        ;
#endif
}               /* end of main() */

static int xl_memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;
    size_t m;

    for (m = 0; m < n; m++) {
        if (p1[m] != p2[m])
            return (int)p1[m] - (int)p2[m];
    }
    return 0;
}

static void init_data(t_pub_rom_loader_info *loader_info,
              struct data_collection *data, struct toc_data *toc_data)
{
    data->loader_info = loader_info;
    data->boot_trace = (t_boot_trace_info *) loader_info->pdebugInfo;
    data->boot_if = data->boot_trace->Bit.DbgBootIf;
    data->boot_info.BootConf = loader_info->bootInfo;
    data->indication.scenario = loader_info->bootIndication;
    data->rom_cut_id = (t_address *) ROM_CUT_ID_ADDRESS;
    data->jmp_table = (void (**)(void))loader_info->bootRomJmpTbl;
    data->boot_status = r0_save;
    data->mem_init_func = (t_mem_init_func)r2_save;

    toc_data->toc_esram = (t_address *) TOC_COPY_ADDRESS;
    toc_data->cert_header = (t_public_code_header *) XLOADER_COPY_ADDRESS;
    toc_data->toc_esram = (t_address *) TOC_COPY_ADDRESS;
    if (toc_find_subtoc((t_uint32) toc_data->toc_esram, &toc_data->subtoc_size)) {
    toc_data->subtoc_esram = (t_address *) SUBTOC_COPY_ADDRESS;
    } else {
        toc_data->subtoc_esram = NULL;
    }
    toc_data->cert_header = (t_public_code_header *) XLOADER_COPY_ADDRESS;

}

static void send_engi_info(t_pub_rom_loader_info *loader_info,
               t_boot_indication *indication)
{

    t_uint32 *engiInfo;

    /* Send Enginneering information on active peipheral */

    /* This function implements VI #27999 */
    /* Data was copied in backup RAM by ISSW */
    engiInfo = (t_uint32 *) ENGINEERING_ID_ADDRESS;

    /* Send engi information only on a FLASHING boot indication */
    if ((indication->scenario & MASK_DEBUG_INFO) == FLASHING_INDICATION) {
        PUTS("Send engi information on a FLASHING boot indication\n");
        xldbg_send_trace(loader_info,
                 (t_address) (&engiInfo[0]),
                 sizeof(engiInfo[0]));
        xldbg_send_trace(loader_info,
                 (t_address) (&engiInfo[1]),
                 sizeof(engiInfo[1]));
        xldbg_send_trace(loader_info,
                 (t_address) (&engiInfo[2]),
                 sizeof(engiInfo[2]));
    }
}

static void store_active_periph(struct data_collection *data)
{
    t_uint32 *backup_ram_active_periph;

    /*
     * Store the active periheral in the backup RAM for
     * external use
     */
    backup_ram_active_periph =
        (t_uint32 *) (BACKUP_RAM_START_ADDR +
              BACKUP_RAM_LENGTH - BACKUP_ACT_PERIPH_POS);

    switch (data->boot_if) {
    case IF_UART:
        *backup_ram_active_periph = UART_CODE;
        break;
    case IF_USB:
        *backup_ram_active_periph = USB_CODE;
        break;
    default:
        *backup_ram_active_periph = 0x00;
        break;
    }
}

static void disconnect_usb(struct data_collection *data)
{

    /*
     * USB_Disconnect in case of NORMAL/PRODUCTION boot
     * and if USB enabled
     */

    if ((((data->indication.scenario & MASK_DEBUG_INFO) ==
         NORMAL_INDICATION) ||
        ((data->indication.scenario & MASK_DEBUG_INFO) ==
         PRODUCTION_INDICATION))  &&
#if defined(MACH_L8540)
		(data->boot_info.Bit.UsbDisable == 0) &&
#else
		(data->boot_info.Bit.UsbEnable == 1) &&
#endif
		(data->boot_trace->Bit.DbgUsbEnumDone == 1)) {
		ROM_SHARED_USB_CUST_DISCONNECT(data->jmp_table);
		XLPUTS("USB now disconnected for this scenario\n");
	}
}

static t_uint32 verify_cut_id(struct data_collection *data,
                  struct toc_data *toc_data)
{
#ifdef XLOADER_OLD_FORMAT
    if ((t_uint32) *data->rom_cut_id !=
        toc_data->cert_header->sectionId.CutId) {
        XLPRINTF("verify_cut_id: ID:s don't match, signed for %d\n",
            toc_data->cert_header->sectionId.CutId);
#if (__STN_9540 > 0)
        if ((*data->rom_cut_id != CUT_ID))
#elif (__STN_8500 == 20)
        /* Temp EPARHAK, January 2011 sign package signs
         * with for U8500 B0, wrong Cut Id... */
        if ((*data->rom_cut_id != 0x008500B1) &&
            (*data->rom_cut_id != 0x008500B2) &&
            (*data->rom_cut_id != 0x008520B2))
#endif
            return XL_ERROR_BAD_CUT_ID;
    }
#endif
    return XL_SUCCESS;
}

static void get_reset_reason(t_pub_rom_loader_info *loader_info)
{
    t_uint32 trace = GET_RESET_REASON;
    t_boot_last_reset last_reset;
    t_uint32 last_reset_32;

    xldbg_send_trace(loader_info, (t_address) (&trace), sizeof(trace));

    trace = XL_GetLastReset((t_boot_last_reset *) (&last_reset));

    xldbg_send_trace(loader_info, (t_address) (&trace), sizeof(trace));

    /* Make sure we send 4 bytes to FlashKit */
    last_reset_32 = (t_uint32)last_reset;

    xldbg_send_trace(loader_info,
             (t_address) (&last_reset_32), sizeof(last_reset_32));
}

static t_uint32 get_prcmufw_status(struct data_collection *data)
{
#if (__STN_9540 >= 10)
    XLPRINTF("XP70_DATA_MBFFC: %d\n", TCDMnSec->ShVar1.pwr_current_state_var);
    if (TCDMnSec->ShVar1.pwr_current_state_var != ApBoot) {
        XLPUTS("Waiting for PRCMU FW to get to ApBoot...\n");
        while (TCDMnSec->ShVar1.pwr_current_state_var != ApBoot)
            ;
    }

    /* Status is updated for mem init */
    XLPUTS("PWR_MNGT_STARTED 1\n");
    data->prcmufw_status = PWR_MNGT_STARTED;
    return XL_SUCCESS;

#elif (__STN_8500 >= 20)
    volatile t_ShVar1 *mailbox_status =
        (t_ShVar1 *)(XP70D_REG_START_ADDR + XP70_DATA_MBFFC);
    volatile t_uint32 *prcmu_status =
        (t_uint32 *)(XP70D_REG_START_ADDR + XP70_DATA_MBFFC);

    XLPRINTF("XP70_DATA_MBFFC: %d\n", *prcmu_status);
    if (mailbox_status->pwr_current_state_var != ApBoot) {
        XLPUTS("Waiting for PRCMU FW to get to ApBoot...\n");
        while (mailbox_status->pwr_current_state_var != ApBoot)
            ;
    }

    /* Status is updated for mem init */
    XLPUTS("PWR_MNGT_STARTED 1\n");
    data->prcmufw_status = PWR_MNGT_STARTED;
    return XL_SUCCESS;

#else
    data->prcmufw_status = PWR_MNGT_STARTED;
    return XL_SUCCESS;
#endif
}

static void print_boot_scenario(struct data_collection *data)
{
    XLPUTS("Boot scenario:");

    switch (data->indication.scenario & MASK_DEBUG_INFO) {
    case NORMAL_INDICATION:
        XLPUTS(" NORMAL\n");
        break;
    case ADL_INDICATION:
        XLPUTS(" ADL\n");
        break;
    case FLASHING_INDICATION:
        XLPUTS(" FLASHING\n");
        break;
    case PRODUCTION_INDICATION:
        XLPUTS(" PRODUCTION\n");
        break;
    default:
        XLPUTS(" unknown\n");
        break;
    }
#if defined(MACH_L8540)	
    XLPRINTF("USB - Disabled: %d Enum done: %d\n",
		 data->boot_info.Bit.UsbDisable,
		 data->boot_trace->Bit.DbgUsbEnumDone);
#else
    XLPRINTF("USB - Enabled: %d Enum done: %d\n",
         data->boot_info.Bit.UsbEnable,
         data->boot_trace->Bit.DbgUsbEnumDone);
#endif
}

static t_uint32 xloader_prepare(struct data_collection *data,
                struct toc_data *toc_data)
{
    t_uint32 ret_sts;
    t_bool status;
    t_bool in_subtoc;

    if ((data->boot_status & BOOT_STATUS_PRCMU_FAIL) != 0) {
        XLPRINTF("PRCMU failed\n");
        return XL_ERROR_XP70_TIMEOUT_NOT_STARTED;
    }

    /* Need to get Xloader item from TOC to get Xloader size */
    status = toc_search_item_in_toc((t_uint32) toc_data->toc_esram,
                    (t_uint32) toc_data->subtoc_esram,
                    (t_uint32) toc_data->subtoc_size,
                    ASCII_XLOADER,
                    &toc_data->xloader_toc_item,
                    &in_subtoc);

    if (status != TRUE) {
        return XL_ERROR_XL_NOT_FOUND;
    }

#ifdef CACHE_OFF
    trace = DISABLE_CACHE_MMU;
    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    XL_DisableCachesMmu();
#endif

    XLPUTS("SECURITY_Init\n");

    SECURITY_Init(BOOTLOGBASEADDR);

    store_active_periph(data);

    print_boot_scenario(data);

    disconnect_usb(data);

    ret_sts = verify_cut_id(data, toc_data);

    if (ret_sts != XL_SUCCESS)
        return ret_sts;

    get_reset_reason(data->loader_info);

    ret_sts = get_prcmufw_status(data);

    return ret_sts;
}

t_boot_error load_toc_item(struct data_collection *data,
               struct toc_data *toc_data,
               t_address read_address,
               t_uint32 *write_address,
               t_boot_toc_item *toc_item,
               t_uint32 token,
               t_uint32 timeout_value)
{
    t_uint32 config_info[2];    /* 0:size of the token, 1: TOKEN */
    t_rpc_feature_flags feature_flags;
    t_uint32 read_size = toc_item->Size;
    t_boot_error ret_sts;

    if ((data->boot_if == IF_UART) || (data->boot_if == IF_USB)) {
        /* Size in bytes of the Token */
        config_info[0] = sizeof(token);
        config_info[1] = token;
    } else {
        /* MMC or oneNAND, config_info not used */
        config_info[0] = 0;
        config_info[1] = 0;
    }

    feature_flags.Reg = 0;

    if ((toc_item->Flags & BOOT_TOC_FLAGS_DMA_MASK) != 0)
        feature_flags.Bit.UseDma = 1;
    else
        feature_flags.Bit.UseDma = 0;

    /* VI #25096 Transfer of big sub images (ex NORMAL)  fails with eMMC
     * Need to split big tranfers on eMMC
     */
    if (data->boot_if == IF_MMC) {
        while (read_size > MAX_TRANSFER_MMC) {

            ret_sts =
                rom_shared_load_image(read_address,
                              MAX_TRANSFER_MMC,
                              write_address,
                              data->boot_if,
                              (t_uint32 *) config_info,
                              feature_flags,
                              timeout_value,
                              data->jmp_table);

            if (ret_sts != BOOT_OK)
                return ret_sts;

            read_size = read_size - MAX_TRANSFER_MMC;

            write_address =
                (t_uint32 *) ((t_uint8 *) write_address +
                          MAX_TRANSFER_MMC);
            read_address =
                (t_address) ((t_uint8) read_address +
                         MAX_TRANSFER_MMC);
        }
        /* Read last bytes with next rom_shared_load_image() */

    }

    return rom_shared_load_image(read_address,
                     read_size,
                     write_address,
                     data->boot_if,
                     (t_uint32 *) config_info,
                     feature_flags,
                     timeout_value, data->jmp_table);

}

void update_caches_after_load(void *start_addr, void *end_addr,
                  t_bool invalidate_icache)
{
#ifdef CACHE_ON
    XL_CleanDcacheArea(start_addr, end_addr);
    if (invalidate_icache)
        XL_InvalidateIcacheArea(start_addr, end_addr);
    XL_CleanInvalidateL2();
#endif
}

static t_boot_error verify_payload(struct data_collection *data,
                   struct toc_data *toc_data,
                   t_boot_toc_item *toc_item,
                   t_uint32 *write_address,
                   void **payload,
                   t_uint32 payload_offset,
                   t_uint32 pl_type,
                   t_bool remove_header)
{
    bass_return_code ret_code;
    bass_hash_t hash;
    bass_vsh_t hinfo;

    /* payload_offset added to compensate for verification
     * routines not being able to verify at address 0x00000000
     */

    ret_code =
        verify_signedheader((uint8_t *) write_address + payload_offset,
                    pl_type, &hinfo);

    if (ret_code != BASS_RC_SUCCESS) {
        PRINTF("verify_signedheader failed\n");
        return BOOT_ERROR_CODE_NOT_EXECUTABLE;
    }

    *payload = (void *)((t_uint8 *) write_address +
                payload_offset + hinfo.hdr_size);

    PRINTF("verify_payload hdr size: %d payload %d\n", hinfo.hdr_size,
            *payload);

    ret_code = bass_check_payload_hash(&hinfo.ht,
                    &hinfo.plt,
                    (void *)(*payload),
                    hinfo.pl_size,
                    (void *)&hinfo.ehash.Value,
                    hinfo.hash_size);

    if (ret_code != BASS_RC_SUCCESS) {
        PRINTF("BASS_APP_CHECK_PAYLOAD_HASH failed\n");
        return BOOT_ERROR_CODE_NOT_EXECUTABLE;
    }

    if (hinfo.hdr_size != 0) {
        if (remove_header) {
            /*
             * Move down the binary to compensate for the
             * signed header and possibly extra extra offset bytes
             */

            memmove((t_uint8 *) toc_item->Load,
                *payload, hinfo.pl_size);
            *payload = (void *)toc_item->Load;
        }
    } else {
        if (payload_offset) {
            memmove((t_uint8 *) toc_item->Load,
                (t_uint8 *) toc_item->Load +
                payload_offset,
                toc_item->Size);

            *payload = (void *)toc_item->Load;
        } else {
            *payload = (void *)write_address;
        }
    }

    return BOOT_OK;
}

static t_uint32 find_executable(const t_uint8 *bin, t_uint32 bin_len,
            soc_setting_exec *exec)
{
    t_uint32 version;
    t_uint32 t; /* type in header */
    t_uint32 l; /* length in header */
    const t_uint8 *p;
    const t_uint8 *ep;
    const t_uint32 hdr_size = sizeof(t) + sizeof(l);

    p = bin;
    ep = bin + bin_len;

    /*
     * Read and check version
     */
    if (!ALIGNMENT_IS_OK(bin, t_uint32))
        return BOOT_ERROR_ADDRESS_OUT_OF_RANGE;

    version = *(const t_uint32 *)p;
    p += sizeof(t_uint32);
    if (version != SOC_SETTINGS_VERSION)
        return BOOT_ERROR_CODE_NOT_EXECUTABLE;

    while (TRUE) {
        /* p is pointing to the header of the next section */

        /* Check that there's space for the header */
        if ((p + hdr_size) >= ep)
            return BOOT_ERROR_CODE_NOT_EXECUTABLE;

        t = *(const t_uint32 *)p;
        p += sizeof(t);
        l = *(const t_uint32 *)p;
        p += sizeof(l);

        /*
         * Check that there's space for the payload, second test
         * for overflow of p + l
         */
        if ((p + l) > ep || (p + l) < p)
            return BOOT_ERROR_CODE_NOT_EXECUTABLE;

        if (t == SOC_SETTINGS_EXEC) {
            *exec = (soc_setting_exec)p;
            return BOOT_OK;
        }

        /* Skip payload */
        p += l;

        /* Skip alignment padding */
        p = (t_uint8 *)ROUNDUP((t_uint32)p, 4);
    }
}

t_uint32 execute_soc_settings(const t_uint8 *bin, t_uint32 bin_len,
            t_uint32 boot_status)
{
    soc_setting_exec exec;
    t_uint32 ret;

    ret = find_executable(bin, bin_len, &exec);
    if (ret != BOOT_OK)
        return ret;

    return exec(bin, bin_len, boot_status);
}

/*
 * Scanbuffer is supposed to be iterated with new buffers until the entire
 * (or predefined size of it) has been scanned.
 * next_offset_p gives the offset from current buffer to next.
 *
 * The buffer read from the boot-partition holds a sequence of records.
 * The record format is here interpreted in the following way:
 * uint32_t ID           record-id (might be magic-word)
 * uint32_t record-size  (total record size, padded to an even 4 bytes)
 * uint32_t data-size    size in bytes for the following data
 * uint8_t  data
 * ...etc
 */
static void scanbuffer(uint32_t *buffer,
            uint32_t *next_offset_p,
            bool *found_eng_magic_p,
            bool *found_sec_magic_p,
            bool *done_looking_p,
            bass_handle_bootp_magic_t *data_p)
{
    uint32_t counter = 0;
    while (counter < BUFFER_LENGTH) {
        /* Early exit if STOP-id encountered */
        if (buffer[counter] == RECORD_ID_STOP_READING ||
                buffer[counter] == RECORD_ID_STOP_READING2) {
            *done_looking_p = TRUE;
            break;
        }

        /*
         * Handle engineering mode id
         * Make sure entire record in buffer or reload buffer
         */
        if (!(*found_eng_magic_p) && buffer[counter] ==
            RECORD_ID_ENG_MODE) {
            if (counter + ENG_MAX_RECORD_LENGTH/sizeof(uint32_t) >
                BUFFER_LENGTH)
                break;
            memcpy(data_p->eng_inbuffer_p,
                &buffer[counter + ENG_DATA_OFFSET],
                buffer[counter + ENG_DATA_SIZE_OFFSET]);
            data_p->eng_inbuffer_size = buffer[counter +
                ENG_DATA_SIZE_OFFSET];
            *found_eng_magic_p = TRUE;
        }

        /*
         * Handle secure profile id
         * Make sure entire record in buffer or reload buffer.
         * Sec-records don't have the DATA_SIZE_OFFSET uint32_t,
         * just the RECORD_SIZE_OFFSET uint32_t to look at.
         */
        if (!(*found_sec_magic_p) && buffer[counter] ==
            RECORD_ID_SEC_PROFILE) {
            if (counter + SEC_MAX_RECORD_LENGTH/sizeof(uint32_t) >
                BUFFER_LENGTH)
                break;
            memcpy(data_p->sec_inbuffer_p,
                &buffer[counter + SEC_DATA_OFFSET],
                buffer[counter + RECORD_SIZE_OFFSET] -
                    (RECORD_SIZE_OFFSET *
                        sizeof(uint32_t)));
            data_p->sec_inbuffer_size = buffer[counter +
                RECORD_SIZE_OFFSET] - (RECORD_SIZE_OFFSET *
                    sizeof(uint32_t));
            *found_sec_magic_p = TRUE;
        }

        /* Exit if found both special record-ids */
        if (*found_eng_magic_p && *found_sec_magic_p) {
            *done_looking_p = TRUE;
            break;
        }

        /* If no size can be read, exit and reload buffer */
        if (counter + RECORD_SIZE_OFFSET > BUFFER_LENGTH)
            break;

        /* Jump to next record */
        counter += (buffer[counter + RECORD_SIZE_OFFSET] /
            sizeof(t_uint32));
    }
    *next_offset_p = counter;
}

/*
 * Switches to boot-partition, looks through records for engineering
 * mode or secure profile magic words. If found, sends data of
 * those records to the secure world to allow handling of it.
 * Switches back to user-partition afterwards.
 */
static bass_return_code handle_bootp_data(struct data_collection *data,
                        struct toc_data *toc_data)
{
    uint32_t buffer[BUFFER_LENGTH];
    uint8_t eng_data[ENG_MAX_RECORD_LENGTH];
    uint8_t sec_data[SEC_MAX_RECORD_LENGTH];
    bool done_looking = FALSE;
    bool found_eng_magic = FALSE;
    bool found_sec_magic = FALSE;
    uint32_t bootp_offset = 0;

    bass_return_code bass_result = BASS_RC_SUCCESS;
    t_boot_error ret_code = BOOT_OK;

    bass_handle_bootp_magic_t found_data;
    memset(&found_data, 0, sizeof(bass_handle_bootp_magic_t));
    found_data.eng_inbuffer_p = eng_data;
    found_data.sec_inbuffer_p = sec_data;

    /*
     * The boot_part parameter in MMC_switch_to_boot_partition takes
     * an 32-bit integer, with 1 or 2 denoting boot-partitions,
     * and 0 a switch back to user partition.
     */
    ret_code = init_emmc(data);
    if (ret_code != BOOT_OK) {
        XLPRINTF("Initialising eMMC failed %d\n", ret_code);
        bass_result = BASS_RC_FAILURE;
        goto error;
    }

    ret_code = MMC_switch_to_boot_partition(data, toc_data, 1);
    if (ret_code != BOOT_OK) {
        XLPRINTF("Switching to boot partition failed %d\n", ret_code);
        bass_result = BASS_RC_FAILURE;
        goto error;
    }

    do {
        uint32_t next_offset = 0;
        ret_code = ROM_SHARED_MMC_READ(get_MmcType(),
                bootp_offset,
                (void *)buffer,
                sizeof(buffer),
                DMA_NOT_USED,
                data->jmp_table);


        if (ret_code != BOOT_OK) {
            XLPRINTF("Read failed %d\n", ret_code);
            bass_result = BASS_RC_FAILURE;
            goto error;
        }

        scanbuffer(buffer,
            &next_offset,
            &found_eng_magic,
            &found_sec_magic,
            &done_looking,
            &found_data);

        bootp_offset += next_offset;

    } while (!done_looking && bootp_offset < MAX_BOOT_OFFSET);

    if (found_data.eng_inbuffer_size == 0)
        found_data.eng_inbuffer_p = NULL;
    if (found_data.sec_inbuffer_size == 0)
        found_data.sec_inbuffer_p = NULL;

    /*
     * Call into the secure world with accumulated
     * data from the special records in the boot-partition.
     *
     * No way of detecting bootp locked, so send in false (0)
     */
    bass_result = bass_handle_bootp_magic(&found_data);

    if (BASS_RC_SUCCESS != bass_result) {
        XLPUTS(("Handle_bootp_magic failed\n"));
        goto error;
    }

    /* Switch back to user partition */
    ret_code = MMC_switch_to_boot_partition(data, toc_data, 0);
    if (ret_code != BOOT_OK) {
        XLPRINTF("Switching back to user partition failed %d\n",
            ret_code);
        bass_result = BASS_RC_FAILURE;
        goto error;
    }

error:
    return bass_result;
}

static t_uint32 load_and_execute_mem_init(struct data_collection *data,
                 struct toc_data *toc_data)
{
    t_uint32 trace;
    t_uint32 mem_init_ret_sts = 0;
    t_boot_error ret_sts = BOOT_OK;
    t_bool status;
    t_bool in_subtoc = FALSE;
    t_uint8 *payload_start;
    t_uint32 header_len;
    t_address read_address;
    t_uint32 *write_address;
    t_uint32 end_address;
    t_boot_toc_item mem_init_toc_item;


    trace = SEARCH_MEM_INIT_FUNC;
    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    /* The MEM_INIT section is copied after the XLOADER in eSRAM
     * scratch PWR_MGT data in eSram... no more needed
     */
#if (__STN_9540 > 0)
    /* Meminit should be copied on top of xloader stack */
    write_address = (t_uint32 *)(&linker_xloader_stack_top + 4);
#else
    write_address = (t_uint32 *) ((t_uint8 *) (toc_data->cert_header) +
                      toc_data->xloader_toc_item.Size);
#endif

    status = toc_search_item_in_toc((t_uint32) toc_data->toc_esram,
                    (t_uint32) toc_data->subtoc_esram,
                    (t_uint32) toc_data->subtoc_size,
                    ASCII_MEM_INIT,
                    (t_boot_toc_item
                     *) (&mem_init_toc_item),
                    &in_subtoc);

    if (status != TRUE)
        return XL_ERROR_MEM_INIT_NOT_FOUND;

    if (in_subtoc != TRUE) {
        /* Only add tocAddrMem if meminit is not found in subtoc */
        read_address = data->loader_info->tocAddrMem
             + mem_init_toc_item.Start;
    }
    else {
        read_address = mem_init_toc_item.Start;
    }

    XLPUTS("About to load MEMINIT\n");

    ret_sts = load_toc_item(data,
                toc_data,
                read_address,
                write_address,
                &mem_init_toc_item,
                MEM_INIT_TOKEN, MEM_INIT_LOADING_TIMEOUT);

    if (ret_sts != BOOT_OK) {
        xldbg_send_trace(data->loader_info,
                 (t_address)(&ret_sts), sizeof(ret_sts));
        goto error;
    }

    XLPUTS("After MEMINIT loaded\n");

    end_address = (t_address) ((t_uint8 *) write_address +
                   mem_init_toc_item.Size);

    /* Invalidate instruction cache before mem_init is
     * verified and executed
     */
    update_caches_after_load((void *)write_address,
                 (void *)end_address, TRUE);

    ret_sts = verify_payload(data,
                 toc_data,
                 &mem_init_toc_item,
                 write_address,
                 (void **)&payload_start,
                 0, ISSW_PL_TYPE_MEMINIT,
                 FALSE);

    if (ret_sts != BOOT_OK) {
        xldbg_send_trace(data->loader_info,
                 (t_address)(&ret_sts), sizeof(ret_sts));
        goto error;
    }
    header_len = (t_uint32)write_address - (t_uint32)payload_start;

    trace = EXECUTE_MEM_INIT_FUNC;
    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    /* Open DDR trackers to allow access on DDR
     * Trackers were closed in speed-up stucture of ISSW
     */
    /* Shouldn't this be done in memem_init_func(0)? */
    *(volatile t_uint32 *)DDR_TRACKER_REG = (unsigned int)0xFF;

    XLPUTS(("About to exec MEMINIT function\n"));

    ret_sts = execute_soc_settings(payload_start,
            mem_init_toc_item.Size - header_len, data->boot_status);

    if (ret_sts != BOOT_OK)
        goto error;

    XL_SendToken(data->loader_info, MEM_INIT_PASSED_TOKEN);

    XLPUTS("MEMINIT passed OK\n");
    return 0;


error:
    /* If internal error is detected in MEM_INIT, Xloader is stopped, and
     * core1 is woken up in a loop
     */

    XLPUTS(("MEMINIT internal error\n"));

    XL_SendToken(data->loader_info, MEM_INIT_FAILED_TOKEN);

    xldbg_send_trace(data->loader_info,
             (t_address) (&mem_init_ret_sts),
             sizeof(mem_init_ret_sts));

    /* Disable L1 caches and MMU */
    trace = DISABLE_CACHE_MMU;
    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    XL_DisableCachesMmu();

    /* Wake up Core1 and make it loop in Non secure */
    ROM_SHARED_FUNC_WAKEUP_CPU1((t_address) &core1_loop,
                    data->jmp_table);
#ifdef ENABLE_SYSTEM_RESET
    if ((data->indication.scenario & MASK_DEBUG_INFO) ==
        FLASHING_INDICATION) {
        XLPUTS("reseting ...\n");
        issw_api_reset();
        while (1)
            ;
    }
#else
    XLPUTS("Wait forever...\n");
    while (1)
        ;
#endif
}

static void set_ab8500_power_config(void)
{
    t_regu_sys_clk_req1_hp_valid1 regu_sys_clk_req1_hp_valid1_val;
    t_regu_hw_hp_req_1_valid1 regu_hw_hp_req_1_valid1_val;
    t_regu_hw_hp_req_2_valid1 regu_hw_hp_req_2_valid1_val;
    t_regu_ctrl_disch_2 regu_ctrl_disch_2_val;


    /*  Set up AB8500 power settings using PRCMU I2C communication */

    regu_sys_clk_req1_hp_valid1_val.reg =
        read_hw_i2c(AB8500_REGUSYSCLKREQ1HPVALID1);

    regu_sys_clk_req1_hp_valid1_val.bit.Vsmps2SysClkReq1HPValid = 0x1;

    write_hw_i2c(AB8500_REGUSYSCLKREQ1HPVALID1,
             regu_sys_clk_req1_hp_valid1_val.reg);

    regu_hw_hp_req_1_valid1_val.reg =
        read_hw_i2c(AB8500_REGUHWHPREQ1VALID1);

    regu_hw_hp_req_1_valid1_val.bit.Vsmps2HwHPReq1Valid = 1;

    write_hw_i2c(AB8500_REGUHWHPREQ1VALID1,
             regu_hw_hp_req_1_valid1_val.reg);

    regu_hw_hp_req_2_valid1_val.reg =
        read_hw_i2c(AB8500_REGUHWHPREQ2VALID1);

    regu_hw_hp_req_2_valid1_val.bit.Vsmps2HwHPReq2Valid = 1;

    write_hw_i2c(AB8500_REGUHWHPREQ2VALID1,
             regu_hw_hp_req_2_valid1_val.reg);

    /*
     *Remove the VsimDisch bit configuration
     *because it is a reserved bit in AB8540.
     */
#ifndef MACH_L8540
    regu_ctrl_disch_2_val.reg = read_hw_i2c(AB8500_REGUCTRLDISCH2);

    regu_ctrl_disch_2_val.bit.VsimDisch = 1;

    write_hw_i2c(AB8500_REGUCTRLDISCH2, regu_ctrl_disch_2_val.reg);
#endif

    return;
}


static t_boot_error search_normal_in_toc(struct data_collection *data,
                  struct toc_data *toc_data,
                  t_boot_toc_item *normal_appli_toc_item,
                  t_bool *in_subtoc)
{
    t_bool status;

    /*
     * Seach NORMAL or PROCUTION or ADL in TOC
     */
    if ((data->indication.scenario & MASK_DEBUG_INFO) ==
        PRODUCTION_INDICATION) {
        status =
            toc_search_item_in_toc((t_uint32) toc_data->toc_esram,
                       (t_uint32) toc_data->subtoc_esram,
                       (t_uint32) toc_data->subtoc_size,
                       ASCII_PRODUCTION,
                       normal_appli_toc_item,
                       in_subtoc);
        if (status != TRUE)
            return XL_ERROR_PRODUCTION_NOT_FOUND;

    } else if ((data->indication.scenario & MASK_DEBUG_INFO) ==
           ADL_INDICATION) {
        status =
            toc_search_item_in_toc((t_uint32) toc_data->toc_esram,
                       (t_uint32) toc_data->subtoc_esram,
                       (t_uint32) toc_data->subtoc_size,
                       ASCII_ADL, normal_appli_toc_item,
                       in_subtoc);
        if (status != TRUE)
            return XL_ERROR_ADL_NOT_FOUND;

    } else {
        status =
            toc_search_item_in_toc((t_uint32) toc_data->toc_esram,
                       (t_uint32) toc_data->subtoc_esram,
                       (t_uint32) toc_data->subtoc_size,
                       ASCII_NORMAL, normal_appli_toc_item,
                       in_subtoc);
        if (status != TRUE) {
            XLPUTS("XL_ERROR_NORMAL_NOT_FOUND\n");
            return XL_ERROR_NORMAL_NOT_FOUND;
        }
    }

    return BOOT_OK;
}

static void update_caches_before_normal_exec(struct data_collection *data)
{
    t_uint32 trace;
    volatile t_l2cc_registers *l2cc_controller =
        (t_l2cc_registers *) L2CC_REG_START_ADDR;

    /* Disable L1 caches and MMU before
     * executing the application
     */
#ifdef CACHE_ON
    trace = DISABLE_CACHE_MMU;

    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    XLPUTS("XL_DisableCachesMmu\n");
    XL_DisableCachesMmu();
    XL_CleanInvalidateL2();
#endif

    /* Call a secure Service to Enable L2CC if not yet enabled */
    if (l2cc_controller->Control.Enable == 0) {
        XLPUTS("Before ENABLE_L2CC\n");

        trace = ENABLE_L2CC;
        xldbg_send_trace(data->loader_info,
                 (t_address) (&trace), sizeof(trace));

        SECURITY_CallSecureService((t_sec_rom_service_id)
					   ISSWAPI_L2CC_ENABLE_INDEX,
                       SEC_ROM_FORCE_CLEAN_MASK, 0, 0);
        XLPUTS("After ENABLE_L2CC\n");
    }
#if defined(SNOWBALL)
    /* Boot is finished, let ISSW flush boot specific code
     * This call can only be made after loading IPL
     */
    XLPUTS("Flush boot issw code\n");
    SECURITY_CallSecureService((t_sec_rom_service_id)
                   ISSWAPI_FLUSH_BOOT_CODE,
                   SEC_ROM_FORCE_CLEAN_MASK, 0, 0);
#endif
}


static t_boot_error load_normal_appli(struct data_collection *data,
                      struct toc_data *toc_data)
{

    t_boot_toc_item normal_appli_toc_item;
    t_address read_address;
    t_pApplication appli;
    t_uint8 *write_address;
    t_uint32 trace;
    t_uint32 payload_offset = 0;
    t_boot_error ret_sts = BOOT_OK;
    t_uint32 pl_type = ISSW_PL_TYPE_AUTH_CHALLENGE;
    issw_signed_header_t *shdr = NULL;
    t_bool in_subtoc;

    /* Loads NORMAL or PROCUTION section and executes it
     * Never returns if success
     */

    XLPUTS("About to load NORMAL\n");

    trace = SEARCH_APPLI;
    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    ret_sts = search_normal_in_toc(data, toc_data, &normal_appli_toc_item, &in_subtoc);

    if (ret_sts != BOOT_OK)
        goto exit;

    if (FALSE == in_subtoc) {
        read_address =
            (t_address) (data->loader_info->tocAddrMem +
                 normal_appli_toc_item.Start);
    } else {
        read_address =
            (t_address) normal_appli_toc_item.Start;
    }
    /* Cannot verify if at address 0, problem with Boot ROM */
    if (normal_appli_toc_item.Load == 0)
        payload_offset = PAYLOAD_OFFSET;

    write_address = (t_uint8 *) normal_appli_toc_item.Load;

    ret_sts = load_toc_item(data,
                toc_data,
                read_address,
                (t_uint32 *)
                ((t_uint8 *) write_address + payload_offset),
                &normal_appli_toc_item,
                NORMAL_TOKEN,
                NORMAL_APPL_LOADING_TIMEOUT);

    if (ret_sts != BOOT_OK)
        goto exit;

    XLPUTS("After NORMAL loaded\n");

    /* Don't need to update I cache now, won't be executed yet */
    update_caches_after_load((void *)
                 ((uint8_t *)normal_appli_toc_item.Load +
                  payload_offset),
                 (void *)
                 ((uint8_t *)normal_appli_toc_item.Load +
                  payload_offset +
                  normal_appli_toc_item.Size),
                 FALSE);

    shdr = (issw_signed_header_t *)((uint8_t *)normal_appli_toc_item.Load +
                    payload_offset);

    /* This assumes that all decisions are equal for the f&c loader and
     * os loader as it will set the type incorrect for example when
     * x-launching the os-loader and it is a naked binary */
    if (shdr->magic == ISSW_SIGNED_HEADER_MAGIC) {
        pl_type = shdr->payload_type;
    } else if ((data->indication.scenario & MASK_DEBUG_INFO) ==
                FLASHING_INDICATION) {
        pl_type = ISSW_PL_TYPE_FC_LOADER;
    } else {
        pl_type = ISSW_PL_TYPE_OS_LOADER;
    }

    ret_sts = verify_payload(data,
                 toc_data,
                 &normal_appli_toc_item,
                 (t_uint32 *) normal_appli_toc_item.Load,
                 (void **)&appli,
                 payload_offset, pl_type, TRUE);

    if (ret_sts != BOOT_OK)
        goto exit;

    update_caches_before_normal_exec(data);

    if ((data->indication.scenario & MASK_DEBUG_INFO) ==
        PRODUCTION_INDICATION) {
        trace = EXECUTE_PRODUCTION;
    } else if ((data->indication.scenario & MASK_DEBUG_INFO) ==
           ADL_INDICATION) {
        trace = EXECUTE_ADL;
    } else {
        trace = EXECUTE_NORMAL;
    }

    xldbg_send_trace(data->loader_info,
             (t_address) (&trace), sizeof(trace));

    /* The Align field in the TOC entry is the entry point of Appli
     * Call NORMAL or PRODUCTION or ADL
     */
    appli = (t_pApplication) (normal_appli_toc_item.Load +
                  normal_appli_toc_item.Align);

    XLPUTS("Before call normal\n");

    /* R2 must contain 0 for Symbian */
    (*appli) (data->loader_info, 0, 0);

    /* Should never come back here....
     * The task for Xloader is finished, external Appli is running...
     */
exit:
    xldbg_send_trace(data->loader_info,
             (t_address) (&ret_sts), sizeof(ret_sts));
    return ret_sts;
}               /* end of load_normal_appli */

#ifdef ENABLE_LOAD_TEE
static void load_tee(struct data_collection *data,
                struct toc_data *toc_data)
{
    t_bool status;
    t_boot_toc_item toc_item;
    t_address read_address;
    t_uint32 *write_address;
    t_bool in_subtoc = FALSE;
    t_uint32 ret_sts;

    XLPUTS("About to load TEE\n");
    status = toc_search_item_in_toc((t_uint32)toc_data->toc_esram,
                                    (t_uint32) toc_data->subtoc_esram,
                                    (t_uint32) toc_data->subtoc_size,
                                    ASCII_TEE, &toc_item, &in_subtoc);

    if (status != TRUE) {
        XLPUTS("No TEE image in TOC\n");
        return;
    }

    read_address = (t_address)(data->loader_info->tocAddrMem +
                    toc_item.Start);
    write_address = (t_uint32 *)toc_item.Load;

    ret_sts = load_toc_item(data,
                            toc_data,
                            read_address,
                            write_address,
                            &toc_item,
                            TEE_TOKEN,
                            TEE_LOADING_TIMEOUT);
    if (ret_sts != BOOT_OK)
        return;
    XLPUTS("TEE image copied from TOC\n");


    /* Clean D cache, not I cache, won't be executed here on this CPU */
    update_caches_after_load((void *)write_address,
                 (void *)(write_address + toc_item.Size),
                 FALSE);

    SECURITY_CallSecureService((t_sec_rom_service_id)ISSWAPI_LOAD_TEE,
                   SEC_ROM_FORCE_CLEAN_MASK, write_address);
    XLPUTS("After TEE loaded\n");
}
#endif /*ENABLE_LOAD_TEE*/

static t_uint32 xloader(struct data_collection *data,
            struct toc_data *toc_data)
{
    t_uint32 ret_sts;

    ret_sts = xloader_prepare(data, toc_data);

    if (ret_sts != BOOT_OK) {
        XLPRINTF("xloader_prepare failed: %d\n",
             ret_sts);
        return ret_sts;
    }

    /* Voltage Check @VAUX2 before accessing the eMMC */
    if (emmc_voltage_check() < 0)
        XLPUTS("EMMC: Failed to check voltage");

#if ((__STN_8500 >= 20) && (__STN_9540 <= 0) && !defined(SNOWBALL))
    if (handle_bootp_data(data, toc_data) != BASS_RC_SUCCESS) {
        XLPUTS("Failed looking for/handling bootpartition magic-ids\n");
        /*
         * Errors in handle_bootp_data stems from boot-partition errors
         * while looking for the magic numbers. Closest boot error used.
         */
        return BOOT_INVALID_MAGIC_NUM;
    }
#endif
#ifdef TEE_TEST
    PRINTF("Running tests before MEMINIT\n");
    tee_test_run(0);
#endif
    /* Memory has to be initialized both for cold boot and soft reset */

    ret_sts = load_and_execute_mem_init(data, toc_data);

    if (ret_sts != BOOT_OK) {
        xldbg_send_trace(data->loader_info,
                 (t_address) (&ret_sts),
                 sizeof(ret_sts));
        return ret_sts;
    }

    set_ab8500_power_config();

#ifdef TEE_TEST
    XLPUTS("Running tests after MEMINIT\n");
    tee_test_run(1);
#endif

    /* ----------------------------------------------------------
       Send Engineering information on peripheral (if required)
       VI #27999
       ---------------------------------------------------------- */
    send_engi_info(data->loader_info, &data->indication);

#ifdef ENABLE_LOAD_TEE
    /* Load and start TEE Core if present */
    load_tee(data, toc_data);
#endif

    ret_sts = load_normal_appli(data, toc_data);

    XLPUTS(("Ret_Sts when loading NORMAL app\n"));
    xldbg_send_trace(data->loader_info,
             (t_address) (&ret_sts), sizeof(ret_sts));
    return ret_sts;

}


t_uint32 core1_loop(void)
{
    while (1)
        ;

    return 0;
}

