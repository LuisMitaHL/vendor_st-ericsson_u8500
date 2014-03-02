/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 /** @file common_sm.h
    @brief This file declares  all state transitions
    that are common to ISP and Camera state machines
*/



#ifndef _COMMON_SM_
#define _COMMON_SM_

#include "qhsm.h" //rename_me "scf_api.h"
#include "qf_os.h"
#include "qevent.h"
#include "ispctl_communication_manager.h"
#include "deferredeventmgr.h"
#include "trace.h"
#include "ENS_Component.h"
#include "sensor.h"
#include "ispctl_component_manager.h"
#include "osi_trace.h"
#include "omx_state_manager.h"
#include "VhcElementDefs.h"
#include <cm/inc/cm.hpp>
#include "host/sendcommand.hpp"
#include "NmfMpc_ProcessingComponent.h"
#include "timed_task.h"


#ifdef __CAMERA_TRACE_ALL
#define ENTRY MSG1("%s-SCF_STATE_ENTRY_SIG\n",__FUNCTION__);
#define EXIT MSG1("%s-SCF_STATE_EXIT_SIG\n",__FUNCTION__);
#define SIG(sig) MSG2("%s-%s\n",__FUNCTION__,#sig);
#define INIT MSG1("%s-SCF_STATE_INIT_SIG\n",__FUNCTION__);
#else
#define ENTRY
#define EXIT
#define INIT
#endif


#define SM_PUSH_STATE(_a_) { bool err = pushNextState((_a_)); DBC_ASSERT(err == false); }
#define SM_PUSH_STATE_STATIC(_a_) SM_PUSH_STATE(SCF_STATE_PTR((_a_)));
#define SM_POP_STATE(_a_)  { (_a_) = popNextState();  }





#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(COM_SM);
#endif
class COM_SM : public CScfStateMachine {

public:
	COM_SM(ENS_Component &enscomp);
	virtual ~COM_SM();

	OMX_BOOL isExpectedState() {
        if STATE_IS_ACTIVE(&COM_SM::WaitEvent)
            return OMX_TRUE;
        else 
            return OMX_FALSE;
	}
    
    void printStatus();

protected:
	virtual bool timeoutAction (void);
	virtual const char *get_state_str(SCF_PSTATE state) { return "Unknown" ; }
	Transform_te colorPrimary2TransformType(OMX_SYMBIAN_COLORPRIMARYTYPE aColorPrimary);
	OutputFormat_te convertFormatForISP(OMX_COLOR_FORMATTYPE omxformat);
	bool pushNextState(SCF_PSTATE next);
	SCF_PSTATE popNextState();

	SCF_STATE  initial(s_scf_event const *e);
	SCF_STATE  final(s_scf_event const *e);


/* shared state hierarchy between imaging components */
/* --------------------------------------------------*/
	SCF_STATE OMX_Executing(s_scf_event const *e);
		SCF_STATE Streaming(s_scf_event const *e);
			SCF_STATE WaitEvent(s_scf_event const *e);
				virtual SCF_STATE ProcessPendingEvents(s_scf_event const *e);


/* pure virtual allowing to start the state machine : this is the entry point */
/* ---------------------------------------------------------------------------*/
		virtual SCF_STATE Booting(s_scf_event const *e)=0;

/* pure virtual allowing to describe behaviour when ports are asked to be disabled or enabled*/
/* ------------------------------------------------------------------------------------------*/
		virtual SCF_STATE	EnablePortInIdleExecutingState(s_scf_event const *e) = 0;


/* state handler allowing to describe the operating mode transition : somehow the Booting state handler will have to connect to them */
/* ----------------------------------------------------------------------------------------------------------------------------------*/
		SCF_STATE OpModeTransition(s_scf_event const *e);
			SCF_STATE EnteringOpMode(s_scf_event const *e);
			SCF_STATE ExitingOpMode(s_scf_event const *e);
				virtual SCF_STATE EnteringPreview(s_scf_event const *e) = 0;


/* pure virtual allowing to implement the img components features*/
/* --------------------------------------------------------------*/
		virtual SCF_STATE SetFeature(s_scf_event const *e);




/* common qstates */
/* ---------------*/
		SCF_STATE SHARED_Error_handling(s_scf_event const *e);


			SCF_STATE SHARED_ControlingISPState_WaitStopped(s_scf_event const *e);
			SCF_STATE SHARED_ControlingISPState_WaitRuning(s_scf_event const *e);
			SCF_STATE SHARED_ControlingISPState_CheckStreaming(s_scf_event const *e);
			SCF_STATE SHARED_ControlingISPState_PowerUp(const s_scf_event*);
			SCF_STATE ControlingISPState_WaitPrepared(s_scf_event const *e);
			virtual SCF_STATE PowerUp_STC(const s_scf_event*);
			virtual SCF_STATE SW3A_Start(const s_scf_event*);

		SCF_STATE SHARED_ConfiguringISPPipe(s_scf_event const *e);
			SCF_STATE SHARED_ConfiguringISPPipe_FormatResolution(s_scf_event const *e);
			SCF_STATE SHARED_ConfiguringISPPipe_Ena_Disa(s_scf_event const *e);
			/*allow to specialize the behaviour for configuring the pipes */
			virtual OMX_ERRORTYPE configureIspResolution(t_uint8)=0;
			virtual OMX_ERRORTYPE configureResolution(void)=0;
			virtual OMX_ERRORTYPE configurePipes(void)=0;


		SCF_STATE SHARED_Debugging(s_scf_event const *e);
			SCF_STATE SHARED_DebuggingGetorWriteListPe(s_scf_event const *e);


		SCF_STATE WriteI2C(s_scf_event const *e);
			SCF_STATE WriteI2C_Do(s_scf_event const *e);


public :
		SCF_STATE SHARED_ControlingISPState(s_scf_event const *e);
		SCF_STATE SHARED_ControlingISPState_GetControlType(s_scf_event const *e);
		SCF_STATE SHARED_ControlingISPState_Boot(s_scf_event const *e);	
		SCF_STATE SHARED_ControlingISPState_Sleep(s_scf_event const *e);
		SCF_STATE SHARED_ControlingISPState_WakeUp(s_scf_event const *e);
		SCF_STATE SHARED_ControlingISPState_Stop(s_scf_event const *e);
		SCF_STATE SHARED_ControlingISPState_Run(s_scf_event const *e);
		SCF_STATE SHARED_ControlingISPState_ReadHostInterfaceStatus(s_scf_event const *e);
	
		virtual void SW3A_start(SCF_PSTATE whatNext);
		virtual void SW3A_stop(SCF_PSTATE whatNext);


		/* CAUTION!!! */
		t_uint8 enabledPort;

		//performance traces start
		//IFM_LATENCY_HSMCAMERA mlatency_Hsmcamera;	/*latency values for hsmcamera in this structure*/
	    IFM_TIME mTime;								/*to record the time at any given instance*/
	    IFM_TIME mTime_e_OMXCAM_StartStreaming_t0;	/*to record the time for e_OMXCAM_StartStreaming_t0 event*/
	    IFM_TIME mTime_e_OMXCAM_StartStreaming_t1;	/*to record the time for e_OMXCAM_StartStreaming_t1 event*/
	    IFM_TIME mTime_e_OMXCAM_StopISP_t0;			/*record the time for e_OMXCAM_StopISP_t0 event*/
	    IFM_TIME mTime_e_OMXCAM_StopISP_t1;			/*record the time for e_OMXCAM_StopISP_t1 event*/
	    //IFM_LATENCY_ISPPROC mlatency_Ispproc;		/*latency values for ispproc in this structure*/
	    void latencyMeasure(IFM_TIME* pTime);
	    //performance traces end

		CDefferedEventMgr * pDeferredEventMgr;

		CIspctlCom * pIspctlCom;
		CTrace * pTrace;
		COmxStateMgr *pOmxStateMgr;

		ENS_Component& mENSComponent;
		t_controling_isp_ctrl_type ControlingISP_ControlType;
		t_configuring_isp_pipe ConfiguringISPPipe_ConfigType;
        OMX_BOOL bTrace_latency;

		CComI2C oComI2C;
		OMX_TIME_TT iTimed_Task[TT_MAX];
/*For Shutterlag*/	
		t_isp_state isp_state; 
		
private:
        SCF_PSTATE whatNext;
};

#endif  /* _COMMON_SM_*/

