/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef XFILEDECODER_H
#define XFILEDECODER_H

#include <stdio.h>

#define CORTEX_L2 1
#define CORTEX_L1 2

#define X_FILE_START_ADDR 0xC2080
#define FILE_INFO_SIZE 22L
#define DLL_INFO 4L

#define LINE_LENGTH 150

/*
 * generate_dump_info
 *
 * Generate readable dump information from binary X-file (modem_shared).
 * Output file will be created next to core dump.
 * input  : Infile path and Outfile path
 * output : 0 if success
 *          Any other value on error
 */
int generate_dump_info(char *infile, char *outfile);

int generate_xfile(char *modem_shared_path, char *xfile_path);

#endif
