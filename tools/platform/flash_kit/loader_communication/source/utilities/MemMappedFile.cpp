/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#if defined(_WIN32)
#define WINVER 0x0500
#include <windows.h>
#elif (defined(__linux__) || defined(__APPLE__))
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
using namespace std;
#else
#error "Unknown target"
#endif

#include "lcdriver_error_codes.h"
#include "MemMappedFile.h"

#ifdef __APPLE__
#define lseek64 lseek
#endif

MemMappedFile::MemMappedFile(uint32 alignmentLength):
    size_(0),
    isMapped_(false),
    mappedData_(0),
    error_(0),
    alignmentLength_(alignmentLength),
#ifdef _WIN32
    handle_(INVALID_HANDLE_VALUE),
    memmap_(INVALID_HANDLE_VALUE)
#else
    descriptor_(-1)
#endif
{
}

MemMappedFile::~MemMappedFile()
{
#ifdef _WIN32

    if (0 != mappedData_) {
        UnmapViewOfFile(mappedData_);
    }

    if (INVALID_HANDLE_VALUE != memmap_) {
        CloseHandle(memmap_);
    }

    if (INVALID_HANDLE_VALUE != handle_) {
        CloseHandle(handle_);
    }

#else

    if (0 != mappedData_) {
        munmap(mappedData_, size_);
    }

    if (-1 != descriptor_) {
        close(descriptor_);
    }

#endif
}

int MemMappedFile::LoadFileData(const char *path)
{
    path_ = path;
#ifdef _WIN32
    handle_ = CreateFile(path_.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == handle_) {
        return FILE_OPENING_ERROR;
    }

    BOOL result = false;

    result = ::GetFileSizeEx(handle_, (PLARGE_INTEGER)&size_);

    if (false == result) {
        return FILE_FAILED_TO_GET_SIZE;
    }

    if (0 == size_) {
        return 0;
    }

    memmap_ = CreateFileMapping(handle_, NULL, PAGE_READONLY, 0, 0, NULL);

    if (INVALID_HANDLE_VALUE == memmap_) {
        return FILE_CREATE_MAPPING_ERROR;
    }

    mappedData_ = static_cast<uint8 *>(MapViewOfFile(memmap_, FILE_MAP_READ, 0, 0, 0));

    if (0 != mappedData_) {
        isMapped_ = true;
        volatile char tmp;

        for (uint64 i = 0; i < size_; i += 4096) {
            tmp = mappedData_[i];
        }
    } else {
        isMapped_ = false;
        CloseHandle(memmap_);
        memmap_ = INVALID_HANDLE_VALUE;
    }

#else
    descriptor_ = open(path_.c_str(), O_RDONLY);

    if (-1 == descriptor_) {
        return FILE_OPENING_ERROR;
    }

    struct stat fileStat;

    if (0 != fstat(descriptor_, &fileStat)) {
        return FILE_FAILED_TO_GET_SIZE;
    }

    size_ = fileStat.st_size;

    /* Map file in memory, BUT DON"T Reserve SWAP memory, use only physical memory */
    mappedData_ = static_cast<uint8 *>(mmap(0, size_, PROT_READ, MAP_PRIVATE | MAP_NORESERVE /*| MAP_POPULATE*/, descriptor_, 0));

    if (MAP_FAILED != mappedData_) {
        isMapped_ = true;
    } else {
        isMapped_ = false;
    }

#endif
    return 0;
}

uint8 *MemMappedFile::AllocateFileData(uint64 offset, uint64 size)
{
    if (size_ < offset + size) {
        error_ = FILE_READ_INVALID_OFFSET;
        return 0;
    }

    uint32 alignedSize = (static_cast<uint32>(size) + (alignmentLength_ - 1)) & (~(alignmentLength_ - 1));

    if (isMapped_) {
        if (size_ < offset + alignedSize) {
            uint8 *data = new uint8[alignedSize];
            memcpy(data, mappedData_ + offset, static_cast<size_t>(size));
            return data;
        } else {
            return mappedData_ + offset;
        }
    } else {
        // file is not memory mapped fall back to plain read
        uint32 readSize = static_cast<uint32>(size);
        uint8 *data = new uint8[alignedSize];
#ifdef _WIN32
        LARGE_INTEGER liOffset;
        liOffset.QuadPart = offset;

        if (!SetFilePointerEx(handle_, liOffset, NULL, FILE_BEGIN)) {
            delete[] data;
            error_ = FILE_READ_INVALID_OFFSET;
            return 0;
        }

        uint32 bytesRead;

        if (ReadFile(handle_, data, readSize, (LPDWORD)&bytesRead, NULL) && bytesRead == size) {
            return data;
        } else {
            delete[] data;
            error_ = FILE_READ_ERROR;
            return 0;
        }

#else

        if (-1 == lseek64(descriptor_, offset, SEEK_SET)) {
            delete[] data;
            error_ = FILE_READ_INVALID_OFFSET;
            return 0;
        }

        if (readSize == (uint32)read(descriptor_, data, readSize)) {
            return data;
        } else {
            delete[] data;
            error_ = FILE_READ_ERROR;
            return 0;
        }

#endif
    }
}

void MemMappedFile::ReleaseFileData(uint8 *data, uint64 offset, uint64 size)
{
    uint32 alignedSize = (static_cast<uint32>(size) + (alignmentLength_ - 1)) & (~(alignmentLength_ - 1));

    if (!isMapped_ || size_ < offset + alignedSize) {
        delete[] data;
    }
}

uint64 MemMappedFile::GetFileSize()
{
    return size_;
}

int MemMappedFile::GetError()
{
    return error_;
}
