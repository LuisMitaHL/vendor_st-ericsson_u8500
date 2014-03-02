/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include "hsmispproc.h"
#include "ispprocport.h"
#include "ispproc.h"
#include "omxispproc.h"
#include "ImgConfig.h"
#include "tuning.h"
#include "deferredeventmgr.h"
#include "MMIO_Camera.h"    // Gamma LUT programming
#include "pictor.h"         // Gamma LUT programming
#include "picture_settings.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_opmode_transition_capture_statemachineTraces.h"
#endif

#define FRAC16_TO_FLOAT(v)  (float)(((float)(v))/65536.0)
#define FRAC14_TO_FLOAT(v) (float)(((float)(v))/16384.0)
#define FRAC8_TO_FLOAT(v)  (float)(((float)(v))/256.0)


#define SW3A_SHAI_Q16

/*#if (IMG_CONFIG_CUSTOMER_FEATURE != 1)
static float i32ToFloat(const t_sint32 i32)
{
	typedef union {
		t_uint32 u32; // 32 bits unsigned representation
		t_sint32 i32; // 32 bits signed representation
		float    f32; // 32 bits float representation
		t_uint32 r32; // 32 bits raw representation
	} t_val32;
	t_val32 val;
	val.i32 = i32;
	return val.f32;
}
#endif*/

SCF_STATE ISPPROC_SM::EnteringRawCapture(s_scf_event const *e) {
	/* _CAM_3PORTS :  */
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
   {
	   MSG0("EnteringRawCapture-ENTRY;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture-ENTRY;", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("EnteringRawCapture-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture-EXIT;", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_INIT_SIG:
   {
	   MSG0("EnteringRawCapture-INIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture-INIT;", (&mENSComponent));
	   //Changes for Stripe
       SCF_INIT(&ISPPROC_SM::EnteringRawCapture_StoppingVpipForStripe);
	   return 0;
   }
   case EVT_ISPCTL_ERROR_SIG:
   {
	   MSG0("EnteringRawCapture-EVT_ISPCTL_INFO_SIG;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
		if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)
		{
			DBGT_ERROR("EnteringRawCapture-ISPCTL_ERR_POLLING_TIMEOUT;");
			OstTraceFiltStatic0 (TRACE_ERROR, "EnteringRawCapture-ISPCTL_ERR_POLLING_TIMEOUT;", (&mENSComponent));
		}
        return 0;
   }
   case Q_DEFERRED_EVENT_SIG:
   {
		MSG0("EnteringRawCapture-DEFERRED_EVENT;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture-DEFERRED_EVENT;", (&mENSComponent));
		return 0;
   }
   default: break;
   }
   return SCF_STATE_PTR(&ISPPROC_SM::EnteringOpMode);//Up_And_Running+
}

//Changes for Stripe
SCF_STATE ISPPROC_SM::EnteringRawCapture_StoppingVpipForStripe(s_scf_event const *e)
{

   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
	   case Q_PONG_SIG:
	   {
            MSG0("EnteringRawCapture_StoppingVpipForStripe-Q_PONG_SIG;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_StoppingVpipForStripe-Q_PONG_SIG;", (&mENSComponent));
            IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
            if (pIspProc->getStripeNumber() > 0)
            {
                SM_PUSH_STATE_STATIC(&ISPPROC_SM::EnteringRawCapture_DoingRawCaptureConfiguration);
			    ControlingISP_ControlType = ControlingISP_SendStop_WaitIspStopStreaming;
		        SCF_TRANSIT_TO(&ISPPROC_SM::SHARED_ControlingISPState);
            }
            else
            {
                SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_DoingRawCaptureConfiguration);
            }
			return 0;
	   }
       default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}

SCF_STATE ISPPROC_SM::EnteringRawCapture_DoingRawCaptureConfiguration(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
	   {
			MSG0("EnteringRawCapture_DoingRawCaptureConfiguration-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_DoingRawCaptureConfiguration-ENTRY;", (&mENSComponent));

			OMX_PARAM_PORTDEFINITIONTYPE port_params;

			ispprocport * 	port = (ispprocport *) mENSComponent.getPort(ISPPROC_PORT_OUT0);

			IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();

            /* tell coverity that getStripeNumber return value does not need to be checked */
            /* coverity[check_return : FALSE] */
			pMemGrabControl->configureGrabResolutionFormat(port, 1, 0, pIspProc->getStripeCount(), pIspProc->getStripeNumber());

			port = (ispprocport *) mENSComponent.getPort(ISPPROC_PORT_OUT1);
			port_params = port->getParamPortDefinition();
			if ((port_params.format.video.eColorFormat == OMX_COLOR_FormatYUV420Planar)||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) ||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar)) {
				t_cm_error cm_error;
			cm_error = pMemGrabControl->allocateGrabCache(port->mInternalFrameWidth);
				if (cm_error != CM_OK)
				{
					DBGT_ERROR("Unable to allocate grab cache in ispproc, error=%d", cm_error);
					OstTraceFiltStatic1 (TRACE_ERROR, "Unable to allocate grab cache in ispproc, error=%d", (&mENSComponent), cm_error);
				}
			}


#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
			// Configure the ISP dampers Still Capture settings
			CTuning* pTuning = NULL;
			pTuning = pTuningDataBase->getObject(TUNING_OBJ_STILL_CAPTURE);
			t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
			dErr = pIspDampers->SetMappings(pTuning);
			if(dErr!=ISP_DAMPERS_OK) {
				DBGT_ERROR("Failed to set ISP dampers mappings: err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
				OstTraceFiltStatic1 (TRACE_ERROR, "Failed to set ISP dampers mappings: err=%d)", (&mENSComponent), dErr);
				DBC_ASSERT(0);
				return 0;
			}
			dErr = pIspDampers->Configure( pTuning, IQSET_OPMODE_SETUP);
			if(dErr!=ISP_DAMPERS_OK) {
				DBGT_ERROR("Failed to configure ISP dampers: err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
				OstTraceFiltStatic1 (TRACE_ERROR, "Failed to configure ISP dampers: err=%d", (&mENSComponent), dErr);
				DBC_ASSERT(0);
				return 0;
			}

			// Configure the Picture Settings
			t_picture_setting_error_code psErr = PICTURE_SETTING_OK;
			psErr = pPictureSettings->Configure(pTuning);
			if(psErr!=PICTURE_SETTING_OK) {
				DBGT_ERROR("Failed to configure Picture Settings: err=%d\n", psErr);
				OstTraceFiltStatic1 (TRACE_ERROR, "Failed to configure Picture Settings: err=%d", (&mENSComponent), psErr);
				DBC_ASSERT(0);
				return 0;
			}
#endif

			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
        case SCF_STATE_EXIT_SIG:
        {
            MSG0("EnteringRawCapture_DoingRawCaptureConfiguration-EXIT;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_DoingRawCaptureConfiguration-EXIT;", (&mENSComponent));
            return 0;
        }

        case Q_PONG_SIG:
        {
			MSG0("EnteringRawCapture_DoingRawCaptureConfiguration-EVT_ISPCTL_INFO_SIG;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_DoingRawCaptureConfiguration-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
			//Changes for Stripe
			IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
   			if (!bExtradataRead)
   			{
   				pIspProc->retrieveCurrentFrameExtradata();
   			}

            IFM_BMS_CAPTURE_CONTEXT BMS_capture_context;
            memset((void*)&BMS_capture_context,0,sizeof(BMS_capture_context));
            pIspProc->getBMSCaptureContext(&BMS_capture_context);

			///////////////////////////////////////////////////////////////////////////
			// Select current frame extradata
			OMX_PARAM_PORTDEFINITIONTYPE port_params;
			OMX_PARAM_PORTDEFINITIONTYPE port_params_in;	//for IN0
			t_sint32 rotation_angle=0;
			ispprocport * 	port = (ispprocport *) mENSComponent.getPort(ISPPROC_PORT_OUT1);
			port_params = port->getParamPortDefinition();	//for OUT1


			ispprocport * port_in = (ispprocport *) mENSComponent.getPort(ISPPROC_PORT_IN0);
   			port_params_in = port_in->getParamPortDefinition();

   			//Obtaining preset information in the case of
   			OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE FrameDescriptionType;
   			if (OMX_ErrorNone == pIspProc->getCaptureParameters(&FrameDescriptionType))
			{
				pPreset = FrameDescriptionType.eRawPreset;
			}
            else {
				DBGT_ERROR("pIspProc->getCaptureParameters failed\n");
				OstTraceFiltStatic0 (TRACE_ERROR, "pIspProc->getCaptureParameters failed\n", (&mENSComponent));
   				DBC_ASSERT(0);
   				return 0;
            }

   			// Obtaining rotation information
			OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
			if (OMX_ErrorNone == pIspProc->getCaptureParameters(&CaptureParameters))
			{
				*pRotation = CaptureParameters.eSceneOrientation;
			}
			else
			{
				DBGT_ERROR("pIspProc->getCaptureParameters failed\n");
				OstTraceFiltStatic0 (TRACE_ERROR, "pIspProc->getCaptureParameters failed\n", (&mENSComponent));
   				DBC_ASSERT(0);
   				return 0;
			}
			((COmxIspProc*)&mENSComponent)->setRotation(*pRotation);

			MSG1("port_params.format.video.eColorFormat=%d\n", port_params.format.video.eColorFormat);
			OstTraceFiltStatic1 (TRACE_DEBUG, "port_params.format.video.eColorFormat=%d", (&mENSComponent), port_params.format.video.eColorFormat);
			MSG1("*pRotation=%d\n", *pRotation);
			OstTraceFiltStatic1 (TRACE_DEBUG, "*pRotation=%d\n", (&mENSComponent), *pRotation);
			MSG1("port_params.format.video.nFrameHeight=%ld\n", port_params.format.video.nFrameHeight);
			OstTraceFiltStatic1 (TRACE_DEBUG, "port_params.format.video.nFrameHeight=%ld", (&mENSComponent), port_params.format.video.nFrameHeight);
			MSG1("port_params.format.video.nFrameWidth=%ld\n", port_params.format.video.nFrameWidth);
			OstTraceFiltStatic1 (TRACE_DEBUG, "port_params.format.video.nFrameWidth=%ld", (&mENSComponent), port_params.format.video.nFrameWidth);
			MSG1("port_params_in.format.video.nFrameHeight=%ld\n", port_params_in.format.video.nFrameHeight);
			OstTraceFiltStatic1 (TRACE_DEBUG, "port_params_in.format.video.nFrameHeight=%ld", (&mENSComponent), port_params_in.format.video.nFrameHeight);
			MSG1("port_params_in.format.video.nFrameWidth=%ld\n", port_params_in.format.video.nFrameWidth);
			OstTraceFiltStatic1 (TRACE_DEBUG, "port_params_in.format.video.nFrameWidth=%ld", (&mENSComponent), port_params_in.format.video.nFrameWidth);


			if (((port_params.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE) OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) || (port_params.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE) OMX_COLOR_FormatYUV420Planar) ||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar))
					&& ((*pRotation == OMX_SYMBIAN_OrientationRowRightColumnTop) || (*pRotation == OMX_SYMBIAN_OrientationRowLeftColumnBottom))
					&&  (*pAutoRotation == OMX_TRUE))
			{
					// check if there is no mismatch between input and output port (that is the height and width can be interchanged)
					if ((port_params.format.video.nFrameHeight < port_params.format.video.nFrameWidth) == (port_params_in.format.video.nFrameHeight < port_params_in.format.video.nFrameWidth))
					{
						bExtradataRead = 1;
						OMX_U32 temp = port_params.format.video.nFrameWidth;
						port_params.format.video.nFrameWidth = port_params.format.video.nFrameHeight;
						port_params.format.video.nFrameHeight = temp;
						port->setParameter(OMX_IndexParamPortDefinition, &port_params);
						mENSComponent.eventHandler(OMX_EventPortSettingsChanged, mENSComponent.getPort(ISPPROC_PORT_OUT1)->getPortIndex(), (OMX_U32)0);
						*pPortSettingChanged = OMX_TRUE;
						SCF_TRANSIT_TO(&ISPPROC_SM::ProcessPendingEvents);
						return 0;
					}
					else
					{
						bExtradataRead = 0;
						MSG2("x size %ld  y size %ld\n", port_params.format.video.nFrameWidth, port_params.format.video.nFrameHeight);
						OstTraceFiltStatic2 (TRACE_DEBUG, "x size %ld  y size %ld", (&mENSComponent), port_params.format.video.nFrameWidth, port_params.format.video.nFrameHeight);
						//Do Nothing as already the port settings are opposite
					}

					//getting in angles the rotation requested as grab needs this in angle
					//if (*pRotation == OMX_SYMBIAN_OrientationRowLeftColumnTop) rotation_angle = 90;
					if (*pRotation == OMX_SYMBIAN_OrientationRowRightColumnTop) rotation_angle = 90;
					//if (*pRotation == OMX_SYMBIAN_OrientationRowRightColumnBottom) rotation_angle = -270;
					if (*pRotation == OMX_SYMBIAN_OrientationRowLeftColumnBottom) rotation_angle = 270;
					MSG1("rotation_angle: =%ld\n", rotation_angle);
					OstTraceFiltStatic1 (TRACE_DEBUG, "rotation_angle: =%ld", (&mENSComponent), rotation_angle);

                    /* tell coverity that getStripeNumber return value does not need to be checked */
                    /* coverity[check_return : FALSE] */
					pMemGrabControl->configureGrabResolutionFormat(port,1,rotation_angle,pIspProc->getStripeCount(), pIspProc->getStripeNumber());
			}
			else if (((port_params.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE) OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) || (port_params.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE) OMX_COLOR_FormatYUV420Planar) ||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar))
					&& ((*pRotation == OMX_SYMBIAN_OrientationRowTopColumnLeft) || (*pRotation == OMX_SYMBIAN_OrientationRowBottomColumnRight))
					&&  (*pAutoRotation == OMX_TRUE))

			{
					if ((port_params.format.video.nFrameHeight > port_params.format.video.nFrameWidth) == (port_params_in.format.video.nFrameHeight < port_params_in.format.video.nFrameWidth))
					{
							bExtradataRead = 1;
							OMX_U32 temp = port_params.format.video.nFrameWidth;
							port_params.format.video.nFrameWidth = port_params.format.video.nFrameHeight;
							port_params.format.video.nFrameHeight = temp;
							port->setParameter(OMX_IndexParamPortDefinition, &port_params);
							mENSComponent.eventHandler(OMX_EventPortSettingsChanged, mENSComponent.getPort(ISPPROC_PORT_OUT1)->getPortIndex(), (OMX_U32)0);
							*pPortSettingChanged = OMX_TRUE;
							SCF_TRANSIT_TO(&ISPPROC_SM::ProcessPendingEvents);
							return 0;
					}
					else
					{
						bExtradataRead = 0;
						MSG2("x size %ld  y size %ld\n", port_params.format.video.nFrameWidth, port_params.format.video.nFrameHeight);
						OstTraceFiltStatic2 (TRACE_DEBUG, "x size %ld  y size %ld", (&mENSComponent), port_params.format.video.nFrameWidth, port_params.format.video.nFrameHeight);
						//Do Nothing as already the port settings are opposite
					}

					//getting in angles the rotation requested as grab needs this in angle
					//if (*pRotation == OMX_SYMBIAN_OrientationRowLeftColumnTop) rotation_angle = 90;
					//if (*pRotation == OMX_SYMBIAN_OrientationRowTopColumnLeft) rotation_angle = 0;
					//if (*pRotation == OMX_SYMBIAN_OrientationRowRightColumnBottom) rotation_angle = -270;
					//if (*pRotation == OMX_SYMBIAN_OrientationRowBottomColumnLeft) rotation_angle = 0;
					rotation_angle = 0;
					MSG1("rotation_angle: =%ld \n", rotation_angle);
					OstTraceFiltStatic1 (TRACE_DEBUG, "rotation_angle: =%ld ", (&mENSComponent), rotation_angle);

                    /* tell coverity that getStripeNumber return value does not need to be checked */
                    /* coverity[check_return : FALSE] */
					pMemGrabControl->configureGrabResolutionFormat(port,1,rotation_angle,pIspProc->getStripeCount(), pIspProc->getStripeNumber());

			}
			else
			{
				rotation_angle = 0;
				bExtradataRead = 0;
                /* tell coverity that getStripeNumber return value does not need to be checked */
                /* coverity[check_return : FALSE] */
				pMemGrabControl->configureGrabResolutionFormat(port,1,rotation_angle,pIspProc->getStripeCount(), pIspProc->getStripeNumber());
			}

			if (OMX_FALSE == BMS_capture_context.TimeNudgeEnabled) {
				ConfiguringISPPipe_ConfigType = ConfiguringISPPipe_StandardBML;
			}
			else {
				ConfiguringISPPipe_ConfigType = ConfiguringISPPipe_TimeNudgeBML;
			}

			if(pIspProc->getStripeNumber() == 0)
			{
                SM_PUSH_STATE_STATIC(&ISPPROC_SM::EnteringRawCapture_DoingBMLConfiguration);
				SCF_TRANSIT_TO(&ISPPROC_SM::SHARED_ConfiguringISPPipe_Ena_Disa);
			}
			else
			{
				SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_DoingBMLConfiguration);
			}
			return 0;
	   }
   default: break;
   }
   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);
}


SCF_STATE ISPPROC_SM::EnteringRawCapture_DoingBMLConfiguration(s_scf_event const *e) {
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("EnteringRawCapture_DoingBMLConfiguration-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_DoingBMLConfiguration-ENTRY;", (&mENSComponent));

			/* restore BMS context */
			IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
			IFM_BMS_CAPTURE_CONTEXT BMS_capture_context;
            memset((void*)&BMS_capture_context,0,sizeof(BMS_capture_context));
			pIspProc->getBMSCaptureContext(&BMS_capture_context);

			pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, BMS_capture_context.Zoom_Control_f_SetFOVX);
			pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0,  BMS_capture_context.Zoom_Control_s16_CenterOffsetX);
			pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0,  BMS_capture_context.Zoom_Control_s16_CenterOffsetY);
			pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0,  BMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX);
			pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0,  BMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize);
			//pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMin_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTXAddrMin); /*CR 417253*/
			//pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMin_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTYAddrMin); /*CR 417253*/
			//pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMax_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTXAddrMax); /*CR 417253*/
			//pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMax_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTYAddrMax); /*CR 417253*/
			pIspctlCom->queuePE(CurrentFrameDimension_f_PreScaleFactor_Byte0,  BMS_capture_context.CurrentFrameDimension_f_PreScaleFactor);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrStart_Byte0,  BMS_capture_context.CurrentFrameDimension_u16_VTXAddrStart);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrStart_Byte0,  BMS_capture_context.CurrentFrameDimension_u16_VTYAddrStart);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrEnd_Byte0,  BMS_capture_context.CurrentFrameDimension_u16_VTXAddrEnd);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrEnd_Byte0,  BMS_capture_context.CurrentFrameDimension_u16_VTYAddrEnd);

			/*
			//Changes for Stripe
			//Need to be configured correctly after getting the right values from firmware.

			if(pIspProc->getStripeNumber() == 0)
                {
                volatile float data;
                data = 3288;//3264;//3280.0;
                pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, *(volatile t_uint32 *)&data);
			    pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0,  BMS_capture_context.Zoom_Control_s16_CenterOffsetX);
			    pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0,  BMS_capture_context.Zoom_Control_s16_CenterOffsetY);
			    pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0,  BMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX);
			    pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0,  BMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY);
			    pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize);
			    pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize);
			    //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMin_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTXAddrMin); //CR 417253
			    //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMin_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTYAddrMin); //CR 417253
			    //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMax_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTXAddrMax); //CR 417253
			    //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMax_Byte0,  BMS_capture_context.SensorFrameConstraints_u16_VTYAddrMax); //CR 417253

                data = 1.0;
			    pIspctlCom->queuePE(CurrentFrameDimension_f_PreScaleFactor_Byte0,  *(volatile t_uint32 *)&data);
			    pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrStart_Byte0,  0);
			    pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrStart_Byte0,  0);
			    pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrEnd_Byte0,    3287);
			    pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrEnd_Byte0,    2465);
                pIspctlCom->queuePE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0,    2);
                }
			*/

			if(pIspProc->getStripeCount() > 1)
				{
				if(pIspProc->getStripeNumber() == 0)
					{
					pIspctlCom->queuePE(Pipe_Scalar_StripeInputs_0_u8_StripeCount_Byte0, pIspProc->getStripeCount());
                    /* tell coverity that getStripeNumber return value does not need to be checked */
                    /* coverity[check_return : FALSE] */
					pIspctlCom->queuePE(Pipe_Scalar_StripeInputs_0_u8_StripeIndex_Byte0, pIspProc->getStripeNumber());
					pIspctlCom->queuePE(DMASetup_e_DMADataBitsInMemory_Byte0, (t_uint32)DMADataBitsInMemory_16);
					pIspctlCom->queuePE(DMASetup_e_DMADataAccessAlignment_Byte0, (t_uint32)DMADataAccessAlignment_e_8Byte);
					}
				else
					{
                    /* tell coverity that getStripeNumber return value does not need to be checked */
                    /* coverity[check_return : FALSE]] */
					pIspctlCom->queuePE(Pipe_Scalar_StripeInputs_0_u8_StripeIndex_Byte0, pIspProc->getStripeNumber());
					}
				}

			pIspctlCom->processQueue();
			return 0;
		}

   		case SCF_STATE_EXIT_SIG:
		{
			MSG0("EnteringRawCapture_DoingBMLConfiguration-EXIT;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_DoingBMLConfiguration-EXIT;", (&mENSComponent));
			return 0;
		}

   		case EVT_ISPCTL_LIST_INFO_SIG:
	   	{
			MSG0("EnteringRawCapture_DoingBMLConfiguration-EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_DoingBMLConfiguration-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;

			IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();

			if(pIspProc->getStripeNumber() == 0)
				{
				SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyIQSettings);
				}
			else
				{
				SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_Start);
				}

			return 0;
	   	}

		default :
			break;
	}
	return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);
}


SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyIQSettings(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("EnteringRawCapture_ApplyIQSettings-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyIQSettings-ENTRY;", (&mENSComponent));
			// Fetch the Still Capture ISP tuning data
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0, 0);
#else
			CTuning* pTuning = NULL;
			pTuning = pTuningDataBase->getObject(TUNING_OBJ_STILL_CAPTURE);
			t_tuning_param peList[ISPCTL_SIZE_TAB_PE];
			t_tuning_error_code tErr = TUNING_OK;
			t_sint32 nbPe = ISPCTL_SIZE_TAB_PE;
			memset((void*)peList,0,ISPCTL_SIZE_TAB_PE*sizeof(t_tuning_param));
			tErr = pTuning->GetParamList( IQSET_OPMODE_SETUP, IQSET_USER_ISP, peList, &nbPe);
			if(tErr != TUNING_OK) {
				DBGT_ERROR("EnteringRawCapture_ApplyIQSettings: failed to fetch tuning data: err=%d (%s)\n",tErr, CTuning::ErrorCode2String(tErr));
				OstTraceFiltStatic1 (TRACE_ERROR, "EnteringRawCapture_ApplyIQSettings: failed to fetch tuning data: err=%d", (&mENSComponent),tErr);
				DBC_ASSERT(0);
				return 0;
			}
			// Apply the Still Capture ISP tuning data
			for (t_uint16 i=0;i<nbPe;i++) {
				pIspctlCom->queuePE((t_uint16)peList[i].addr, peList[i].value );
			}
#endif
			pIspctlCom->processQueue();
			return 0;
		}

		case SCF_STATE_EXIT_SIG:
		{
			MSG0("EnteringRawCapture_ApplyIQSettings-EXIT;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyIQSettings-EXIT;", (&mENSComponent));
			return 0;
		}

		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("EnteringRawCapture_ApplyIQSettings-EVT_ISPCTL_INFO_SIG;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyIQSettings-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_DampedIQSettings);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);
}


SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_DampedIQSettings(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("EnteringRawCapture_ApplyCaptureContext_DampedIQSettings-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_DampedIQSettings-ENTRY;", (&mENSComponent));

			// Retrieves the Dampers Bases from the extradata
			IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
			IFM_DAMPERS_DATA DampersData;
			OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
			OMX_ERRORTYPE oErr = OMX_ErrorNone;
			t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;

			oErr = pIspProc->getDampersData(&DampersData);
			if(oErr!=OMX_ErrorNone) {
				DBGT_ERROR("Failed to get dampers extradata\n");
				OstTraceFiltStatic0 (TRACE_ERROR, "Failed to get dampers extradata", (&mENSComponent));
				DBC_ASSERT(0);
				return 0;
			}
			oErr = pIspProc->getCaptureParameters(&CaptureParameters);
			if(oErr!=OMX_ErrorNone) {
				DBGT_ERROR("Failed to get capture parameters extradata\n");
				OstTraceFiltStatic0 (TRACE_ERROR, "Failed to get capture parameters extradata", (&mENSComponent));
				DBC_ASSERT(0);
				return 0;
			}

			// Compute and queue the ISP damped PEs
			pIspDampers->SetSharpnessOmxSetting( CaptureParameters.nSharpness);
			dErr = pIspDampers->ComputeAndQueuePEs( pIspctlCom, &DampersData);
			if(dErr!=ISP_DAMPERS_OK) {
				DBGT_ERROR("Failed to compute/queue ISP damped PEs: err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
				OstTraceFiltStatic1 (TRACE_ERROR, "Failed to compute/queue ISP damped PEs: err=%d", (&mENSComponent), dErr);
				DBC_ASSERT(0);
				return 0;
			}
			pIspctlCom->processQueue();
			return 0;
		}

		case SCF_STATE_EXIT_SIG:
		{
			MSG0("EnteringRawCapture_ApplyCaptureContext_DampedIQSettings-EXIT;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_DampedIQSettings-EXIT;", (&mENSComponent));
			return 0;
		}

		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("EnteringRawCapture_ApplyCaptureContext_DampedIQSettings-EVT_ISPCTL_INFO_SIG;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_DampedIQSettings-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_AWBSettings);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);
}


SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_AWBSettings(s_scf_event const *e)
{
   switch (e->sig) {

   case SCF_STATE_ENTRY_SIG:
   {
		MSG0("EnteringRawCapture_ApplyCaptureContext_AWBSettings-ENTRY;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_AWBSettings-ENTRY;", (&mENSComponent));

		/* OMX_SYMBIAN_CAPTUREPARAMETERSTYPE  */
		IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
		OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
        OMX_ERRORTYPE error = OMX_ErrorUndefined;
        error = pIspProc->getCaptureParameters(&CaptureParameters);
        if (OMX_ErrorNone != error) {
            MSG0("ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_AWBSettings error;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_AWBSettings error;", (&mENSComponent));
            DBC_ASSERT(0);
            return 0;
        }

        pIspctlCom->queuePE(WhiteBalanceControl_f_RedManualGain_Byte0,   (float)CaptureParameters.xGainR / 0x10000);
        pIspctlCom->queuePE(WhiteBalanceControl_f_GreenManualGain_Byte0, (float)CaptureParameters.xGainG / 0x10000);
        pIspctlCom->queuePE(WhiteBalanceControl_f_BlueManualGain_Byte0,  (float)CaptureParameters.xGainB / 0x10000);
	
		pIspctlCom->processQueue();

		return 0;
	}
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("EnteringRawCapture_ApplyCaptureContext_AWBSettings-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_AWBSettings-EXIT;", (&mENSComponent));
	   return 0;
   }

   case EVT_ISPCTL_LIST_INFO_SIG:
		MSG0("EnteringRawCapture_ApplyCaptureContext_AWBSettings-Q_PONG_SIG;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_AWBSettings-Q_PONG_SIG;", (&mENSComponent));
		SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_CE1);
		return 0;
   default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}


#if (IMG_CONFIG_CUSTOMER_FEATURE == 1)
static void ispproc_mvmult(float a_outV[3], float a_inM[3][3], float a_inV[3]) {
        a_outV[0] = a_inM[0][0] * a_inV[0] + a_inM[1][0] * a_inV[1] + a_inM[2][0] * a_inV[2];
        a_outV[1] = a_inM[0][1] * a_inV[0] + a_inM[1][1] * a_inV[1] + a_inM[2][1] * a_inV[2];
        a_outV[2] = a_inM[0][2] * a_inV[0] + a_inM[1][2] * a_inV[1] + a_inM[2][2] * a_inV[2];
    }
#endif

SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_CE1(s_scf_event const *e)
{
   switch (e->sig) {

   case SCF_STATE_ENTRY_SIG:
   {
		MSG0("EnteringRawCapture_ApplyCaptureContext_CE1-ENTRY;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_CE1-ENTRY;", (&mENSComponent));

		/* OMX_SYMBIAN_CAPTUREPARAMETERSTYPE  */
		IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
		OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
        OMX_ERRORTYPE error = OMX_ErrorUndefined;
        error = pIspProc->getCaptureParameters(&CaptureParameters);
        if(OMX_ErrorNone != error) {
            DBGT_ERROR("Failed to get capture parameters extradata\n");
            OstTraceFiltStatic0 (TRACE_ERROR, "Failed to get capture parameters extradata", (&mENSComponent));
            DBC_ASSERT(0);
            return 0;
        }

        /* OMX_STE_CAPTUREPARAMETERSTYPE  */        
        OMX_STE_CAPTUREPARAMETERSTYPE ExtraCaptureParameters;
        memset((void*)&ExtraCaptureParameters,0,sizeof(ExtraCaptureParameters));
        pIspProc->getCaptureParameters(&ExtraCaptureParameters);

        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_RedInRed_Byte0,     FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[0]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_RedInGreen_Byte0,   FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[1]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_RedInBlue_Byte0,    FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[2]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_GreenInRed_Byte0,   FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[3]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_GreenInGreen_Byte0, FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[4]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_GreenInBlue_Byte0,  FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[5]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_BlueInRed_Byte0,    FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[6]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_BlueInGreen_Byte0,  FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[7]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_1_f_BlueInBlue_Byte0,   FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[8]));

#if (IMG_CONFIG_CUSTOMER_FEATURE == 1)
        float a_rgbM[3][3];
        float a_preGainOffsets[3];
        float a_postGainOffsets[3];

        a_rgbM[0][0] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[0]);
        a_rgbM[0][1] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[1]);
        a_rgbM[0][2] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[2]);
        a_rgbM[1][0] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[3]);
        a_rgbM[1][1] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[4]);
        a_rgbM[1][2] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[5]);
        a_rgbM[2][0] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[6]);
        a_rgbM[2][1] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[7]);
        a_rgbM[2][2] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[8]);

        // CaptureParameters offsets are expressed with reference to a 16b HW pipe. As the color engine is 10b wide, the offset must be scaled accordingly (>> 6).
        // CaptureParameters offsets are expected to be removed from the pixel values, while the CE adds its configure offsets. Therefore negate them.
        a_preGainOffsets[0] = (- ExtraCaptureParameters.nOffsetR / 64) * FRAC16_TO_FLOAT(CaptureParameters.xGainR);
        a_preGainOffsets[1] = (- ExtraCaptureParameters.nOffsetG / 64) * FRAC16_TO_FLOAT(CaptureParameters.xGainG);
        a_preGainOffsets[2] = (- ExtraCaptureParameters.nOffsetB / 64) * FRAC16_TO_FLOAT(CaptureParameters.xGainB);
        ispproc_mvmult(a_postGainOffsets, a_rgbM, a_preGainOffsets);

        pIspctlCom->queuePE(CE_ColourMatrixCtrl_1_s16_Offset_R_Byte0, (t_sint16)a_postGainOffsets[0]);
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_1_s16_Offset_G_Byte0, (t_sint16)a_postGainOffsets[1]);
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_1_s16_Offset_B_Byte0, (t_sint16)a_postGainOffsets[2]);
#else
        // This is a temporary patch. Remove ifdef once SW3A implementations are aligned.
        // CaptureParameters offsets are expressed with reference to a 16b HW pipe. As the color engine is 10b wide, the offset must be scaled accordingly (>> 6).
        // CaptureParameters offsets are expected to be removed from the pixel values, while the CE adds its configure offsets. Therefore negate them.
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_1_s16_Offset_R_Byte0, (t_sint16)(- ExtraCaptureParameters.nOffsetR / 64));
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_1_s16_Offset_G_Byte0, (t_sint16)(- ExtraCaptureParameters.nOffsetG / 64));
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_1_s16_Offset_B_Byte0, (t_sint16)(- ExtraCaptureParameters.nOffsetB / 64));
#endif

        // Picture settings related PEs
        t_sint16 iLumaOffset = pPictureSettings->iBrightness.ComputeLumaOffsetPE(CaptureParameters.nBrightness);
        pIspctlCom->queuePE( CE_LumaOffset_1_s16_LumaOffset_Byte0, iLumaOffset);
        MSG2("[ISPPROC] Brightness: Omx=%lu, LumaOffsetPE[1]=%d\n", CaptureParameters.nBrightness, iLumaOffset);
        OstTraceFiltStatic2 (TRACE_DEBUG, "[ISPPROC] Brightness: Omx=%lu, LumaOffsetPE[1]=%d", (&mENSComponent), CaptureParameters.nBrightness, iLumaOffset);

        t_uint8 iColourSaturation = pPictureSettings->iSaturation.ComputeColourSaturationPE(CaptureParameters.nSaturation);
        pIspctlCom->queuePE( CE_YUVCoderControls_1_u8_ColourSaturation_Byte0, (t_uint32)iColourSaturation);
        MSG2("[ISPPROC] Saturation: Omx=%ld, ColourSaturationPE[1]=%d\n", CaptureParameters.nSaturation, iColourSaturation);
        OstTraceFiltStatic2 (TRACE_DEBUG, "[ISPPROC] Saturation: Omx=%ld, ColourSaturationPE[1]=%d", (&mENSComponent), CaptureParameters.nSaturation, iColourSaturation);

        t_uint8 iContrast = pPictureSettings->iContrast.ComputeContrastPE(CaptureParameters.nContrast);
        pIspctlCom->queuePE( CE_YUVCoderControls_1_u8_Contrast_Byte0, (t_uint32)iContrast);
        MSG2("[ISPPROC] Contrast: Omx=%ld, ContrastPE[1]=%d\n", CaptureParameters.nContrast, iContrast);
        OstTraceFiltStatic2 (TRACE_DEBUG, "[ISPPROC] Contrast: Omx=%ld, ContrastPE[1]=%d", (&mENSComponent), CaptureParameters.nContrast, iContrast);

        // NB: Special effects related PEs are handled in EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting()
           
		pIspctlCom->processQueue();

		return 0;
	}
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("EnteringRawCapture_ApplyCaptureContext_CE1-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_CE1-EXIT;", (&mENSComponent));
	   return 0;
   }

   case EVT_ISPCTL_LIST_INFO_SIG:
		MSG0("EnteringRawCapture_ApplyCaptureContext_CE1-Q_PONG_SIG;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_CE1-Q_PONG_SIG;", (&mENSComponent));
		SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_CE0);
		return 0;
   default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}


SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_CE0(s_scf_event const *e)
{
   switch (e->sig) {

   case SCF_STATE_ENTRY_SIG:
   {
		MSG0("EnteringRawCapture_ApplyCaptureContext_CE0-ENTRY;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_CE0-ENTRY;", (&mENSComponent));

		/* OMX_SYMBIAN_CAPTUREPARAMETERSTYPE  */
		IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
		OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
        OMX_ERRORTYPE error = OMX_ErrorUndefined;
        error = pIspProc->getCaptureParameters(&CaptureParameters);
        if(OMX_ErrorNone != error) {
            DBGT_ERROR("Failed to get capture parameters extradata\n");
            OstTraceFiltStatic0 (TRACE_ERROR, "Failed to get capture parameters extradata", (&mENSComponent));
            DBC_ASSERT(0);
            return 0;
        }

        /* OMX_STE_CAPTUREPARAMETERSTYPE  */
        OMX_STE_CAPTUREPARAMETERSTYPE ExtraCaptureParameters;
        memset((void*)&ExtraCaptureParameters,0,sizeof(ExtraCaptureParameters));
        pIspProc->getCaptureParameters(&ExtraCaptureParameters);

        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_RedInRed_Byte0,     FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[0]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_RedInGreen_Byte0,   FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[1]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_RedInBlue_Byte0,    FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[2]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_GreenInRed_Byte0,   FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[3]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0, FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[4]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0,  FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[5]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_BlueInRed_Byte0,    FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[6]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0,  FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[7]));
        pIspctlCom->queuePE(CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0,   FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[8]));

#if (IMG_CONFIG_CUSTOMER_FEATURE == 1)
        float a_rgbM[3][3];
        float a_preGainOffsets[3];
        float a_postGainOffsets[3];

        a_rgbM[0][0] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[0]);
        a_rgbM[0][1] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[1]);
        a_rgbM[0][2] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[2]);
        a_rgbM[1][0] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[3]);
        a_rgbM[1][1] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[4]);
        a_rgbM[1][2] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[5]);
        a_rgbM[2][0] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[6]);
        a_rgbM[2][1] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[7]);
        a_rgbM[2][2] = FRAC8_TO_FLOAT(ExtraCaptureParameters.nColorMatrix[8]);

        // CaptureParameters offsets are expressed with reference to a 16b HW pipe. As the color engine is 10b wide, the offset must be scaled accordingly (>> 6).
        // CaptureParameters offsets are expected to be removed from the pixel values, while the CE adds its configure offsets. Therefore negate them.
        a_preGainOffsets[0] = (- ExtraCaptureParameters.nOffsetR / 64) * FRAC16_TO_FLOAT(CaptureParameters.xGainR);
        a_preGainOffsets[1] = (- ExtraCaptureParameters.nOffsetG / 64) * FRAC16_TO_FLOAT(CaptureParameters.xGainG);
        a_preGainOffsets[2] = (- ExtraCaptureParameters.nOffsetB / 64) * FRAC16_TO_FLOAT(CaptureParameters.xGainB);
        ispproc_mvmult(a_postGainOffsets, a_rgbM, a_preGainOffsets);

        pIspctlCom->queuePE(CE_ColourMatrixCtrl_0_s16_Offset_R_Byte0, (t_sint16)a_postGainOffsets[0]);
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_0_s16_Offset_G_Byte0, (t_sint16)a_postGainOffsets[1]);
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_0_s16_Offset_B_Byte0, (t_sint16)a_postGainOffsets[2]);
#else
        // This is a temporary patch. Remove ifdef once SW3A implementations are aligned.
        // CaptureParameters offsets are expressed with reference to a 16b HW pipe. As the color engine is 10b wide, the offset must be scaled accordingly (>> 6).
        // CaptureParameters offsets are expected to be removed from the pixel values, while the CE adds its configure offsets. Therefore negate them.
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_0_s16_Offset_R_Byte0, (t_sint16)(- ExtraCaptureParameters.nOffsetR / 64));
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_0_s16_Offset_G_Byte0, (t_sint16)(- ExtraCaptureParameters.nOffsetG / 64));
        pIspctlCom->queuePE(CE_ColourMatrixCtrl_0_s16_Offset_B_Byte0, (t_sint16)(- ExtraCaptureParameters.nOffsetB / 64));
#endif

        // Picture settings related PEs
        t_sint16 iLumaOffset = pPictureSettings->iBrightness.ComputeLumaOffsetPE(CaptureParameters.nBrightness);
        pIspctlCom->queuePE( CE_LumaOffset_0_s16_LumaOffset_Byte0, iLumaOffset);
        MSG2("[ISPPROC] Brightness: Omx=%lu, LumaOffsetPE[0]=%d\n", CaptureParameters.nBrightness, iLumaOffset);
        OstTraceFiltStatic2 (TRACE_DEBUG, "[ISPPROC] Brightness: Omx=%lu, LumaOffsetPE[0]=%d", (&mENSComponent), CaptureParameters.nBrightness, iLumaOffset);

        t_uint8 iColourSaturation = pPictureSettings->iSaturation.ComputeColourSaturationPE(CaptureParameters.nSaturation);
        pIspctlCom->queuePE( CE_YUVCoderControls_0_u8_ColourSaturation_Byte0, (t_uint32)iColourSaturation);
        MSG2("[ISPPROC] Saturation: Omx=%ld, ColourSaturationPE[0]=%d\n", CaptureParameters.nSaturation, iColourSaturation);
        OstTraceFiltStatic2 (TRACE_DEBUG, "[ISPPROC] Saturation: Omx=%ld, ColourSaturationPE[0]=%d", (&mENSComponent), CaptureParameters.nSaturation, iColourSaturation);

        t_uint8 iContrast = pPictureSettings->iContrast.ComputeContrastPE(CaptureParameters.nContrast);
        pIspctlCom->queuePE( CE_YUVCoderControls_0_u8_Contrast_Byte0, (t_uint32)iContrast);
        MSG2("[ISPPROC] Contrast: Omx=%ld, ContrastPE[0]=%d\n", CaptureParameters.nContrast, iContrast);
        OstTraceFiltStatic2 (TRACE_DEBUG, "[ISPPROC] Contrast: Omx=%ld, ContrastPE[0]=%d", (&mENSComponent), CaptureParameters.nContrast, iContrast);

        // NB: Special effects related PEs are handled in EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting()

		pIspctlCom->processQueue();
		return 0;
   }
   case SCF_STATE_EXIT_SIG:
   {
       MSG0("EnteringRawCapture_ApplyCaptureContext_CE0-EXIT;\n");
       OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_CE0-EXIT;", (&mENSComponent));
       return 0;
   }

   case EVT_ISPCTL_LIST_INFO_SIG:
   {
        MSG0("EnteringRawCapture_ApplyCaptureContext_CE0-Q_PONG_SIG;\n");
        OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_CE0-Q_PONG_SIG;", (&mENSComponent));

        /* OMX_STE_CAPTUREPARAMETERSTYPE  */
        IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
        OMX_STE_CAPTUREPARAMETERSTYPE ExtraCaptureParameters;
        memset((void*)&ExtraCaptureParameters,0,sizeof(ExtraCaptureParameters));
        pIspProc->getCaptureParameters(&ExtraCaptureParameters);

        if (ExtraCaptureParameters.bValidGamma) {
            SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_GammaLutSetting);
        }
        else {
            SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting);
        }
        //SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_Start);
        return 0;
   }
   
   default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}


SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_GammaLutSetting(s_scf_event const * e)
{
    switch (e->sig) {
        
       case SCF_STATE_ENTRY_SIG:
       {               
            MSG0("EnteringRawCapture_ApplyCaptureContext_GammaLutSetting-ENTRY: Got new gamma LUT to write to the ISP.\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_GammaLutSetting-ENTRY: Got new gamma LUT to write to the ISP;", (&mENSComponent));
         
            pIspctlCom->queuePE(CE_GammaControl_0_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Disable);
            pIspctlCom->queuePE(CE_GammaControl_1_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Disable);

            /* OMX_STE_CAPTUREPARAMETERSTYPE  */
            IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
            OMX_STE_CAPTUREPARAMETERSTYPE ExtraCaptureParameters;
            pIspProc->getCaptureParameters(&ExtraCaptureParameters);

            OMX_ERRORTYPE e_ret = OMX_ErrorNone;
            bool bHasErr = false;

            // Program the sharpened and unsharpened LUT memory of all color channels.
            // Program the LUT last elements, which do not fit into the memory.
            long *lutAddr;
            int lutCount;
            t_uint16 u16_last;             

            /* Write Red Gamma Lut = According to PictorBug #111096, Red PEs should be programmed by values corresponding to Green channel */        
                lutAddr = (long *) &(ExtraCaptureParameters.nGreenGammaLUT[0]);
                lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1; //write 128 data
                e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_RED_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_RED_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_RED_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_RED_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                u16_last = ExtraCaptureParameters.nGreenGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Red_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Red_Byte0, (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Red_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Red_Byte0, (t_uint32)u16_last);                

            /* Write Green Gamma Lut = According to PictorBug #111096, Green PEs should be programmed by values corresponding to Blue channel*/            
                lutAddr = (long *)&(ExtraCaptureParameters.nBlueGammaLUT[0]);
                lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1; //write 128 data
                e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_GREEN_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_GREEN_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                u16_last = ExtraCaptureParameters.nBlueGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];            
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIR_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIR_Byte0, (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIR_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIR_Byte0, (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIB_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIB_Byte0, (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIB_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIB_Byte0, (t_uint32)u16_last);
            
            /* Write Blue Gamma Lut = According to PictorBug #111096, Blue PEs should be programmed by values corresponding to Red channel*/            
                lutAddr = (long *)&(ExtraCaptureParameters.nRedGammaLUT[0]);
                lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1;
                e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_BLUE_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_BLUE_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                u16_last = ExtraCaptureParameters.nRedGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];            
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Blue_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Blue_Byte0, (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Blue_Byte0,   (t_uint32)u16_last);
                pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Blue_Byte0, (t_uint32)u16_last);

            if (bHasErr) {
                 DBGT_ERROR("EnteringRawCapture_ApplyCaptureContext_GammaLutSetting- ERROR: could not write LUT to ISP memory.\n");
                 OstTraceFiltStatic0 (TRACE_ERROR, "EnteringRawCapture_ApplyCaptureContext_GammaLutSetting- ERROR: could not write LUT to ISP memory;", (&mENSComponent));
                 DBC_ASSERT(e_ret);
                 return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture_Start);
            }

            pIspctlCom->queuePE(CE_GammaControl_0_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Custom);
            pIspctlCom->queuePE(CE_GammaControl_1_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Custom);
                    
            pIspctlCom->processQueue();
            return 0;
        }

        case SCF_STATE_EXIT_SIG:
       {
            MSG0("EnteringRawCapture_ApplyCaptureContext_GammaLutSetting-EXIT;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_GammaLutSetting-EXIT;", (&mENSComponent));
            return 0;
       }

       case EVT_ISPCTL_LIST_INFO_SIG:
       case EVT_ISPCTL_INFO_SIG:
       {
            MSG0("EnteringRawCapture_ApplyCaptureContext_GammaLutSetting-EVT_ISPCTL_INFO_SIG\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_GammaLutSetting-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));

            switch(e->type.ispctlListInfo.info_id) {
                case ISP_WRITELIST_DONE:
                {
                    SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting);
                    break;
                 }
             
                 default: break;
            }
            
            return 0;
       }
       
       default: break;

    }

    return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);          

}

SCF_STATE ISPPROC_SM::EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting(s_scf_event const * e)
{
   switch (e->sig) {

   case SCF_STATE_ENTRY_SIG:
   {
        MSG0("EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting-ENTRY;\n");
        OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting-ENTRY;", (&mENSComponent));

        /* OMX_STE_CAPTUREPARAMETERSTYPE  */
        IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
        OMX_STE_CAPTUREPARAMETERSTYPE ExtraCaptureParameters;
        memset((void*)&ExtraCaptureParameters,0,sizeof(ExtraCaptureParameters));
        pIspProc->getCaptureParameters(&ExtraCaptureParameters);
     
        // Warning: The FW solarize and negative control is expressed... in negative logic (that is not a joke), as specified by the xp70 FW User Manual.
        /* FIXME:
         *      Clarify to which extent the special effects could be applied to the LR port (1).
         */ 
        if (OMX_TRUE == ExtraCaptureParameters.bValidSFX) {
            pIspctlCom->queuePE(SpecialEffects_Control_0_e_SFXSolarisControl_Byte0, ExtraCaptureParameters.nSFXSolarisControl);
            pIspctlCom->queuePE(SpecialEffects_Control_0_e_SFXNegativeControl_Byte0, ExtraCaptureParameters.nSFXNegativeControl);          
            pIspctlCom->queuePE(SpecialEffects_Control_1_e_SFXSolarisControl_Byte0, ExtraCaptureParameters.nSFXSolarisControl);
            pIspctlCom->queuePE(SpecialEffects_Control_1_e_SFXNegativeControl_Byte0, ExtraCaptureParameters.nSFXNegativeControl);          
        } else {
            pIspctlCom->queuePE(SpecialEffects_Control_0_e_SFXSolarisControl_Byte0,  (t_uint32)Flag_e_TRUE);
            pIspctlCom->queuePE(SpecialEffects_Control_0_e_SFXNegativeControl_Byte0, (t_uint32)Flag_e_TRUE);          
            pIspctlCom->queuePE(SpecialEffects_Control_1_e_SFXSolarisControl_Byte0,  (t_uint32)Flag_e_TRUE);
            pIspctlCom->queuePE(SpecialEffects_Control_1_e_SFXNegativeControl_Byte0, (t_uint32)Flag_e_TRUE);          
        }

        if (OMX_TRUE == ExtraCaptureParameters.bValidEffect) {
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, (t_uint32)ExtraCaptureParameters.uAdsocPKEmbossEffectCtrl);
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Byte0, (t_uint32)ExtraCaptureParameters.uAdsocPKFlipperEffectCtrl);         
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Byte0, (t_uint32)ExtraCaptureParameters.uAdsocPKGrayBackEffectCtrl);           
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, (t_uint32)ExtraCaptureParameters.uAdsocPKEmbossEffectCtrl);
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Byte0, (t_uint32)ExtraCaptureParameters.uAdsocPKFlipperEffectCtrl);         
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Byte0, (t_uint32)ExtraCaptureParameters.uAdsocPKGrayBackEffectCtrl);           
        } else {
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, 0);
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Byte0, 0);         
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Byte0, 0);           
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, 0);
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Byte0, 0);         
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Byte0, 0);           
        }

        pIspctlCom->processQueue();
        return 0;
   }

   case SCF_STATE_EXIT_SIG:  
   {
       MSG0("EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting-EXIT;\n");
       OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting-EXIT;", (&mENSComponent));
       return 0;
   }

   case EVT_ISPCTL_LIST_INFO_SIG:
   {
        MSG0("EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting: EVT_ISPCTL_LIST_INFO_SIG\n");           
        OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ApplyCaptureContext_ExtraCaptureParametersSetting: EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
        SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_Start);        
        return 0;
   }
    
   default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}



SCF_STATE ISPPROC_SM::EnteringRawCapture_Start(s_scf_event const *e)
{
   switch (e->sig) {

   case SCF_STATE_ENTRY_SIG:

		MSG0("EnteringRawCapture_Start-ENTRY;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_Start-ENTRY;", (&mENSComponent));
		pResourceSharerManager->mRSPing.Ping(0);
		return 0;

   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("EnteringRawCapture_Start-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_Start-EXIT;", (&mENSComponent));
	   return 0;
   }

   case Q_PONG_SIG:

		MSG0("EnteringRawCapture_Start-Q_PONG_SIG;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_Start-Q_PONG_SIG;", (&mENSComponent));
		SM_PUSH_STATE_STATIC(&ISPPROC_SM::EnteringRawCapture_ConfigureMemGrab);
		ControlingISP_ControlType = ControlingISP_SendRun_WaitIspLoadReady;
		SCF_TRANSIT_TO(&ISPPROC_SM::SHARED_ControlingISPState);

		   return 0;
   default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}

SCF_STATE ISPPROC_SM::EnteringRawCapture_ConfigureMemGrab(s_scf_event const *e)
{
    switch (e->sig) {

        case SCF_STATE_ENTRY_SIG:

            MSG0("EnteringRawCapture_ConfigureMemGrab-ENTRY;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ConfigureMemGrab-ENTRY;", (&mENSComponent));
	    pIspctlCom->queuePE(RequestedFrameDimension_u16_VTLineLengthPck_Byte0,0);

            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHStart_Byte0, 0);
            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_1_s16_StripeInCropHStart_Byte0, 0);

            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte0,0);
            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_1_s16_StripeInCropHSize_Byte0,0);

            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropHStart_Byte0,0);
            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropHStart_Byte0,0);

            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropHSize_Byte0,0);
            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropHSize_Byte0,0);

            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_0_s16_StripeScalarOutputHSize_Byte0,0);
            pIspctlCom->queuePE(Pipe_Scalar_StripeOutputs_1_s16_StripeScalarOutputHSize_Byte0,0);

			pIspctlCom->readQueue();

            return 0;

        case SCF_STATE_EXIT_SIG:
            MSG0("EnteringRawCapture_ConfigureMemGrab-EXIT;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ConfigureMemGrab-EXIT;", (&mENSComponent));
            return 0;

        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("EnteringRawCapture_ConfigureMemGrab-EVT_ISPCTL_INFO_SIG;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_ConfigureMemGrab-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
            if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break; // not our message

            for (unsigned int i=0; i < 10; i++)
                {
                MSG3("PE:0x%x (%s) val:0x%lx\n", e->type.ispctlInfo.Listvalue[i].pe_addr,
                        pIspctlCom->pIspctlSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr),
                        e->type.ispctlInfo.Listvalue[i].pe_data);
                OstTraceFiltStatic2 (TRACE_DEBUG, "PE:0x%x val:0x%lx", (&mENSComponent), e->type.ispctlInfo.Listvalue[i].pe_addr, e->type.ispctlInfo.Listvalue[i].pe_data);
                }

            t_uint32 VTLineLength = e->type.ispctlListInfo.Listvalue[0].pe_data;
			t_uint32 stripe_size = e->type.ispctlListInfo.Listvalue[3].pe_data;

            ispprocport* port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_IN0);
            /* tell coverity that isSupportedFmt return value does not need to be checked */
            /* coverity[check_return : FALSE] */
            DBC_ASSERT(port->isSupportedFmt(ISPPROC_PORT_IN0,port->getParamPortDefinition().format.video.eColorFormat));

            IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
            IFM_BMS_CAPTURE_CONTEXT BMS_capture_context;
            memset((void*)&BMS_capture_context,0,sizeof(BMS_capture_context));
            pIspProc->getBMSCaptureContext(&BMS_capture_context);
            e_BMLPixelOrder PixelOrder = (e_BMLPixelOrder)BMS_capture_context.SystemConfig_Status_e_PixelOrder;
            t_uint32 NumberofNonActiveLinesAtTopEdge = BMS_capture_context.CurrentFrameDimension_u8_NumberOfStatusLines;

            /* tell coverity that getStripeNumber return value does not need to be checked */
            /* coverity[check_return : FALSE] */
            pMemGrabControl->configureMemGrabResolutionFormat( port, NumberofNonActiveLinesAtTopEdge, VTLineLength, PixelOrder, pIspProc->getStripeCount(),
															   pIspProc->getStripeNumber(), stripe_size);

            SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture_SendBuffers);

            return 0;
        }
        default: break;
    }

    return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}


SCF_STATE ISPPROC_SM::EnteringRawCapture_SendBuffers(s_scf_event const *e)
{
    switch (e->sig) {

        case SCF_STATE_ENTRY_SIG:

            MSG0("EnteringRawCapture_SendBuffers-ENTRY;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_SendBuffers-ENTRY;", (&mENSComponent));
            pResourceSharerManager->mRSPing.Ping(0);

            return 0;

        case SCF_STATE_EXIT_SIG:
            MSG0("EnteringRawCapture_SendBuffers-EXIT;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_SendBuffers-EXIT;", (&mENSComponent));
            return 0;

        case Q_PONG_SIG:
        {
            MSG0("EnteringRawCapture_SendBuffers-Q_PONG_SIG;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_SendBuffers-Q_PONG_SIG;", (&mENSComponent));
            //if (e->type.ispctlInfo.info_id != ISP_READ_DONE) break; // not our message

            ispprocport* port0 = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT0);
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition0,mParamPortDefinition1;
            mParamPortDefinition0.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition0.nVersion);
            mParamPortDefinition1.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition1.nVersion);

            port0->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition0);

            ispprocport* port1 = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT1);
            port1->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition1);

            if((mParamPortDefinition0.bEnabled==OMX_FALSE)&&(mParamPortDefinition1.bEnabled==OMX_TRUE))
                pMemGrabControl->setStartOneShotCaptureAll(ISPPROC_PORT_OUT1,ISPPROC_PORT_IN0,ISPPROC_PORT_OUT0, 2);

            if((mParamPortDefinition0.bEnabled==OMX_TRUE)&&(mParamPortDefinition1.bEnabled==OMX_FALSE))
                pMemGrabControl->setStartOneShotCaptureAll(ISPPROC_PORT_OUT0,ISPPROC_PORT_IN0,ISPPROC_PORT_OUT1, 2);

            if((mParamPortDefinition0.bEnabled==OMX_TRUE)&&(mParamPortDefinition1.bEnabled==OMX_TRUE))
                pMemGrabControl->setStartOneShotCaptureAll(ISPPROC_PORT_OUT0,ISPPROC_PORT_OUT1,ISPPROC_PORT_IN0,3);

            SCF_TRANSIT_TO(&ISPPROC_SM::WaitEvent);

            return 0;
        }
        default: break;
    }

    return SCF_STATE_PTR(&ISPPROC_SM::EnteringRawCapture);

}






SCF_STATE ISPPROC_SM::ExitingRawCapture(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
   {
	   MSG0("ExitingRawCapture-ENTRY;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture-ENTRY;", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("ExitingRawCapture-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture-EXIT;", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_INIT_SIG:
   {
	   MSG0("ExitingRawCapture-INIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture-INIT;", (&mENSComponent));
	   SCF_INIT(&ISPPROC_SM::ExitingRawCapture_StoppingVpip);
	   return 0;
   }
   case EVT_ISPCTL_ERROR_SIG:
	{
		MSG0("ExitingRawCapture-EVT_ISPCTL_INFO_SIG;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
		if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)
		{
			MSG0("ExitingRawCapture-ISPCTL_ERR_POLLING_TIMEOUT;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture-ISPCTL_ERR_POLLING_TIMEOUT;", (&mENSComponent));
		}
        return 0;
	}
   case Q_DEFERRED_EVENT_SIG:
   {
	   MSG0("ExitingRawCapture-DEFERRED_EVENT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture-DEFERRED_EVENT;", (&mENSComponent));
	   return 0;
   }
   default: break;

   }
   return SCF_STATE_PTR(&ISPPROC_SM::ExitingOpMode);//Up_And_Running
}
SCF_STATE ISPPROC_SM::ExitingRawCapture_StoppingVpip(s_scf_event const *e)
{

   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:

		MSG0("ExitingRawCapture_StoppingVpip-ENTRY;\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture_StoppingVpip-ENTRY;", (&mENSComponent));

		pResourceSharerManager->mRSPing.Ping(0);
		return 0;

   case SCF_STATE_EXIT_SIG:
	   MSG0("ExitingRawCapture_StoppingVpip-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture_StoppingVpip-EXIT;", (&mENSComponent));
	   return 0;

   case Q_PONG_SIG:

		MSG0("ExitingRawCapture_StoppingVpip-Q_PONG_SIG\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture_StoppingVpip-Q_PONG_SIG", (&mENSComponent));

        SM_PUSH_STATE_STATIC(&ISPPROC_SM::ExitingRawCapture_FreeIspProc);
		ControlingISP_ControlType = ControlingISP_SendStop_WaitIspStopStreaming;
		SCF_TRANSIT_TO(&ISPPROC_SM::SHARED_ControlingISPState);

		return 0;
   default: break;

   }

   return SCF_STATE_PTR(&ISPPROC_SM::ExitingRawCapture);

}

SCF_STATE ISPPROC_SM::ExitingRawCapture_FreeIspProc(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
	   {
			MSG0("ExitingRawCapture_FreeIspProc-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture_FreeIspProc-ENTRY;", (&mENSComponent));
			//pResourceSharerManager->mRSPing.Ping(0);
			//RSRSRS
			pResourceSharerManager->mRSRelease.Release(1); //1 is  ISPPROC_SNAP requester, how to share it with rs, tbd.
			/* TBD ; this should be Release(2) for ispproc still */
			return 0;
	   }
	case SCF_STATE_EXIT_SIG:
		{
			MSG0("ExitingRawCapture_FreeIspProc-EXIT;\n"); 
			OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture_FreeIspProc-EXIT;", (&mENSComponent));
			return 0;
		}

   case Q_PONG_SIG:
	   {
			MSG0("ExitingRawCapture_FreeIspProc-Q_PONG_SIG;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "ExitingRawCapture_FreeIspProc-Q_PONG_SIG;", (&mENSComponent));

			pResourceSharerManager->mRSFree.Free(0);
			SCF_TRANSIT_TO(&ISPPROC_SM::WaitEvent);

			return 0;
	   }

/* RSRSRS */
	case Q_RELEASED_SIG:
	{
			SCF_TRANSIT_TO(&ISPPROC_SM::WaitEvent);
			return 0;
	}
    default: break;

	}
   return SCF_STATE_PTR(&ISPPROC_SM::ExitingRawCapture);
}


