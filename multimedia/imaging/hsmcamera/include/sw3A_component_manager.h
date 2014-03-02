/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SW3A_COMPONENT_MANAGER_H_
#define _SW3A_COMPONENT_MANAGER_H_

#include "OMX_Core.h"
#include "trace.h"

#include <cm/inc/cm.hpp>
#include "sw_3A_wrp.hpp"
#include "timer/timerWrapped.hpp"

#include "deferredeventmgr.h"
#include "ispctl_eventclbk.h"
#include "sensor.h"     // For enumCameraSlot
#include "cam_shared_memory.h"
#include "flash.h"
#include "damper_base.h"

class CAM_SM;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(Sw3AProcessingComp);
#endif

/* SEMC_BEGIN (making semc extension) */
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
#define SW3A_PELISTS_QUEUE_SIZE   32   // Hard-coded, SEMC needs more queue .
#else
#define SW3A_PELISTS_QUEUE_SIZE   10   // Hard-coded, as we do not expect to have more queued lists
#endif //CAMERA_ENABLE_OMX_3A_EXTENSION
/* SEMC_END (making semc extension) */
#define SW3A_CONFIGQUEUE_SIZE     25    // Do not allow too many consecutive setConfig() calls in the NMF "control" interface queue. Shall be large enough to convey the complete list of initialization configs in a raw, however.
#define SW3A_CONFIGSTORE_SIZE     30   // Up to SW3A_CONFIGSTORE_SIZE different OMX configuration can be simultaneously referenced for deferred passing to SW3A.

typedef enum {
   SW3A_AEC_CONVERGENCE_STATUS_NOT_CONVERGED,
   SW3A_AEC_CONVERGENCE_STATUS_CONVERGED,
   SW3A_AEC_CONVERGENCE_STATUS_TIMEOUT
} Sw3AAecConvergenceStatus_t;

typedef enum {
    SW3A_LIST_FLAG_NONE = 0,
    SW3A_LIST_FLAG_LAST_AF,
    SW3A_LIST_FLAG_LAST_AEW
} Sw3APeListFlag_t;

typedef struct {
    t_sw3A_PageElementList list;
    Sw3APeListFlag_t flag; // Indicates whether the list is the last one of the group (corresponds to one and only one SW3A_LOOP_CONTROL_SIG event).
} Sw3APeList_t;

typedef struct {
    OMX_INDEXTYPE index;
    OMX_PTR pData;
} Sw3A_Config_t;

class CSw3APeListQueue
{   // NB: * The thread protection is guaranteed by NMF.
    //     * There is no overflow or underflow detection.
    public:
        CSw3APeListQueue() { reset(); }
        // Push by value.
        void push(t_sw3A_PageElementList *pPeList, Sw3APeListFlag_t flag) {
            a_lists[write % SW3A_PELISTS_QUEUE_SIZE].flag = flag;
            a_lists[write++ % SW3A_PELISTS_QUEUE_SIZE].list = *pPeList;
        }
        // Pop by reference.
        Sw3APeList_t * pop(void) { return ((read == write) ? NULL : &a_lists[read++ % SW3A_PELISTS_QUEUE_SIZE]); }
        // Reset the passed list (advisable for debug purpose)
        static void clear(Sw3APeList_t *pPeList) { memset(pPeList, 0, sizeof(Sw3APeList_t)); }
        void reset(void) {
            write = 0;  read = 0;
            for (int i=0; i < SW3A_PELISTS_QUEUE_SIZE; i++) {
                clear(&a_lists[i]);
            }
        }
    private:
        Sw3APeList_t a_lists[SW3A_PELISTS_QUEUE_SIZE];    // Circular buffer
        int write;
        int read;
};

class CSw3AConfigStore
{
    public:
        CSw3AConfigStore() { reset(); }
        // Push by reference. The camera proxy must ensure that the data are persistent in memory until the 3A component is stopped.
        void push(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) {
            // Look for a pending config for the same index
            int slot = -1;
            for (int i=read; (i < write) && (slot == -1); i++) {
                if (a_configs[i].index == nParamIndex) {
                    slot = i;
                }
            }
            if (slot >= 0) {
                // Replace the existing slot
                a_configs[slot].pData = pComponentParameterStructure;
            } else {
                // Store at a new slot position
                if ((write - read) >= SW3A_CONFIGSTORE_SIZE) {   // NB: potential false positive if write > 2^31
                    MSG1("CSw3AConfigStore::push - Error: config storage overflow: discarding index %08x\n", nParamIndex);
                    DBC_ASSERT(0);
                }
                a_configs[write % SW3A_CONFIGSTORE_SIZE].index = nParamIndex;
                a_configs[write++ % SW3A_CONFIGSTORE_SIZE].pData = pComponentParameterStructure;
            }
        }
        Sw3A_Config_t * pop(void) { return ((read == write) ? NULL : &a_configs[read++ % SW3A_CONFIGSTORE_SIZE]); }
        void reset(void) {
            write = 0;  read = 0;
            for (int i=0; i < SW3A_CONFIGSTORE_SIZE; i++) {
                a_configs[i].index = (OMX_INDEXTYPE) 0;
                a_configs[i].pData = NULL;
            }
        }

    private:
        Sw3A_Config_t a_configs[SW3A_CONFIGSTORE_SIZE];    // Circular buffer
        int write;
        int read;
};

class CSw3AOpaqueBuffer
{
    public:
        CSw3AOpaqueBuffer(TraceObject *traceobj);
        t_sw3A_Buffer * create(OMX_U32 size);
        void destroy(void);
        void clear(void);
        t_sw3A_Buffer *  set(t_sw3A_Buffer *pInBuf);
        t_sw3A_Buffer * get(bool bIfNewOnly = false);
	
	TraceObject* mTraceObject;
    private:
        OMX_U32 bufMaxSize;
        t_sw3A_Buffer buf;
        bool bIsNew;
};

class Sw3AProcessingComp :
    public sw_3A_api_doneDescriptor,
    public timer_api_alarmDescriptor
{
   public:
      Sw3AProcessingComp(TraceObject *traceobj);

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
      virtual OMX_ERRORTYPE instantiate(OMX_HANDLETYPE omxhandle);
      virtual OMX_ERRORTYPE deInstantiate(OMX_HANDLETYPE omxhandle);

      virtual OMX_ERRORTYPE open(enumCameraSlot camSlot, CFlashDriver *pFlashDriver);
      virtual OMX_ERRORTYPE close();
      virtual OMX_ERRORTYPE start();
      virtual OMX_ERRORTYPE stop();

      virtual OMX_ERRORTYPE start3A();
      virtual OMX_ERRORTYPE stop3A();

      virtual void setSM(CAM_SM *pSM);
      virtual CAM_SM * getSM();
      virtual void setDeferredEventManager(CDefferedEventMgr *pDeferredEventMgr);
      virtual void setIspCtlClbks(CIspctlEventClbks *pIspCtlEventClbks);
      virtual void setSharedMem(CCamSharedMemory *pSharedMemory);
      virtual void setFramerate(t_uint32 maxframerate_x100,t_uint32 minframerate_x100 , bool bFixedFramerate);
      virtual void setAecStateConverged(Sw3AAecConvergenceStatus_t aecConvergenceStatus);
      
      virtual Sw3AAecConvergenceStatus_t getAecStateConverged();
      virtual bool isStillModeOn();
      virtual bool isPreCaptureFlashNeeded();
      virtual bool isPostCaptureFlashNeeded();
      virtual void startPreCaptureFlashes();
      virtual void startPostCaptureFlashes();

      virtual OMX_ERRORTYPE setMode(t_sw3A_LoopState loopState, t_uint32 numberOfStats);

      virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
      virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
      virtual OMX_ERRORTYPE run3A(enum e_ispctlInfo info);

      virtual OMX_S32 getMakernotesAllocSize();
      virtual OMX_S32 getExtradataAllocSize();
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
      OMX_ERRORTYPE instantiate(OMX_HANDLETYPE omxhandle);
      OMX_ERRORTYPE deInstantiate(OMX_HANDLETYPE omxhandle);
      OMX_ERRORTYPE open(enumCameraSlot camSlot, CFlashDriver *pFlashDriver);
      OMX_ERRORTYPE close();
      OMX_ERRORTYPE start();
      OMX_ERRORTYPE stop();

      OMX_ERRORTYPE start3A();
      OMX_ERRORTYPE stop3A();

      void setSM(CAM_SM *pSM);
      CAM_SM * getSM();
      void setDeferredEventManager(CDefferedEventMgr *pDeferredEventMgr);
      void setIspCtlClbks(CIspctlEventClbks *pIspCtlEventClbks);
      void setSharedMem(CCamSharedMemory *pSharedMemory);
      void setFramerate(t_uint32 maxframerate_x100,t_uint32 minframerate_x100 , bool bFixedFramerate);
      void setAecStateConverged(Sw3AAecConvergenceStatus_t aecConvergenceStatus);
      
      Sw3AAecConvergenceStatus_t getAecStateConverged();
      bool isStillModeOn();
      bool isPreCaptureFlashNeeded();
      bool isPostCaptureFlashNeeded();
      void startPreCaptureFlashes();
      void startPostCaptureFlashes();

      OMX_ERRORTYPE setMode(t_sw3A_LoopState loopState, t_uint32 numberOfStats);

      OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
      OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
      OMX_ERRORTYPE run3A(enum e_ispctlInfo info);

      OMX_S32 getMakernotesAllocSize();
      OMX_S32 getExtradataAllocSize();
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
      t_sw3A_Buffer * getPendingMakernotes();
      t_sw3A_Buffer * getPendingExtradata();
      void releasePendingMakernotes(t_sw3A_Buffer *pMakernotes);
      void releasePendingExtradata(t_sw3A_Buffer *pMakernotes);
      t_sw3A_Buffer * getFlashStatusData();// Done for flash production test
      IFM_DAMPERS_DATA * getDampersData(bool bOnlyNew);
      OMX_SYMBIAN_CAPTUREPARAMETERSTYPE * getCaptureParameters(bool bOnlyNew);
      OMX_STE_CAPTUREPARAMETERSTYPE * getSteExtraCaptureParameters(bool bOnlyNew);
      OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE * getFocusStatus(bool bOnlyNew);

      virtual void done(t_sw3A_PageElementList peList, t_sw3A_LoopState e_State, t_sw3A_metadata *p_metadata, t_sw3A_FlashDirective *p_flashDirective, t_sw3A_StillSynchro *p_stillSynchro); // sw_3A_api_doneDescriptor callback implementation
      virtual void info(t_sw3A_Msg msg, t_sw3A_MsgData msgData);
      virtual void signal(void);
        const char* getCurrentSw3AStateStr();
      Itimer_api_timer flashtimeritf;   // Must be public because it is used from static callback handlers
      CFlashController mFlashController;
      CFlashSequencer mFlashSequencer;
      SCF_PSTATE whatNextAfterStart; /* Worth a getter&setter */
      SCF_PSTATE whatNextAfterStop;  /* Worth a getter&setter */

      SCF_PSTATE whatNextAfterRendezvous; /*For Shutterlag */	  

      CSw3APeListQueue peListQueue;
      t_sw3A_PageElementList    fwStatus; // List of PE's containing initial coin values that sw3A component will use.
      bool                      bStillSynchro_StatsReceived;
      bool                      bStillSynchro_GrabAllowed;
      bool                      bStillSynchro_GrabStarted;
      bool                      bReachedLastOfListGroup;
      bool                      bClosePending;   // Means that the sw3A component manager has to wait that the setConfig() queue is empty before actually closing the 3A lib.
	  t_sw3A_LoopState lastsw3AloopState;

      t_uint32 frame_counter;

      /* Temporary context for SW3A hsm */
      bool bHasToStopBeforeGammaChange;
      bool bLUTWritten;
       t_uint32 Gamma_Flag;//To check if piperunning?
       t_uint32 gamma_LR_HR_done;
      TraceObject* mTraceObject;
      bool isFixedFramerate () { return mbFixedFramerate; }
        t_sw3A_stat_status mSw3AStatStatus;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    protected:
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
    private:
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
        OMX_ERRORTYPE isConfigAvailable(OMX_INDEXTYPE pIndex, OMX_PTR pComponentParameterStructure);
        static void copyPeList(s_scf_event *pEvent, t_sw3A_PageElementList *peList);

        sw_3A_wrp *mSW3A;
        Isw_3A_api_control controlitf;
        Isw_3A_api_sync    syncitf;

        timerWrapped *mFlashSeqTimer;

        CSw3AOpaqueBuffer makernotesBuf;
        CSw3AOpaqueBuffer extradataBuf;
        CSw3AOpaqueBuffer captureParametersBuf;
        CSw3AOpaqueBuffer dampersDataBuf;
        CSw3AOpaqueBuffer flashStatusBuf;         /* Used in flash production test */
        CSw3AOpaqueBuffer steExtraCaptureParametersBuf;
        CSw3AOpaqueBuffer focusStatusBuf;

        CSw3AConfigStore mConfigStore;

        t_uint32 mMaxFramerateRequested_x100;
	 t_uint32 mMinFramerateRequested_x100;
        bool mbFixedFramerate;
        t_sint32 mConfigQueue_usedCount;

        Sw3AAecConvergenceStatus_t mAecConvergenceStatus;
        /** When the 3A component manager is in Still mode, it sets the bStillSynchro_StatsReceived flag
         * upon metadata reception in the done() callback interface, so that they can be synchronized with the BMS buffer. */
        bool mbStillModeOn;

        // Reference to camera objects required to build and submit PE lists, access the XP70 shared memory and communicate with the flash device.
        CAM_SM *mSM;
        CDefferedEventMgr *mDeferredEventMgr;
        CIspctlEventClbks *mIspCtlEventClbks;
        CCamSharedMemory  *mSharedMemory;
        /* Store the current sw3A state */
        t_sw3A_Msg mSw3AState;
};

#endif // _SW3A_COMPONENT_MANAGER_H_

