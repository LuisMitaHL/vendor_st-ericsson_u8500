/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_opmode_transition_record_statemachineTraces.h"
#endif


/* */
/* ExitingRecord state machine */
/**/
SCF_STATE CAM_SM::ExitingRecord(s_scf_event const *e)
{
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			return 0;
		}
		case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
		case SCF_STATE_INIT_SIG: INIT; SCF_INIT(&CAM_SM::ExitingRecord_WaitingBuffers); return 0;
		case EVT_ISPCTL_ERROR_SIG:
		{
			DBGT_ERROR("ExitingRecord-EVT_ISPCTL_ERROR_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "ExitingRecord-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
			if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)  {
				DBGT_ERROR("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;\n");
				OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_POLLING_TIMEOUT_ERROR;", (&mENSComponent));
				DBC_ASSERT(0);
			}
			if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)  {
				DBGT_ERROR("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;\n");
				OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;", (&mENSComponent));
				DBC_ASSERT(0);
			}
            return 0;
		}
		case Q_DEFERRED_EVENT_SIG:
			MSG0("ExitingRecord-DEFERRED_EVENT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExitingRecord-DEFERRED_EVENT;", (&mENSComponent));
			return 0;
		default: break;

	}
	return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);
}

SCF_STATE CAM_SM::ExitingRecord_WaitingBuffers(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        case SCF_STATE_EXIT_SIG: 
            EXIT; 
            return 0;
        case Q_PONG_SIG:
        {
	     	MSG0("CAM_SM::ExitingRecord_WaitingBuffers-Q_PONG_SIG;\n");
            pOpModeMgr->UpdateCurrentOpMode();
            pDeferredEventMgr->onlyDequeuePriorEvents(true);
            // reset internal variables
            pOpModeMgr->isEOS[CAMERA_PORT_OUT0] = OMX_FALSE;
            pOpModeMgr->isEOS[CAMERA_PORT_OUT2] = OMX_FALSE;
		
     		// we never need an EOS on VPB0 => we can abort capture
            pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT0);
		 	pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT1);
	     	GrabAbortStatus = GRAB_ABORT_REQUESTED;
            // streaming is not stopped on VPB2 (only paused during still capture) => we can abort capture
            if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2])
            {		
                pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT2);
            }
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
            // streaming is not stopped but MPC doesn't have buffer => we can abort capture.
            else if (pOpModeMgr->nBufferAtMPC[CAMERA_PORT_OUT2] == 0)
            {
                pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT2);
            }
#endif
            // streaming is stopped on VPB2 => we need to wait for a buffer to flag it with EOS
            else
            {
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
              pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT2); // TODO: can stop immediately ?
#else
                pGrabControl->setEndCapture(CAMERA_PORT_OUT2);		
#endif
            }

            /* Handle properly the case of capturing bit that is reset 
               while the port is not enabled. In this case, we cannot 
               receive nor generate the EOS, so we need to trigger here 
               a fake internal EOS to avoid hanging the state machine */
            if(!(mENSComponent.getPort(CAMERA_PORT_OUT2)->isEnabled()))
            {
	    		s_scf_event event;
		    	event.type.other=CAMERA_PORT_OUT2;
		    	event.sig = CAMERA_EOS_VPB02_SIG;
			    event.type.capturingStruct.nPortIndex=CAMERA_PORT_OUT2;
		    	pDeferredEventMgr->queuePriorEvent(&event);
				
            }
			
			SM_PUSH_STATE_STATIC(&CAM_SM::RendezvousState);
			ControlingISP_ControlType = ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A;
			DBC_ASSERT(p3AComponentManager->whatNextAfterRendezvous== NULL); 
			//for ER 427419 start
			MSG1("\nbMoveTo Video = %d\n",bMoveToVideo);
			if(OMX_TRUE == bMoveToVideo)
			{
			    MSG0("CAM_SM::OMX_TRUE == bMoveToVideo\n");
				p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::ExitingPreview_StoppingFlash);
			}
			else
			{
			    MSG0("CAM_SM::OMX_FALSE == bMoveToVideo\n");
				p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::ExitingRecord_StoppingFlash);
			}
			//for ER 427419 start
			SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);	
		
            return 0;
        }
        default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ExitingRecord);
}



SCF_STATE CAM_SM::ExitingRecord_StoppingFlash(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
            //Fix for ER 413992: The light will off for a short time when start record and end record under camcorder mode
            t_sw3A_FlashDirective flashDirective = p3AComponentManager->mFlashController.getDirective();
            if((pOpModeMgr->CurrentOperatingMode != OpMode_Cam_VideoPreview) || (flashDirective.flashState != SW3A_FLASH_VIDEOLED))
            {
                // disable flash mode
                cam_flash_err_e err = p3AComponentManager->mFlashController.unConfigure();
                if (err != CAM_FLASH_ERR_NONE)
                {
                    DBGT_ERROR("Error: Flash unconfigure failed.\n");
                    OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash unconfigure failed.", (&mENSComponent));
                    DBC_ASSERT(0);
                }
            }
            else
            {
                MSG0("from VideoRecord to VideoVF, if VIDEOLED is on ... skipping flash unConfigure\n");
            }

			MSG0("OMX_BUFFERFLAG_EOS + OMX_EventBufferFlag\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "OMX_BUFFERFLAG_EOS + OMX_EventBufferFlag;", (&mENSComponent));
			mENSComponent.eventHandlerCB(OMX_EventBufferFlag, RecordPort, OMX_BUFFERFLAG_EOS, 0);

            if(OMX_TRUE == pOpModeMgr->AutoPauseAfterCapture) {
                // here the camera component calls itself through OMX interface
                // It is important to do this internally (through mENSComponent)
                // and NOT through OMX_SendCommand, to avoid going through ENS wrapper 
                // thus taking twice the mutex !
                mENSComponent.SendCommand(&mENSComponent, OMX_CommandStateSet, OMX_StatePause, NULL);
            }

			pResourceSharerManager->mRSRelease.Release(CAMERA_VF);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
		case Q_RELEASED_SIG:
		{
			MSG0("ExitingRecord_StoppingFlash-Q_RELEASED_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExitingRecord_StoppingFlash-Q_RELEASED_SIG;", (&mENSComponent));
			pDeferredEventMgr->onlyDequeuePriorEvents(false);
			SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
			return 0;
		}
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::ExitingRecord);
}

