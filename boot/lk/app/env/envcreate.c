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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crc32.h"


/*******************************************************************************
 * Types, constants
 ******************************************************************************/


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

static uint8_t *read_input(char *name);
static uint8_t *cr_lf_to_zero (unsigned char *raw_data, int len, int *out_len);

/*
 * Input boot env ASCII format:
 * <variable1>=<value><LF>
 * <variable2>=<value><LF>
 * ...
 * <variableN>=<value><LF>
 *
 *
 * Output boot env binary format:
 * [crc32][<variable1>=<value>]['\0'][<variable2>=<value>]['\0']...
 * [<variableN>=<value>]['\0']['\0']
 *
 * crc32 is calculated on entire bin file, excluding crc data.
 * crc32 is 4 bytes.
 */


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

int main (int argc, char **argv)
{
    FILE      *outfile;
    uint8_t   *raw_data;
    uint8_t   *data;
    uint32_t  crc;
    int       data_len;

    if (argc < 3) {
        printf("Syntax: envcreate <infile> <outfile>\n");
        return 1;
    }

    printf ("envconfig: Creating %s from %s\n", argv[2], argv[1]);

    /* Read raw data from input file */
    raw_data = read_input (argv[1]);
    if (!raw_data) {
        printf ("Failed to read input file %s\n", argv[1]);
        return 1;
    }

    /* Convert CR/LF pairs to zeroes, i.e. bootenv format
     * data will reside at data[sizeof(uint32_t)],
     * leaving room for CRC at data[0]
     * data_len will be length of data excluding CRC */
    data = cr_lf_to_zero (raw_data, strlen(raw_data), &data_len);
    free (raw_data); /* Don't need this anymore */

    printf ("len after stripping cr/lf 0x%08x (%d) bytes\n", data_len, data_len);

    if (!data) {
        printf ("Failed to convert CR/LF\n");
        return 1;
    }

    init_crc32();
    crc = calc_crc32((void*)&data[sizeof(uint32_t)], data_len);
    memcpy (data, &crc, sizeof(uint32_t));

    /* Finally write output file */
    outfile = fopen (argv[2], "wb");
    if (!outfile) {
        printf("Failed to create output file %s\n",  argv[2]);
        free (data);
        return 1;
    }

    fwrite ((void*)data, sizeof(uint32_t) + data_len, 1, outfile);
    fclose(outfile);
    free (data);

    printf ("envconfig: Done\n");

    return 0;
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

static uint8_t *read_input(char *name)
{
    FILE        *infile;
    uint8_t     *data = NULL;
    uint32_t    len;

    infile = fopen (name, "rb");
    if (!infile) {
        printf ("Failed to open infile %s\n", name);
        goto ErrorExit;
    }

    /* Determine input file length */
    fseek (infile, 0, SEEK_END);
    len = ftell (infile);

    /* Allocate buffer. Make room for terminating 0 */
    data = (uint8_t*)malloc (len + 1);
    if (!data) {
        printf ("Failed to allocate data for input\n");
        goto ErrorExit;
    }
    memset (data, 0, len + 1);

    /* Reset file pointer */
    fseek (infile, 0, SEEK_SET);

    /* Read file */
    fread (data, len, 1, infile);

ErrorExit:
    if (infile)
        fclose (infile);

    return data;
}


static uint8_t *cr_lf_to_zero (unsigned char *raw_data, int len, int *out_len)
{
    uint8_t *data;
    int     rd, wr;

    *out_len = 0;

    /* Allocate output data, make room for CRC and terminating 0 */
    data = malloc (len + sizeof(uint32_t) + 1);
    if (!data) {
        printf ("Failed to allocate for CR/LF stripped string\n");
        return NULL;
    }

    /* Replace 0x0a with zero and skip any 0x0d
     * This makes function work with both Unix and DOS files
     * Leave room for CRC first in output
     */
    for (rd = 0, wr = sizeof(uint32_t); rd < len; rd++) {
        if (raw_data[rd] == 0x0a) {
            data[wr] = 0;
            wr++;
        } else if (raw_data[rd] != 0x0d) {
            data[wr] = raw_data[rd];
            wr++;
        }
    }
    data[wr] = 0; /* Double 0 terminated */
    wr++;

    /* Length of final string, excluding CRC */
    *out_len = wr - sizeof(uint32_t);

    return data;
}
