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
#include <cstdlib>
#include <cstring>
#include "Serialization.h"
#include "LCDriverMethods.h"
#include "command_ids.h"

ListDevice_t Devices[MAX_NO_OF_DEVICES];
uint32 DevicesNumber = 0;

Serialization::Serialization(): logger_(0)
{
}

Serialization::~Serialization()
{
}

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void Serialization::get_block(const void **data_pp, void *target_p, uint32 length)
{
    memcpy(target_p, *data_pp, length);
    *(const uint8 **)data_pp += length;
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tget_block\tLength=%i", (void *)length);
#endif
}

SupportedCommand_t *Serialization::get_supportedcommands(const void **data_pp, uint32 CommandsCount)
{
    SupportedCommand_t *SupportedCommands = new SupportedCommand_t[CommandsCount];

    for (uint32 i = 0; i < CommandsCount; i++) {
        SupportedCommands[i].Group = get_uint32_le(data_pp);
        SupportedCommands[i].Command = get_uint32_le(data_pp);
        SupportedCommands[i].Permitted = get_uint32_le(data_pp);
    }

    return SupportedCommands;
}

ListDevice_t *Serialization::get_devices(const void **data_pp, uint32 DevicesCount)
{
    size_t size = 0;
    const char *str;

    DevicesNumber = DevicesCount;

    for (uint32 i = 0; i < DevicesCount; i++) {
        str = skip_str(data_pp);
        size = get_uint32_le((const void **)&str);
        memset(Devices[i].Path_p, 0, 256);
        memcpy(Devices[i].Path_p, str, size);
        Devices[i].Path_p[size] = '\0';

        str = skip_str(data_pp);
        size = get_uint32_le((const void **)&str);
        memset(Devices[i].Type_p, 0, 256);
        memcpy(Devices[i].Type_p, str, size);
        Devices[i].Type_p[size] = '\0';

        Devices[i].BlockSize = get_uint64_le(data_pp);
        Devices[i].Start = get_uint64_le(data_pp);
        Devices[i].Length = get_uint64_le(data_pp);
    }

    return Devices;
}

DirEntry_t *Serialization::get_direntries(const void **data_pp, uint32 DirectoriesCount)
{
    DirEntry_t *Direntries = new DirEntry_t[DirectoriesCount];
    size_t size = 0;
    const char *str;

    for (uint32 i = 0; i < DirectoriesCount; i++) {
        str = skip_str(data_pp);
        size = get_uint32_le((const void **)&str);
        Direntries[i].Name_p = new char[size + 1];
        memcpy(Direntries[i].Name_p, str, size);
        Direntries[i].Name_p[size] = '\0';

        Direntries[i].Size = get_uint64_le(data_pp);
        Direntries[i].Mode = get_uint32_le(data_pp);
        Direntries[i].Time = get_uint32_le(data_pp);
    }

    return Direntries;
}

uint8 Serialization::get_uint8(const void **data_pp)
{
    const uint8 *d = *(const uint8 **)data_pp;
    *(const uint8 **)data_pp += sizeof(uint8);
#ifdef _SERIALIZATIONDEBUG
    unsigned char v = d[0];
    PrintF("Serialization\tget_uint8\tReturned val=0x%x", (void *)v);
#endif
    return d[0];
}
uint16 Serialization::get_uint16_le(const void **data_pp)
{
    const uint8 *d = *(const uint8 **)data_pp;
    *(const uint8 **)data_pp += sizeof(uint16);
#ifdef _SERIALIZATIONDEBUG
    unsigned short v = (d[1] << 8) | d[0];
    PrintF("Serialization\tget_uint16_le\tReturned val=0x%x", (void *)v);
#endif
    return (d[1] << 8) | d[0];
}
uint16 Serialization::get_uint16_be(const void **data_pp)
{
    const uint8 *d = *(const uint8 **)data_pp;
    *(const uint8 **)data_pp += sizeof(uint16);
#ifdef _SERIALIZATIONDEBUG
    unsigned short v = (d[0] << 8) | d[1];
    PrintF("Serialization\tget_uint16_be\tReturned val=0x%x", (void *)v);
#endif
    return (d[0] << 8) | d[1];
}
uint32 Serialization::get_uint32_le(const void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    *(const uint8 **)data_pp += sizeof(uint32);
#ifdef _SERIALIZATIONDEBUG
    unsigned int v = ((d[3] << 8 | d[2]) << 8 | d[1]) << 8 | d[0];
    PrintF("Serialization\tget_uint32_le\tReturned val=0x%x", (void *)v);
#endif
    return ((d[3] << 8 | d[2]) << 8 | d[1]) << 8 | d[0];
}

uint64 Serialization::get_uint64_le(const void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    *(const uint8 **)data_pp += sizeof(uint64);
#ifdef _SERIALIZATIONDEBUG
    uint64 v = (((((((uint64)d[7] << 8 | d[6]) << 8 | d[5]) << 8 | d[4]) << 8 | d[3]) << 8 | d[2]) << 8 | d[1]) << 8 | d[0];
    PrintF("Serialization\tget_uint64_le\tReturned val=0x%x", (void *)v);
#endif
    return (((((((uint64)d[7] << 8 | d[6]) << 8 | d[5]) << 8 | d[4]) << 8 | d[3]) << 8 | d[2]) << 8 | d[1]) << 8 | d[0];
}

uint32 Serialization::get_uint32_be(const void **data_pp)
{
    const uint8 *d = *(const uint8 **) data_pp;
    *(const uint8 **)data_pp += sizeof(uint32);
#ifdef _SERIALIZATIONDEBUG
    unsigned int v = ((d[0] << 8 | d[1]) << 8 | d[2]) << 8 | d[3];
    PrintF("Serialization\tget_uint32_be\tReturned val=0x%x", (void *)v);
#endif
    return ((d[0] << 8 | d[1]) << 8 | d[2]) << 8 | d[3];
}

void Serialization::put_block(void **data_pp, const void *source_p, uint32 length)
{
    if (length > 0) {
        memcpy(*data_pp, source_p, length);
        *(const uint8 **)data_pp += length;
    }

#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_block\tLength=%i", (void *)length);
#endif
}

void Serialization::put_string(void **data_pp, const void *source_p, uint32 length)
{
    if (length > 0) {
        memcpy(*data_pp, &length, sizeof(uint32));
        *(const uint8 **)data_pp += sizeof(uint32);
        memcpy(*data_pp, source_p, length);
        *(const uint8 **)data_pp += length;
    }
}

void Serialization::put_uint8(void **data_pp, uint8 v)
{
    **(uint8 **) data_pp = v;
    *(uint8 **) data_pp += sizeof(uint8);
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint8\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::put_uint16_le(void **data_pp, uint16 v)
{
    uint8 *d = *(uint8 **) data_pp;
    d[0] = v & 0xff;
    d[1] = v >> 8;
    *(uint8 **)data_pp += sizeof(uint16);
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint16_le\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::put_uint16_be(void **data_pp, uint16 v)
{
    uint8 *d = *(uint8 **) data_pp;
    d[1] = v & 0xff;
    d[0] = v >> 8;
    *(uint8 **)data_pp += sizeof(uint16);
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint16_be\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::put_uint32_le(void **data_pp, uint32 v)
{
    uint8 *d = *(uint8 **) data_pp;
    uint8 *v_p = (uint8 *)&v;
    d[0] = v_p[0];
    d[1] = v_p[1];
    d[2] = v_p[2];
    d[3] = v_p[3];
    *(uint8 **)data_pp += sizeof(uint32);
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint32_le\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::put_uint32_be(void **data_pp, uint32 v)
{
    uint8 *d = *(uint8 **) data_pp;
    uint8 *v_p = (uint8 *)&v;
    d[3] = v_p[0];
    d[2] = v_p[1];
    d[1] = v_p[2];
    d[0] = v_p[3];
    *(uint8 **)data_pp += sizeof(uint32);
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint32_be\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::put_uint64_le(void **data_pp, uint64 v)
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
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint64_le\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::put_uint64_be(void **data_pp, uint64 v)
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
#ifdef _SERIALIZATIONDEBUG
    PrintF("Serialization\tput_uint64_be\tInput val=0x%x", (void *)v);
#endif
}

void Serialization::skip_uint8(const void **data_pp)
{
    *(const uint8 **)data_pp += sizeof(uint8);
}

void Serialization::skip_uint16(const void **data_pp)
{
    *(const uint8 **)data_pp += sizeof(uint16);
}

void Serialization::skip_uint32(const void **data_pp)
{
    *(const uint8 **)data_pp += sizeof(uint32);
}

void Serialization::skip_block(const void **data_pp, uint32 length)
{
    *(const uint8 **)data_pp += length;
}

char *Serialization::skip_str(const void **data_pp)
{
    char *Result_p = NULL;
    uint32 StrLength;

    Result_p = *(char **)data_pp;
    StrLength = get_uint32_le(data_pp);
    *(char **)data_pp += StrLength;
    return Result_p;
}

void Serialization::PrintF(const char *text, void *pVoid)
{
    if (NULL != logger_) {
        logger_->log(text, pVoid);
    }
}
