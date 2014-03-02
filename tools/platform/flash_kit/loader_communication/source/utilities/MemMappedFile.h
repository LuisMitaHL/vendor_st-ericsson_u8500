/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _MEMMAPPEDFILE_H_
#define _MEMMAPPEDFILE_H_

#include "t_basicdefinitions.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

class MemMappedFile
{
public:
    MemMappedFile(uint32 alignmentLength = 512);
    ~MemMappedFile();

    uint8 *AllocateFileData(uint64 offset, uint64 size);
    void ReleaseFileData(uint8 *data, uint64 offset, uint64 size);
    uint64 GetFileSize();
    int LoadFileData(const char *path);
    int GetError();
private:
    std::string path_;
    uint64 size_;
    bool isMapped_;
    uint8 *mappedData_;
    int error_;
    uint32 alignmentLength_;
#ifdef _WIN32
    HANDLE handle_;
    HANDLE memmap_;
#else
    int descriptor_;
#endif
};

#endif // _MEMMAPPEDFILE_H_
