/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _COMMANDS_TYPES_H_
#define _COMMANDS_TYPES_H_

#include "t_basicdefinitions.h"
#include "LCDriver.h"
#include <string>

/// <summary>
/// Description of ME device.
/// </summary>
/// <param name="szPath">Absolute device path pointing at the described device.</param>
/// <param name="iPathSize">Size of pchPath.</param>
/// <param name="szType">Type of device.</param>
/// <param name="iTypeSize">Size of pchType.</param>
/// <param name="uiBlockSize">Size of the smallest addressable unit in the device [Byte].</param>
/// <param name="uiStart">Offset [Byte] of the start of the device relative its parent's
/// offset 0 with granularity of its parent's block size.</param>
/// <param name="uiLength">Length of the device [Byte].</param>
struct TDevicesInternal {
    std::string Path;
    std::string Type;
    uint64 uiBlockSize;
    uint64 uiStart;
    uint64 uiLength;

    operator TDevices() const {
        TDevices tDevice = {
            Path.c_str(),
            Path.length(),
            Type.c_str(),
            Type.length(),
            uiBlockSize,
            uiStart,
            uiLength
        };

        return tDevice;
    }
};

struct TEntriesInternal {
    std::string Name;
    uint64 uiSize;
    int iMode;
    int iTime;

    operator TEntries() const {
        TEntries tEntry = {
            Name.c_str(),
            Name.length(),
            uiSize,
            iMode,
            iTime
        };

        return tEntry;
    }
};

#endif // _COMMANDS_TYPES_H_
