/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * \addtogroup ldr_security_algorithms
 * @{
 */

/*************************************************************************
* Includes
*************************************************************************/
#include "Hash.h"
#include "SecurityAlgorithms.h"
#include <cstdlib>
#include <cstring>

/***********************************************************************
 * Declaration of file local functions
 **********************************************************************/
/***********************************************************************
 * Definition of external functions
 **********************************************************************/
const uint32 red[16] = {
    0,
    (0x1 << 16) ^(0x1021 << 0), \
    (0x2 << 16) ^(0x1021 << 1), \
    (0x3 << 16) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0x4 << 16) ^(0x1021 << 2), \
    (0x5 << 16) ^(0x1021 << 2) ^(0x1021 << 0), \
    (0x6 << 16) ^(0x1021 << 2) ^(0x1021 << 1), \
    (0x7 << 16) ^(0x1021 << 2) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0x8 << 16) ^(0x1021 << 3), \
    (0x9 << 16) ^(0x1021 << 3) ^(0x1021 << 0), \
    (0xa << 16) ^(0x1021 << 3) ^(0x1021 << 1), \
    (0xb << 16) ^(0x1021 << 3) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0xc << 16) ^(0x1021 << 3) ^(0x1021 << 2), \
    (0xd << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 0), \
    (0xe << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 1), \
    (0xf << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 1) ^(0x1021 << 0)
};

Hash::Hash()
{
    m_RequestQueue = new CSemaphoreQueue(16);
    m_Thread = new CThreadWrapper(WorkerThread, this);
    m_Thread->ResumeThread();
}

Hash::~Hash()
{
    m_RequestQueue->SignalEvent();
    m_Thread->WaitToDie();
    delete m_Thread;
    delete m_RequestQueue;
}

/*
 * Hash device shutdown.
 *
 * @param [in]  Object_p       Initialized buffer context.
 * @param [in]  HashDevice_pp  Pointer to hash device descriptor.
 *
 * @return none.
 */
void Hash::Cancel(HashDevice_t **HashDevice_pp)
{
    delete *HashDevice_pp;
}

#ifdef _WIN32
unsigned int WINAPI Hash::WorkerThread(void *arg)
#else
void *Hash::WorkerThread(void *arg)
#endif
{
    Hash *pThis = (Hash *)arg;
    HashRequest *request = 0;

    while (true) {
        RemoveResult result = pThis->m_RequestQueue->RemoveHead((void **)(&request), INFINITE);

        if (REMOVE_SUCCESS != result) {
            break;
        }

        switch (request->Type) {
        case HASH_SHA256:
            memset(request->Hash_p, 0, SHA256_LENGTH);

            if (request->Length != 0) {
                SecurityAlgorithms::SHA256(request->Data_p, request->Length, request->Hash_p);
            }

            break;

        case HASH_CRC16:
            memset(request->Hash_p, 0, CRC16_LENGTH);

            if (request->Length != 0) {
                uint16 CRC16 = crc16(request->Data_p, request->Length, 0);
                memcpy(request->Hash_p, &CRC16, sizeof(uint16));
            }

            break;

        case HASH_SIMPLE_XOR:
            request->Hash_p[0] = 0;

            if (request->Length != 0) {
                request->Hash_p[0] = hashxor(request->Data_p, request->Length);
            }

            break;

        case HASH_NONE:
            break;

        default:
            // unsupported hash type, nothing to do
            delete request;
            continue;
        }

        request->Callback(request->Data_p, request->Length, request->Hash_p, request->Param_p);

        delete request;
    }

    return 0;
}

/**
 * Asynchronous hashing of data.
 *
 * Calculates a hash over the specified block of data, calling the call-
 * back function on completion. Note that the callback function might
 * execute in interrupt context, so due care should be taken when
 * writing it.
 *
 * The hashing might be delayed if another operation is currently
 * running.
 *
 * @param [in]  Object_p  Initialized buffer context.
 * @param [in]  Type      Hash type.
 * @param [in]  Data_p    Start of the block of data to hash.
 * @param [in]  Length    The length in bytes of the block of data to hash.
 * @param [out] Hash_p    Start of the buffer that should hold the hash
 *                        value.
 * @param [in]  Callback  The function to call when the hash calculation
 *                        completes.
 * @param [in,out] Param_p The value to use as parameter to Param_p.
 *
 *  @note Hash_p must be large enough to contain the selected hash type.
 *  Use the "HASH"_LENGTH macros when you know beforehand which hash
 *  type to use, or Do_Crypto_GetHashLength() for dynamically selected
 *  hashes.
 *
 */
void Hash::Calculate(HashType_e Type, void *Data_p, uint32 Length, uint8 *Hash_p, HashCallback_fn Callback, void *Param_p)
{
    HashRequest *currentRequest = new HashRequest;
    currentRequest->Type = Type;
    currentRequest->Data_p = (uint8 *)Data_p;
    currentRequest->Length = Length;
    currentRequest->Hash_p = Hash_p;
    currentRequest->Callback = Callback;
    currentRequest->Param_p = Param_p;

    m_RequestQueue->AddTail(currentRequest);
    /* coverity[leaked_storage] */
}

uint8 Hash::hashxor(const uint8 *Buf_p, uint32 Length)
{
    uint8 Value = 0;
    const uint8 *Stop_p = Buf_p + Length;

    do {
        Value ^= *Buf_p++;
    } while (Buf_p < Stop_p);

    return Value;
}

uint16 Hash::crc16(const uint8 *p, int count, uint16 crc_in)
{
    register uint32 crc = crc_in;

    while (--count >= 0) {
        crc = crc ^(uint8) * p++ << 8;
        crc ^= red[crc >> 16];
        crc = (crc << 4);
        crc ^= red[crc >> 16];
        crc = (crc << 4);
        crc ^= red[crc >> 16];

    }

    return (uint16) crc;
}
