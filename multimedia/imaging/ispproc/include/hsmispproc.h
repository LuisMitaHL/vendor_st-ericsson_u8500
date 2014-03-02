/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISPPROC_SM_H_
#define _ISPPROC_SM_H_

#include "hsm.h"
#include "resource_sharer_manager.h"
#include "memgrabctl.h"
#include "ispprocport.h"
#include "tuning_data_base.h"
#include "isp_dampers.h"
#include "picture_settings.h"
#include "sensor.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(ISPPROC_SM);
#endif
class ISPPROC_SM : public COM_SM {

public:
	ISPPROC_SM(ENS_Component &enscomp):COM_SM(enscomp) {
		captureRequest[ISPPROC_PORT_IN0]=0;
		captureRequest[ISPPROC_PORT_OUT0]=0;
		captureRequest[ISPPROC_PORT_OUT1]=0;
		isTransitionDone=OMX_FALSE;
		pRotation = NULL;
		bExtradataRead = 0;
		pAutoRotation = NULL;
		pPreset = 1;
	}
	CResourceSharerManager * pResourceSharerManager;
	CMemGrabControl * pMemGrabControl;

	t_uint16 captureRequest[ISPPROC_NB_PORTS];//should  be an OpmodeMgr
	OMX_BOOL isTransitionDone;

	CTuningDataBase* pTuningDataBase;
	CIspDampers* pIspDampers;
	CPictureSettings* pPictureSettings;
	int* pRotation;
	bool bExtradataRead;				//check if already the rotation information is not extracted(hence no need to retrieve current frame extradata)
	OMX_BOOL* pAutoRotation;
	OMX_BOOL* pPortSettingChanged;
	int pPreset;


protected:

	virtual SCF_STATE ProcessPendingEvents(s_scf_event const *e);

/* ------------------ */
/* Boot state machine */
/* ------------------ */

		virtual SCF_STATE Booting(s_scf_event const *e);
		SCF_STATE BootingStart(s_scf_event const *e);




/* --------------- */
/* Operating modes */
/* --------------- */

		virtual SCF_STATE EnteringPreview(s_scf_event const *e);


		SCF_STATE EnteringRawCapture(s_scf_event const *e) ;
        SCF_STATE EnteringRawCapture_StoppingVpipForStripe(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_DoingRawCaptureConfiguration(s_scf_event const *e) ;
		SCF_STATE EnteringRawCapture_DoingBMLConfiguration(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_ApplyIQSettings(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_ApplyCaptureContext_DampedIQSettings(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_ApplyCaptureContext_AWBSettings(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_ApplyCaptureContext_CE1(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_ApplyCaptureContext_CE0(s_scf_event const *e);
		
		//Added for Extra Capture Parameters
		SCF_STATE EnteringRawCapture_ApplyCaptureContext_GammaLutSetting(s_scf_event const * e);
		SCF_STATE EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting(s_scf_event const * e);
		
		SCF_STATE EnteringRawCapture_Start(s_scf_event const *e) ;
		SCF_STATE EnteringRawCapture_SendBuffers(s_scf_event const *e) ;
		SCF_STATE EnteringRawCapture_ConfigureMemGrab(s_scf_event const *e) ;
		SCF_STATE ExitingRawCapture(s_scf_event const *e) ;
		SCF_STATE ExitingRawCapture_StoppingVpip(s_scf_event const *e);
		SCF_STATE ExitingRawCapture_FreeIspProc(s_scf_event const *e) ;



/* ---------------*/
/* Enabling Ports */
/* ---------------*/
		virtual SCF_STATE	EnablePortInIdleExecutingState(s_scf_event const *e);


		SCF_STATE Freed(s_scf_event const *e);
		SCF_STATE Freed_StateMachine(s_scf_event const *e);



/* Methods that need to be overriden for configuring pipes */
		virtual OMX_ERRORTYPE configureIspResolution(t_uint8);
		virtual OMX_ERRORTYPE configureResolution(void);
		virtual OMX_ERRORTYPE configurePipes(void);
		virtual void SW3A_start(SCF_PSTATE whatNext);
};

#endif

