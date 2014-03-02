/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _HASH_H_
#define _HASH_H_

#include "t_communication_service.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#endif

struct HashRequest {
    HashType_e Type;
    uint8 *Data_p;
    uint32 Length;
    uint8 *Hash_p;
    HashCallback_fn Callback;
    void *Param_p;
};

class Hash
{
public:
    Hash();
    ~Hash();
    void Cancel(HashDevice_t **HashDevice_pp);
    void Calculate(HashType_e Type, void *Data_p, const uint32 Length, uint8 *Hash_p, HashCallback_fn Callback, void *Param_p);
    uint32 GetLength(HashType_e Type);
private:
    CThreadWrapper *m_Thread;
    CSemaphoreQueue *m_RequestQueue;

#ifdef _WIN32
    static unsigned int WINAPI WorkerThread(void *arg);
#else
    static void *WorkerThread(void *arg);
#endif

    static uint8 hashxor(const uint8 *Buf_p, uint32 Length);
    static uint16 crc16(const uint8 *p, int count, uint16 crc_in);
};

#endif // _HASH_H_
