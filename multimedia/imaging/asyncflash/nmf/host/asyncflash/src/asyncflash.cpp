/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "ASYNCFLASH"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "asyncflash.nmf"
#include "asyncflash_types.idt.h"
#include <stdio.h>
#include "osi_trace.h"
#include "asyncflash/inc/async_flash_api.h"

/*
 * Local stuff
 */
#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    int id;
    const char* name;
} t_asyncflash_return_entry;

static const t_asyncflash_return_entry KFioReturnCodesList[] =
{
    HASH_ENTRY(FLASH_RET_NONE),
    HASH_ENTRY(FLASH_RET_MODE_NOT_SUPPORTED),
    HASH_ENTRY(FLASH_RET_MODE_ENABLE_FAILURE),
    HASH_ENTRY(FLASH_RET_UNSUPPORTED_SETTING),
    HASH_ENTRY(FLASH_RET_STROBE_FAILURE),
    HASH_ENTRY(FLASH_RET_STATUS_FAILURE),
    HASH_ENTRY(FLASH_RET_COUNTER_FAILURE),
    HASH_ENTRY(FLASH_RET_INVALID_CALL),
    HASH_ENTRY(FLASH_RET_OUT_OF_MEM),
    HASH_ENTRY(FLASH_RET_UNKNOWN),
};

#define KFioReturnCodesListSize (sizeof(KFioReturnCodesList)/sizeof(KFioReturnCodesList[0]))

/*
 * Constructor
 */
asyncflash::asyncflash()
{
    IN0(" ");
    pFlashDriver = CFlashDriver::Open();
    if(pFlashDriver==NULL) {
       MSG0("asyncflash: flash driver open failed\n");
    }
    OUT0(" ");
    return;
}

/*
 * Destructor
 */
asyncflash::~asyncflash()
{
    IN0(" ");
    pFlashDriver->Close();
    pFlashDriver = NULL;
    OUT0(" ");
    return;
}


/*
 * Supported flash modes method
 */
void asyncflash::getSupportedFlashModes(void* pReqArg)
{
    ASYNC_IN0(" ");
    GetSupportedFlashModes_t* pReq = (GetSupportedFlashModes_t*)pReqArg;
    pFlashDriver->GetSupportedFlashModes(pReq->aModes, pReq->aCameraId);
    response.asyncFlashReturnCode(FLASH_RET_NONE);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Flash mode details method
 */
void asyncflash::getFlashModeDetails(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    GetFlashModeDetails_t* pReq = (GetFlashModeDetails_t*)pReqArg;
    ret = pFlashDriver->GetFlashModeDetails(pReq->aFlashMode, pReq->aDetails, pReq->aCameraId);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Enable flash mode method
 */
void asyncflash::enableFlashMode(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    EnableFlashMode_t* pReq = (EnableFlashMode_t*)pReqArg;
    ret = pFlashDriver->EnableFlashMode( pReq->aFlashMode,
                                         pReq->aCallback,
                                         pReq->apContext,
                                         pReq->aCameraId);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Configure flash mode method
 */
void asyncflash::configureFlashMode(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    ConfigureFlashMode_t* pReq = (ConfigureFlashMode_t*)pReqArg;
    ret = pFlashDriver->ConfigureFlashMode( pReq->aFlashMode,
                                            pReq->aDuration,
                                            pReq->aIntensity,
                                            pReq->aTimeout,
                                            pReq->aCameraId);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Strobe method
 */
void asyncflash::strobe(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    Strobe_t* pReq = (Strobe_t*)pReqArg;
    ret = pFlashDriver->Strobe(pReq->aFlashMode, pReq->aEnable, pReq->aCameraId);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Get Status method
 */
void asyncflash::getStatus(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    GetStatus_t* pReq = (GetStatus_t*)pReqArg;
    ret = pFlashDriver->GetStatus(pReq->aFlashMode, pReq->aFlashStatus, pReq->aCameraId);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Get Xenon life counter method
 */
void asyncflash::getXenonLifeCounter(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    GetXenonLifeCounter_t* pReq = (GetXenonLifeCounter_t*)pReqArg;
    ret = pFlashDriver->GetXenonLifeCounter(pReq->aCounter, pReq->aCameraId);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Get supported selftests method
 */
void asyncflash::getSupportedSelfTests(void* pReqArg)
{
    ASYNC_IN0(" ");
    GetSupportedSelfTests_t* pReq = (GetSupportedSelfTests_t*)pReqArg;
    pFlashDriver->GetSupportedSelfTests(pReq->aSelfTests, pReq->aCameraId);
    response.asyncFlashReturnCode(FLASH_RET_NONE);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Get fault registers method
 */
void asyncflash::getFaultRegisters(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    GetFaultRegisters_t* pReq = (GetFaultRegisters_t*)pReqArg;
    ret = pFlashDriver->GetFaultRegisters(pReq->aFlashModes, pReq->apFaultRegisters);
    response.asyncFlashReturnCode(ret);
    ASYNC_OUT0(" ");
    return;
}


/*
 * Selftest method
 */
void asyncflash::doSelfTest(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    DoSelfTest_t* pReq = (DoSelfTest_t*)pReqArg;
    ret = pFlashDriver->DoSelfTest(pReq->aSelfTest, FlashTest_cb, this, pReq->aCameraId);
    // Send immediate response only if function call fails
    // FlashTest_cb will send actual status
    if (FLASH_RET_NONE != ret) {
        response.asyncFlashReturnCode(ret);
    }
    ASYNC_OUT0(" ");
    return;
}


/*
 * Get NVM method
 */
void asyncflash::getNVM(void* pReqArg)
{
    ASYNC_IN0(" ");
    TFlashReturnCode ret = FLASH_RET_NONE;
    GetNVM_t* pReq = (GetNVM_t*)pReqArg;
    ret = pFlashDriver->GetNVM( pReq->aFlashModes,
                                pReq->BufferSize,
                                pReq->NVMBuffer,
                                FlashNVM_cb,
                                this,
                                pReq->aCameraId);
    // Send immediate response only if function call fails
    // FlashNVM_cb will send actual status
    if( ret != FLASH_RET_NONE ) {
        response.asyncFlashReturnCode(ret);
    }
    ASYNC_OUT0(" ");
    return;
}

/*
 * Flash test callback method
 */
void asyncflash::FlashTest_cb (TFlashSelftest aResult, void* pContext)
{
    ASYNC_IN0(" ");
    asyncflash* pThis = (asyncflash*)pContext;
    pThis->response.asyncFlashSelfTestResult(aResult);
    ASYNC_OUT0(" ");
    return;
}

/*
 * Flash NVM callback method
 */
void asyncflash::FlashNVM_cb (TFlashReturnCode aReturnCode, void* pContext)
{
    ASYNC_IN0(" ");
    asyncflash* pThis = (asyncflash*)pContext;
    pThis->response.asyncFlashReturnCode(aReturnCode);
    ASYNC_OUT0(" ");
    return;
}

/*
 * returnCode2String method
 */
const char* asyncflash::returnCode2String(t_uint32 aErr)
{
    for(unsigned int i=0; i<KFioReturnCodesListSize; i++) {
        if(KFioReturnCodesList[i].id == (TFlashReturnCode) aErr)
            return KFioReturnCodesList[i].name;
    }
    return "*** RETURN CODE NOT FOUND ***";
}


