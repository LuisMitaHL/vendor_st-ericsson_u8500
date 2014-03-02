/*
 * RPMB Driver
 *
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* Provides method definitions and return values for
 * accessing the rpmb partition on the mmc devices.
 */

#include <stdint.h>

struct rpmb_ext_csd {
    /* EXT CSD-slice 168 "RPMB Size" */
    uint8_t rpmb_size_mult;

    /* EXT CSD-slice 222 "Reliable Write Sector Count" */
    uint8_t rel_wr_sec_c;
};


/*
 * Returns the number of available RPMB block devices.
 * Also sets max_path_len to the maximum length of path found for RPMB
 * partitions (includes space for NUL terminator).
 *
 * If there is an error during the function call -1 will be returned
 * and the count will be invalid.
 *
 * The number of devices, and max path length is used by the caller to allocate
 * the necessary memory for the RPMB partition devices returned by
 * rpmb_get_devices().
 */
int rpmb_get_num_devices(size_t *max_path_len);


/*
 * Get the paths for the supported by RPMB partitions.
 * The caller must have pre-allocated memory for each of the RPMB partitions
 * available and rpmb_get_num_devices() may be used to determine both the
 * number of RPMB partitions and the maximum path length of them.
 *
 * If there is an error during the function call -1 will be returned.
 *
 * Example usage pseudo-code:
 * {
 *     int ret;
 *     size_t max_path_len;
 *     int found = rpmb_get_num_devices(&max_path_len);
 *
 *     // Assume that found = 2
 *     // Assume that max_path_len = 25
 *     if (found > 0)
 *     {
 *          char path1[25];
 *          char path2[25];
 *          char *devices[2] = {path1, path2};
 *
 *          ret = rpmb_get_devices(devices, 2);
 *     }
 * }
 */
int rpmb_get_devices(char *devices[], size_t num_devices);


/*
 * No functionality in OS_FREE.
 */
int rpmb_open(const char *device);


/*
 * No functionality in OS_FREE.
 */
int rpmb_close(int dev);


/*
 * Pair of function calls which are required before, and after each RPMB
 * operation. They configure the MMC registers so that
 * the device is ready to access the RPMB partition and temporarily "claim the
 * host" so that the RPMB operations can complete without interruption.
 *
 * rpmb_pre_cmds() must be called before sending any operations to the RPMB
 * partition.
 * rpmb_post_cmds() must be called after sending any operations to the RPMB
 * partition.
 *
 * Usage:
 * {
 *      uint32_t Partition;
 *      int Result;
 *
 *      Result = rpmb_pre_cmds(dev, &Partition);
 *      if (Result)
 *          return Result;
 *
 *      ... // Do rpmb_write() and rpmb_read() operations here
 *
 *      Result = rpmb_post_cmds(dev, Partition);
 *      return Result;
 * }
 */
int rpmb_pre_cmds(int dev, uint32_t *part);
int rpmb_post_cmds(int dev, uint32_t part);


/*
 * Writes 1, or more, RPMB data frames to the RPMB partition.
 * Each data frame must be as defined in JEDEC Standard JESD84-A441.
 * Each data frame, therefore, must be 512 bytes in length and the usage is as
 * follows (example has 2 frames but it doesn't have to be this number):
 * {
 *      int Result
 *      uint8_t Frame1[512];
 *      uint8_t Frame2[512];
 *      uint8_t *DataFrames[2] = {Frame1, Frame2};
 *      size_t NumFrames = sizeof DataFrames / sizeof DataFrames[0];
 *
 *      ...
 *      Result = rpmb_write(dev, DataFrames, NumFrames);
 * }
 *
 * The caller of rpmb_write() must have ensured that Frame1 and Frame2 have been
 * initialised so that they contain the data to be sent to the RPMB partition.
 *
 * If reliable writes are required, they are handled in MMC_RPMB_Raw_Write() function.
 *
 * returns 0 if write-to-MMC-device operation was successful.
 *
 * N.B. The write to the RPMB partition within the MMC device may still fail.
 * See JEDEC Standard JESD84-A441 for more information.
 */
int rpmb_write(int dev, uint8_t *data_frames[], size_t num_frames);


/*
 * Read 1, or more, RPMB data frames from the RPMB partition.
 * Each data frame will be populated as defined in JEDEC Standard JESD84-A441.
 * Each data frame, therefore, must be 512 bytes in length and the usage is as
 * follows (example has 2 frames but it doesn't have to be this number):
 * {
 *      int Result
 *      uint8_t Frame1[512];
 *      uint8_t Frame2[512];
 *      uint8_t *DataFrames[2] = {Frame1, Frame2};
 *      size_t NumFrames = sizeof DataFrames / sizeof DataFrames[0];
 *
 *      ...
 *      Result = rpmb_read(dev, DataFrames, NumFrames);
 * }
 *
 * If there has not been any serious problems the read operation will copy the
 * RPMB data frames, read from the RPMB partition, into Frame1 and Frame2.
 * The caller of rpmb_read() can unpack the data.
 *
 * returns 0 if read-to-MMC-device operation was successful.
 *
 * N.B. The read from the RPMB partition within the MMC device may return a data
 * frame which indicates that the operation, or previous operation, has failed.
 * See JEDEC Standard JESD84-A441 for more information.
 */
int rpmb_read(int dev, uint8_t *data_frames[], size_t num_frames);


/*
 * Extracts some EXT CSD parameters associated with the MMC device being
 * used.
 * These values may be required by the caller to ensure that data transfers
 * to / from the RPMB partition are ok.
 *
 * Usage:
 * {
 *      int Result
 *      rpmb_ext_csd Params;
 *
 *      ...
 *      Result = rpmb_get_ext_csd_params(dev, &Params);
 * }
 *
 * If Result is 0, Params will have been updated some EXT CSD parameters.
 *
 * This user-space drive sends the data to the kernel driver using IOCTL.
 *
 * returns 0 if operation was successful.
 */
int rpmb_get_ext_csd_params(int dev, struct rpmb_ext_csd *params);

/*
 * Returns the CID (Device IDentification) information of the mmc device.
 */
int rpmb_get_cid(char *device, uint8_t *cid, const uint32_t len);
