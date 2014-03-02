/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define DBGT_PREFIX "SW3A_COMP_MGR"
#include "ENS_Component_Fsm.h"
#include "OMX_CoreExt.h"
#include "cm/inc/cm_macros.h"
#include "OMX_CoreExt.h"


#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#include "sw3A_component_manager.h"
#include "VhcElementDefs.h"
#include "hsmcam.h"
#include "camera.h"
#include "IFM_Types.h"
#include "IFM_Trace.h"
#include "ImgConfig.h"
extern "C"{
#include <cm/inc/cm_macros.h>
#include "osi_trace.h"
}

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_sw3A_component_managerTraces.h"
#endif

// Local prototypes
static void sw3A_ispctlInfo(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd);
static void sw3A_flashSeqEnd(flashSeqClbkCtxtHnd_t ctxtHnd);
static void sw3A_flashSeqTimerStart(t_uint32 sleepTime_ms, flashSeqClbkCtxtHnd_t ctxtHnd);
static void sw3A_flashSeqTimerStop(flashSeqClbkCtxtHnd_t ctxtHnd);

Sw3AProcessingComp::Sw3AProcessingComp(TraceObject *traceobj):
		mFlashController(traceobj),
		mFlashSequencer(traceobj),
		mTraceObject(traceobj),
		makernotesBuf(traceobj),
		extradataBuf(traceobj),
		captureParametersBuf(traceobj),
		dampersDataBuf(traceobj),
		flashStatusBuf(traceobj),
		steExtraCaptureParametersBuf(traceobj), 
		focusStatusBuf(traceobj)
{
    mSW3A = NULL;
    mSM = NULL;
    mDeferredEventMgr = NULL;
    mIspCtlEventClbks = NULL;
    mSharedMemory = NULL;
    mFlashSeqTimer = (timerWrapped*)(NULL);
    mMaxFramerateRequested_x100 = 0;
    mMinFramerateRequested_x100 = 0;
    mbFixedFramerate = false;
    mConfigQueue_usedCount = 0;
    mAecConvergenceStatus = SW3A_AEC_CONVERGENCE_STATUS_NOT_CONVERGED;
    mbStillModeOn = false;
    whatNextAfterStart = NULL;
    whatNextAfterStop = NULL;
    whatNextAfterRendezvous = NULL;
    

    bStillSynchro_StatsReceived = false;
    bStillSynchro_GrabAllowed   = false;
    bStillSynchro_GrabStarted   = false;
    bReachedLastOfListGroup = false;
    bClosePending = false;

    // Register the PE  which status is needed by sw3A component at start.
    // Todo: The list should be provided by the sw3A component (globals? #defines? Synchronous interface?).
    fwStatus.size = 0;
    fwStatus.a_list[fwStatus.size++].pe_addr = Glace_Status_u8_ParamUpdateCount_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = SystemConfig_Status_e_Coin_Status_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = AFStats_Status_e_Coin_ZoneConfigStatus_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = AFStats_Status_e_Coin_AFStatsExportStatus_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = FocusControl_Status_e_Coin_Status_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = FLADriver_NVMStoredData_s32_NVMInfinityFarEndPos_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = FLADriver_NVMStoredData_s32_NVMInfinityHorPos_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = FLADriver_NVMStoredData_s32_NVMMacroNearEndPos_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = FLADriver_NVMStoredData_s32_NVMMacroHorPos_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = SMS_Status_u32_LineLength_pck_Byte0; // provisionnal request; to be used later ... maybe
    fwStatus.a_list[fwStatus.size++].pe_addr = SMS_Status_u32_FrameLength_lines_Byte0; // provisionnal request; to be used later ... maybe
    fwStatus.a_list[fwStatus.size++].pe_addr = SMS_Status_u32_Min_ExposureTime_us_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = SMS_Status_u32_Max_ExposureTime_us_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = SMS_Status_u32_ExposureQuantizationStep_us_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = SMS_Status_u32_ActiveData_ReadoutTime_us_Byte0;
    fwStatus.a_list[fwStatus.size++].pe_addr = HDR_Status_e_Coin_Byte0;

    frame_counter = 0;

    // Gamma LUT hsm internal variables
    bHasToStopBeforeGammaChange = false;
    bLUTWritten = false;
    memset(&mSw3AStatStatus, 0, sizeof(t_sw3A_stat_status));
    Gamma_Flag = 0;
    gamma_LR_HR_done = 0;
}

OMX_ERRORTYPE Sw3AProcessingComp::instantiate(OMX_HANDLETYPE omxhandle)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::instantiate", (mTraceObject));
    t_nmf_error error;
    // Init and binding of arm nmf components
// SW3A
    MSG0("Instantiating 3A arm nmf component.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Instantiating 3A arm nmf component.", (mTraceObject));
    mSW3A = sw_3A_wrpCreate();
    if (NULL == mSW3A) {
        DBGT_ERROR("Error: sw_3A_wrpCreate\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Error: sw_3A_wrpCreate", (mTraceObject));
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }

    error = mSW3A->construct();
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mSW3A->construct() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mSW3A->construct() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    // Direct call interface
    error = mSW3A->bindFromUser("control", 5 + SW3A_CONFIGQUEUE_SIZE, &controlitf) ;
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mSW3A->bindFromUser() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mSW3A->bindFromUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    // Callback interface
    error = EnsWrapper_bindToUser(omxhandle, mSW3A, "done", (sw_3A_api_doneDescriptor *)this, 8 + SW3A_CONFIGQUEUE_SIZE); // 5 for done() + 3 for info()
    if (error != NMF_OK) {
        DBGT_ERROR("Error: EnsWrapper_bindToUser() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: EnsWrapper_bindToUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    // Synchronous config interrogation interface
    // WARNING: 'sync' interface is bound synchronously.
    //           Therefore the memory context of the mSW3A is not guaranteed to be consistent
    //           and only static structures should be accessed within those calls.
    error = mSW3A->getInterface("sync", &syncitf) ;
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mSW3A->getInterface() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mSW3A->getInterface() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }

// Flash sequencer timer
    MSG0("Instantiating 3A arm nmf component.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Instantiating 3A arm nmf component.", (mTraceObject));
    mFlashSeqTimer = timerWrappedCreate();
    if (NULL == mFlashSeqTimer) {
        DBGT_ERROR("Error: timerWrappedCreate() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: timerWrappedCreate() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }

    error = mFlashSeqTimer->construct();
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mFlashSeqTimer->construct() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mFlashSeqTimer->construct() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    // Direct, synchronous call interface
    error = mFlashSeqTimer->getInterface("timer", &flashtimeritf);
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mFlashSeqTimer->bindFromUser() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mFlashSeqTimer->bindFromUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    // Callback interface
    error = EnsWrapper_bindToUser(omxhandle, mFlashSeqTimer, "alarm", (timer_api_alarmDescriptor*)this, 2);    // A 1-slot queue should be enough. Get 2 to be safe.
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mFlashSeqTimer->EnsWrapper_bindToUser() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mFlashSeqTimer->EnsWrapper_bindToUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }

    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::instantiate (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::deInstantiate(OMX_HANDLETYPE omxhandle)
{
    IN0("\n");
    t_nmf_error error;
// SW3A
    MSG0("Uninstantiating 3A arm nmf component.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Uninstantiating 3A arm nmf component.", (mTraceObject));
    error = mSW3A->unbindFromUser("control") ;
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mSW3A->unbindFromUser() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mSW3A->unbindFromUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::deInstantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    error = EnsWrapper_unbindToUser(omxhandle, mSW3A, "done");
    if (error != NMF_OK) {
        DBGT_ERROR("Error: EnsWrapper_unbindToUser(mSW3A) 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: EnsWrapper_unbindToUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::deInstantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    mSW3A->destroy();
    sw_3A_wrpDestroy(mSW3A);
// Flash sequencer timer
    MSG0("Uninstantiating flash sequencer timer nmf component.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Uninstantiating flash sequencer timer nmf component.", (mTraceObject));
    error = EnsWrapper_unbindToUser(omxhandle, mFlashSeqTimer, "alarm");
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mFlashSeqTimer->unbindToUser() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mFlashSeqTimer->unbindToUser() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::deInstantiate (%d)", (mTraceObject), OMX_ErrorHardware);
        return OMX_ErrorHardware;
    }
    
    error = mFlashSeqTimer->destroy();
    if (error != NMF_OK) {
        DBGT_ERROR("Error: mFlashSeqTimer->destroy() 0x%X\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: mFlashSeqTimer->destroy() 0x%X", (mTraceObject), error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::deInstantiate OMX_ErrorHardware", (mTraceObject));
        return OMX_ErrorHardware;
    }
    timerWrappedDestroy(mFlashSeqTimer);

    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::deInstantiate (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

void Sw3AProcessingComp::setSM(CAM_SM *pSM)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::setSM", (mTraceObject));
    mSM = pSM;
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::setSM", (mTraceObject));
}

CAM_SM * Sw3AProcessingComp::getSM()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getSM", (mTraceObject));
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::getSM", (mTraceObject));
    return mSM;
}

void Sw3AProcessingComp::setDeferredEventManager(CDefferedEventMgr *pDeferredEventMgr)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::setDeferredEventManager", (mTraceObject));
    mDeferredEventMgr = pDeferredEventMgr;
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::setDeferredEventManager", (mTraceObject));
}

void Sw3AProcessingComp::setIspCtlClbks(CIspctlEventClbks *pIspCtlEventClbks)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::setIspCtlClbks", (mTraceObject));
    mIspCtlEventClbks = pIspCtlEventClbks;
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::setIspCtlClbks", (mTraceObject));
}

void Sw3AProcessingComp::setSharedMem(CCamSharedMemory *pSharedMemory)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::setSharedMem", (mTraceObject));
    mSharedMemory = pSharedMemory;
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::setSharedMem", (mTraceObject));
}

void Sw3AProcessingComp::setFramerate(t_uint32 maxframerate_x100,t_uint32 minframerate_x100 , bool bFixedFramerate) {
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::setFramerate", (mTraceObject));
    MSG3("Sw3AProcessingComp::setFramerate maxframerate_x100=%d, minframerate_x100=%d and bFixedFramerate=%d",(int)maxframerate_x100,(int)minframerate_x100,bFixedFramerate);	
    mMaxFramerateRequested_x100 = maxframerate_x100;
    mMinFramerateRequested_x100=minframerate_x100;
    mbFixedFramerate = bFixedFramerate;
#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
    controlitf.setFrameRate(mMaxFramerateRequested_x100,mMinFramerateRequested_x100, mbFixedFramerate);
#endif
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::setFramerate", (mTraceObject));
}

void Sw3AProcessingComp::setAecStateConverged(Sw3AAecConvergenceStatus_t aecConvergenceStatus)
{
    mAecConvergenceStatus = aecConvergenceStatus;
}

Sw3AAecConvergenceStatus_t Sw3AProcessingComp::getAecStateConverged()
{
    return mAecConvergenceStatus;
}

bool Sw3AProcessingComp::isStillModeOn()
{
    return mbStillModeOn;
}

OMX_ERRORTYPE Sw3AProcessingComp::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::setConfig", (mTraceObject));
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (bClosePending == false) {
        err = isConfigAvailable(nParamIndex, pComponentParameterStructure);
        if (err == OMX_ErrorNone) {
            if (mConfigQueue_usedCount < SW3A_CONFIGQUEUE_SIZE) {
                mConfigQueue_usedCount++;
#ifdef SW3A_TRACE_CONFIGQUEUE
                printf("Sw3AProcessingComp::setConfig - relaying config %08x at rank %d.\n", nParamIndex, (int) mConfigQueue_usedCount);
#endif /* SW3A_TRACE_CONFIGQUEUE */
                controlitf.setConfig(nParamIndex, pComponentParameterStructure);
            } else {
#ifdef SW3A_TRACE_CONFIGQUEUE
            printf("Sw3AProcessingComp::setConfig %08x - Delaying or overwriting a pending config, because rank is %d.\n", nParamIndex, (int) mConfigQueue_usedCount);
#endif /* SW3A_TRACE_CONFIGQUEUE */
            mConfigStore.push(nParamIndex, pComponentParameterStructure);
            }
        }
    } else { // Close() is pending: ignore the setConfig() call and report an error.
        err = OMX_ErrorIncorrectStateOperation;
#ifdef SW3A_TRACE_CONFIGQUEUE
        printf("Sw3AProcessingComp::setConfig %d - Error: close() pending; no more setConfig is allowed.\n", nParamIndex);
#endif /* SW3A_TRACE_CONFIGQUEUE */
        DBGT_ERROR("setConfig %08x - Error: close() pending; no more setConfig is allowed.\n", nParamIndex);
        OstTraceFiltStatic1(TRACE_ERROR, "setConfig %08x - Error: close() pending; no more setConfig is allowed.", (mTraceObject), nParamIndex);
    }
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::setConfig (%d)", (mTraceObject), err);
    return err;
}

OMX_ERRORTYPE Sw3AProcessingComp::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getConfig", (mTraceObject));
    OMX_ERRORTYPE err = (OMX_ERRORTYPE) syncitf.getConfig(nParamIndex, pComponentParameterStructure);
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::getConfig (%d)", (mTraceObject), err);
    return err;
}

OMX_ERRORTYPE Sw3AProcessingComp::isConfigAvailable(OMX_INDEXTYPE pIndex, OMX_PTR pComponentParameterStructure)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::isConfigAvailable", (mTraceObject));
    // Note : the underlying function's signature is: t_int32 isConfigAvailable(t_uint32 pIndex, void* pp)
    OMX_ERRORTYPE err = (OMX_ERRORTYPE) syncitf.isConfigAvailable((t_uint32)pIndex, (void*)pComponentParameterStructure);
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::isConfigAvailable (%d)", (mTraceObject), err);
    return err;
}


OMX_ERRORTYPE Sw3AProcessingComp::setMode(t_sw3A_LoopState loopState, t_uint32 numberOfStats)
{
    IN0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Entry Sw3AProcessingComp::setMode %d", (mTraceObject), loopState);

    if (loopState == SW3A_MODE_SET_STILL) {
        mbStillModeOn = true;
        bStillSynchro_StatsReceived = false;
        bStillSynchro_GrabAllowed = false;
        bStillSynchro_GrabStarted = false;
    } 
	else if (loopState == SW3A_MODE_SET_HDR_CAPTURE) {
        mbStillModeOn = true;
        bStillSynchro_StatsReceived = false;
        bStillSynchro_GrabAllowed = false;
        bStillSynchro_GrabStarted = false;
    }
	else {
        mbStillModeOn = false;
        bStillSynchro_StatsReceived = false;
        bStillSynchro_GrabAllowed = false;
        bStillSynchro_GrabStarted = false;
    }
    if (loopState == SW3A_MODE_SET_VIDEO && lastsw3AloopState!=SW3A_MODE_SET_STILL_IN_VIDEO) {
       setAecStateConverged(SW3A_AEC_CONVERGENCE_STATUS_NOT_CONVERGED);
    }
	lastsw3AloopState=loopState;
    controlitf.setMode(loopState, numberOfStats);
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::setMode (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::start()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::start", (mTraceObject));
    mSW3A->start() ;
    mFlashSeqTimer->start();
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::start (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::open(enumCameraSlot camSlot, CFlashDriver *pFlashDriver)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::open", (mTraceObject));
    if (pFlashDriver == NULL) {
        DBGT_ERROR("Error: invalid flash driver handle.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Error: invalid flash driver handle.", (mTraceObject));
        DBC_ASSERT(0);
        return OMX_ErrorBadParameter;
    }

    t_sw3A_ISPSharedBuffer sharedBuffer;
    // pre-condition: CamSharedMemory::create();
    camSharedMemChunk_t tempChunk;
    camSharedMemError_t csm_err = mSharedMemory->getChunk(&tempChunk, CAM_SHARED_MEM_CHUNK_3A_STATS);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Error: Could not get 3A shared memory chunk.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not get 3A shared memory chunk.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    sharedBuffer.pData = tempChunk.armLogicalAddress;
    sharedBuffer.ispAddress = (t_uint32) tempChunk.ispLogicalAddress;
    sharedBuffer.size = tempChunk.size;
    (void) mSharedMemory->getBufHnd(&sharedBuffer.hwBufHnd);    // No need to test the return code if the chunk was already obtained.

    mFlashSequencer.setTimerCallbacks((sw3A_flashSeqTimerStart), (sw3A_flashSeqTimerStop), (flashSeqClbkCtxtHnd_t) this);
    mFlashController.setFlashDriver(pFlashDriver);
    mFlashController.updateSupportedFlashModes(camSlot);
    t_sw3A_FlashModeSet flashModesSet = mFlashController.getSupportedFlashModes();

    controlitf.open(ISPCTL_SIZE_TAB_PE, sharedBuffer, flashModesSet);
    mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_GLACE_STATS_READY, (sw3A_ispctlInfo), (t_ispctlEventClbkCtxtHnd) this);
    mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_AUTOFOCUS_STATS_READY, (sw3A_ispctlInfo), (t_ispctlEventClbkCtxtHnd) this);
    mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_FLADRIVER_LENS_STOP, (sw3A_ispctlInfo), (t_ispctlEventClbkCtxtHnd) this);

    // Allocate and initialize the local copy of dampers, makernotes and sw3A extradata.

    t_sw3A_Buffer *ptBuf = NULL;
    ptBuf = makernotesBuf.create(getMakernotesAllocSize());
    if (ptBuf == NULL) {
        DBGT_ERROR("Could not allocate local copy of makernotes.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not allocate local copy of makernotes.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    ptBuf = extradataBuf.create(getExtradataAllocSize());
    if (ptBuf == NULL) {
        DBGT_ERROR("Could not allocate local copy of extradata.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not allocate local copy of makernotes.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    ptBuf = captureParametersBuf.create(sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE));
    if (ptBuf == NULL) {
        DBGT_ERROR("Could not allocate local copy of CaptureParameters.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not allocate local copy of CaptureParameters.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    ptBuf = dampersDataBuf.create(sizeof(IFM_DAMPERS_DATA));
    if (ptBuf == NULL) {
        DBGT_ERROR("Could not allocate local copy of dampersData.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not allocate local copy of dampersData.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    ptBuf = flashStatusBuf.create(sizeof(OMX_STE_PRODUCTIONTESTTYPE));
    if (ptBuf == NULL) {
        DBGT_ERROR("Could not allocate local copy of flashStatus.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not allocate local copy of flashStatus.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    ptBuf = steExtraCaptureParametersBuf.create(sizeof(OMX_STE_CAPTUREPARAMETERSTYPE));
    if (ptBuf == NULL) {
        DBGT_ERROR("Could not allocate local copy of steExtraCaptureParametersBuf.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not allocate local copy of steExtraCaptureParametersBuf.", (mTraceObject));
        OUT0("\n");
        OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::open OMX_ErrorInsufficientResources", (mTraceObject));
        return OMX_ErrorInsufficientResources;
    }
    ptBuf = focusStatusBuf.create(IFM_CONFIG_EXTFOCUSSTATUS_SIZE(IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS));
    if (ptBuf == NULL) {
        MSG0("Could not allocate local copy of focusStatus.\n");
        OUT0("\n"); return OMX_ErrorInsufficientResources;
    }

    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::open (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::close()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::close", (mTraceObject));
    OMX_ERRORTYPE err = OMX_ErrorNone;
    // Check that there is no pending setConfig() in NMF queue.
    // It is not necessary to check the config shadow storage status as, by design, the usedCount cannot be zero when the shadow storage is not empty.
    if (mConfigQueue_usedCount > 0) {
        MSG0("There are pending setConfig() in SW3A NMF queue. Posponing the close().\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "There are pending setConfig() in SW3A NMF queue. Posponing the close().", (mTraceObject));
        bClosePending = true;
    } else {
        makernotesBuf.destroy();
        extradataBuf.destroy();
        captureParametersBuf.destroy();
        dampersDataBuf.destroy();
        flashStatusBuf.destroy();
        steExtraCaptureParametersBuf.destroy();
    focusStatusBuf.destroy();

        mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_GLACE_STATS_READY);
        mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_AUTOFOCUS_STATS_READY);
        mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_FLADRIVER_LENS_STOP);
        /* Fixme: SW3A close() shall not be called from hsmcamera as long as there is no way
         * to wait for an asynchronous event before destroying the sw3A NMF component at Idle->Loaded transition.
         * In the meantime, the method will be called from within sw3A at NMF stop time. */
        // controlitf.close();
        camSharedMemError_t csm_err = mSharedMemory->releaseChunk(CAM_SHARED_MEM_CHUNK_3A_STATS);
        // post-condition: CamSharedMemory::destroy();
        if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
            DBGT_ERROR("Could not release 3A shared memory chunk.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Could not release 3A shared memory chunk.", (mTraceObject));
            err = OMX_ErrorUndefined;
        }
    }
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::close (%d)", (mTraceObject), err);
    return err;
}

OMX_ERRORTYPE Sw3AProcessingComp::stop()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::stop", (mTraceObject));
    mFlashSeqTimer->stop_flush();
    mSW3A->stop_flush();
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::stop (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::start3A()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::start3A", (mTraceObject));
    MSG0("Kicking the 3A loop.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Kicking the 3A loop.", (mTraceObject));
    controlitf.startLoop(false, &fwStatus);  // Do not request a nofification as it cannot be used during the boot process.
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::start3A (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::stop3A()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::stop3A", (mTraceObject));
    MSG0("Stopping the 3A loop.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Stopping the 3A loop.", (mTraceObject));
    MSG0("\n timer TT_SW3A_STOP_REQ start\n");
    mSM->iTimed_Task[TT_SW3A_STOP_REQ].current_time = 0;
    mSM->iTimed_Task[TT_SW3A_STOP_REQ].timer_running = TRUE;
    controlitf.stopLoop(true);
    mSw3AStatStatus.afStatReceived = 0;
    mSw3AStatStatus.glaceStatReceived = 0;
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::stop3A (%d)", (mTraceObject), OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingComp::run3A(enum e_ispctlInfo info)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::run3A", (mTraceObject));
    MSG0("3A iteration.\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "3A iteration.", (mTraceObject));
    OMX_ERRORTYPE err = OMX_ErrorNone;
    // NB: statistics and capture live parameters are passed directly in the FW/SW shared memory
    switch(info) {
	  case  	ISP_SENSOR_COMMIT_NOTIFICATION:
	  	break;
        case ISP_ISP_COMMIT_NOTIFICATION:
		break;	
        case ISP_GLACE_STATS_READY:
            mSw3AStatStatus.glaceStatReceived++;
            controlitf.process(SW3A_STAT_EVENT_AEW);
            break;
        case ISP_AUTOFOCUS_STATS_READY:
            mSw3AStatStatus.afStatReceived++;
            controlitf.process(SW3A_STAT_EVENT_AF);
            break;
        default: // Unknown event. Do nothing.
            DBC_ASSERT(0);
            err = OMX_ErrorNotReady;
    }
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit Sw3AProcessingComp::run3A (%d)", (mTraceObject), err);
    return err;
}

OMX_S32 Sw3AProcessingComp::getMakernotesAllocSize()
{
    return (OMX_S32) syncitf.getMakenotesAllocSize();
}

/** Get access to the newest makernotes build by the SW_3A component.
 */
t_sw3A_Buffer * Sw3AProcessingComp::getPendingMakernotes()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getPendingMakernotes", (mTraceObject));
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::getPendingMakernotes", (mTraceObject));
    return makernotesBuf.get();
}

void  Sw3AProcessingComp::releasePendingMakernotes(t_sw3A_Buffer *pMakernotes)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::releasePendingMakernotes", (mTraceObject));
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::releasePendingMakernotes", (mTraceObject));
}

OMX_S32 Sw3AProcessingComp::getExtradataAllocSize()
{
    return (OMX_S32) syncitf.getExtradataAllocSize();
}

/** Get access to the newest makernotes build by the SW_3A component.
 */
t_sw3A_Buffer * Sw3AProcessingComp::getPendingExtradata()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getExtradataAllocSize", (mTraceObject));
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::getPendingExtradata", (mTraceObject));
    return extradataBuf.get();
}

void  Sw3AProcessingComp::releasePendingExtradata(t_sw3A_Buffer *pExtradata)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::releasePendingExtradata", (mTraceObject));
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::releasePendingExtradata", (mTraceObject));
}

/** Get access to the newest dampers bases values computed by SW_3A component.
 */
IFM_DAMPERS_DATA * Sw3AProcessingComp::getDampersData(bool bOnlyNew)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getDampersData", (mTraceObject));
    IFM_DAMPERS_DATA *pRet = NULL;
    t_sw3A_Buffer *pBuf = dampersDataBuf.get(bOnlyNew);
    if (pBuf != NULL) {
        DBC_ASSERT(pBuf->size == sizeof(IFM_DAMPERS_DATA));
        pRet = (IFM_DAMPERS_DATA *) pBuf->pData;
    }
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::getDampersData", (mTraceObject));
    return (IFM_DAMPERS_DATA *) pRet;
}

/** Get access to the newest CaptureParameters values computed by SW_3A component.
 */
OMX_SYMBIAN_CAPTUREPARAMETERSTYPE * Sw3AProcessingComp::getCaptureParameters(bool bOnlyNew)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getCaptureParameters", (mTraceObject));
    OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *pRet = NULL;
    t_sw3A_Buffer *pBuf = captureParametersBuf.get(bOnlyNew);
    if (pBuf != NULL) {
        DBC_ASSERT(pBuf->size == sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE));
        pRet = (OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *) pBuf->pData;
    }
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::getCaptureParameters", (mTraceObject));
    return pRet;
}

/** Get access to the newest steExtraCaptureParameters values computed by SW_3A component.
 */
OMX_STE_CAPTUREPARAMETERSTYPE * Sw3AProcessingComp::getSteExtraCaptureParameters(bool bOnlyNew)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::getSteExtraCaptureParameters", (mTraceObject));
    OMX_STE_CAPTUREPARAMETERSTYPE *pRet = NULL;
    t_sw3A_Buffer *pBuf = steExtraCaptureParametersBuf.get(bOnlyNew);
    if (pBuf != NULL) {
        DBC_ASSERT(pBuf->size == sizeof(OMX_STE_CAPTUREPARAMETERSTYPE));
        pRet = (OMX_STE_CAPTUREPARAMETERSTYPE *) pBuf->pData;
    }
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::getSteExtraCaptureParameters", (mTraceObject));
    return pRet;
}

/** Get access to the newest FocusStatus values computed by SW_3A component.
 */
OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE * Sw3AProcessingComp::getFocusStatus(bool bOnlyNew)
{
    IN0("\n");
    OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE *pRet = NULL;
    t_sw3A_Buffer *pBuf = focusStatusBuf.get(bOnlyNew);
    if (pBuf != NULL) {
        DBC_ASSERT(pBuf->size == IFM_CONFIG_EXTFOCUSSTATUS_SIZE(IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS));
        pRet = (OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE *) pBuf->pData;
    }
    OUT0("\n");
    return pRet;
}

/* tell coverity that it is normal that peList is passed by value, even if it has a huge size */
/* coverity[pass_by_value : FALSE] */
void Sw3AProcessingComp::done(t_sw3A_PageElementList peList, t_sw3A_LoopState e_State,
        t_sw3A_metadata * p_metadata, t_sw3A_FlashDirective *p_flashDirective, 
        t_sw3A_StillSynchro *p_stillSynchro)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::done", (mTraceObject));
    MSG0("Sw3AProcessingComp::done \n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Sw3AProcessingComp::done ", (mTraceObject));
    IMG_TIME_LOG(IMG_TIME_3A_DONE);

    if(p_metadata != NULL) { // Denotes that an AEW stats set has been completely processed.
        MSG0("Sw3AProcessingComp::done: Received metadata from sw3A.\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "Sw3AProcessingComp::done: Received metadata from sw3A.", (mTraceObject));
        t_sw3A_Buffer *ptBuf = NULL;
        if (p_metadata->makernotesBuf.pData != NULL) {
            ptBuf = makernotesBuf.set(&p_metadata->makernotesBuf);
            DBC_ASSERT(ptBuf);
        }
        if (p_metadata->sw3AextradataBuf.pData != NULL) {
            ptBuf = extradataBuf.set(&p_metadata->sw3AextradataBuf);
            DBC_ASSERT(ptBuf);
        }
        if (p_metadata->sw3AflashstatusBuf.pData != NULL) {
            ptBuf = flashStatusBuf.set(&p_metadata->sw3AflashstatusBuf);
            DBC_ASSERT(ptBuf);
        }
        if (p_metadata->captureParametersBuf.pData != NULL) {
            // Patch the 'orientation' extradata field which actual value comes from a 'setConfig'
            ((OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *)p_metadata->captureParametersBuf.pData)->eSceneOrientation = ((COmxCamera*)&mSM->mENSComponent)->mSceneOrientation.eOrientation;
            ptBuf = captureParametersBuf.set(&p_metadata->captureParametersBuf);
            //Update Focal length
            Camera* Cam = (Camera*)&(mSM->mENSComponent).getProcessingComponent();
            if(0 != ((OMX_STE_CAPTUREPARAMETERSTYPE*)p_metadata->steCaptureParametersBuf.pData)->nFocalLength)
            {
                Cam->mLensParameters.nFocalLength = ((OMX_STE_CAPTUREPARAMETERSTYPE*)p_metadata->steCaptureParametersBuf.pData)->nFocalLength;
            }
            DBC_ASSERT(ptBuf);
        }
        if(p_metadata->dampersDataBuf.pData != NULL) {
            ptBuf = dampersDataBuf.set(&p_metadata->dampersDataBuf);
            DBC_ASSERT(ptBuf);
        }
        //Added for extra capture data
        if (p_metadata->steCaptureParametersBuf.pData != NULL) {
            ptBuf = steExtraCaptureParametersBuf.set(&p_metadata->steCaptureParametersBuf);
            DBC_ASSERT(ptBuf);
        }
        if(p_metadata->focusStatusBuf.pData != NULL) {
            ptBuf = focusStatusBuf.set(&p_metadata->focusStatusBuf);
            DBC_ASSERT(ptBuf);
        }
        frame_counter = p_metadata->frame_counter;
    }

    if (p_flashDirective != NULL) {
        mFlashController.setDirective(p_flashDirective);
    }

    if (p_stillSynchro != NULL) {
        MSG2("Sw3AProcessingComp::done - stillSynchro bGrabAllowed=%d bStatsReceived=%d\n", (int) p_stillSynchro->bGrabAllowed, (int) p_stillSynchro->bStatsReceived);
        // NB: Convert from sint to bool.
        bStillSynchro_GrabAllowed = (p_stillSynchro->bGrabAllowed == 0) ? false : true;
        bStillSynchro_StatsReceived = (p_stillSynchro->bStatsReceived == 0) ? false : true;
    }
    
    // Save the PE list reference into the camera context so that it can be retrieved from the SW3A state.
    // SW3A_SYNC_LIST event is resynchronized by the COM_SM.
    // SW3A_ENQUEUE_LIST simply enqueues the passed list, which will be only actually issued once SW3A_COMMIT_LIST or SW3A_SYNC_LIST is received.
    // SW3A_COMMIT_LIST event is deferred.

    // Rely on the the metadata availability to distinguish between AEW and AF commands.
    Sw3APeListFlag_t last_flag = (p_metadata == NULL) ? SW3A_LIST_FLAG_LAST_AF : SW3A_LIST_FLAG_LAST_AEW;
    switch(e_State) {
        case SW3A_ENQUEUE_LIST:
            peListQueue.push(&peList, SW3A_LIST_FLAG_NONE);
            break;
        case SW3A_SYNC_LIST:
        {
            peListQueue.push(&peList, last_flag);

            s_scf_event devent;
            devent.sig = SW3A_LOOP_CONTROL_SIG;
            devent.type.other = 0;
            mSM->ProcessEvent(&devent);
            break;
        }
        case SW3A_COMMIT_LIST:
        {
            MSG0("Committing a group of PE lists.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "Committing a group of PE lists", (mTraceObject));
            peListQueue.push(&peList, last_flag);

            s_scf_event event;
            event.type.other=0;
            event.sig = SW3A_LOOP_CONTROL_SIG;
            mDeferredEventMgr->queuePriorEvent(&event);

            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            devent.type.other = 0;
            mSM->ProcessEvent(&devent);
            break;
        }
        default:
            DBC_ASSERT(0);
            break;
    }

    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::done", (mTraceObject));
}

// Done for flash production test
t_sw3A_Buffer* Sw3AProcessingComp::getFlashStatusData()
{
    return flashStatusBuf.get();
}

void Sw3AProcessingComp::info(t_sw3A_Msg msg, t_sw3A_MsgData msgData)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::info", (mTraceObject));
    MSG1("Sw3AProcessingComp::info %d\n", msg);
    OstTraceFiltStatic1(TRACE_DEBUG, "Sw3AProcessingComp::info %d", (mTraceObject), msg);
    mSw3AState = msg;
    t_sw3A_stat_status *p_SW3A_stat_status;
    switch(msg) {
        /*
        case SW3A_MSG_MODE_COMPLETE:*/
        case SW3A_MSG_START_COMPLETE:
            /*
             * Nobody is interested in this event for the moment
            setModeEvent.sig = SW3A_STARTED_SIG;
            mSM->ProcessEvent(&setModeEvent);
            */
            break;
	    case SW3A_MSG_STOP_COMPLETE:
    	{   
	    	MSG0("\n timer TT_SW3A_STOP_REQ Stop\n");
		    mSM->iTimed_Task[TT_SW3A_STOP_REQ].timer_running = FALSE;

		    s_scf_event event;
		    event.type.other=0;
		    event.sig = SW3A_STOPPED_SIG;
	    	mDeferredEventMgr->queuePriorEvent(&event);

	    	s_scf_event devent;
	    	devent.sig = Q_DEFERRED_EVENT_SIG;
	    	mSM->ProcessEvent(&devent);
	    	MSG0("Sw3AProcessingComp::info--SW3A_MSG_STOP_COMPLETE \n");
	    	break;
    	}

	/*For Shutterlag */
	 case SW3A_MSG_STOP_ACK:
        {
            s_scf_event event;
            event.type.other=0;
            event.sig = SW3A_STOP_ACK_SIG;
            mDeferredEventMgr->queuePriorEvent(&event);

            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            mSM->ProcessEvent(&devent);
            break;
        }
        case SW3A_MSG_OPEN_COMPLETE:
        {
            s_scf_event devent;
            devent.sig = SW3A_STARTED_SIG;
            mSM->ProcessEvent(&devent);
            break;
        }
        case SW3A_MSG_SETCONFIG_COMPLETE:
            if (mConfigQueue_usedCount <= 0) {
#ifdef SW3A_TRACE_CONFIGQUEUE
                printf("Sw3AProcessingComp::info - Error: notifing an end of setConfig, while none was requested.\n");
#endif /* SW3A_TRACE_CONFIGQUEUE */
            }
            DBC_ASSERT(mConfigQueue_usedCount > 0); // No reason for being notified if no setConfig was queued in NMF control interface.
            if (bClosePending) {
#ifdef SW3A_TRACE_CONFIGQUEUE
                printf("Sw3AProcessingComp::info - Close pending. Delayed because %d configs are still pending.\n", (int) mConfigQueue_usedCount);
#endif /* SW3A_TRACE_CONFIGQUEUE */
                mConfigQueue_usedCount --;
                if (mConfigQueue_usedCount == 0) {
                    MSG0("Sw3AProcessingComp::info - Finally calling close(), after all NMF-queued setConfig() are through.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "Sw3AProcessingComp::info - Finally calling close(), after all NMF-queued setConfig() are through.", (mTraceObject));
#ifdef SW3A_TRACE_CONFIGQUEUE
                    printf("Sw3AProcessingComp::info - Finally closing.\n");
#endif /* SW3A_TRACE_CONFIGQUEUE */
                    OMX_ERRORTYPE omxerr = OMX_ErrorUndefined;
                    omxerr = close();
                    if (omxerr != OMX_ErrorNone){
                        DBGT_ERROR("Sw3AProcessingComp::info close error %d\n",omxerr);
                        OstTraceFiltStatic1(TRACE_ERROR, "Sw3AProcessingComp::info close error %d", (mTraceObject),omxerr);
                        DBC_ASSERT(0);
                        return;
                    }
                    bClosePending = false;
                }
            } else {
                // In case a configuration was delayed due to the FIFO state, relay it now.
                Sw3A_Config_t *pConfig = mConfigStore.pop();
                if (pConfig != NULL) {
#ifdef SW3A_TRACE_CONFIGQUEUE
                    printf("Sw3AProcessingComp::info - relay setConfig %08x from config queue.\n", pConfig->index);
#endif /* SW3A_TRACE_CONFIGQUEUE */
                    controlitf.setConfig(pConfig->index, pConfig->pData);

                    pConfig->index = (OMX_INDEXTYPE) 0; // Only for debug purpose. The storage user is not supposed to modify the data by direct addressing.
                    pConfig->pData = NULL;              // Only for debug purpose. The storage user is not supposed to modify the data by direct addressing.
                } else {
                    mConfigQueue_usedCount--;
                }
            }
            break;
        case SW3A_MSG_AEC_TIMEOUT:
            setAecStateConverged(SW3A_AEC_CONVERGENCE_STATUS_TIMEOUT);
            break;
        case SW3A_MSG_AEC_CONVERGED:
            setAecStateConverged(SW3A_AEC_CONVERGENCE_STATUS_CONVERGED);
            break;
        case SW3A_MSG_OMXEVENT:
        {
            t_uint32 index = *((t_uint32 *) msgData);

            //To prevent metering to be set true from camera from loaded to idle
            if(index == OMX_STE_IndexConfigMeteringOn)
                ((COmxCamera*)&mSM->mENSComponent)->mMeteringOn.bEnabled = OMX_FALSE;

            //performance traces start
            Camera* Cam = (Camera*)&(mSM->mENSComponent).getProcessingComponent();
             if ((0 != Cam->latency_AFStart) && (OMX_Symbian_IndexConfigFocusLock == index)) {
                Cam->p_cam_sm->latencyMeasure(&(Cam->p_cam_sm->mTime));
                Cam->mlatency_Hsmcamera.e_OMXCAM_AF.t0 = Cam->p_cam_sm->mTime;
                OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_AF %d", 0);
                Cam->latency_AFStart = 0;
            }
             //performance traces end

            mSM->mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, OMX_ALL, index, NULL);
            break;
        }
        case SW3A_MSG_STAT_STATUS:
            p_SW3A_stat_status = (t_sw3A_stat_status *)msgData;
            if (p_SW3A_stat_status != NULL) {
                mSw3AStatStatus.bLensMoved = p_SW3A_stat_status->bLensMoved;
                mSw3AStatStatus.afMoveAndExportStatRequested = p_SW3A_stat_status->afMoveAndExportStatRequested;
                mSw3AStatStatus.afExportStatRequested = p_SW3A_stat_status->afExportStatRequested;
                mSw3AStatStatus.glaceStatRequested = p_SW3A_stat_status->glaceStatRequested;
            }
            break;
        default:
            MSG1("Error: received unsupported event %d\n", msg);
            OstTraceFiltStatic1(TRACE_DEBUG, "Error: received unsupported event %d", (mTraceObject), msg);
            //DBC_ASSERT(0);
            break;
    }
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::info", (mTraceObject));
}

/* timer_api_alarm callback */
/**/
void Sw3AProcessingComp::signal(void)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::signal", (mTraceObject));
    mFlashSequencer.sigTimer();
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::signal", (mTraceObject));
}


// Copy a sw3A-generated PE list into an COM_SM event for further routing.
void Sw3AProcessingComp::copyPeList(s_scf_event *pEvent, t_sw3A_PageElementList *peList)
{
    IN0("\n");
    DBC_ASSERT(peList->size <= IFM_SIZE_LIST_PE_WRITE)
    pEvent->type.writeStruct.nSize = peList->size;  // WARNING: here, the nSize field is used to pass the PE array size
    for (t_uint32 i=0; i < peList->size; i++) {
        pEvent->type.writeStruct.nPEList_addr[i] = peList->a_list[i].pe_addr;
        pEvent->type.writeStruct.nPEList_data[i] = peList->a_list[i].pe_data;
    }
}

bool Sw3AProcessingComp::isPreCaptureFlashNeeded() {
    return mFlashController.getDirective().bNeedPreCaptureFlash;
}
bool Sw3AProcessingComp::isPostCaptureFlashNeeded() {
    return mFlashController.getDirective().bNeedPostCaptureFlash;
}

void Sw3AProcessingComp::startPreCaptureFlashes() {
    cam_flash_err_e ferr = mFlashSequencer.startSeq(CFlashSequencer::RED_EYE_REMOVAL_SEQ, &mFlashController, (sw3A_flashSeqEnd), (flashSeqClbkCtxtHnd_t) this);
    DBC_ASSERT(ferr == CAM_FLASH_ERR_NONE);
}

void Sw3AProcessingComp::startPostCaptureFlashes() {
    /* NB: No expected callback: Do not bother with waiting for the end of the flash strobe to return to the HSM. */
    cam_flash_err_e ferr = mFlashSequencer.startSeq(CFlashSequencer::PRIVACY_SEQ, &mFlashController, 0, NULL);
    DBC_ASSERT(ferr == CAM_FLASH_ERR_NONE);
}

/** CSw3AOpaqueBuffer */
CSw3AOpaqueBuffer::CSw3AOpaqueBuffer(TraceObject *traceobj): mTraceObject(traceobj)
{
    buf.pData = NULL;
    buf.size = 0;
    bufMaxSize = 0;
    bIsNew = false;
}

t_sw3A_Buffer * CSw3AOpaqueBuffer::create(OMX_U32 size) {
    buf.pData = new t_uint8[size];
    if (buf.pData == NULL) {
        DBGT_ERROR("Error: allocating %d bytes buffer.\n", (int) size);
        OstTraceFiltStatic1(TRACE_ERROR, "Error: allocating %d bytes buffer.", (mTraceObject), (int) size);
        return NULL;
    } else {
        bufMaxSize = size;
        buf.size = 0;
        clear();
        return &buf;
    }
}

void  CSw3AOpaqueBuffer::destroy(void) {
    if (buf.pData != NULL) {
        delete[] buf.pData;
        buf.pData = NULL;
        buf.size = 0;
        bufMaxSize = 0;
    } else {
        DBGT_ERROR("Error: CSw3AOpaqueBuffer::destroy() tried to free a NULL pointer.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Error: CSw3AOpaqueBuffer::destroy() tried to free a NULL pointer.", (mTraceObject));
        DBC_ASSERT(0);
    }
}

void CSw3AOpaqueBuffer::clear(void) {
    memset(buf.pData, 0, bufMaxSize);
    buf.size = 0;
    bIsNew = false;
}

t_sw3A_Buffer * CSw3AOpaqueBuffer::set(t_sw3A_Buffer *pInBuf) {
    if ((pInBuf == NULL) || (pInBuf->pData == NULL) || (pInBuf->size > bufMaxSize)) {   //  Bad parameter, or too big a buffer.
        DBGT_ERROR("Error: CSw3AOpaqueBuffer::set(): Bad parameter or too big buffer.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Error: CSw3AOpaqueBuffer::set(): Bad parameter or too big a buffer.", (mTraceObject));
        return NULL;
    } else {    // Proceed
        memcpy(buf.pData, pInBuf->pData, pInBuf->size);                     // Copy the payload.
        memset(buf.pData + pInBuf->size, 0, bufMaxSize - pInBuf->size);     // Clear the rest of the buffer.
        buf.size = pInBuf->size;
        bIsNew = true;
        return &buf;
    }
}

t_sw3A_Buffer * CSw3AOpaqueBuffer::get(bool bIfNewOnly) {
    t_sw3A_Buffer *pRetBuf = NULL;
    if (bIfNewOnly) {   // Return the buffer only if the contents has been updated since last get(true).
        if (bIsNew) {
            bIsNew = false;
            pRetBuf = (buf.size > 0) ? &buf : NULL; // Do not return an empty buffer.
        }
    } else {
        pRetBuf = (buf.size > 0) ? &buf : NULL;     // Do not return an empty buffer.
    }
    return pRetBuf;
}

static void sw3A_flashSeqTimerStart(t_uint32 sleepTime_ms, flashSeqClbkCtxtHnd_t ctxtHnd) {
    Sw3AProcessingComp *sw3Acomp = (Sw3AProcessingComp *) ctxtHnd;
    sw3Acomp->flashtimeritf.startTimer(sleepTime_ms * 1000, 0); //  Alarm once after sleepTime_ms.
}

static void sw3A_flashSeqTimerStop(flashSeqClbkCtxtHnd_t ctxtHnd) {
    Sw3AProcessingComp *sw3Acomp = (Sw3AProcessingComp *) ctxtHnd;
    sw3Acomp->flashtimeritf.stopTimer();
}

/** Flash sequencer end of task callback implementation */
static void sw3A_flashSeqEnd(flashSeqClbkCtxtHnd_t ctxtHnd)
{
    ASYNC_IN0("\n");
    Sw3AProcessingComp *sw3Acomp = (Sw3AProcessingComp *) ctxtHnd;
    CAM_SM *camSm = sw3Acomp->getSM();

    s_scf_event devent;
    devent.sig = SW3A_PRECAPTURE_FLASH_END;
    devent.type.other = 0;
    camSm->ProcessEvent(&devent);
    ASYNC_OUT0("\n");
    return;
}

/** ISP info callback hook implementation */
static void sw3A_ispctlInfo(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd)
{
    ASYNC_IN0("\n");
    //OstTraceFiltStatic0(TRACE_FLOW, "Entry Sw3AProcessingComp::copyPeList", (mTraceObject));
    Sw3AProcessingComp *sw3Acomp = (Sw3AProcessingComp *) ctxtHnd;
    CAM_SM *camSm = sw3Acomp->getSM();
    CSelfTest *pSelfTest = camSm->pSelfTest;
    if(OMX_TRUE == pSelfTest->pTesting->bEnabled) {
        if (infoID == ISP_FLADRIVER_LENS_STOP) {
            s_scf_event event;
            event.sig = EVT_ISPCTL_INFO_SIG;
            event.type.ispctlInfo.info_id = infoID;
            camSm->ProcessEvent(&event);
        }
    } else {
        if (infoID != ISP_FLADRIVER_LENS_STOP) {
        IMG_TIME_LOG((infoID == ISP_AUTOFOCUS_STATS_READY) ? IMG_TIME_3A_STATS_AF_READY : IMG_TIME_3A_STATS_AEW_READY);
            (void) sw3Acomp->run3A(infoID);
        }
    }
    ASYNC_OUT0("\n");
    //OstTraceFiltStatic0(TRACE_FLOW, "Exit Sw3AProcessingComp::copyPeList", (mTraceObject));
    return;
}

#define STRINGIFY(mode) case mode: return #mode
const char* Sw3AProcessingComp::getCurrentSw3AStateStr()
{
    switch (mSw3AState) {
        STRINGIFY(SW3A_MSG_OK);
        STRINGIFY(SW3A_MSG_START_COMPLETE);
        STRINGIFY(SW3A_MSG_STOP_COMPLETE);
        STRINGIFY(SW3A_MSG_MODE_COMPLETE);
        STRINGIFY(SW3A_MSG_SETCONFIG_COMPLETE);
        STRINGIFY(SW3A_MSG_OMXEVENT);
        STRINGIFY(SW3A_MSG_OPEN_COMPLETE);
        STRINGIFY(SW3A_MSG_AEC_CONVERGED);
        STRINGIFY(SW3A_MSG_AEC_TIMEOUT);
        STRINGIFY(SW3A_MSG_ERROR_RESOURCE);
        STRINGIFY(SW3A_MSG_ERROR_STATE);
        STRINGIFY(SW3A_MSG_STOP_ACK);
        STRINGIFY(SW3A_MSG_STAT_STATUS);
        STRINGIFY(SW3A_MSG_ERROR_BAD_PARAMETER);
        default: return "Invalid sw3A state";
    }
}
