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

/* FatFS disk I/O adaption to LK eMMC*/

#include <debug.h>
#include <string.h>
#include <stdlib.h>

#include "target_config.h"
#include "diskio.h"
#include "target.h"
#include "mmc_if.h"
#include "ff.h"


/*******************************************************************************
 * Types, constants
 ******************************************************************************/


/*******************************************************************************
 * Static variables
 ******************************************************************************/

static mmc_properties_t    *sd = NULL;


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/



/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

DSTATUS disk_initialize (BYTE drive)
{
    int status = 0;

    if (0 > enable_vaux3()) {
        dprintf (CRITICAL, "%s: Failed to init VAUX3 for SD-card\n", __func__);
        return STA_NOINIT;
    }

    if(!mmc_sd_detected()) {
        return STA_NODISK;
    }

    /* Init internal L/S for SDMMC0 */
    if (mmc_sd_set_startup_iosv() < 0)
        return STA_NOINIT;

    sd = mmc_init (U8500_SDI0_BASE, &status, EXPECT_SD);
    dprintf (INFO, "%s: mmc_init status %d\n", __func__, status);
    if (MMC_OK == status) {
        return 0;
    }

    return STA_NOINIT;
}


DSTATUS disk_status (BYTE drive)
{
    return (NULL == sd) ? STA_NOINIT : 0;
}


DRESULT disk_read (BYTE drive, BYTE* buffer, DWORD sectorno, BYTE n_sectors)
{
    if ((uint32_t)buffer & 0x03)
        return RES_ERROR;

    if (MMC_OK == mmc_read (sd, sectorno, n_sectors, (uint32_t*)buffer)) {
        return RES_OK;
    }

    return RES_ERROR;
}


#if _READONLY == 0
DRESULT disk_write (BYTE drive, const BYTE* buffer, DWORD sectorno, BYTE n_sectors)
{
    if ((uint32_t)buffer & 0x03)
        return RES_ERROR;

    if (MMC_OK == mmc_write (sd, sectorno, n_sectors, (uint32_t*)buffer)) {
        return RES_OK;
    }

    return RES_ERROR;
}
#endif


DRESULT disk_ioctl (BYTE drive, BYTE command, void* buffer)
{
    DRESULT result = RES_OK;

    switch (command) {
    case CTRL_SYNC:
        /* NA for LK eMMC */
        break;

    case GET_SECTOR_COUNT:
        *(WORD*)buffer = (WORD)(sd->device_size / sd->read_bl_len);
        break;

    case GET_SECTOR_SIZE:
        *(WORD*)buffer = (WORD)sd->device_size;
        break;

    case GET_BLOCK_SIZE:
        *(DWORD*)buffer = sd->erase_group_size;
        break;

#if _USE_ERASE == 1
    case CTRL_ERASE_SECTOR:
        if (MMC_OK != mmc_erase (sd,
                                 *(DWORD*)buffer[0],
                                 (*(DWORD*)buffer[1] - *(DWORD*)buffer[0]) + 1)) {
            return RES_ERROR;
        }
        break;
#endif

    default:
        result = RES_PARERR;
        break;
    }

    return result;
}


DWORD get_fattime (void)
{
    /* Hardcoded to 1980 01 01 00 00 00 pending UTC to localtime support*/
    /* yyyyyyy mmmm ddddd hhhhh mmmmmm sssss */
    /* 0000000 0001 00001 00000 000000 00000 */
    return 0x210000;
}
