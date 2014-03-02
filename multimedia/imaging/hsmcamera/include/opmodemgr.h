/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _OPMODEMGR_H_
#define _OPMODEMGR_H_

#include "OMX_Index.h"
#include "ENS_Component.h"
#include "grabctl/api/configure.hpp"
#include "camport.h"
#include "IFM_Index.h"
#include "IFM_Types.h"
#include "osi_trace.h"
#include "tuning.h"
#include "tuning_data_base.h"


/* for traces */

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


#define CAMERA_NB_MODE_OP 11




#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CopModeMgr);
#endif
class CopModeMgr
{

    public :

        CopModeMgr(ENS_Component* pENSComp);
        void Init(t_uint16 rawCapturePortId, t_uint16 recordPortId, 
                camport *ViewfinderPort, camport *RawCapturePort, camport *RecordPort) {
            mViewfinderPort = ViewfinderPort;
            mRawCapturePort = RawCapturePort;
            mRecordPort = RecordPort;
        }

		void UpdateCurrentOpMode(void);
		OMX_ERRORTYPE Compute_NumberOfBuffersForStill(void);
		void SetBurstFrameLimit(t_uint32 frameLimit)    { u32_frameLimit = frameLimit; };
		void SetBurstLimited(bool bLimited)             { bBurstFrameLimited = bLimited; };
		void SetBracketingCount(t_uint32 count)         { u32_bracketLimit = count; };
		t_uint32 GetBurstFrameLimit()                   { return u32_frameLimit; };
		bool IsBurstLimited()                           { return bBurstFrameLimited; };
		void SetFrameBefore(OMX_U32 frameBefore)        { nFrameBefore = frameBefore; }
		void SetPrepareCapture(OMX_BOOL prepareCapture) { bPrepareCapture = prepareCapture; }
		OMX_U32 GetFrameBefore(void)                    { return nFrameBefore; }
		OMX_BOOL GetPrepareCapture(void)                { return bPrepareCapture; }
		OMX_BOOL IsTimeNudgeEnabled(void);
		t_uint32 GetBurstNberOfFrames(void);

		void SetTuningDataBase(CTuningDataBase*);
		CTuning* GetOpModeTuning();

		t_uint32 GetFrameLimit();
		bool IsLimited();

		t_operating_mode_camera CurrentOperatingMode;

		/* number of frames grabbed since VF */
        /* number of frames grabbed in burst on user point of view */
        /* linked to FrameLimit */
		t_uint16 UserBurstNbFrames;
        /* number of frames grabbed since last BML (or VF if ispproc has not processed buffers yet) */
        /* number of frames grabbed in burst on camera point of view */
        /* linked to NumberOfBuffersForStill */        
		t_uint16 CameraBurstNbFrames;
        
		/* number of buffers that are used for current still / camera burst */
		t_uint8 NumberOfBuffersForStill; 

		/* in infinite burst still capture */
		/* true if grab was stopped but not 3A */
		OMX_BOOL NeedToStop3A;

		/* count how many StillCapture are ongoing */
		t_uint16 captureRequest[CAMERA_NB_OUT_PORTS];

		/* says if one streaming is done */
		OMX_BOOL isEOS[CAMERA_NB_OUT_PORTS];
		/* says if we have to wait for an EOS */
		OMX_BOOL waitingEOS[CAMERA_NB_OUT_PORTS];

		/* configure the CaptureOrRecord : ConfigCapturing/ConfigCapturePort
		 * allows to know in which port the IndexConfigCapturing has been triggered thus
		 * allows to know if we are recording or Capturing */
		OMX_BOOL ConfigCapturing[CAMERA_NB_OUT_PORTS];
		t_uint16 ConfigCapturePort;
		t_operating_mode_camera PreviousOperatingMode;
		t_uint16 nBufferAtMPC[CAMERA_NB_PORTS];
		
		/* Coming from OM API needed in core */
		OMX_BOOL AutoPauseAfterCapture;

        /* get width required for allocating grab cache. This depends 
        on opmode, as width can be from HR or LR */
        t_uint16 getGrabCacheWidth();
        /* get LR and 'HR' ports for configuring stab resolutions. Depending on opmode, HR 
        may actually be LR
        TODO: refactor: stab should only really care about LR and 'HR' resolutions, not whole ports */
        void getStabPorts(camport **portVF, camport **portRecord, OMX_U32 *HRColorFormat);
        void getPorts(camport **portVF, camport **portRecord);
        OMX_BOOL mStabEnabled;
        OMX_BOOL nWaitingForBuffer[CAMERA_NB_OUT_PORTS];
        const char* getOpModeStr();

    private:
        CTuningDataBase* pTuningDataBase;
        camport *mViewfinderPort;
        camport *mRawCapturePort;
        camport *mRecordPort;

    private:
        /* maybe redirected to a CaptureRecordMgr*/
        /* configure the StillCapture (burst or single) */
        t_uint32 u32_frameLimit;
        t_uint32 u32_bracketLimit;
        bool bBurstFrameLimited;
        OMX_U32 nFrameBefore;
        OMX_BOOL bPrepareCapture;
        ENS_Component* pENSComponent;
};


#endif
