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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <platform/timer.h>


#include "mmc_types.h"
#include "mmc_host.h"
#include "mmc_if.h"
#include "dev/gpio.h"
#include "abx500.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/


/* eMMC Card Specific Data according to Jedec Standard no. 84-A44 */
bit_range_t csd_bit_map [MAX_BITS] = {
    {127, 126}, /* CSD_STRUCTURE      */
    {125, 122}, /* SPEC_VERS          */
    {121, 120}, /* RESERVED_3         */
    {119, 112}, /* TAAC               */
    {111, 104}, /* NSAC               */
    {103, 96},  /* TRAN_SPEED         */
    {95, 84},   /* CCC                */
    {83, 80},   /* READ_BL_LEN        */
    {79, 79},   /* READ_BL_PARTIAL    */
    {78, 78},   /* WRITE_BLK_MISALIGN */
    {77, 77},   /* READ_BLK_MISALIGN  */
    {76, 76},   /* DSR_IMP            */
    {75, 74},   /* RESERVED_2         */
    {73, 62},   /* C_SIZE             */
    {61, 59},   /* VDD_R_CURR_MIN     */
    {58, 56},   /* VDD_R_CURR_MAX     */
    {55, 53},   /* VDD_W_CURR_MIN     */
    {52, 50},   /* VDD_W_CURR_MAX     */
    {49, 47},   /* C_SIZE_MULT        */
    {46, 42},   /* ERASE_GRP_SIZE     */
    {41, 37},   /* ERASE_GRP_MULT     */
    {36, 32},   /* WP_GRP_SIZE        */
    {31, 31},   /* WP_GRP_ENABLE      */
    {30, 29},   /* DEFAULT_ECC        */
    {28, 26},   /* R2W_FACTOR         */
    {25, 22},   /* WRITE_BL_LEN       */
    {21, 21},   /* WRITE_BL_PARTIAL   */
    {20, 17},   /* RESERVED_1         */
    {16, 16},   /* CONTENT_PROT_APP   */
    {15, 15},   /* FILE_FORMAT_GRP    */
    {14, 14},   /* COPY               */
    {13, 13},   /* PERM_WRITE_PROTECT */
    {12, 12},   /* TMP_WRITE_PROTECT  */
    {11, 10},   /* FILE_FORMAT        */
    {9, 8},     /* ECC                */
    {7, 1},     /* CRC                */
    {0, 0}      /* NOT_USED           */
};


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/
static int mmc_send_go_idle(mmc_properties_t *device);
static int mmc_send_op_cond(mmc_properties_t *device, int sd);
static int mmc_send_all_send_cid(mmc_properties_t *device);
static int mmc_send_set_relative_addr(mmc_properties_t *device);
static int mmc_send_sd_set_bus_width(mmc_properties_t *device);
static int mmc_send_switch(mmc_properties_t *device,
                           uint8_t mode,
                           unsigned char index,
                           unsigned char value);
static int mmc_send_status(mmc_properties_t *device, uint32_t *status);
static int mmc_send_select_card(mmc_properties_t *device, int standby);
static int mmc_send_ext_csd(mmc_properties_t *device, uint8_t *ext_csd);
static int mmc_send_csd(mmc_properties_t *device);
static int mmc_send_cid(mmc_properties_t *device);
static int mmc_send_stop_transmission(mmc_properties_t *device);
static int mmc_send_set_blocklen(mmc_properties_t *device, uint32_t len);
static int mmc_send_set_block_count(mmc_properties_t *device,
                                    uint32_t n_blocks);
static int mmc_send_cmd_write_multiple_block(mmc_properties_t *device,
                                             uint32_t *source,
                                             uint32_t start_block,
                                             uint32_t n_blocks);
static int mmc_send_erase_group_start (mmc_properties_t *device,
                                       uint32_t         start_block);
static int mmc_send_erase_group_end (mmc_properties_t   *device,
                                     uint32_t           end_block);
static int mmc_send_mc_erase (mmc_properties_t *device);
static int mmc_send_app_cmd (mmc_properties_t *device);
static int mmc_send_sd_send_if_cond (mmc_properties_t *device);
static void parse_csd (mmc_properties_t *device);
static int mmc_set_hs_timing (mmc_properties_t *device);
static int mmc_set_frequency (mmc_properties_t *device);
static int mmc_set_bus (mmc_properties_t *device);

static int mmc_read_single_block (mmc_properties_t *device,
                                  uint32_t    start_block,
                                  uint32_t   *dest);
static int mmc_read_multiple_blocks (mmc_properties_t *device,
                                     uint32_t    start_block,
                                     uint32_t    n_blocks,
                                     uint32_t   *dest);
static int mmc_write_single_block (mmc_properties_t *device,
                                   uint32_t    start_block,
                                   uint32_t   *source);
static int mmc_write_multiple_blocks (mmc_properties_t *device,
                                      uint32_t    start_block,
                                      uint32_t    n_blocks,
                                      uint32_t   *source);


static uint32_t two_pwr_n(uint32_t N);

static void dump_csd_to_debug (mmc_properties_t *device);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * mmc_init
 */
mmc_properties_t* mmc_init (uint32_t sdi_base_address,
                            int *status,
                            int expect_sd)
{
    mmc_properties_t *device;

    *status = MMC_ERROR;

	/* Voltage Check@VAUX2 before accessing the eMMC */

	uint8_t vaux = ab8500_reg_read(AB8500_REGU_VAUX2_SEL_REG);

	if ((vaux > AB8500_VAUX2_2V7) && (vaux <= AB8500_VAUX2_MAX))
		dprintf(INFO,"Proper voltage 0x0%x applied to eMMC\n", vaux);
	else {
		if (ab8500_reg_write(AB8500_REGU_VAUX2_SEL_REG, AB8500_VAUX2_2V9) < 0)
			dprintf(INFO,"EMMC: Failed to set default voltage\n");
		else
			dprintf(INFO,"EMMC:Set to Default 0x0%x as Voltage is not in the Range!\n", vaux);
	}

    /* Create meta data struct */
    device = u8500_alloc_mmc_struct();
    if (!device) {
        dprintf(INFO, "error allocating mmc structure\n");
        return NULL;
    }

    /* Init host. Cast to void, function return 0 in all cases... */
    if (EXPECT_SD == expect_sd) {
        (void)u8500_mmc_host_init(device,
                                  (struct sdi_registers *)sdi_base_address);
    } else {
        (void)u8500_emmc_host_init(device,
                                  (struct sdi_registers *)sdi_base_address);
    }


    /* Reset host */
    *status = mmc_host_reset(device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    /* Setup bus width, initial value, changed after investigating host
     * capabilites */
    device->bus_width = BUS_WIDTH_1;

    /* Setup clock , initial value, changed after investigating host
     * capabilites */
    device->device_clock = INIT_CLOCK;

    /* Set bus and clock */
    host_set_ios(device);

    /* Reset card */
    *status = mmc_send_go_idle(device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    /* Test for SD card */
    *status = mmc_send_sd_send_if_cond (device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) Check for SD: status %d\n", __LINE__, *status);
    }

    /* Check if SD-card */
    *status = mmc_send_op_cond(device, 1);
    if (MMC_OK == *status) {
        device->is_sd_card = 1;
    } else {
        dprintf(INFO,"mmc_init SD check (%d) status %d. Not an SD card\n", __LINE__, *status);
        /* Try eMMMC */
        *status = mmc_send_op_cond(device, 0);
        if (MMC_OK != *status) {
            dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
            goto ErrorExit;
        }
    }

    *status = mmc_send_all_send_cid(device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    *status = mmc_send_set_relative_addr(device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

   *status = mmc_send_csd(device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    *status = mmc_send_select_card(device, 0);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    if (!device->is_sd_card) {
        *status = mmc_send_ext_csd(device, device->ext_csd);
        if (MMC_OK != *status) {
            dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
            goto ErrorExit;
        }
    }

    parse_csd(device);
    dump_csd_to_debug(device);

    *status = mmc_set_hs_timing (device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    /* Set ecsd buswidth and device buswidth according to host capabilities */
    *status = mmc_set_bus (device);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

    /* Set host buswidth and clock */
    device->device_clock = MAX_CLOCK;
    if (device->is_sd_card)
      device->device_clock = MAX_CLOCK / 2;

    host_set_ios(device);

    if (device->rel_write == REL_WRITE_TRUE) {
        *status = mmc_send_switch(device, ECSD_ACCESS_MODE_WRITE_BYTE, ECSD_WR_REL_SET, 0x1F);
        if (MMC_OK != *status) {
            dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
            goto ErrorExit;
        }
    }

    *status = mmc_send_set_blocklen(device, MMC_BLOCK_SIZE_512);
    if (MMC_OK != *status) {
        dprintf(INFO,"mmc_init (%d) status %d\n", __LINE__, *status);
        goto ErrorExit;
    }

ErrorExit:
    if (MMC_OK != *status) {
        dprintf (INFO, "\n\nINIT EMMC FAILED %d\n\n", *status);
        device = mmc_close (device);
    }

    return device;
}


/*
 * mmc_close
 */
mmc_properties_t *mmc_close (mmc_properties_t *device)
{
    if (device) {
        if (device->host) {
            free (device->host);
        }
        free (device);
        device = NULL;
    }

    return NULL;
}


/*
 * mmc_read
 */
int mmc_read (mmc_properties_t *device,
              uint32_t    start_block,
              uint32_t    n_blocks,
              uint32_t   *dest)
{
    int status;

    if (!device) {
        return MMC_ERROR;
    }

    if (n_blocks == 1) {
        status = mmc_read_single_block (device, start_block, dest);
    } else {
        status = mmc_read_multiple_blocks (device, start_block, n_blocks, dest);
    }

    return status;
}


/*
 * mmc_write
 */
int mmc_write (mmc_properties_t    *device,
               uint32_t       start_block,
               uint32_t       n_blocks,
               uint32_t      *source)
{
    int status;

    if (!device) {
        return MMC_ERROR;
    }

    if (n_blocks == 1) {
        status = mmc_write_single_block (device, start_block, source);
    } else {
        status = mmc_write_multiple_blocks (device, start_block, n_blocks, source);
    }

    return status;
}


/*
 * mmc_erase
 *
 */
int mmc_erase (mmc_properties_t    *device,
               uint32_t       start_block,
               uint32_t       n_blocks)
{
    int status;

    if (!device) {
        return MMC_ERROR;
    }

    if (0 == n_blocks) {
        return MMC_OK;
    }

    status = mmc_send_erase_group_start (device, start_block);
    if (MMC_OK != status) {
        return status;
    }

    status = mmc_send_erase_group_end (device, start_block + (n_blocks - 1));
    if (MMC_OK != status) {
        return status;
    }

    status = mmc_send_mc_erase (device);

    return status;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

static int mmc_send_go_idle(mmc_properties_t *device)
{
    mmc_cmd_t cmd;
    int status;

    udelay(uS1000);

    FILL_CMD (cmd, MC_GO_IDLE_STATE,MC_RESP_NONE,0,0,0,0);

    status = host_request(device, &cmd, NULL);
    if (MMC_OK != status) {
        return status;
    }

    udelay(uS2000);

    return MMC_OK;
}

static int mmc_send_op_cond(mmc_properties_t *device, int sd)
{
    int timeout = OP_COND_RETRIES;
    mmc_cmd_t cmd;
    int status;

    if (sd) {
        FILL_CMD(cmd, SD_SEND_OP_COND,MC_RESP_R3, 0, 0, 0, 0);
        cmd.arg = device->voltage_window | OCR_HCS;
    } else {
        FILL_CMD(cmd, MC_SEND_OP_COND,MC_RESP_R3, 0, 0, 0, 0);
        cmd.arg = OCR_SECTOR_MODE | device->voltage_window;
    }

    do {
        if (sd) {
            status = mmc_send_app_cmd (device);
            if (MMC_OK != status){
                dprintf(INFO,"mmc_send_op_cond (%d) status %d\n", __LINE__, status);
                return status;
            }
        }

        status = host_request(device, &cmd, NULL);
        if (MMC_OK != status){
            dprintf(INFO,"mmc_send_op_cond (%d) status %d\n", __LINE__, status);
            return status;
        }

        udelay(uS1000);
    } while (!(cmd.resp_data[0] & OCR_BUSY_BIT) && timeout--);

    if (timeout <= 0)
        return MMC_UNUSABLE_ERR;

    device->spec_vers = SPEC_VERS_UNKNOWN;
    device->ocr = cmd.resp_data[0];

    device->sector_mode = ((device->ocr & OCR_SECTOR_MODE) == OCR_SECTOR_MODE);
    device->rca = 0;

    return MMC_OK;
}


static int mmc_send_all_send_cid(mmc_properties_t *device)
{
    int status;
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_ALL_SEND_CID, MC_RESP_R2, 1, 1, 0, 0);

    status = host_request(device, &cmd, NULL);
    if (MMC_OK != status) {
        return status;
    }

    memcpy(device->cid, cmd.resp_data, 16);

    return status;
}

static int mmc_send_set_relative_addr(mmc_properties_t *device)
{
    int status;
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_SET_RELATIVE_ADDR, MC_RESP_R1, 0, 1, device->rca << 16, 0);

    status = host_request(device, &cmd, NULL);
    if (MMC_OK != status) {
        return status;
    }

    if (device->is_sd_card) {
        device->rca = (cmd.resp_data[0] >> 16) & 0xffff;
    }

    return status;
}


static int mmc_send_switch(mmc_properties_t *device, uint8_t mode, unsigned char index, unsigned char value)
{
    int err;
    uint32_t status;
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_SWITCH, MC_RESP_R1b, 0, 1, 0, 0);
    cmd.arg = (mode << 24) | (index << 16) | (value << 8);

    err = host_request(device, &cmd, NULL);
    if (err)
        return err;

    /* Must check state to be sure of no errors */
    do {
        err = mmc_send_status(device, &status);
        if (err)
            return err;
    } while (R1_CURRENT_STATE(status) == R1_STATE_PRG);

    return 0;
}

static int mmc_send_status(mmc_properties_t *device, uint32_t *status)
{
    int err;
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_SEND_STATUS, MC_RESP_R1, 0, 1, device->rca << 16, 0);

    err = host_request(device, &cmd, NULL);
    if (err)
        return err;

    if (status)
        *status = cmd.resp_data[0];

    return 0;
}

static int mmc_send_select_card(mmc_properties_t *device, int standby)
{
    int status;
    mmc_cmd_t cmd;
    uint32_t rca;

    rca = standby ? 0 : device->rca << 16;

    FILL_CMD(cmd, MC_SELECT_CARD, MC_RESP_R1b, 0, 1 ,rca, 0);

    status = host_request(device, &cmd, NULL);

    return status;
}


static int mmc_send_ext_csd(mmc_properties_t *device, uint8_t *ext_csd)
{
    mmc_cmd_t cmd;
    mmc_data_t data;

    if ((uint32_t)ext_csd & 0x3) {
        return MMC_ADDR_MISALIGNED;
    }

    FILL_CMD(cmd, MC_SEND_EXT_CSD, MC_RESP_R1, 0, 1, 0, 0);

    data.data = (uint32_t*)ext_csd;
    data.n_blocks = 1;
    data.block_size = MMC_BLOCK_SIZE_512;
    data.rd_wr = DATA_READ;

   return host_request(device, &cmd, &data);
}


static int mmc_send_csd(mmc_properties_t *device)
{
    int status;
    mmc_cmd_t cmd;
    uint32_t rca;

    rca = device->rca << 16;

    FILL_CMD(cmd, MC_SEND_CSD, MC_RESP_R2, 1, 1, rca, 1);

    status = host_request(device, &cmd, NULL);

    if (MMC_OK == status) {
        device->csd[0] = cmd.resp_data[0];
        device->csd[1] = cmd.resp_data[1];
        device->csd[2] = cmd.resp_data[2];
        device->csd[3] = cmd.resp_data[3];
    }

    return status;
}


static int mmc_send_stop_transmission(mmc_properties_t *device)
{
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_STOP_TRANSMISSION, MC_RESP_R1b, 0, 1, 0, 0);

    return host_request(device, &cmd, NULL);
}


static int mmc_send_set_blocklen(mmc_properties_t *device, uint32_t len)
{
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_SET_BLOCKLEN, MC_RESP_R1, 0, 1, len, 0);

    return host_request(device, &cmd, NULL);
}


static int mmc_send_set_block_count(mmc_properties_t *device, uint32_t n_blocks)
{
    mmc_cmd_t cmd;

    FILL_CMD(cmd, MC_SET_BLOCK_COUNT, MC_RESP_R1, 0, 1, 0, 0);
    cmd.arg = device->rel_write | n_blocks;

    return host_request(device, &cmd, NULL);
}


static int mmc_send_cmd_write_multiple_block(mmc_properties_t *device, uint32_t *source, uint32_t start_block, uint32_t n_blocks)
{
    mmc_cmd_t cmd;
    mmc_data_t data;

    FILL_CMD(cmd, MC_WRITE_MULTIPLE_BLOCK, MC_RESP_R1, 0, 1, 0, DATA_WRITE);
    cmd.arg = (device->sector_mode) ?
                    start_block :
                    start_block * device->write_bl_len;

    data.block_size = device->write_bl_len;
    data.rd_wr = DATA_WRITE;
    data.data = source;
    data.n_blocks = n_blocks;

    return host_request(device, &cmd, &data);
}


static int mmc_send_erase_group_start (mmc_properties_t *device,
                                       uint32_t         start_block)
{
    mmc_cmd_t  cmd;

    FILL_CMD(cmd, MC_ERASE_GROUP_START, MC_RESP_R1, 0, 1, 0, 0);
    if (device->sector_mode)
        cmd.arg = start_block;
    else
        cmd.arg = start_block * device->read_bl_len;

    return host_request(device, &cmd, NULL);
}


static int mmc_send_erase_group_end (mmc_properties_t   *device,
                                     uint32_t           end_block)
{
    mmc_cmd_t  cmd;

    FILL_CMD(cmd, MC_ERASE_GROUP_END, MC_RESP_R1, 0, 1, 0, 0);
    if (device->sector_mode)
        cmd.arg = end_block;
    else
        cmd.arg = end_block * device->read_bl_len;

    return host_request(device, &cmd, NULL);
}


static int mmc_send_mc_erase (mmc_properties_t *device)
{
    mmc_cmd_t   cmd;
    uint32_t    arg;

    arg = 0;
    if (device->ext_csd[ECSD_SEC_FEATURE_SUPPORT] & SEC_ER_EN) {
        arg |= SEC_FEATURE_ARG_SECURE;
    }

    if (device->ext_csd[ECSD_SEC_FEATURE_SUPPORT] & SEC_GB_CL_EN) {
        arg |= SEC_FEATURE_ARG_GARBAGE_COLLECT;
        arg |= SEC_FEATURE_ARG_ID_WRITE_BLOCK;
    }

    FILL_CMD(cmd, MC_ERASE, MC_RESP_R1, 0, 1, arg, 0);

    return host_request(device, &cmd, NULL);
}


static int mmc_send_app_cmd (mmc_properties_t *device)
{
    mmc_cmd_t   cmd;
    uint32_t    arg;

    arg = device->rca << 16;

    FILL_CMD(cmd, MC_APP_CMD, MC_RESP_R1, 0, 1, arg, 0);

    return host_request(device, &cmd, NULL);
}


static int mmc_send_sd_send_if_cond (mmc_properties_t *device)
{
    mmc_cmd_t   cmd;
    uint32_t    arg;
    int         status;

    arg = 0x1aa; //((device->voltage_window & 0xff8000) != 0) << 8 | 0xaa;
    FILL_CMD(cmd, SD_SEND_IF_COND, MC_RESP_R7, 0, 1, arg, 0);

    status = host_request(device, &cmd, NULL);

/*    if (MMC_OK != status) {
        dprintf(INFO,"mmc_send_sd_send_if_cond (%d) status %d\n", __LINE__, status);
    }*/

    if ((cmd.resp_data[0] & 0xff) != 0xaa) {
        return MMC_UNUSABLE_ERR;
    } else {
        device->spec_vers = SDSC_V2x;
    }

	return MMC_OK;

}

static int mmc_send_sd_set_bus_width(mmc_properties_t *device)
{
    int status;
    mmc_cmd_t cmd;
    uint32_t arg = 0;

    if (device->bus_width == BUS_WIDTH_4)
        arg = 1 << 1;

    FILL_CMD(cmd, SD_SET_BUS_WIDTH, MC_RESP_R1, 0, 1, arg, 0);

    /* Some cards seem to need a small delay */
    udelay (10000);

    status = mmc_send_app_cmd (device);
    if (MMC_OK != status){
        dprintf(INFO,"mmc_send_sd_set_bus_width (%d) status %d\n", __LINE__, status);
        return status;
    }
    status = host_request(device, &cmd, NULL);

    return status;
}

static void parse_csd (mmc_properties_t *device)
{
    uint32_t unit;
    uint32_t multiplier;
    uint32_t freq;
    uint32_t c_size;
    uint32_t c_size_mult;
    uint32_t erase_grp_size;
    uint32_t erase_grp_mult;
    uint32_t read_bl_partial;
    uint32_t write_bl_partial;

    /* SPEC_VER */
    device->spec_vers = csd_get_value(SPEC_VERS, device->csd);

    /* TRAN_SPEED */
    freq = csd_get_value(TRAN_SPEED, device->csd);
    unit = freq & 0x7;
    multiplier = (freq >> 3) & 0xf;
    device->tran_speed = unit * multiplier;

    /* READ_BL_LEN */
    device->read_bl_len = csd_get_value(READ_BL_LEN, device->csd);

    /* WRITE_BL_LEN */
    device->write_bl_len = csd_get_value(WRITE_BL_LEN, device->csd);

    /* Device size */
    c_size = csd_get_value(C_SIZE, device->csd);
    c_size_mult = csd_get_value(C_SIZE_MULT, device->csd);

    if (c_size == 0xfff) { /* > 2GB */
        device->device_size = (uint64_t)device->ext_csd[ECSD_SEC_COUNT / 4];
        device->device_size = (uint64_t)(device->device_size * MMC_BLOCK_SIZE_512);
    } else {              /* <= 2GB */
        if ((c_size_mult < 8) && (device->read_bl_len < 12)) {
            device->device_size = (uint64_t)(((c_size + 1) * two_pwr_n(c_size_mult + 2)) * two_pwr_n(device->read_bl_len));
        } else { // Invalid parameters!
            device->device_size = 0;
        }
    }

    device->wr_rel_param = device->ext_csd[ECSD_WR_REL_PARAM];
    device->rel_wr_sec_c = device->ext_csd[ECSD_REL_WR_SEC_C];

    /* Calc erase group size */
    erase_grp_size = csd_get_value(ERASE_GRP_SIZE, device->csd);
    erase_grp_mult = csd_get_value(ERASE_GRP_MULT, device->csd);
    device->erase_group_size = (erase_grp_size + 1) * (erase_grp_mult + 1);

    /* Calc actual values for read_bl_len and write_bl_len */
    device->read_bl_len = two_pwr_n (device->read_bl_len);
    read_bl_partial = csd_get_value (READ_BL_PARTIAL, device->csd);

    device->write_bl_len = two_pwr_n (device->write_bl_len);
    write_bl_partial = csd_get_value (WRITE_BL_PARTIAL, device->csd);

    dprintf (SPEW, "device->read_bl_len 0x%08x, device->write_bl_len 0x%08x\n", device->read_bl_len, device->write_bl_len);

    /* TODO: Limited to 512 byte pages for now */
    if (device->read_bl_len > MMC_BLOCK_SIZE_512)
        device->read_bl_len = MMC_BLOCK_SIZE_512;

    if (!write_bl_partial)
        device->write_bl_len = MMC_BLOCK_SIZE_512;

    if (device->is_sd_card) {
        device->read_bl_len = MMC_BLOCK_SIZE_512;
        device->write_bl_len = device->read_bl_len;
    }

    dprintf (SPEW, "device->read_bl_len 0x%08x, device->write_bl_len 0x%08x\n", device->read_bl_len, device->write_bl_len);
}

static int mmc_set_hs_timing (mmc_properties_t *device)
{
    int status;

    /* HS supported from ver 4 */
    if (device->spec_vers != SPEC_VERS_MMC_4) {
        return MMC_OK;
    }

    status = mmc_send_switch(device, ECSD_ACCESS_MODE_WRITE_BYTE, ECSD_HS_TIMING, 1);
    if (MMC_OK != status)
        return status;

    if (device->wr_rel_param & ECSD_WR_REL_PARAM_HS_CTRL_REL)
        status = mmc_send_switch(device, ECSD_ACCESS_MODE_WRITE_BYTE, ECSD_WR_REL_SET, 1);

    return MMC_OK;
}


static int mmc_set_bus (mmc_properties_t *device)
{
    int     status;
    uint8_t ecsd_bw;

    ecsd_bw = ECSD_BW_1;
    device->bus_width = BUS_WIDTH_1;

    if (device->host->host_bus_width & BUS_WIDTH_8) {
        device->bus_width = BUS_WIDTH_8;
        if (device->ddr == DDR_ENABLED) {
            ecsd_bw = ECSD_BW_8_DDR;
        } else {
            ecsd_bw = ECSD_BW_8;
        }
    } else if (device->host->host_bus_width & BUS_WIDTH_4) {
        device->bus_width = BUS_WIDTH_4;
        if (device->ddr == DDR_ENABLED) {
            ecsd_bw = ECSD_BW_4_DDR;
        } else {
            ecsd_bw = ECSD_BW_4;
        }
    }

    if (device->is_sd_card) {
        /* Set card in transfer state */
        /* send ACMD6 */
        status = mmc_send_sd_set_bus_width(device);
        if (MMC_OK != status) {
            dprintf (INFO, "Failed setting bus %d\n", status);
            return status;
        }
    } else {
        status = mmc_send_switch(device, ECSD_ACCESS_MODE_WRITE_BYTE, ECSD_BUS_WIDTH, ecsd_bw);
        if (MMC_OK != status) {
            dprintf (INFO, "Failed setting bus %d\n", status);
            return status;
        }

        if (device->ddr == DDR_ENABLED) {
            status = mmc_send_switch(device, ECSD_ACCESS_MODE_WRITE_BYTE, ECSD_POWER_CLASS, 0xAA);
            if (MMC_OK != status) {
                return status;
            }
        }
    }

    return status;
}


static int mmc_read_single_block (mmc_properties_t *device,
                                  uint32_t    start_block,
                                  uint32_t   *dest)
{
    mmc_cmd_t cmd;
    mmc_data_t data;

    dprintf(SPEW, "mmc_read_single 0x%08x 0x%08x\n", start_block, start_block*device->read_bl_len);

    FILL_CMD(cmd, MC_READ_SINGLE_BLOCK, MC_RESP_R1, 0, 1, 0, 0);

    if (device->sector_mode)
        cmd.arg = start_block;
    else
        cmd.arg = start_block * device->read_bl_len;

    data.data = dest;
    data.n_blocks = 1;
    data.block_size = device->read_bl_len;
    data.rd_wr = DATA_READ;

    return host_request(device, &cmd, &data);
}


static int mmc_read_multiple_blocks (mmc_properties_t *device,
                                     uint32_t    start_block,
                                     uint32_t    n_blocks,
                                     uint32_t   *dest)
{
    mmc_cmd_t  cmd;
    mmc_data_t data;
    int             status = MMC_OK;
    uint32_t        block_count = 0;
    uint32_t        blocks_to_rd;

    dprintf (SPEW, "mmc: read multiple, start 0x%08x n 0x%08x\n", start_block, n_blocks);

    while (block_count < n_blocks) {
        blocks_to_rd = n_blocks < device->host->host_blocksize ? n_blocks : device->host->host_blocksize;

        FILL_CMD(cmd, MC_READ_MULTIPLE_BLOCK, MC_RESP_R1, 0, 1, 0, 0);
        if (device->sector_mode)
            cmd.arg = start_block;
        else
            cmd.arg = start_block * device->read_bl_len;

        data.block_size = device->read_bl_len;
        data.rd_wr = DATA_READ;
        data.data = dest;
        data.n_blocks = blocks_to_rd;

        status = host_request(device, &cmd, &data);
        if (MMC_OK != status) {
            goto ErrorExit;
        }

        status = mmc_send_stop_transmission(device);
        if (MMC_OK != status) {
            goto ErrorExit;
        }

        block_count += blocks_to_rd;
        start_block += blocks_to_rd;
        dest += (blocks_to_rd * device->read_bl_len) / sizeof(uint32_t);
    }

ErrorExit:

    dprintf (SPEW, "mmc: read multiple, status %d\n", status);

    return status;
}


static int mmc_write_single_block (mmc_properties_t *device,
                                   uint32_t    start_block,
                                   uint32_t   *source)
{
    mmc_cmd_t cmd;
    mmc_data_t data;

    dprintf (SPEW, "mmc: write single, start 0x%08x\n", start_block);

    FILL_CMD(cmd, MC_WRITE_SINGLE_BLOCK, MC_RESP_R1, 0, 1, 0, 0);

    if (device->sector_mode)
        cmd.arg = start_block;
    else
        cmd.arg = start_block * device->write_bl_len;

    data.data = source;
    data.n_blocks = 1;
    data.block_size = device->write_bl_len;
    data.rd_wr = DATA_WRITE;

    return host_request(device, &cmd, &data);
}


static int mmc_write_multiple_blocks (mmc_properties_t *device,
                                      uint32_t    start_block,
                                      uint32_t    n_blocks,
                                      uint32_t   *source)
{
    int             status = MMC_OK;
    uint32_t        blocks_to_wr;
    uint32_t        blocks_written;
    uint32_t        max_blocks = device->host->host_blocksize;

    status = MMC_OK;
    blocks_written = 0;

    dprintf (SPEW, "mmc: write multiple, start 0x%08x n 0x%08x\n", start_block, n_blocks);

    if ((device->rel_write == REL_WRITE_TRUE) &&
       !(device->wr_rel_param & ECSD_WR_REL_PARAM_EN_REL_WR)) {
        max_blocks = max_blocks < device->rel_wr_sec_c ? max_blocks : device->rel_wr_sec_c;
    }

    while (blocks_written < n_blocks) {
        blocks_to_wr = n_blocks - blocks_written;
        blocks_to_wr = blocks_to_wr > max_blocks ? max_blocks : blocks_to_wr;

        if (device->rel_write == REL_WRITE_TRUE) {
            status = mmc_send_set_block_count(device, blocks_to_wr);
            if (MMC_OK != status) {
                goto ErrorExit;
            }
        }

        status = mmc_send_cmd_write_multiple_block(device, source, start_block, blocks_to_wr);
        if (MMC_OK != status) {
            goto ErrorExit;
        }

        if (!(device->rel_write == REL_WRITE_TRUE) ||
             (max_blocks != device->rel_wr_sec_c)) {
            status = mmc_send_stop_transmission(device);
            if (MMC_OK != status) {
                goto ErrorExit;
            }
        }

        blocks_written += blocks_to_wr;
        source += (blocks_to_wr * device->write_bl_len) / sizeof(uint32_t);
        start_block += blocks_to_wr;
    }

ErrorExit:
    return status;
}


uint32_t csd_get_value(csd_emmc_bits_t bits, uint32_t *csd)
{
    uint32_t value;
    uint32_t index_hi;
    uint32_t index_lo;
    uint32_t n_bits;
    uint32_t n_bits_lo;
    uint32_t n_bits_hi;
    uint32_t shift;
    uint32_t mask;
    uint32_t mask_lo;
    uint32_t ii;

    /* Determine index in csd vector from bits parameter
    ** hi/lo since they might overlap and need different treatment
    */
    index_hi = 3 - (csd_bit_map[bits].hi / 32);
    index_lo = 3 - (csd_bit_map[bits].lo / 32);

    /* Determine number of bits */
    n_bits = (csd_bit_map[bits].hi - csd_bit_map[bits].lo) + 1;

    /* Create mask for n_bits shifted */
    for (mask = 0, ii = 0; ii < n_bits; ii++) {
        mask = mask << 1;
        mask += 1;
    }

    /* If indexes are the same, all bits reside in one 32-bit word */
    if (index_hi == index_lo) {
        shift = csd_bit_map[bits].lo % 32;
        value = (csd[index_hi] >> shift) & mask;
    } else {
        /* n_bits from lo and hi respectively */
        n_bits_lo = 32 - (csd_bit_map[bits].lo % 32);
        n_bits_hi = (csd_bit_map[bits].hi % 32) + 1;

        /* assign value and make room for lo bits */
        value = csd[index_hi] << n_bits_lo;

        /* Determine lo bits left shift */
        shift = 32 - n_bits_lo;

        /* Determine lo bits mask */
        for (mask_lo = 0, ii = 0; ii < n_bits_lo; ii++) {
            mask_lo = mask_lo << 1;
            mask_lo += 1;
        }

        /* Calc final value */
        value = (value | ((csd[index_lo] >> shift) & mask_lo)) & mask;
    }

    return value;
}


static uint32_t two_pwr_n(uint32_t N)
{
    if (N > 31) {
        return 0;
    }

    return 1 << N;
}

static void dump_csd_to_debug (mmc_properties_t *device)
{
#if DEBUG > 1
    char *str[MAX_BITS] = {
    "CSD_STRUCTURE      ", /* 2    R    [127:126] */
    "SPEC_VERS          ", /* 4    R    [125:122] */
    "RESERVED_3         ", /* 2    R    [121:120] */
    "TAAC               ", /* 8    R    [119:112] */
    "NSAC               ", /* 8    R    [111:104] */
    "TRAN_SPEED         ", /* 8    R    [103:96] */
    "CCC                ", /*12    R    [95:84] */
    "READ_BL_LEN        ", /* 4    R    [83:80] */
    "READ_BL_PARTIAL    ", /* 1    R    [79:79] */
    "WRITE_BLK_MISALIGN ", /* 1    R    [78:78] */
    "READ_BLK_MISALIGN  ", /* 1    R    [77:77] */
    "DSR_IMP            ", /* 1    R    [76:76] */
    "RESERVED_2         ", /* 2    R    [75:74] */
    "C_SIZE             ", /*12    R    [73:62] */
    "VDD_R_CURR_MIN     ", /* 3    R    [61:59] */
    "VDD_R_CURR_MAX     ", /* 3    R    [58:56] */
    "VDD_W_CURR_MIN     ", /* 3    R    [55:53] */
    "VDD_W_CURR_MAX     ", /* 3    R    [52:50] */
    "C_SIZE_MULT        ", /* 3    R    [49:47] */
    "ERASE_GRP_SIZE     ", /* 5    R    [46:42] */
    "ERASE_GRP_MULT     ", /* 5    R    [41:37] */
    "WP_GRP_SIZE        ", /* 5    R    [36:32] */
    "WP_GRP_ENABLE      ", /* 1    R    [31:31] */
    "DEFAULT_ECC        ", /* 2    R    [30:29] */
    "R2W_FACTOR         ", /* 3    R    [28:26] */
    "WRITE_BL_LEN       ", /* 4    R    [25:22] */
    "WRITE_BL_PARTIAL   ", /* 1    R    [21:21] */
    "RESERVED_1         ", /* 4    R    [20:17] */
    "CONTENT_PROT_APP   ", /* 1    R    [16:16] */
    "FILE_FORMAT_GRP    ", /* 1   R/W   [15:15] */
    "COPY               ", /* 1   R/W   [14:14] */
    "PERM_WRITE_PROTECT ", /* 1   R/W   [13:13] */
    "TMP_WRITE_PROTECT  ", /* 1  R/W/E  [12:12] */
    "FILE_FORMAT        ", /* 2   R/W   [11:10] */
    "ECC                ", /* 2  R/W/E  [9:8] */
    "CRC                ", /* 7  R/W/E  [7:1] */
    "NOT_USED           " /* 1    1    [0:0] ,always 1 */
    };
    int ii;

    for (ii = CSD_STRUCTURE; ii < MAX_BITS; ii++) {
        dprintf (SPEW, "%s 0x%04x\n", str[ii], (uint16_t)csd_get_value(ii, device->csd));
    }
#else
    device = device;
#endif
}
