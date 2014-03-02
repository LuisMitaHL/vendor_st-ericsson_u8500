/*****************************************************************************/
/**
 * RPMB Driver
 *
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Provides methods and return values for
 * accessing the rpmb partition on the mmc devices.
 */
/*****************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "r_debug.h"
#include "r_memory_utils.h"
#include "mmc_p.h"
#include "rpmb.h"
#include "flash_manager.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
const char device0[] = "/flash0/";
const char device1[] = "/flash1/";

/****************************************************************************/
/*       NAME : rpmb_get_num_devices                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Returns the number of available flash devices.              */
/*                                                                          */
/* PARAMETERS :                                                             */
/*       OUT : size_t max_path_len:Maximum length of path found for flash   */
/*                                 partitions (includes space for NUL       */
/*                                 terminator)                              */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_get_num_devices(size_t *max_path_len)
{
    uint8 available_flash_devices = Do_FPD_GetNrOfAvailableFlashDevices();
    *max_path_len = (size_t)(strlen(device0) + 1);
    return (int)available_flash_devices;
}

/****************************************************************************/
/*       NAME : rpmb_get_devices                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Get the paths for the supported by RPMB partitions.         */
/*                                                                          */
/* PARAMETERS :                                                             */
/*       OUT : char devices:Paths supported by RPMB partitions              */
/*        IN : size_t num_devices:Number of RPMB partitions.                */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_get_devices(char *devices[], size_t num_devices)
{
    int ret = 0;

    if (devices == NULL || num_devices == 0) {
        A_(printf("emmc_rpmb_cops_wrap.c (%d): Invalid input params\n", __LINE__);)
        goto ErrorExit;
    }

    strncpy(devices[0], device0, (strlen(device0) + 1));

    if (2 == num_devices) {
        strncpy(devices[1], device1, strlen(device1) + 1);
    } else if (num_devices > 2) {
        goto ErrorExit;
    }

    return 0;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_open                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: No functionality in OS_FREE.                                */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : char device:Path of the device.                              */
/*                                                                          */
/*    RETURN : int:File handle is returned.                                 */
/****************************************************************************/
int rpmb_open(const char *device)
{
    FLASH_DEVICE_t FlashDevice;
    int len = strlen(device0);

    if (device == NULL) {
        A_(printf("emmc_rpmb_cops_wrap.c (%d): Asked to open device with NULL name\n", __LINE__);)
        goto ErrorExit;
    }

    if (0 == strncmp(device, device0, len)) {
        FlashDevice = FLASH_0;
    } else if (0 == strncmp(device, device1, len)) {
        FlashDevice = FLASH_1;
    } else {
        A_(printf("emmc_rpmb_cops_wrap.c (%d): Unknown device\n", __LINE__);)
        goto ErrorExit;
    }

    return FlashDevice;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_close                                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Dummy function.                                             */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : int dev:Represents which flash device to be operated on.     */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_close(int dev)
{
    return 0;
}

/****************************************************************************/
/*       NAME : rpmb_pre_cmds                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Function call which is required before each RPMB operation. */
/*              Configures the MMC registers so that the device is ready to */
/*              access the RPMB partition.                                  */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : int dev:Represents which flash device to be operated on.     */
/*       OUT : uint32_t part:                                               */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_pre_cmds(int dev, uint32_t *part)
{
    t_emmc_error error = { 0 };
    error.mmc_error = MMC_OK;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig = {0,};

    ReturnValue = Do_FPD_GetInfo(dev, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    error = MMC_SetdeviceRPMBMode(1, MMC_ENABLE, FPDConfig.DeviceId);

    if (MMC_OK != error.mmc_error) {
        goto ErrorExit;
    }

    return 0;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_post_cmds                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Function call which is required after each RPMB operation.  */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : int dev:Represents which flash device to be operated on.     */
/*        IN : uint32_t part:                                               */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_post_cmds(int dev, uint32_t part)
{
    t_emmc_error error = { 0 };
    error.mmc_error = MMC_OK;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig = {0,};

    ReturnValue = Do_FPD_GetInfo(dev, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    error = MMC_SetdeviceRPMBMode(1, MMC_DISABLE, FPDConfig.DeviceId);

    if (MMC_OK != error.mmc_error) {
        goto ErrorExit;
    }

    return 0;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_write                                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Writes 1, or more, RPMB data frames to the RPMB partition.  */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : int dev:Represents which flash device to be operated on.     */
/*     INOUT : uint8_t data_frames:Data frames.                             */
/*        IN : size_t num_frames:Number of frames to be written.            */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_write(int dev, uint8_t *data_frames[], size_t num_frames)
{
    t_emmc_error error = { 0 };
    error.mmc_error = MMC_OK;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig = {0,};

    ReturnValue = Do_FPD_GetInfo(dev, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    B_( {
        printf("emmc_rpmb_cops_wrap.c (%d):rpmb_write() **data_frames buffer = \n", __LINE__);

        for (j = 0; j < num_frames; j++) {
            for (i = 0; i < 512; i++) {
                printf("%02X ", (*data_frames)[i + (512 * j)]);

                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }

            printf("\n");
        }
    })

    error = MMC_RPMB_Raw_Write(1, *data_frames,
                               (t_uint32) num_frames, FPDConfig.DeviceId);

    if (MMC_OK != error.mmc_error) {
        goto ErrorExit;
    }

    return 0;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_read                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Reads 1, or more, RPMB data frames to the RPMB partition.   */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : int dev:Represents which flash device to be operated on.     */
/*     INOUT : uint8_t data_frames:Data frames.                             */
/*        IN : size_t num_frames:Number of frames to be read.               */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_read(int dev, uint8_t *data_frames[], size_t num_frames)
{
    t_emmc_error error = { 0 };
    error.mmc_error = MMC_OK;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig = {0,};
    int i, j;

    ReturnValue = Do_FPD_GetInfo(dev, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    error = MMC_RPMB_Raw_Read(1, *data_frames,
                              (t_uint32) num_frames, FPDConfig.DeviceId);

    B_( {
        printf("emmc_rpmb_cops_wrap.c (%d):rpmb_read() **data_frames buffer = \n", __LINE__);

        for (j = 0; j < num_frames; j++) {
            for (i = 0; i < 512; i++) {
                printf("%02X ", (*data_frames)[i + (512 * j)]);

                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }

            printf("\n");
        }
    })

    if (MMC_OK != error.mmc_error) {
        goto ErrorExit;
    }

    return 0;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_get_ext_csd_params                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Returns some EXT CSD parameters associated with the MMC     */
/*              device being.                                               */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : int dev:Represents which flash device to be operated on.     */
/*       OUT : rpmb_ext_csd params:Params will be updated with some         */
/*                                 EXT CSD parameters.                      */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_get_ext_csd_params(int dev, struct rpmb_ext_csd *params)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig = {0,};

    if (params == NULL) {
        goto ErrorExit;
    }

    ReturnValue = Do_FPD_GetInfo(dev, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    params->rel_wr_sec_c = FPDConfig.RelWrSecC;
    params->rpmb_size_mult = FPDConfig.RpmbSizeMult;

    return 0;
ErrorExit:
    return -1;
}

/****************************************************************************/
/*       NAME : rpmb_get_cid                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Returns the CID of the MMC device.                          */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : char device:Path of the device.                              */
/*       OUT : uint8_t cid:Field containing CID data.                       */
/*        IN : uint32_t len:Length of cid field.                            */
/*                                                                          */
/*    RETURN : int                                                          */
/****************************************************************************/
int rpmb_get_cid(char *device, uint8_t *cid, const uint32_t len)
{
    int dev = 0;
    t_mmc_error error = MMC_OK;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig = {0,};

    if (EMMC_CID_SIZE > len) {
        A_(printf("emmc_rpmb_cops_wrap.c (%d): Length of data field is not big enough\n", __LINE__);)
        goto ErrorExit;
    }

    dev = rpmb_open(device);

    if (-1 == dev) {
        goto ErrorExit;
    }

    ReturnValue = Do_FPD_GetInfo(dev, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    error = MMC_getCID(1, cid, len, FPDConfig.DeviceId);

    if (MMC_OK != error) {
        goto ErrorExit;
    }

    return 0;
ErrorExit:
    return -1;
}
