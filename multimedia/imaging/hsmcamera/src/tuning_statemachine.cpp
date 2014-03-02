/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h"
#include "osi_trace.h"
#include "ImgConfig.h"

#include "VhcElementDefs.h"
#include "host/ispctl_types.idt.h"
#include "ispctl_communication_manager.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_tuning_statemachineTraces.h"
#endif

#include "camera.h"

/* */
/* LoadingIQSettings state machine */
/**/
SCF_STATE CAM_SM::CAMERA_LoadingIQSettings(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:  ENTRY; return 0;
	   case SCF_STATE_EXIT_SIG:  EXIT; return 0;
	   case SCF_STATE_INIT_SIG:  INIT; SCF_INIT(&CAM_SM::CAMERA_LoadingIQSettings_OpModeSetup); return 0;
	   case Q_DEFERRED_EVENT_SIG:
		   MSG0("LoadingIQSettings-DEFERRED_EVENT;\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "LoadingIQSettings-DEFERRED_EVENT;", (&mENSComponent));
		   return 0;
	   case SCF_PARENT_SIG:break;
	   default:
		   MSG1("LoadingIQSettings-default %d\n",e->sig);
		   OstTraceFiltStatic1(TRACE_DEBUG, "LoadingIQSettings-default %d", (&mENSComponent),e->sig);
		   break;
   }
   return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}

SCF_STATE CAM_SM::CAMERA_LoadingIQSettings_OpModeSetup(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			// Don't need to reconfigure the ISP dampers
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
			// Reconfigure the ISP dampers
			CTuning* pTuning = pOpModeMgr->GetOpModeTuning();
			t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
			dErr = pIspDampers->SetMappings(pTuning);
			if(dErr!=ISP_DAMPERS_OK) {
				DBGT_ERROR("Failed to set ISP dampers mappings: err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
				OstTraceFiltStatic1(TRACE_ERROR, "Failed to set ISP dampers mappings: err=%d", (&mENSComponent), dErr);
				DBC_ASSERT(0);
				return 0;
			}
			dErr = pIspDampers->Configure( pTuning, IQSET_OPMODE_SETUP);

                     /*restoring damper configuration written during loaded state*/
                     Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
                     for (t_uint8 i  = 0; i < 8; i++)
                     {
                            if (Cam->isDamperConfigReqd[i].bIndexToBeConfigured == OMX_TRUE)
                            {
                                   Cam->configureDamper(Cam->isDamperConfigReqd[i].nIndex);
                            }
                     }

			if(dErr!=ISP_DAMPERS_OK) {
				DBGT_ERROR("Failed to configure ISP dampers: err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
				OstTraceFiltStatic1(TRACE_ERROR, "Failed to configure ISP dampers: err=%d", (&mENSComponent), dErr);
				DBC_ASSERT(0);
				return 0;
			}

			// Configure the Picture Settings
			t_picture_setting_error_code psErr = PICTURE_SETTING_OK;
			psErr = pPictureSettings->Configure(pTuning);
			if(psErr!=PICTURE_SETTING_OK) {
				DBGT_ERROR("Failed to configure Picture Settings: err=%d\n", psErr);
				OstTraceFiltStatic1(TRACE_ERROR, "Failed to configure Picture Settings: err=%d", (&mENSComponent), psErr);
				DBC_ASSERT(0);
				return 0;
			}
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case Q_PONG_SIG:
		{
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			// Don't need to apply tuning data (it is come from upper layer)
			pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0, 0);
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
			// Fetch the Op Mode ISP settings
			CTuning* pTuning = pOpModeMgr->GetOpModeTuning();
			t_tuning_param peList[ISPCTL_SIZE_TAB_PE] = {{0,0,0}};
			t_tuning_error_code tErr = TUNING_OK;
			t_sint32 nbPe = ISPCTL_SIZE_TAB_PE;
			tErr = pTuning->GetParamList( IQSET_OPMODE_SETUP, IQSET_USER_ISP, peList, &nbPe);
			if(tErr != TUNING_OK) {
				DBGT_ERROR("CAMERA_LoadingIQSettings_OpModeSetup: failed to visit Op Mode tuning object: err=%d (%s)\n",tErr, CTuning::ErrorCode2String(tErr));
				OstTraceFiltStatic1(TRACE_ERROR, "CAMERA_LoadingIQSettings_OpModeSetup: failed to visit Op Mode tuning object: err=%d", (&mENSComponent),tErr);
				DBC_ASSERT(0);
				return 0;
			}
			// Apply the Op Mode ISP settings
			for (t_uint16 i=0;i<nbPe;i++) {
				pIspctlCom->queuePE((t_uint16)peList[i].addr, peList[i].value );
			}
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
			pIspctlCom->processQueue();
			return 0;
		}
		case SCF_STATE_EXIT_SIG:   EXIT; return 0;
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("CAMERA_LoadingIQSettings_OpModeSetup-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "CAMERA_LoadingIQSettings_OpModeSetup-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&CAM_SM::CAMERA_LoadingIQSettings_ZoomPreRun);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::CAMERA_LoadingIQSettings);
}


SCF_STATE CAM_SM::CAMERA_LoadingIQSettings_ZoomPreRun(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;

			MSG0("LoadingIQSettings_ZoomPreRun : changeover allowed\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "LoadingIQSettings_ZoomPreRun : changeover allowed", (&mENSComponent));

			pIspctlCom->queuePE(HostFrameConstraints_e_Flag_AllowChangeOver_Byte0, (t_uint32)Flag_e_TRUE);

			if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled() ||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoRecord)||(OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]) )
			{
#if (IMG_CONFIG_SW_PLATFORM == 1)
				pIspctlCom->queuePE(SystemSetup_e_GrabMode_Ctrl_Byte0, (t_uint32)GrabMode_e_FORCE_OK);
#endif
			}
			else
			{
#if (IMG_CONFIG_SW_PLATFORM == 1)
				pIspctlCom->queuePE(SystemSetup_e_GrabMode_Ctrl_Byte0, (t_uint32)GrabMode_e_NORMAL);
#endif
			}
			if((pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureSingle)
					||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureSingleInVideo)
					||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureBurstLimited)
					||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureBurstInfinite)
					||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureBurstLimitedInVideo)
					||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureBurstInfiniteInVideo)
			)
			{
				pIspctlCom->queuePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, 1);
			}
			else
			{
					pIspctlCom->queuePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, 15);
			}
			pIspctlCom->processQueue();
			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:   EXIT; return 0;
	   case EVT_ISPCTL_LIST_INFO_SIG:
	   {
			MSG0("LoadingIQSettings_ZoomPreRun-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "LoadingIQSettings_ZoomPreRun-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			/* TODO : Other Feature Pre-Run */
            SCF_PSTATE next;
            SM_POP_STATE(next);
            SCF_TRANSIT_TO_PSTATE(next);
			return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::CAMERA_LoadingIQSettings);
}
