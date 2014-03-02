/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "t_bulk_protocol.h"
#include "MemMappedFile.h"
#include "Queue.h"
#include "Logger.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#endif

#include <string>
#include <vector>

struct TBulkVector {
    uint8 *HeaderBuffer_p;
    uint8 *Data_p;
    uint64 Offset;
    uint32 Length;
    uint32 ChunkSize;

    TBulkVector();
    ~TBulkVector();
};

struct TBulkFile {
    TBulkFile();
    ~TBulkFile();

    std::string strFileName;
    std::vector<TBulkVector *> bulkVectors;
    size_t refCount;
    int iAllSessionsCompleted;
    MemMappedFile *pMemMappedFile;

    CCriticalSectionObject csBulkVectors;
};

class Buffers
{
public:
    Buffers();
    ~Buffers();

    void SetLogger(Logger *logger) {
        logger_ = logger;
    }
private:
    //Member variables for Bulk Flash.
    static std::vector<TBulkFile *> m_BulkFiles;
    static CCriticalSectionObject csBulkFiles;
    TBulkFile *m_BulkFile;
public:
    ErrorCode_e Init();
    void *Allocate(int Size);
    ErrorCode_e Release(void *Buffer_p, int Size);
    uint32 Available(int BufferSize);
    void Deinit();

    //Functions for Bulk Flash
    int AllocateBulkFile(std::string strFile);
    uint64 GetBulkFileLength();

    int AllocateBulkVector(TL_BulkVectorList_t *BulkVector_p, uint32 iChunkSize, uint64 lOffset, uint32 iLength);
    void ReleaseBulkVector(TL_BulkVectorList_t *BulkVector_p);

    void ReleaseBulkFile();
    static void ReleaseAllBulkFiles();
private:
    uint32  buffersCount_;
    Logger *logger_;
    static const uint32 MAX_BUFFERS_COUNT;
    void PrintF(const char *text, void *pVoid);
};

#endif // _BUFFER_H_
