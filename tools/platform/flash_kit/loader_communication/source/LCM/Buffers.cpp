/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

/*
 * \addtogroup ldr_communication_buffer
 * @{
 */

/***********************************************************************
 * Includes
 **********************************************************************/
#include "Buffers.h"
#include "lcdriver_error_codes.h"
#include "t_r15_network_layer.h"
#include "Error.h"
#include <cstdlib>
using namespace std;

vector<TBulkFile *> Buffers::m_BulkFiles;
CCriticalSectionObject Buffers::csBulkFiles;
const uint32 Buffers::MAX_BUFFERS_COUNT = 32;

#define MAX_BUFFER_SIZE BULK_BUFFER_SIZE

TBulkVector::TBulkVector():
    HeaderBuffer_p(NULL),
    Data_p(NULL),
    Offset(0),
    Length(0),
    ChunkSize(0)
{
}

TBulkVector::~TBulkVector()
{
    delete[] HeaderBuffer_p;
}

TBulkFile::TBulkFile(): refCount(1)
{
    iAllSessionsCompleted = 0;
    pMemMappedFile = 0;
}

TBulkFile::~TBulkFile()
{
    delete pMemMappedFile;

    for (vector<TBulkVector *>::iterator i = bulkVectors.begin(); i != bulkVectors.end(); ++i) {
        delete *i;
    }
}

Buffers::Buffers():
    buffersCount_(0),
    logger_(0)
{
    m_BulkFile = 0;
}

Buffers::~Buffers()
{
    Deinit();
}

/***********************************************************************
 * Definition of internal functions
 **********************************************************************/
ErrorCode_e Buffers::Init()
{
    return E_SUCCESS;
}

void *Buffers::Allocate(int Size)
{
    void *Buffer_p = 0;

    if (buffersCount_ < MAX_BUFFERS_COUNT) {
        Buffer_p = static_cast<uint8 *>(new uint8[Size]);
        ++buffersCount_;
#ifdef _BUFFERDEBUG
        PrintF("Buffers: Allocate - Buffer 0x%p", Buffer_p);
#endif
    } else {
#ifdef _BUFFERDEBUG
        PrintF("Buffers: Allocate - Failed, Count %u", (void *)buffersCount_);
#endif
    }

    return Buffer_p;
}

ErrorCode_e Buffers::Release(void *Buffer_p, int Size)
{
#ifdef _BUFFERDEBUG
    PrintF("Buffers: Release - Buffer 0x%p", Buffer_p);
#endif
    delete[] static_cast<uint8 *>(Buffer_p);
    --buffersCount_;
    return E_SUCCESS;
}

uint32 Buffers::Available(int BufferSize)
{
#ifdef _BUFFERDEBUG
    PrintF("Buffers: Available - Available %d", (void *)(MAX_BUFFERS_COUNT - buffersCount_));
#endif
    return MAX_BUFFERS_COUNT - buffersCount_;
}

void Buffers::Deinit()
{
#ifdef _BUFFERDEBUG

    if (buffersCount_) {
        PrintF("Buffers: Deinit - Buffers still allocated %d", (void *)buffersCount_);
    }

#endif
}

int Buffers::AllocateBulkFile(string strFile)
{
    CLockCS CsLock(csBulkFiles);

    for (vector<TBulkFile *>::iterator i = m_BulkFiles.begin(); i != m_BulkFiles.end(); ++i) {
        if (strFile.compare((*i)->strFileName) == 0) {
            m_BulkFile = *i;
            ++m_BulkFile->refCount;
            return E_SUCCESS;
        }
    }

    TBulkFile *newFile = new TBulkFile;
    newFile->strFileName = strFile;

    MemMappedFile *file = new MemMappedFile;
#ifdef _BUFFERDEBUG
    PrintF("Buffers:  LoadFileData %s", (void *)strFile.c_str());
#endif
    int iReturn = file->LoadFileData(strFile.c_str());

    if (iReturn) {
        delete newFile;
        delete file;
        return iReturn;
    }

    newFile->pMemMappedFile = file;

    m_BulkFiles.push_back(newFile);
    m_BulkFile = newFile;

    return E_SUCCESS;
}

uint64 Buffers::GetBulkFileLength()
{
    return m_BulkFile->pMemMappedFile->GetFileSize();
}

int Buffers::AllocateBulkVector(TL_BulkVectorList_t *BulkVector_p, uint32 iChunkSize, uint64 lOffset, uint32 iLength)
{
    CLockCS CsLock(m_BulkFile->csBulkVectors);
    int ReturnValue = E_SUCCESS;

    int HeaderBufferSize = ALIGNED_HEADER_LENGTH + ALIGNED_BULK_EXTENDED_HEADER_LENGTH;
    int CurrentHeaderBufferSize = (BulkVector_p->Buffers) * HeaderBufferSize;

    TBulkVector *currentVector = NULL;

    //Check that flash buffers have been initialized.
    VERIFY(0 != m_BulkFile, BUFFER_BULK_FILE_NOT_ALOCATED);

    currentVector = new TBulkVector();
    currentVector->HeaderBuffer_p = new uint8[CurrentHeaderBufferSize];
    currentVector->Data_p = m_BulkFile->pMemMappedFile->AllocateFileData(lOffset, iLength);
    currentVector->Offset = lOffset;
    currentVector->Length = iLength;
    currentVector->ChunkSize = iChunkSize;
    m_BulkFile->bulkVectors.push_back(currentVector);
    VERIFY(0 != currentVector->Data_p, m_BulkFile->pMemMappedFile->GetError());

#ifdef _BUFFERDEBUG
    PrintF("Buffers: AllocateFlashBuffers - Created new bulk vector 0x%p", (void *)currentVector);
#endif

    for (uint32 i = 0; i < BulkVector_p->Buffers; i++) {
        uint8 *ChunkHeader_p = currentVector->HeaderBuffer_p + i * HeaderBufferSize;
        uint8 *ChunkPayload_p = currentVector->Data_p + i * iChunkSize;

        PacketMeta_t *Packet_p = BulkVector_p->Entries[i].Buffer_p;
        Packet_p->Buffer_p = ChunkHeader_p;
        Packet_p->ExtendedHeader_p = ChunkHeader_p + (uint64)Packet_p->ExtendedHeader_p;
        Packet_p->Payload_p = ChunkPayload_p;

        BulkVector_p->Entries[i].Payload_p = ChunkPayload_p;
    }

ErrorExit:

    if ((0 != currentVector) && (0 == currentVector->Data_p)) {
        m_BulkFile->bulkVectors.pop_back();
        delete currentVector;
    }

    return ReturnValue;
}

void Buffers::ReleaseBulkVector(TL_BulkVectorList_t *BulkVector_p)
{
    CLockCS CsLock(m_BulkFile->csBulkVectors);

    for (vector<TBulkVector *>::iterator i = m_BulkFile->bulkVectors.begin(); i != m_BulkFile->bulkVectors.end(); ++i) {
        if (BulkVector_p->Offset == (*i)->Offset &&
                BulkVector_p->Length == (*i)->Length &&
                BulkVector_p->ChunkSize == (*i)->ChunkSize) {
            m_BulkFile->pMemMappedFile->ReleaseFileData((*i)->Data_p, (*i)->Offset, (*i)->Length);
            delete *i;
            m_BulkFile->bulkVectors.erase(i);
            break;
        }
    }
}

void Buffers::ReleaseAllBulkFiles()
{
    CLockCS CsLock(csBulkFiles);

    for (vector<TBulkFile *>::iterator i = m_BulkFiles.begin(); i != m_BulkFiles.end(); ++i) {
        delete *i;
    }

    m_BulkFiles.clear();
}

void Buffers::ReleaseBulkFile()
{
    CLockCS CsLock(csBulkFiles);

    if (0 == m_BulkFile || --m_BulkFile->refCount > 0) {
        return;
    }

    // remove bulk file from static list
    for (vector<TBulkFile *>::iterator i = m_BulkFiles.begin(); i != m_BulkFiles.end(); ++i) {
        if (m_BulkFile == *i) {
            m_BulkFiles.erase(i);
            break;
        }
    }

    delete m_BulkFile;
    m_BulkFile = 0;
}

void Buffers::PrintF(const char *text, void *pVoid)
{
    if (NULL != logger_) {
        logger_->log(text, pVoid);
    }
}
