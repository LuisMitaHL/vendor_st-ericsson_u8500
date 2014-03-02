/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>

#include "memfile.h"


#define min(x, y) ((x) < (y) ? (x) : (y))

/**
 * @brief Opens the memory file pointed to by memfile.
 *
 * @param [in] memfile Memory file data.
 * @param [in] length  Size of the file data.
 *
 * @return Returns on success a pointer to a file stream, or NULL at failure.
 */
MFILE *memfopen(const char *memfile, unsigned int length)
{
    MFILE *stream = malloc(sizeof(*stream));
    if (stream) {
        stream->data = memfile;
        stream->length = length;
        stream->offset = 0;
    }
    return stream;
}

/**
 * @brief Closes the file stream.
 *
 * @param [in] stream Pointer to a file stream.
 */
void memfclose(MFILE *stream)
{
    free(stream);
}

/**
 * @brief Reads one line from the file stream.
 *
 * @param [in]  stream Pointer to a file stream.
 * @param [out] out    Output buffer.
 * @param [in]  size   Size of output buffer.
 *
 * @return Return the number of bytes read, or EOF at end of file.
 */
int memfreadline(MFILE *stream, char *out, size_t size)
{
    char *pos;
    const char *start = stream->data + stream->offset;
    int bytes_read = EOF;

    if ((NULL == stream) || (NULL == out) || (size == 0)) {
        return 0;
    }

    out[0] = '\0';

    if (stream->offset < (stream->length - 1)) {
        pos = strchr(start, '\n');
        if (pos) {
            size_t length = min(pos - start, (int)size - 1);
            memcpy(out, start, length);
            out[length] = '\0';
            bytes_read = length + 1;
            stream->offset += bytes_read;
        }
    }
    return bytes_read;
}

/**
 * @brief Resets the file stream.
 *
 * @param [in] stream Pointer to a file stream.
 */
void memfreset(MFILE *stream)
{
    if (NULL != stream) {
        stream->offset = 0;
    }
}

