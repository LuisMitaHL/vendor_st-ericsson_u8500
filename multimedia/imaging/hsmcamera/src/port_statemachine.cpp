/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "hsmcam.h"
#include "osi_trace.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_port_statemachineTraces.h"
#endif


SCF_STATE CAM_SM::EnablePortInIdleExecutingState(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG: ENTRY;return 0;
	   case SCF_STATE_EXIT_SIG:  EXIT; return 0;
	   case SCF_STATE_INIT_SIG:  INIT; SCF_INIT(&CAM_SM::EnablePortInIdleExecutingState_StopVpip); return 0;
	   case Q_DEFERRED_EVENT_SIG:
		   MSG0("EnablePortInIdleExecutingState-Q_DEFERRED_EVENT_SIG\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
		   return 0;
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}

SCF_STATE CAM_SM::EnablePortInIdleExecutingState_StopVpip(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
		   ENTRY;
		   pResourceSharerManager->mRSPing.Ping(0);
		   return 0;
	   }
	   case SCF_STATE_EXIT_SIG:  EXIT; return 0;

	   case Q_PONG_SIG:
	   {
		   MSG0("EnablePortInIdleExecutingState_StopVpip-Q_PONG_SIG\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_StopVpip-Q_PONG_SIG", (&mENSComponent));


			OMX_STATETYPE currentState;
			mENSComponent.GetState(&mENSComponent, &currentState);
			/* in executing state ISP must be stopped */
			if (OMX_StateExecuting == currentState)
			{
				SM_PUSH_STATE_STATIC(&CAM_SM::RendezvousState);
				p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState_ConfigurePipe);
				ControlingISP_ControlType = ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A;//STOP sw3A in port disable-enable:ER 406209
				SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
			}
			else
			{
				SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_ConfigurePipe);
			}
			return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


SCF_STATE CAM_SM::EnablePortInIdleExecutingState_ConfigurePipe(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:   EXIT;  return 0;

	   case Q_PONG_SIG:
	   {
			MSG0("EnablePortInIdleExecutingState_ConfigurePipe-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_ConfigurePipe-Q_PONG_SIG", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState_DoingRate);
			ConfiguringISPPipe_ConfigType = ConfiguringISPPipe_ResolutionOnly;
			SCF_TRANSIT_TO(&CAM_SM::SHARED_ConfiguringISPPipe_FormatResolution);
			return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


SCF_STATE CAM_SM::EnablePortInIdleExecutingState_DoingRate(s_scf_event const *e) {
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
			MSG0("EnablePortInIdleExecutingState_DoingRate-Q_PONG_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_DoingRate-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState_ConfigureZoom);
			SCF_TRANSIT_TO(&CAM_SM::Rating);
			return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


SCF_STATE CAM_SM::EnablePortInIdleExecutingState_ConfigureZoom(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:   EXIT;  return 0;

	   case Q_PONG_SIG:
	   {
			MSG0("EnablePortInIdleExecutingState_ConfigureZoom-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_ConfigureZoom-Q_PONG_SIG", (&mENSComponent));
			if( isp_state == ISP_STATE_STREAMING)
			{
			       SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState_CfgGrab);
				pZoom->internalZoomConfig = internalZoomConfig_setCurrentFOVandCenter;
				SCF_TRANSIT_TO(&CAM_SM::ZoomBeforeStateExecuting);
			}
			else
			{
		              SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState_StartVpip);
				pZoom->internalZoomConfig = internalZoomConfig_resetFOV;
				SCF_TRANSIT_TO(&CAM_SM::ZoomingConfig);
			}
            return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


SCF_STATE CAM_SM::EnablePortInIdleExecutingState_StartVpip(s_scf_event const *e) {
   
   			camport * 	portRecord = (camport *) mENSComponent.getPort(RecordPort);
			camport * 	portVF = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);
   
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:   EXIT;  return 0;
	   case Q_PONG_SIG:
	   {
		   MSG0("EnablePortInIdleExecutingState_StartVpip-Q_PONG_SIG\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_StartVpip-Q_PONG_SIG", (&mENSComponent));

		   OMX_STATETYPE currentState;
		   mENSComponent.GetState(&mENSComponent, &currentState);

			/* in executing state ISP must be restarted */
			if (OMX_StateExecuting == currentState)
			{
				SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState_ConfigureZoom);
				ControlingISP_ControlType = ControlingISP_SendRun_WaitIspStreaming_Start3A; //Starting sw3A in port enable-disable :ER 406209
				SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
			}
			else
			{
				SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_CfgGrab);
			}
		   return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}

SCF_STATE CAM_SM::EnablePortInIdleExecutingState_CfgGrab(s_scf_event const *e) {
    camport * 	portRecord = (camport *) mENSComponent.getPort(RecordPort);
    camport * 	portVF = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);

   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;
			OMX_ERRORTYPE error = pGrabControl->configureStab(pGrabControl->mEsramSuspended,((COmxCamera*)&mENSComponent)->nRotation);
			if(error==OMX_ErrorNone)
			{
			  pGrabControl->mWaitingForEsram=false;
			  pGrabControl->configureGrabResolutionFormat(portRecord,1,((COmxCamera*)&mENSComponent)->nRotation);
			  pGrabControl->configureGrabResolutionFormat(portVF,1);
			  pResourceSharerManager->mRSPing.Ping(0);
			}
			else
			{
			  MSG0("EnablePortInIdleExecutingState_CfgGrab-Resources are suspended, cannot allocate in ESRAM\n");
			  OstTraceFiltStatic0(TRACE_ERROR, "EnablePortInIdleExecutingState_CfgGrab-Resources are suspended, cannot allocate in ESRAM", (&mENSComponent));
			  pGrabControl->mWaitingForEsram=true;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                          SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState_StartVpip);
#endif
			  SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
			}
            return 0;
	   }
	   case SCF_STATE_EXIT_SIG:   EXIT;  return 0;
	   case Q_PONG_SIG:
	   {
		   MSG0("EnablePortInIdleExecutingState_CfgGrab-Q_PONG_SIG\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_CfgGrab-Q_PONG_SIG", (&mENSComponent));
		   
			/* TO DO : send it only in case the settings changed */ 
		   mENSComponent.eventHandler(OMX_EventPortSettingsChanged, (OMX_U32)0, portRecord->getPortIndex());
		   mENSComponent.eventHandler(OMX_EventPortSettingsChanged, (OMX_U32)0, portVF->getPortIndex());
		   
                 SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_SendOmxCb);

		   return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}





#include "IFM_HostNmfProcessingComp.h"

SCF_STATE CAM_SM::EnablePortInIdleExecutingState_SendOmxCb(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
	   {
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);

			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:  EXIT; return 0;

	   case Q_PONG_SIG:
	   {
			MSG0("EnablePortInIdleExecutingState_SendOmxCb-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "EnablePortInIdleExecutingState_SendOmxCb-Q_PONG_SIG", (&mENSComponent));


			IFM_HostNmfProcessingComp* Cam = (IFM_HostNmfProcessingComp*)&mENSComponent.getProcessingComponent();
			(Cam->getNmfSendCommandItf())->sendCommand(OMX_CommandPortEnable,enabledPort);

			SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
			return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


