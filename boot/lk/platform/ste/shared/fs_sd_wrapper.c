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

#include <stdio.h>
#include <debug.h>
#include <reg.h>
#include <platform/timer.h>

#include "target_config.h"
#include "abx500.h"
#include "mmc_if.h"
#include "platform.h"
#include "machineid.h"
#include <target.h>


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

uint32_t fs_sd_init ()
{
    int status;

    if (0 > enable_vaux3()) {
        dprintf (CRITICAL, "Failed to init VAUX3 for SD-card\n");
    }

    /* wait for SD card */
    if(!mmc_sd_detected()) {
        printf ("Crash handling: Please insert SD card\n");
        do {
            putc('.');
            thread_sleep(1000); /* ms */
        } while(!mmc_sd_detected());
        putc('\n');
    }

    /* Init internal L/S for SDMMC0 */
    if (mmc_sd_set_startup_iosv() < 0)
        return 1;

    sd = mmc_init (U8500_SDI0_BASE, &status, EXPECT_SD);
    dprintf (INFO, "fs_sd_mount: mmc_init status %d\n", status);
    if (MMC_OK == status) {
        return 0;
    }

    return 1;
}


uint32_t fs_sd_mount ()
{
    /* Nothing to do here, really
     * SD card init done by fs_sd_init
     * Sector size must be known in advance...
     */
    return 0;
}


uint32_t fs_sd_unmount ()
{
    sd = mmc_close (sd);

    (void)disable_vaux3 ();

    return 0;
}


uint32_t fs_sd_read_sectors (uint32_t start_sector,
                             uint32_t n_sectors,
                             void *buffer)
{
    int status;

    status = mmc_read (sd, start_sector, n_sectors, (uint32_t*)buffer);
    if (MMC_OK == status) {
        return 0;
    }

    dprintf (CRITICAL, "fs_sd_read_sectors failed: s 0x%08x, n 0x%08x, status = %d\n",
             start_sector, n_sectors, status);

    return 1;
}


uint32_t fs_sd_write_sectors (uint32_t start_sector,
                              uint32_t n_sectors,
                              void *buffer)
{
    int status;

    status = mmc_write (sd, start_sector, n_sectors, (uint32_t*)buffer);
    if (MMC_OK == status) {
        return 0;
    }

    dprintf (CRITICAL, "fs_sd_write_sectors failed: s 0x%08x, n 0x%08x, status = %d\n",
             start_sector, n_sectors, status);

    return 1;
}


uint32_t fs_sd_get_block_len ()
{
    if (sd) {
        return sd->read_bl_len;
    }

    return 0;
}


uint64_t fs_sd_get_device_size ()
{
    if (sd) {
        return sd->device_size;
    }

    return 0;
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/


