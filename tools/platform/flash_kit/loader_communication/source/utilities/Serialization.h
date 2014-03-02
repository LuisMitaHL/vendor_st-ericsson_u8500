/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include "t_basicdefinitions.h"
#include "command_ids.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#endif
#include "Logger.h"
#include "LCDriver.h"
#include "commands_types.h"

const uint32 MAX_NO_OF_DEVICES = 1024;

class Serialization
{
public:
    Serialization(void);
    ~Serialization(void);

    static void put_block(void **data_pp, const void *source_p, uint32 length);
    static void put_string(void **data_pp, const void *source_p, uint32 length);

    static uint8 get_uint8(const void **data_pp);
    static uint16 get_uint16_le(const void **data_pp);
    static uint16 get_uint16_be(const void **data_pp);
    static uint32 get_uint32_le(const void **data_pp);
    static uint32 get_uint32_be(const void **data_pp);
    static uint64 get_uint64_le(const void **data_pp);
    static void put_uint8(void **data_pp, uint8 v);
    static void put_uint16_le(void **data_pp, uint16 v);
    static void put_uint16_be(void **data_pp, uint16 v);
    static void put_uint32_le(void **data_pp, uint32 v);
    static void put_uint32_be(void **data_pp, uint32 v);
    static void put_uint64_le(void **data_pp, uint64 v);
    static void put_uint64_be(void **data_pp, uint64 v);
    static void skip_uint8(const void **data_pp);
    static void skip_uint16(const void **data_pp);
    static void skip_uint32(const void **data_pp);
    static void get_block(const void **data_pp, void *target_p, uint32 length);
    static void skip_block(const void **data_pp, uint32 length);
    static char *skip_str(const void **data_pp);

    static SupportedCommand_t *get_supportedcommands(const void **data_pp, uint32 CommandsCount);
    static ListDevice_t *get_devices(const void **data_pp, uint32 DevicesCount);
    static DirEntry_t *get_direntries(const void **data_pp, uint32 DirectoriesCount);

public:
    void SetLogger(Logger *logger) {
        logger_ = logger;
    }
private:
    Logger *logger_;
    void PrintF(const char *text, void *pVoid);
};

#endif // _SERIALIZATION_H_
