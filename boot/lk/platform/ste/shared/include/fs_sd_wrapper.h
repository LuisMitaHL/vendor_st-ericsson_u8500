#ifndef _FS_SD_WRAPPER_H
#define _FS_SD_WRAPPER_H
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
 * Init Sd-card
 *
 * Returns 0 if successful, 1 if failed
 */
uint32_t fs_sd_init (void);

/*
 * Mount Sd-card
 *
 * Returns 0 if successful, 1 if failed
 */
uint32_t fs_sd_mount (void);


/*
 * Unmount Sd-card
 *
 * Returns 0 if successful, 1 if failed
 */
uint32_t fs_sd_unmount (void);


/*
 * Read n_sectors from offset start_sector to buffer
 *
 * Returns 0 if successful, 1 if failed
 */
uint32_t fs_sd_read_sectors (uint32_t start_sector,
                             uint32_t n_sectors,
                             void *buffer);


/*
 * Write n_sectors to offset start_sector from buffer
 *
 * Returns 0 if successful, 1 if failed
 */
uint32_t fs_sd_write_sectors (uint32_t start_sector,
                              uint32_t n_sectors,
                              void *buffer);


/*
 * Retreive size in bytes of device sector.
 *
 * Returns 0 if failed
 */
uint32_t fs_sd_get_block_len (void);


/*
 * Retreive total size of device
 */
uint64_t fs_sd_get_device_size (void);


#endif  /* _FS_SD_WRAPPER_H */
