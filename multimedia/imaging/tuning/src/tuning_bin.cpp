/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING_BIN"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include <string.h>
#include "osi_trace.h"
#include "tuning_bin.h"

/*
 * Local stuff
 */
#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    t_tuning_bin_data_error id;
    const char* name;
} t_tuning_bin_data_error_entry;

static const t_tuning_bin_data_error_entry KTuningBinErrorCodesList[] =
{
    HASH_ENTRY(TUNING_BIN_OK),
    HASH_ENTRY(TUNING_BIN_BAD_ARGUMENT),
    HASH_ENTRY(TUNING_BIN_ALREADY_CONSTRUCTED),
    HASH_ENTRY(TUNING_BIN_NOT_CONSTRUCTED),
    HASH_ENTRY(TUNING_BIN_MALLOC_FAILED),
    HASH_ENTRY(TUNING_BIN_MEMCPY_WOULD_OVERFLOW)
};

#define KBinTuningErrorCodesListSize (sizeof(KTuningBinErrorCodesList)/sizeof(KTuningBinErrorCodesList[0]))

// Prevent export of symbols
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningBinData);
#endif

/*
 * Tuning Binary object
 */

void CTuningBinData::init()
{
    pInstanceName = "Anonymous";
    pData = NULL;
    iSize = 0;
    iStride = 0;
}
CTuningBinData::CTuningBinData()
{
    IN0("\n");
    init();
    MSG1("[%s] Instantiated\n", pInstanceName);
    OUT0("\n");
}

CTuningBinData::CTuningBinData(const char* aInstanceName)
{
    IN0("\n");
    init();
    if(aInstanceName!=NULL) {
        pInstanceName = aInstanceName;
    }
    MSG1("[%s] Instantiated\n", pInstanceName);
    OUT0("\n");
}

CTuningBinData::~CTuningBinData()
{
    IN0("\n");
    destroy();
    OUT0("\n");
}

t_tuning_bin_data_error CTuningBinData::construct(int aSize)
{
    IN0("\n");
    MSG2("[%s] Construction: size=%d\n", pInstanceName, aSize);
    if(pData!=NULL) {
        MSG2("[%s] Instance already constructed: pData=%p\n", pInstanceName, pData);
        OUTR(" ", TUNING_BIN_ALREADY_CONSTRUCTED);
        return TUNING_BIN_ALREADY_CONSTRUCTED;
    }
    if(aSize<=0) {
        MSG2("[%s] Bad argument: aSize=%d\n", pInstanceName, aSize);
        OUTR(" ", TUNING_BIN_BAD_ARGUMENT);
        return TUNING_BIN_BAD_ARGUMENT;
    }
    // Allocate a 4-bytes aligned storage area
    pData = (unsigned char*) new int[(aSize+sizeof(int))/sizeof(int)];
    if(pData==NULL) {
        MSG2("[%s] Could not allocate %d bytes for binary data storage\n", pInstanceName, aSize);
        OUTR(" ", TUNING_BIN_MALLOC_FAILED);
        return TUNING_BIN_MALLOC_FAILED;
    }
    MSG3("[%s] Allocated %d bytes for binary data storage: pData=%p\n", pInstanceName, aSize, pData);
    iSize = aSize;
    OUTR(" ", TUNING_BIN_OK);
    return TUNING_BIN_OK;
}

t_tuning_bin_data_error CTuningBinData::construct(int aSize, const void* aSrcData)
{
    IN0("\n");
    MSG3("[%s] Construction: size=%d, data=%p\n", pInstanceName, aSize, aSrcData);
    t_tuning_bin_data_error err = TUNING_BIN_OK;
    // Allocate space for data
    err = construct(aSize);
    if(err!=TUNING_BIN_OK) {
        MSG1("[%s] Instance constructruction failed\n", pInstanceName);
        OUTR(" ", err);
        return err;
    }
    // Copy data
    err = fill(aSrcData);
    if(err!=TUNING_BIN_OK) {
        MSG1("[%s] Instance constructruction failed\n", pInstanceName);
        OUTR(" ", err);
        return err;
    }
    OUTR(" ", TUNING_BIN_OK);
    return TUNING_BIN_OK;
}

void CTuningBinData::destroy()
{
    IN0("\n");
    MSG1("[%s] Destruction\n", pInstanceName);
    if(pData!=NULL) {
        MSG2("[%s] Deleting binary data storage: pData=%p\n", pInstanceName, pData);
        delete [] pData;
        pInstanceName = "Anonymous";
        pData = NULL;
        iSize = 0;
    }
    OUT0("\n");
}

t_tuning_bin_data_error CTuningBinData::fill(const void* aSrc)
{
    IN0("\n");
    if(pData==NULL) {
        MSG1("[%s] Instance not constructed\n",pInstanceName);
        OUTR(" ", TUNING_BIN_NOT_CONSTRUCTED);
        return TUNING_BIN_NOT_CONSTRUCTED;
    }
    if(aSrc==NULL) {
        MSG2("[%s] Bad argument: aSrc=%p\n", pInstanceName, aSrc);
        OUTR(" ", TUNING_BIN_BAD_ARGUMENT);
        return TUNING_BIN_BAD_ARGUMENT;
    }
    MSG4("[%s] Copying %d bytes from aSrc=%p to pData=%p\n", pInstanceName, iSize, aSrc, pData);
    memcpy(pData, aSrc, iSize);
    OUTR(" ", TUNING_BIN_OK);
    return TUNING_BIN_OK;
}

t_tuning_bin_data_error CTuningBinData::fill(const void* aSrc, const int aDestOffset, const int aSizeToBeCopied)
{
    IN0("\n");
    if(pData==NULL) {
        MSG1("[%s] Instance not constructed\n",pInstanceName);
        OUTR(" ", TUNING_BIN_NOT_CONSTRUCTED);
        return TUNING_BIN_NOT_CONSTRUCTED;
    }
    if(aSrc==NULL || aSizeToBeCopied==0) {
        MSG3("[%s] Bad argument: aSrc=%p, aSizeToBeCopied=%d\n", pInstanceName, aSrc, aSizeToBeCopied);
        OUTR(" ", TUNING_BIN_BAD_ARGUMENT);
        return TUNING_BIN_BAD_ARGUMENT;
    }
    if(aDestOffset+aSizeToBeCopied > iSize) {
        MSG4("[%s] Would overflow: iSize=%d, aDestOffset=%d, aSize=%d\n", pInstanceName, iSize, aDestOffset, aSizeToBeCopied);
        OUTR(" ", TUNING_BIN_MEMCPY_WOULD_OVERFLOW);
        return TUNING_BIN_MEMCPY_WOULD_OVERFLOW;
    }
    MSG4("[%s] Copying %d bytes from aSrc=%p to pData=%p\n", pInstanceName, aSizeToBeCopied, aSrc, &((char*)pData)[aDestOffset]);
    memcpy(&((char*)pData)[aDestOffset], aSrc, aSizeToBeCopied);
    OUTR(" ", TUNING_BIN_OK);
    return TUNING_BIN_OK;
}

void* CTuningBinData::getAddr()
{
    return pData;
}

const char* CTuningBinData::getName()
{
    return pInstanceName;
}

int CTuningBinData::getStride()
{
    return iStride;
}

void CTuningBinData::setStride(int aStride)
{
    iStride = aStride;
}

int CTuningBinData::getSize()
{
    return iSize;
}

const char* CTuningBinData::errorCode2String(t_tuning_bin_data_error aErr)
{
    for(unsigned int i=0; i<KBinTuningErrorCodesListSize; i++) {
        if(KTuningBinErrorCodesList[i].id == aErr)
            return KTuningBinErrorCodesList[i].name;
    }
    return "*** ERROR CODE NOT FOUND ***";
}
