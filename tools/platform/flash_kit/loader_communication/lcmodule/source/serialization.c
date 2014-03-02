/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup serialization
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_serialization.h"
#include <string.h>
#include <stdlib.h>
#include "r_debug_macro.h"
/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void get_block(const void **data_pp, void *target_p, uint32 length)
{
    memcpy(target_p, *data_pp, length);
    *(const uint8 **)data_pp += length;
}

uint8 get_uint8(void **data_pp)
{
    const uint8 *d = *(const uint8 **)data_pp;
    *(const uint8 **)data_pp += sizeof(uint8);
    return d[0];
}

uint16 get_uint16_le(void **data_pp)
{
    const uint8 *d = *(const uint8 **)data_pp;
    *(const uint8 **)data_pp += sizeof(uint16);
    return (d[1] << 8) | d[0];
}

uint16 get_uint16_be(void **data_pp)
{
    const uint8 *d = *(const uint8 **)data_pp;
    *(const uint8 **)data_pp += sizeof(uint16);
    return (d[0] << 8) | d[1];
}

uint32 get_uint32_le(void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    *(const uint8 **)data_pp += sizeof(uint32);
    return ((d[3] << 8 | d[2]) << 8 | d[1]) << 8 | d[0];
}

uint64 get_uint64_le(void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    *(const uint8 **)data_pp += sizeof(uint64);
    return (((((((uint64)d[7] << 8 | d[6]) << 8 | d[5]) << 8 | d[4]) << 8 | d[3]) << 8 | d[2]) << 8 | d[1]) << 8 | d[0];
}

uint32 get_uint32_string_le(void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    //*(const uint8 **)data_pp += sizeof(uint32);
    return ((d[3] << 8 | d[2]) << 8 | d[1]) << 8 | (d[0] + sizeof(uint32));
}

uint32 get_uint32_be(void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    *(const uint8 **)data_pp += sizeof(uint32);
    return ((d[0] << 8 | d[1]) << 8 | d[2]) << 8 | d[3];
}

void put_block(void **data_pp, const void *source_p, uint32 length)
{
    if (length > 0) {
        memcpy(*data_pp, source_p, length);
        *(const uint8 **)data_pp += length;
    }
}

void put_string(void **data_pp, const void *source_p, uint32 length)
{
    if (length > 0) {
        memcpy(*data_pp, &length, sizeof(uint32));
        *(const uint8 **)data_pp += sizeof(uint32);
        memcpy(*data_pp, source_p, length);
        *(const uint8 **)data_pp += length;
    }
}

void put_uint8(void **data_pp, uint8 v)
{
    **(uint8 **) data_pp = v;
    *(uint8 **) data_pp += sizeof(uint8);
}

void put_uint16_le(void **data_pp, uint16 v)
{
    uint8 *d = *(uint8 **) data_pp;
    d[0] = v & 0xff;
    d[1] = v >> 8;
    *(uint8 **)data_pp += sizeof(uint16);
}

void put_uint16_be(void **data_pp, uint16 v)
{
    uint8 *d = *(uint8 **) data_pp;
    d[1] = v & 0xff;
    d[0] = v >> 8;
    *(uint8 **)data_pp += sizeof(uint16);
}

void put_uint32_le(void **data_pp, uint32 v)
{
    uint8 *d = *(uint8 **) data_pp;
    uint8 *v_p = (uint8 *)&v;
    d[0] = v_p[0];
    d[1] = v_p[1];
    d[2] = v_p[2];
    d[3] = v_p[3];
    *(uint8 **)data_pp += sizeof(uint32);
}

void put_uint32_be(void **data_pp, uint32 v)
{
    uint8 *d = *(uint8 **) data_pp;
    uint8 *v_p = (uint8 *)&v;
    d[3] = v_p[0];
    d[2] = v_p[1];
    d[1] = v_p[2];
    d[0] = v_p[3];
    *(uint8 **)data_pp += sizeof(uint32);
}

void put_uint64_le(void **data_pp, uint64 v)
{
    uint8 *d = *(uint8 **) data_pp;
    uint8 *v_p = (uint8 *)&v;
    d[0] = v_p[0];
    d[1] = v_p[1];
    d[2] = v_p[2];
    d[3] = v_p[3];
    d[4] = v_p[4];
    d[5] = v_p[5];
    d[6] = v_p[6];
    d[7] = v_p[7];
    *(uint8 **)data_pp += sizeof(uint32);
    *(uint8 **)data_pp += sizeof(uint32);
}

void put_uint64_be(void **data_pp, uint64 v)
{
    uint8 *d = *(uint8 **) data_pp;
    uint8 *v_p = (uint8 *)&v;
    d[7] = v_p[0];
    d[6] = v_p[1];
    d[5] = v_p[2];
    d[4] = v_p[3];
    d[3] = v_p[4];
    d[2] = v_p[5];
    d[1] = v_p[6];
    d[0] = v_p[7];
    *(uint8 **)data_pp += sizeof(uint32);
    *(uint8 **)data_pp += sizeof(uint32);
}

void skip_uint8(void **data_pp)
{
    *(const uint8 **)data_pp += sizeof(uint8);
}

void skip_uint16(void **data_pp)
{
    *(const uint8 **)data_pp += sizeof(uint16);
}

void skip_uint32(void **data_pp)
{
    *(const uint8 **)data_pp += sizeof(uint32);
}

void skip_block(void **data_pp, uint32 length)
{
    *(const uint8 **)data_pp += length;
}

char *skip_str(void **data_pp)
{
    char *Result_p = NULL;
    uint32 StrLength;

    ASSERT(NULL != data_pp);
    Result_p = *(char **)data_pp;
    ASSERT(NULL != Result_p);
    StrLength = get_uint32_le(data_pp);
    *(char **)data_pp += StrLength;
    return Result_p;
}

char *get_strdup(void *String_p)
{
    char *Result_p;
    uint32 StrLength = strlen((char *)String_p) + 1;

    Result_p = (char *)malloc(StrLength);
    ASSERT(Result_p != NULL);

    strncpy(Result_p, (char *)String_p, StrLength);
    Result_p[StrLength - 1] = 0;
    return Result_p;
}

void insert_string(char **data_pp, const char *source_p, uint32 length)
{
    *data_pp = NULL;

    *data_pp = (char *)malloc(sizeof(uint32) + length);
    ASSERT(NULL != *data_pp);

    memcpy(*data_pp, &length, sizeof(uint32));
    memcpy(*data_pp + sizeof(uint32), source_p, length);
}

#if defined(CFG_ENABLE_LOADER_SERIALIZATION)
uint32 get_directory_entries_len(const DirEntry_t *source_p, uint32 DirectoryEntriesCount)
{
    uint32 EntriesLen = 0;
    uint32 i = 0;

    for (i = 0; i < DirectoryEntriesCount; i++) {
        EntriesLen += get_uint32_string_le((void **)&source_p[i].Name_p);
        EntriesLen += sizeof(uint32) + sizeof(uint32) + sizeof(uint64);
    }

    return EntriesLen;
}

void serialize_directory_entries(void **data_pp, const DirEntry_t *source_p, uint32 DirectoryEntriesCount)
{
    uint32 NameLength = 0;
    uint32 i = 0;

    for (i = 0; i < DirectoryEntriesCount; i++) {
        NameLength = get_uint32_string_le((void **)&source_p[i].Name_p);
        memcpy(*data_pp, source_p[i].Name_p, NameLength);
        *(const uint8 **)data_pp += NameLength;
        memcpy(*data_pp, &source_p[i].Size, sizeof(uint64));
        *(const uint8 **)data_pp += sizeof(uint64);
        memcpy(*data_pp, &source_p[i].Mode, sizeof(uint32));
        *(const uint8 **)data_pp += sizeof(uint32);
        memcpy(*data_pp, &source_p[i].Time, sizeof(uint32));
        *(const uint8 **)data_pp += sizeof(uint32);
    }
}

uint32 get_device_entries_len(const ListDevice_t *source_p, uint32 DeviceEntriesCount)
{
    uint32 EntriesLen = 0;
    uint32 i = 0;

    for (i = 0; i < DeviceEntriesCount; i++) {
        if (NULL != source_p[i].Path_p) {
            EntriesLen += get_uint32_string_le((void **)&source_p[i].Path_p);
        } else {
            EntriesLen += sizeof(uint32);
        }

        if (NULL != source_p[i].Type_p) {
            EntriesLen += get_uint32_string_le((void **)&source_p[i].Type_p);
        } else {
            EntriesLen += sizeof(uint32);
        }

        EntriesLen += sizeof(uint64) + sizeof(uint64) + sizeof(uint64);
    }

    return EntriesLen;
}

void serialize_device_entries(void **data_pp, const ListDevice_t *source_p, uint32 DeviceEntriesCount)
{
    uint32 PathLen = 0;
    uint32 TypeLen = 0;
    uint32 i = 0;

    for (i = 0; i < DeviceEntriesCount; i++) {
        if (NULL != source_p[i].Path_p) {
            PathLen = get_uint32_string_le((void **)&source_p[i].Path_p);
            memcpy(*data_pp, source_p[i].Path_p, PathLen);
            *(const uint8 **)data_pp += PathLen;
        } else {
            memset(*data_pp, 0x00, sizeof(char *));
            *(const uint8 **)data_pp += sizeof(char *);
        }

        if (NULL != source_p[i].Type_p) {
            TypeLen = get_uint32_string_le((void **)&source_p[i].Type_p);
            memcpy(*data_pp, source_p[i].Type_p, TypeLen);
            *(const uint8 **)data_pp += TypeLen;
        } else {
            memset(*data_pp, 0x00, sizeof(char *));
            *(const uint8 **)data_pp += sizeof(char *);
        }

        memcpy(*data_pp, &source_p[i].BlockSize, sizeof(uint64));
        *(const uint8 **)data_pp += sizeof(uint64);
        memcpy(*data_pp, &source_p[i].Start, sizeof(uint64));
        *(const uint8 **)data_pp += sizeof(uint64);
        memcpy(*data_pp, &source_p[i].Length, sizeof(uint64));
        *(const uint8 **)data_pp += sizeof(uint64);
    }
}
#endif

/* @} */
/* @} */

