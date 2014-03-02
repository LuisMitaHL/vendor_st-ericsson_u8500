/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "hsm.h"
#include "VhcElementDefs.h"
#include "ImgConfig.h"
#include "MMIO_Camera.h"
#include "IFM_Trace.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_isp_state_controllingTraces.h"
#endif

/*
 * Is the parent for Controlling the ISP State
 * Each COM_SM inherits from this state machine to control the ISP State
 */

SCF_STATE COM_SM::SHARED_ControlingISPState(s_scf_event const *e) {
    //MSG2("SHARED_ControlingISPState: %d (%s)\n",ControlingISP_ControlType,CError::stringIspctlCtrlType(ControlingISP_ControlType));
    //OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState: %d", (&mENSComponent),ControlingISP_ControlType);
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: ENTRY; 
		MSG2("SHARED_ControlingISPState: %d (%s)\n",ControlingISP_ControlType,CError::stringIspctlCtrlType(ControlingISP_ControlType));
		return 0;
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        case SCF_STATE_INIT_SIG: INIT; 
	switch (ControlingISP_ControlType){
	    case ControlingISP_SendBoot_WaitBootComplete:
            SCF_INIT(&COM_SM::SHARED_ControlingISPState_Boot);
	    break;

	    case ControlingISP_SendSleep_WaitIspSleeping:
	    SCF_INIT(&COM_SM::SHARED_ControlingISPState_Sleep);
	    break;

	    case ControlingISP_SendWakeUp_WaitIspWorkenUp:
	    case ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle:
	    SCF_INIT(&COM_SM::SHARED_ControlingISPState_WakeUp);
	    break;

	    case ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A:
        case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A:
	    {
			MSG0("\n timer TT_ISP_STOP_STREAMING_REQ start\n");
			iTimed_Task[TT_ISP_STOP_STREAMING_REQ].current_time = 0 ;
			iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =TRUE;

			pDeferredEventMgr->onlyDequeuePriorEvents(true);
	        SW3A_stop(SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Stop));
      	    break;
	    }

        case ControlingISP_SendStop_WaitIspSensorStopStreaming:
	    case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle:
	    case ControlingISP_SendStop_WaitIspStopStreaming:
	    {
			MSG0("\n timer TT_ISP_STOP_STREAMING_REQ start\n");
			iTimed_Task[TT_ISP_STOP_STREAMING_REQ].current_time = 0 ;
			iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =TRUE;

	    	SCF_INIT(&COM_SM::SHARED_ControlingISPState_Stop);
	    }
	    break;

	    case ControlingISP_SendRun_WaitIspStreaming_SendExecuting:
	    case ControlingISP_SendRun_WaitIspStreaming_SendPause:
        {
            MSG0("\n timer TT_ISP_START_STREAMING_REQ start\n");
            iTimed_Task[TT_ISP_START_STREAMING_REQ].current_time = 0;
            iTimed_Task[TT_ISP_START_STREAMING_REQ].timer_running =TRUE;

        	pDeferredEventMgr->onlyDequeuePriorEvents(true);
            SCF_INIT(&COM_SM::SHARED_ControlingISPState_Run);
            break;
        }
	    case ControlingISP_SendRun_WaitIspStreaming:
	    case ControlingISP_SendRun_WaitIspLoadReady:
	    {
            MSG0("\n timer TT_ISP_START_STREAMING_REQ start\n");
            iTimed_Task[TT_ISP_START_STREAMING_REQ].current_time = 0;
            iTimed_Task[TT_ISP_START_STREAMING_REQ].timer_running =TRUE;
            SCF_INIT(&COM_SM::SHARED_ControlingISPState_Run);
	        break;
	    }

	    case ControlingISP_SendRun_WaitIspStreaming_Start3A:
	    {
            MSG0("\n timer TT_ISP_START_STREAMING_REQ start\n");
            iTimed_Task[TT_ISP_START_STREAMING_REQ].current_time = 0;
            iTimed_Task[TT_ISP_START_STREAMING_REQ].timer_running =TRUE;
            SW3A_start(SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Run));
	    }
	    break;

 	    case ControlingISP_GetState_GoSlept_Stop3A: 
	    pDeferredEventMgr->onlyDequeuePriorEvents(true);
	    SW3A_stop(SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_ReadHostInterfaceStatus));
	    break;

        case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
	    case ControlingISP_GetState_GoRunning_SendExecuting:
	    case ControlingISP_GetState_GoRunning_SendPause:
	    case ControlingISP_GetState_GoRunning_SendPause_Start3A:
        {
	        pDeferredEventMgr->onlyDequeuePriorEvents(true);
	        SCF_INIT(&COM_SM::SHARED_ControlingISPState_ReadHostInterfaceStatus);
            break;
        }
	    case ControlingISP_GetState_GoRunning:
	    case ControlingISP_GetState_GoRunning_Start3A:	
	    case ControlingISP_GetState_GoSlept:
	        SCF_INIT(&COM_SM::SHARED_ControlingISPState_ReadHostInterfaceStatus);
	        break;

	    default:
	        DBC_ASSERT(0);
	        break;
	}
	return 0;
        case SCF_PARENT_SIG:break;
        case EVT_ISPCTL_ERROR_SIG:
        {
           /* Today assert in case of Error coming from the ISP : no error recovery is implemented */
	   DBGT_ERROR("SHARED_ControlingISPState-EVT_ISPCTL_ERROR_SIG\n");
	   OstTraceFiltStatic0(TRACE_ERROR, "SHARED_ControlingISPState-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
	   if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)  {
	       DBGT_ERROR("ControlingISPState-ISP_POLLING_TIMEOUT_ERROR;\n");
	       OstTraceFiltStatic0(TRACE_ERROR, "ControlingISPState-ISP_POLLING_TIMEOUT_ERROR;", (&mENSComponent));
	       DBC_ASSERT(0);
	   }
	   if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)  {
	       DBGT_ERROR("ControlingISPState-ISP_DOES_NOT_RESPOND_ERROR;\n");
	       OstTraceFiltStatic0(TRACE_ERROR, "ControlingISPState-ISP_DOES_NOT_RESPOND_ERROR;", (&mENSComponent));
	       DBC_ASSERT(0);
	   }
	   /*
	   if (e->type.ispctlError.error_id == ISP_MASTER_I2C_ACCESS_FAILURE)  {
	       DBGT_ERROR("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;\n");
	       OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;", (&mENSComponent));
	       DBC_ASSERT(0);
	   }*/
        }
	case EVT_ISPCTL_INFO_SIG :
	{
		MSG0("SHARED_ControlingISPState-EVT_ISPCTL_INFO_SIG;\n");
		OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
		if (ISP_READ_DONE == e->type.ispctlInfo.info_id) /* Acknowledge to read */
		{
			PowerCommand_te ecommand = (PowerCommand_te)e->type.ispctlInfo.value;
			Flag_te status = Flag_e_FALSE;
			switch (ecommand)
			{
				case PowerCommand_e_voltage_on:
					MSG0("SHARED_ControlingISPState command_e_voltage_on\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState command_e_voltage_on", (&mENSComponent));
					if(MMIO_Camera::powerSensor(OMX_TRUE))
					{
						DBGT_ERROR ("ERROR in MMIO_Camera::powerSensor !\n");
						OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::powerSensor !", (&mENSComponent));
					}
					else status = Flag_e_TRUE;
					break;
				case PowerCommand_e_voltage_off:
					MSG0("SHARED_ControlingISPState command_e_voltage_off\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState command_e_voltage_off", (&mENSComponent));
					if(MMIO_Camera::powerSensor(OMX_FALSE))
					{
						DBGT_ERROR ("ERROR in MMIO_Camera::powerSensor !\n");
						OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::powerSensor !", (&mENSComponent));
					}
					else status = Flag_e_TRUE;
					break;
				case PowerCommand_e_ext_clk_on:
					MSG0("SHARED_ControlingISPState command_e_external_clock_on\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState command_e_external_clock_on", (&mENSComponent));
					if(MMIO_Camera::setExtClk(OMX_TRUE))
					{
						DBGT_ERROR("ERROR in MMIO_Camera::setExtClk !\n");
						OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setExtClk !", (&mENSComponent));
					}
					else status = Flag_e_TRUE; 
					break;
				case PowerCommand_e_ext_clk_off:
					MSG0("SHARED_ControlingISPState command_e_external_clock_off\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState command_e_external_clock_off", (&mENSComponent));
					if(MMIO_Camera::setExtClk(OMX_FALSE))
					{
						DBGT_ERROR ("ERROR in MMIO_Camera::setExtClk !\n");
						OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setExtClk !", (&mENSComponent));
					}
					else status = Flag_e_TRUE;
					break;
				case PowerCommand_e_x_shutdown_on:
					MSG0("SHARED_ControlingISPState command_e_external_shutdown_on\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState command_e_external_shutdown_on", (&mENSComponent));
					break;
				case PowerCommand_e_x_shutdown_off:
					MSG0("SHARED_ControlingISPState command_e_external_shutdown_off\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState command_e_external_shutdown_off", (&mENSComponent));
					break;

				default:
					break;

			}
			pIspctlCom->queuePE(SensorPowerManagement_Control_e_Flag_Result_Byte0,(t_uint32)status);
			pIspctlCom->queuePE(SensorPowerManagement_Control_e_PowerAction_Byte0,(t_uint32)PowerAction_e_complete);
			pIspctlCom->processQueue();
		}else if (e->type.ispctlInfo.info_id == ISP_POWER_NOTIFICATION){
			switch (ControlingISP_ControlType){
				case ControlingISP_SendBoot_WaitBootComplete:
				case ControlingISP_SendSleep_WaitIspSleeping:
				case ControlingISP_SendWakeUp_WaitIspWorkenUp:
				case ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle:
	    			case ControlingISP_GetState_GoRunning_SendExecuting:
				case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
	    			case ControlingISP_GetState_GoRunning_SendPause:
				case ControlingISP_GetState_GoRunning_SendPause_Start3A:
				case ControlingISP_GetState_GoRunning:
			        case ControlingISP_GetState_GoRunning_Start3A:		
	    			case ControlingISP_GetState_GoSlept:
				case ControlingISP_GetState_GoSlept_Stop3A:
				pIspctlCom->requestPE(SensorPowerManagement_Status_e_PowerCommand_Byte0);
				break;
				default:
				DBC_ASSERT(0);
				break;
				
			}
		}else break;
		return 0;

	}
	case EVT_ISPCTL_LIST_INFO_SIG:
		MSG0("SHARED_ControlingISPState-EVT_ISPCTL_LIST_INFO_SIG;\n");
		OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_ControlingISPState-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
		return 0;

        case Q_DEFERRED_EVENT_SIG:
        {
        	MSG0("ControlingISP-DEFERRED_EVENT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISP-DEFERRED_EVENT;", (&mENSComponent));
        	return 0;
        }
        default:
        break;
    }
    return SCF_STATE_PTR(&COM_SM::OMX_Executing);
}

SCF_STATE COM_SM::SHARED_ControlingISPState_Boot(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: 
	    ENTRY; 
	    MSG0("ControlingISPState_Boot-ENTRY;\n"); 
	    OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Boot-ENTRY;", (&mENSComponent)); 
	    pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_BOOT);
	    return 0;
        case SCF_STATE_EXIT_SIG:
        {
        	EXIT;
        	MSG0("ControlingISPState_Boot-EXIT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Boot-EXIT;", (&mENSComponent));
        	return 0;
        }
       	case SCF_STATE_INIT_SIG:
       	{
       		INIT;
       		MSG0("ControlingISPState_Boot-INIT;\n");
       		OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Boot-INIT;", (&mENSComponent));
       		return 0;
       	}

	case EVT_ISPCTL_INFO_SIG:
        {
            MSG1("SHARED_ControlingISPState_Boot-EVT_ISPCTL_INFO_SIG : %d (%s)\n",e->type.ispctlInfo.info_id,CError::stringIspctlInfo(e->type.ispctlInfo.info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState_Boot-EVT_ISPCTL_INFO_SIG : %d", (&mENSComponent),e->type.ispctlInfo.info_id);

	    if (e->type.ispctlInfo.info_id ==ISP_BOOT_COMPLETE){
		MSG0("COM_SM::SHARED_ControlingISPState_Boot--isp_state = ISP_STATE_BOOTED;  \n");
		isp_state = ISP_STATE_BOOTED;		
		if (ControlingISP_ControlType == ControlingISP_SendBoot_WaitBootComplete) {
    			SCF_PSTATE next;
		        SM_POP_STATE(next);
		        SCF_TRANSIT_TO_PSTATE(next);
		}
		else DBC_ASSERT(0);
	    }
 	    else if (e->type.ispctlInfo.info_id == ISP_WRITE_DONE){
             /* Acknowledge write CMD*/
            }
	    else if (e->type.ispctlInfo.info_id == ISP_CDCC_AVAILABLE)
	    {
//		SCF_TRANSIT_TO(&COM_SM::PowerUp_STC);
	    }
	    else break; /*ISP_POWER_NOTIFICATION handled by the parent */
	    return 0;
	}
       	default:
       	break;
   }
   return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}

SCF_STATE COM_SM::SHARED_ControlingISPState_Sleep(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: 
	    ENTRY; 
	    MSG0("ControlingISPState_Sleep-ENTRY;\n"); 
	    OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Sleep-ENTRY;", (&mENSComponent)); 
	    pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_SLEEP);
	    return 0;
        case SCF_STATE_EXIT_SIG:
        {
        	EXIT;
        	MSG0("ControlingISPState_Sleep-EXIT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Sleep-EXIT;", (&mENSComponent));
        	return 0;
        }
       	case SCF_STATE_INIT_SIG:
       	{
       		INIT;
       		MSG0("ControlingISPState_Sleep-INIT;\n");
       		OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Sleep-INIT;", (&mENSComponent));
       		return 0;
       	}

	case EVT_ISPCTL_INFO_SIG:
        {
            MSG1("SHARED_ControlingISPState_Sleep-EVT_ISPCTL_INFO_SIG : %d (%s)\n",e->type.ispctlInfo.info_id,CError::stringIspctlInfo(e->type.ispctlInfo.info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState_Sleep-EVT_ISPCTL_INFO_SIG : %d", (&mENSComponent),e->type.ispctlInfo.info_id);
	    if (e->type.ispctlInfo.info_id == ISP_SLEEPING){
		MSG0(" COM_SM::SHARED_ControlingISPState_Sleep--Setting isp_state = ISP_STATE_SLEPT;  \n");
		isp_state = ISP_STATE_SLEPT;
		switch (ControlingISP_ControlType){
		    case ControlingISP_GetState_GoSlept_Stop3A:
		{
			// if (MMIO_Camera::desinitBoard())
			//DBGT_ERROR("ERROR in MMIO_Camera::desinitBoard !\n"); /* Release GPIO and ARM */
			//OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::desinitBoard !", (&mENSComponent)); /* Release GPIO and ARM */
			//pIspctlCom->allowSleep(); // don't use this for the moment. This is supposed to send DSP to TCM retention		
			SCF_PSTATE next;
			SM_POP_STATE(next);
			SCF_TRANSIT_TO_PSTATE(next);
			pDeferredEventMgr->onlyDequeuePriorEvents(false);
			break;
		}
		    case ControlingISP_SendSleep_WaitIspSleeping:
		    case ControlingISP_GetState_GoSlept:
            {
		        // if (MMIO_Camera::desinitBoard())
		        //DBGT_ERROR("ERROR in MMIO_Camera::desinitBoard !\n"); /* Release GPIO and ARM */
		        //OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::desinitBoard !", (&mENSComponent)); /* Release GPIO and ARM */
		        //pIspctlCom->allowSleep(); // don't use this for the moment. This is supposed to send DSP to TCM retention
		            SCF_PSTATE next;
		        SM_POP_STATE(next);
		        SCF_TRANSIT_TO_PSTATE(next);
		        break;
            }
		    default:
		        DBC_ASSERT(0);
		        break;
		}
	    }
 	    else if (e->type.ispctlInfo.info_id == ISP_WRITE_DONE){
             /* Acknowledge write CMD*/
            }
	    else break;/*ISP_POWER_NOTIFICATION  handled by the parent */
	    return 0;
	}
       	default:
       	break;
   }
   return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}

SCF_STATE COM_SM::SHARED_ControlingISPState_WakeUp(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: 
	    ENTRY; 
	    MSG0("ControlingISPState_WakeUp-ENTRY;\n"); 
	    OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_WakeUp-ENTRY;", (&mENSComponent)); 
	    //pIspctlCom->preventSleep(); // Don't use this for the moment, it is to exit DSP TCM retention
            //if (MMIO_Camera::initBoard())
	    	//DBGT_ERROR("ERROR in MMIO_Camera::initBoard !\n");
	    	//OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::initBoard !", (&mENSComponent));
	    pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_WAKEUP);
	    return 0;
        case SCF_STATE_EXIT_SIG:
        {
        	EXIT;
        	MSG0("ControlingISPState_WakeUp-EXIT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_WakeUp-EXIT;", (&mENSComponent));
        	return 0;
        }
       	case SCF_STATE_INIT_SIG:
       	{
       		INIT;
       		MSG0("ControlingISPState_WakeUp-INIT;\n");
       		OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_WakeUp-INIT;", (&mENSComponent));
       		return 0;
       	}

	case EVT_ISPCTL_INFO_SIG:
        {
            MSG1("SHARED_ControlingISPState_WakeUp-EVT_ISPCTL_INFO_SIG : %d (%s)\n",
				e->type.ispctlInfo.info_id,CError::stringIspctlInfo(e->type.ispctlInfo.info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState_WakeUp-EVT_ISPCTL_INFO_SIG : %d", (&mENSComponent),e->type.ispctlInfo.info_id);
	    if (e->type.ispctlInfo.info_id == ISP_WOKEN_UP){
			
		MSG0(" COM_SM::SHARED_ControlingISPState_WakeUp--Setting isp_state = ISP_STATE_WAKEUP;  \n");
		isp_state = ISP_STATE_WAKEUP;
		
		switch(ControlingISP_ControlType){
		    case ControlingISP_SendWakeUp_WaitIspWorkenUp:
		    {
		        SCF_PSTATE next;
  		        SM_POP_STATE(next);
		        SCF_TRANSIT_TO_PSTATE(next);
		    }
		    break;
		    case ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle:
		    {
		         pOmxStateMgr->changeState(OMX_StateIdle);
		         SCF_PSTATE next;
		         SM_POP_STATE(next);
		         SCF_TRANSIT_TO_PSTATE(next);
		         pDeferredEventMgr->onlyDequeuePriorEvents(false);
		    }
		    break;
		    case ControlingISP_GetState_GoRunning_Start3A:		
		    case ControlingISP_GetState_GoRunning_SendPause_Start3A:
		    case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
		    SW3A_start(SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Run));
		    break;	
		    case ControlingISP_GetState_GoRunning_SendExecuting:
		    case ControlingISP_GetState_GoRunning_SendPause:
		    case ControlingISP_GetState_GoRunning:
		    SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState_Run);
		    break;
 		    default:
		    DBC_ASSERT(0);
		    break;
		}
	    }	  

 	    else if (e->type.ispctlInfo.info_id == ISP_WRITE_DONE){
             /* Acknowledge write CMD*/
            }
	    else break;/*ISP_POWER_NOTIFICATION and handled by the parent */
	    return 0;
	}
       	default:
       	break;
   }
   return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}

SCF_STATE COM_SM::SHARED_ControlingISPState_Stop(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: 
	    ENTRY; 
	    MSG0("ControlingISPState_Stop-ENTRY;\n"); 
	    OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Stop-ENTRY;", (&mENSComponent)); 
	    if(OMX_TRUE == bTrace_latency)
		{
			//performance traces start
			latencyMeasure(&mTime);
			mTime_e_OMXCAM_StartStreaming_t0= mTime;
			mTime_e_OMXCAM_StartStreaming_t1= mTime;
			OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_StartStreaming %d", 1);
			OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_StartStreaming %d", 0);
			//performance traces end
		}
	    pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_STOP);
	    return 0;
        case SCF_STATE_EXIT_SIG:
        {
        	EXIT;
        	MSG0("ControlingISPState_Stop-EXIT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Stop-EXIT;", (&mENSComponent));
        	return 0;
        }
       	case SCF_STATE_INIT_SIG:
       	{
       		INIT;
       		MSG0("ControlingISPState_Stop-INIT;\n");
       		OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Stop-INIT;", (&mENSComponent));
       		return 0;
       	}

	case EVT_ISPCTL_INFO_SIG:
        {
            MSG1("SHARED_ControlingISPState_Stop-EVT_ISPCTL_INFO_SIG : %d (%s)\n",
				e->type.ispctlInfo.info_id,CError::stringIspctlInfo(e->type.ispctlInfo.info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState_Stop-EVT_ISPCTL_INFO_SIG : %d", (&mENSComponent),e->type.ispctlInfo.info_id);

	    if (e->type.ispctlInfo.info_id == ISP_STOP_STREAMING){
		    //stop timer TT_ISP_STOP_STREAMING_REQ
		    MSG0("\n timer TT_ISP_STOP_STREAMING_REQ stop\n");
		    iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =FALSE;
		switch (ControlingISP_ControlType){
		    case ControlingISP_SendStop_WaitIspStopStreaming:
		    SCF_PSTATE next;
		    SM_POP_STATE(next);
		    SCF_TRANSIT_TO_PSTATE(next);
		    break;
		    case ControlingISP_GetState_GoSlept:
		    case ControlingISP_GetState_GoSlept_Stop3A:
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming:
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A:
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle:
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A:
		    break;
		    default:
		    DBC_ASSERT(0);
		    break;
	        }
	    }		
	    else if (e->type.ispctlInfo.info_id == ISP_SENSOR_STOP_STREAMING){
		switch (ControlingISP_ControlType){
		    case ControlingISP_GetState_GoSlept:
		    case ControlingISP_GetState_GoSlept_Stop3A:
		    SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState_Sleep);
		    break;
		     
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A:
	    	{
			    //stop timer TT_ISP_STOP_STREAMING_REQ
			    MSG0("\n timer TT_ISP_STOP_STREAMING_REQ stop\n");
			    iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =FALSE;

		    	    //pDeferredEventMgr->onlyDequeuePriorEvents(false); ER: 362910
			    /*For shutterlag*/	
			    MSG0(" COM_SM::SHARED_ControlingISPState_Stop-- Setting isp_state = ISP_STATE_STOPSTREAMING_STOP3A;  \n");
			    isp_state = ISP_STATE_STOP_STREAMING_STOP3A;
			    SCF_PSTATE next;
			    SM_POP_STATE(next);
			    SCF_TRANSIT_TO_PSTATE(next);
			    break;
		    }
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming:
		    {
		    	//stop timer TT_ISP_STOP_STREAMING_REQ
		    	MSG0("\n timer TT_ISP_STOP_STREAMING_REQ stop\n");
                iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =FALSE;
                isp_state = ISP_STATE_STOP_STREAMING;
                SCF_PSTATE next;
		    	SM_POP_STATE(next);
		    	SCF_TRANSIT_TO_PSTATE(next);
		        break;
		    }
	        case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A:
		    {
			    //stop timer TT_ISP_STOP_STREAMING_REQ
			    MSG0("\n timer TT_ISP_STOP_STREAMING_REQ stop\n");
			    iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =FALSE;
			    pDeferredEventMgr->onlyDequeuePriorEvents(false);
			    /*For shutterlag*/
			    MSG0("COM_SM::SHARED_ControlingISPState_GetControlTypeSetting- isp_state = ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE;  \n");
			    isp_state = ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE;
			    SCF_PSTATE next;
			    SM_POP_STATE(next);
			    SCF_TRANSIT_TO_PSTATE(next);
		    	break;
		    }
		    case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle:
	    	{
		    	//stop timer TT_ISP_STOP_STREAMING_REQ
			    MSG0("\n timer TT_ISP_STOP_STREAMING_REQ stop\n");
			    MSG0("COM_SM::SHARED_ControlingISPState_GetControlTypeSetting- isp_state = ISP_STATE_STOP_STREAMING_SENDIDLE;  \n");
			    isp_state = ISP_STATE_STOP_STREAMING_SENDIDLE;
		    	iTimed_Task[TT_ISP_STOP_STREAMING_REQ].timer_running =FALSE;
		    	pOmxStateMgr->changeState(OMX_StateIdle);
		    	SCF_PSTATE next;
		    	SM_POP_STATE(next);
		    	SCF_TRANSIT_TO_PSTATE(next);
	    	}
		    break;
		    default:
		    DBC_ASSERT(0);
		    break;
		}
	    }
 	    else if (e->type.ispctlInfo.info_id == ISP_WRITE_DONE){
             /* Acknowledge write CMD*/
            }
	    else break;
	    return 0;
	}
       	default:
       	break;
   }
   return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}

SCF_STATE COM_SM::SHARED_ControlingISPState_Run(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: 
	    ENTRY; 
	    MSG0("ControlingISPState_Run-ENTRY;\n"); 
	    OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Run-ENTRY;", (&mENSComponent)); 
	    if(OMX_TRUE == bTrace_latency)
		{
			//performance traces start
			latencyMeasure(&mTime);
			mTime_e_OMXCAM_StopISP_t0= mTime;
			mTime_e_OMXCAM_StopISP_t1= mTime;
			OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_StopISP %d", 0);
			OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_StopISP %d", 1);
			//performance traces end
		}
	    pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_RUN);
	    return 0;
        case SCF_STATE_EXIT_SIG:
        {
        	EXIT;
        	MSG0("ControlingISPState_Run-EXIT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Run-EXIT;", (&mENSComponent));
        	return 0;
        }
       	case SCF_STATE_INIT_SIG:
       	{
       		INIT;
       		MSG0("ControlingISPState_Run-INIT;\n");
       		OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Run-INIT;", (&mENSComponent));
       		return 0;
       	}

	case EVT_ISPCTL_INFO_SIG:
        {
            MSG1("SHARED_ControlingISPState_Run-EVT_ISPCTL_INFO_SIG : %d (%s)\n",e->type.ispctlInfo.info_id,CError::stringIspctlInfo(e->type.ispctlInfo.info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState_Run-EVT_ISPCTL_INFO_SIG : %d", (&mENSComponent),e->type.ispctlInfo.info_id);
	    if (e->type.ispctlInfo.info_id == ISP_STREAMING){
			
			MSG0("COM_SM::SHARED_ControlingISPState_GetControlTypeSetting- isp_state = ISP_STATE_STREAMING;  \n");
			isp_state = ISP_STATE_STREAMING;
			MSG0("\n timer TT_ISP_START_STREAMING_REQ stop\n");
			iTimed_Task[TT_ISP_START_STREAMING_REQ].timer_running =FALSE;

		switch (ControlingISP_ControlType){
		    case ControlingISP_SendRun_WaitIspStreaming:
		    case ControlingISP_SendRun_WaitIspStreaming_Start3A:
		    case ControlingISP_GetState_GoRunning:
		    case ControlingISP_GetState_GoRunning_Start3A:
		    break;

	            case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
		    case ControlingISP_GetState_GoRunning_SendExecuting:
		    case ControlingISP_SendRun_WaitIspStreaming_SendExecuting:
     		    pOmxStateMgr->changeState(OMX_StateExecuting);
		    pDeferredEventMgr->onlyDequeuePriorEvents(false);
		    break;
		    case ControlingISP_GetState_GoRunning_SendPause:
		    case ControlingISP_GetState_GoRunning_SendPause_Start3A:
		    case ControlingISP_SendRun_WaitIspStreaming_SendPause:
		    pOmxStateMgr->changeState(OMX_StatePause);
		    pDeferredEventMgr->onlyDequeuePriorEvents(false);
		    break;
                    default:
		    DBC_ASSERT(0);
		    break;
	         }
	    SCF_PSTATE next;
	    SM_POP_STATE(next);
	    SCF_TRANSIT_TO_PSTATE(next);
            }
	    else if (e->type.ispctlInfo.info_id == ISP_LOAD_READY){
	        if (ControlingISP_ControlType == ControlingISP_SendRun_WaitIspLoadReady) {
				MSG0("\n timer TT_ISP_START_STREAMING_REQ stop\n");
				iTimed_Task[TT_ISP_START_STREAMING_REQ].timer_running =FALSE;
		    	SCF_PSTATE next;
  		        SM_POP_STATE(next);
			    SCF_TRANSIT_TO_PSTATE(next);
		}
		else DBC_ASSERT(0);
	    }
 	    else if (e->type.ispctlInfo.info_id == ISP_WRITE_DONE){
             /* Acknowledge write CMD*/
            }
	    else break;
	    return 0;
	}
       	default:
       	break;
   }
   return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}

SCF_STATE COM_SM::SHARED_ControlingISPState_ReadHostInterfaceStatus(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG: 
	    ENTRY; 
	    MSG0("ControlingISPState_ReadHostInterfaceStatus-ENTRY;\n"); 
	    OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_ReadHostInterfaceStatus-ENTRY;", (&mENSComponent)); 
	    pIspctlCom->requestPE(HostInterface_Status_e_HostInterfaceLowLevelState_Current_Byte0);
	    return 0;
        case SCF_STATE_EXIT_SIG:
        {
        	EXIT;
        	MSG0("ControlingISPState_ReadHostInterfaceStatus-EXIT;\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_ReadHostInterfaceStatus-EXIT;", (&mENSComponent));
        	return 0;
        }
       	case SCF_STATE_INIT_SIG:
       	{
       		INIT;
       		MSG0("ControlingISPState_ReadHostInterfaceStatus-INIT;\n");
       		OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_ReadHostInterfaceStatus-INIT;", (&mENSComponent));
       		return 0;
       	}

	case EVT_ISPCTL_INFO_SIG:
        {
            MSG1("SHARED_ControlingISPState_ReadHostInterfaceStatus-EVT_ISPCTL_INFO_SIG : %d\n",e->type.ispctlInfo.info_id);
            OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_ControlingISPState_ReadHostInterfaceStatus-EVT_ISPCTL_INFO_SIG : %d", (&mENSComponent),e->type.ispctlInfo.info_id);
            if (e->type.ispctlInfo.info_id == ISP_READ_DONE ){
		if (e->type.ispctlInfo.value == HostInterfaceLowLevelState_e_SLEPT){
		    switch(ControlingISP_ControlType){
			case ControlingISP_GetState_GoSlept:
			case ControlingISP_GetState_GoSlept_Stop3A:
			SCF_PSTATE next;
  		        SM_POP_STATE(next);
			SCF_TRANSIT_TO_PSTATE(next);
			break;
		 	case ControlingISP_GetState_GoRunning_SendExecuting:
			case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
			case ControlingISP_GetState_GoRunning_SendPause:
			case ControlingISP_GetState_GoRunning_SendPause_Start3A:
			case ControlingISP_GetState_GoRunning:
			case ControlingISP_GetState_GoRunning_Start3A:
			SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState_WakeUp);
			break;
		     	default:
		    	DBC_ASSERT(0);
		    	break;
		    }
		}
		else if (e->type.ispctlInfo.value == HostInterfaceLowLevelState_e_STOPPED){
		    switch(ControlingISP_ControlType){
			case ControlingISP_GetState_GoSlept:
			case ControlingISP_GetState_GoSlept_Stop3A:
		  	SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState_Sleep);
			break;
			case ControlingISP_GetState_GoRunning_SendPause_Start3A:
			case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
			case ControlingISP_GetState_GoRunning_Start3A:
			SW3A_start(SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Run));
			break;
		 	case ControlingISP_GetState_GoRunning_SendExecuting:
			case ControlingISP_GetState_GoRunning_SendPause:
			case ControlingISP_GetState_GoRunning:
			SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState_Run);
			break;
		     	default:
		    	DBC_ASSERT(0);
		    	break;
		    }
		}	
		else if (e->type.ispctlInfo.value == HostInterfaceLowLevelState_e_RUNNING){
		    switch(ControlingISP_ControlType){
			case ControlingISP_GetState_GoSlept:
			case ControlingISP_GetState_GoSlept_Stop3A:
			SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState_Stop);
			break;
			case ControlingISP_GetState_GoRunning_Start3A:
			SW3A_start(whatNext);	
			break;
			case ControlingISP_GetState_GoRunning:
			{
			    SCF_PSTATE next;
			    SM_POP_STATE(next);
			    SCF_TRANSIT_TO_PSTATE(next);
			}
			break;
			case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:
			pOmxStateMgr->changeState(OMX_StateExecuting);
			SW3A_start(whatNext);
		        pDeferredEventMgr->onlyDequeuePriorEvents(false);
			break;
		 	case ControlingISP_GetState_GoRunning_SendExecuting:
        	{
                pOmxStateMgr->changeState(OMX_StateExecuting);
			    SCF_PSTATE next;
			    SM_POP_STATE(next);
			    SCF_TRANSIT_TO_PSTATE(next);
		        pDeferredEventMgr->onlyDequeuePriorEvents(false);
			}
			break;
			case ControlingISP_GetState_GoRunning_SendPause:
			{
			    pOmxStateMgr->changeState(OMX_StatePause);
			    SCF_PSTATE next;
			    SM_POP_STATE(next);
			    SCF_TRANSIT_TO_PSTATE(next);
			    pDeferredEventMgr->onlyDequeuePriorEvents(false);
			}
			break;
			case ControlingISP_GetState_GoRunning_SendPause_Start3A:
			pOmxStateMgr->changeState(OMX_StatePause);
			SW3A_start(whatNext);
			pDeferredEventMgr->onlyDequeuePriorEvents(false);
            break;
			default:
		    	DBC_ASSERT(0);
		    	break;
		    }	
		}

	    }
	    else break;
	    return 0;
	}
       	default:
       	break;
   }
   return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}


SCF_STATE COM_SM::PowerUp_STC(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			MSG0("ControlingISPState_PowerUp_STC-ENTRY;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_PowerUp_STC-ENTRY;", (&mENSComponent));
			DBC_ASSERT(0);
			return 0;
		case SCF_STATE_EXIT_SIG:
			MSG0("ControlingISPState_PowerUp_STC-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_PowerUp_STC-EXIT;", (&mENSComponent));
			DBC_ASSERT(0);
			return 0;
		default:
			DBC_ASSERT(0);
			break;
	}
	MSG0("PowerUp_STC\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC", (&mENSComponent));
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}

SCF_STATE COM_SM::SW3A_Start(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			MSG0("ControlingISPState_SW3A_Start-ENTRY;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_SW3A_Start-ENTRY;", (&mENSComponent));
			DBC_ASSERT(0);
			return 0;
		case SCF_STATE_EXIT_SIG:
			MSG0("ControlingISPState_SW3A_Start-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_SW3A_Start-EXIT;", (&mENSComponent));
			DBC_ASSERT(0);
			return 0;
		default:
			DBC_ASSERT(0);
			break;
	}
	MSG0("SW3A_Start\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Start", (&mENSComponent));
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState);
}
void COM_SM::SW3A_start(SCF_PSTATE whatNext){
	DBC_ASSERT(0);
}
void COM_SM::SW3A_stop(SCF_PSTATE whatNext){
	DBC_ASSERT(0);
}

