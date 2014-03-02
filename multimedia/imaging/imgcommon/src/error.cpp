/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define OMXCOMPONENT "ERROR"

#include "error.h"


#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


CError::CError()
{
	//IN0("\n");
	//OUT0("\n");
}

CError::~CError()
{
	//IN0("\n");
	//OUT0("\n");
}

const char * CError::stringIspctlInfo(e_ispctlInfo index)
{
	   const char *result;
	   switch(index) {
	    case ISP_INF_NONE:result=  "ISP_INF_NONE"; break;
		case ISP_READ_DONE :result=  "ISP_READ_DONE"; break;
	    case ISP_WRITE_DONE:result=  "ISP_WRITE_DONE"; break;
	    case ISP_INIT_DONE :result=  "ISP_INIT_DONE"; break;
	    case ISP_READLIST_DONE :result=  "ISP_READLIST_DONE"; break;
	    case ISP_WRITELIST_DONE :result=  "ISP_WRITELIST_DONE"; break;
	    case ISP_POLLING_PE_VALUE_DONE :result=  "ISP_POLLING_PE_VALUE_DONE"; break;
	    case ISP_HOST_COMMS_READY:result=  "ISP_HOST_COMMS_READY"; break;
	    case ISP_BOOT_COMPLETE:result=  "ISP_BOOT_COMPLETE"; break;
	    case ISP_SLEEPING:result=  "ISP_SLEEPING"; break; 						//= 0x9,	/**< Means ISP has transitioned to the sleep state following a host SLEEP command*/
	    case ISP_WOKEN_UP:result=  "ISP_WOKEN_UP"; break; 						//= 0xA,	/**< Means ISP has transitioned out of the sleep state to stop state following a host WAKE_UP command*/
	    case ISP_STREAMING:result=  "ISP_STREAMING"; break; 						//= 0xB,	/**< Means ISP has started the streaming operation following a host RUN command*/
	    case ISP_STOP_STREAMING:result=  "ISP_STOP_STREAMING"; break; 					//= 0xC,	/**< Means ISP has stopped streaming, following a host ISP STOP or STOP command*/
	    case ISP_SENSOR_START_STREAMING	:result=  "ISP_SENSOR_START_STREAMING"; break; 		//= 0xD,	/**< Means SENSOR has started the streaming following a host RUN command*/
	    case ISP_SENSOR_STOP_STREAMING:result=  "ISP_SENSOR_STOP_STREAMING"; break; 			//= 0xE,	/**< Means SENSOR has stopped streaming, following a host STOP command*/
	    case ISP_HOST_TO_SENSOR_ACCESS_COMPLETE:result=  "ISP_HOST_TO_SENSOR_ACCESS_COMPLETE"; break; 	//= 0xF,	/**< Raised in context of a host to master i2c access operation initiated by the host.*/
	    case ISP_LOAD_READY:result=  "ISP_LOAD_READY"; break; 						//= 0x10,
	    case ISP_ZOOM_CONFIG_REQUEST_DENIED:result=  "ISP_ZOOM_CONFIG_REQUEST_DENIED"; break; 		//= 0x11,
	    case ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED:result=  "ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED"; break;  	//= 0x12,
	    case ISP_ZOOM_STEP_COMPLETE:result=  "ISP_ZOOM_STEP_COMPLETE"; break; 				//= 0x13,
	    case ISP_ZOOM_SET_OUT_OF_RANGE:result=  "ISP_ZOOM_SET_OUT_OF_RANGE"; break; 			//= 0x14,
		case ISP_RESET_COMPLETE:result=  "ISP_RESET_COMPLETE"; break; 					//= 0x15,
		case ISP_GLACE_STATS_READY:                         result = "ISP_GLACE_STATS_READY";                         break; //= 0x16
		case ISP_HISTOGRAM_STATS_READY:                     result = "ISP_HISTOGRAM_STATS_READY";                     break; //= 0x17
		case ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED:        result=  "ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED";        break; //= 0x18
		case ISP_AUTOFOCUS_STATS_READY:                     result = "ISP_AUTOFOCUS_STATS_READY";                     break; //= 0x19
		case ISP_FLADRIVER_LENS_STOP:                       result = "ISP_FLADRIVER_LENS_STOP";                       break; //= 0x20
		case ISP_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY:        result = "ISP_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY";        break; //= 0x21
		case ISP_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE:       result = "ISP_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE";       break; //= 0x22
		case ISP_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE:       result = "ISP_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE";       break; //= 0x23
		case ISP_SDL_UPDATE_READY:                          result = "ISP_SDL_UPDATE_READY";                          break; //= 0x24
		case ISP_NVM_EXPORT_DONE:                           result = "ISP_NVM_EXPORT_DONE";                           break; //= 0x25
		case ISP_CDCC_AVAILABLE:                            result = "ISP_CDCC_AVAILABLE";                            break; //= 0x26
		case ISP_POWER_NOTIFICATION:                        result = "ISP_POWER_NOTIFICATION";                        break; //= 0x27
		case ISP_SMS_NOTIFICATION:                          result = "ISP_SMS_NOTIFICATION";                          break; //= 0x28
		case ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION:    result = "ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION"; 	  break; //= 0x29
		case ISP_VALID_BMS_FRAME_NOTIFICATION:    			result = "ISP_VALID_BMS_FRAME_NOTIFICATION"; 			  break; //= 0x30
		case ISP_SENSOR_COMMIT_NOTIFICATION:    			result = "ISP_SENSOR_COMMIT_NOTIFICATION"; 				  break; //= 0x31
		case ISP_ISP_COMMIT_NOTIFICATION:					result = "ISP_ISP_COMMIT_NOTIFICATION"; 			  	  break; //= 0x32
		default: result = ("*** ispctl info ID not found ***");break;
	}
 return(result);
}


const char * CError::stringIspctlError(e_ispctlError index)
{
	   const char *result;
	   switch(index) {
	     case ISP_NO_ERROR:result=  "ISP_NO_ERROR"; break;
	     case ISP_POLLING_TIMEOUT_ERROR	:result=  "ISP_POLLING_TIMEOUT_ERROR"; break;
	     case ISP_CHECK_VALUE_ERROR:result=  "ISP_CHECK_VALUE_ERROR"; break;
	     case ISP_FORBIDDEN_STATE_TRANSITION_ERROR:result=  "ISP_FORBIDDEN_STATE_TRANSITION_ERROR"; break;
	     case ISP_READ_ONLY_ERROR:result=  "ISP_READ_ONLY_ERROR"; break;
	     case ISP_READ_ONLY_IN_LIST_ERROR:result=  "ISP_READ_ONLY_IN_LIST_ERROR"; break;
	     case ISP_WRITELIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR:result=  "ISP_WRITELIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR"; break;
	     case ISP_READLIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR:result=  "ISP_READLIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR"; break;
		 case ISP_SENSOR_TYPE_NOT_DEFINED_ERROR:result=  "ISP_SENSOR_TYPE_NOT_DEFINED_ERROR"; break;
	     case ISP_STREAMING_ERROR:result=  "ISP_STREAMING_ERROR"; break;
	     case ISP_DOES_NOT_RESPOND_ERROR:result=  "ISP_DOES_NOT_RESPOND_ERROR"; break;
		 case ISP_MASTER_I2C_ACCESS_FAILURE:result=  "ISP_MASTER_I2C_ACCESS_FAILURE"; break;
		 case ISP_ALREADY_IN_PROCESSING_ERROR:result=  "ISP_ALREADY_IN_PROCESSING_ERROR"; break;
		 default: result = ("*** ispctl error ID not found ***");break;
	}
 return(result);
}


const char * CError::stringSMSig(e_scf_signal index)
{
		const char *result;
		switch(index) {

		case SCF_PARENT_SIG  :result=  "SCF_PARENT_SIG"; break;
		case SCF_STATE_INIT_SIG :result=  "SCF_STATE_INIT_SIG"; break;
		case SCF_STATE_ENTRY_SIG :result=  "SCF_STATE_ENTRY_SIG"; break;
		case SCF_STATE_EXIT_SIG :result=  "SCF_STATE_EXIT_SIG"; break;
		case EVT_ISPCTL_INFO_SIG :result=  "EVT_ISPCTL_INFO_SIG"; break;
		case EVT_ISPCTL_LIST_INFO_SIG :result=  "EVT_ISPCTL_LIST_INFO_SIG"; break;
		case EVT_ISPCTL_ERROR_SIG :result=  "EVT_ISPCTL_ERROR_SIG"; break;
		case EVT_ISPCTL_DEBUG_SIG :result=  "EVT_ISPCTL_DEBUG_SIG"; break;
		case EVT_GRABCTL_CONFIGURED_SIG :result=  "EVT_GRABCTL_CONFIGURED_SIG"; break;
		case EVT_GRAB_INFO_SIG :result=  "EVT_GRAB_INFO_SIG "; break;
		case EVT_GRAB_ERROR_SIG :result=  "EVT_GRAB_ERROR_SIG"; break;
		case EVT_GRAB_DEBUG_SIG :result=  "EVT_GRAB_DEBUG_SIG"; break;
		case TUNING_LOADER_OPERATION_DONE_SIG: result = "TUNING_LOADER_OPERATION_DONE_SIG"; break;
		case TUNING_LOADER_OPERATION_FAILED_SIG: result = "TUNING_LOADER_OPERATION_FAILED_SIG"; break;
		case CAMERA_INDEX_CONFIG_ZOOM_SIG :result=  "CAMERA_INDEX_CONFIG_ZOOM_SIG"; break;
		case CAMERA_INDEX_CONFIG_TEST_MODE_SIG :result=  "CAMERA_INDEX_CONFIG_TEST_MODE_SIG"; break;
		case CAMERA_INDEX_CONFIG_SELFTEST_SIG :result=  "CAMERA_INDEX_CONFIG_SELFTEST_SIG"; break;
		case CAMERA_INDEX_CONFIG_MIRROR_SIG : result= "CAMERA_INDEX_CONFIG_MIRROR_SIG";break;
		case CONFIGURE_SIG :           result=  "CONFIGURE_SIG"; break;
		case CONFIGURE_ABORT_SIG :     result=  "CONFIGURE_ABORT_SIG"; break;
		case SEND_COMMAND_SIG :        result=  "SEND_COMMAND_SIG"; break;
		case EXECUTING_TO_IDLE_START_SIG :result=  "EXECUTING_TO_IDLE_START_SIG"; break;
		case CAMERA_EXECUTING_TO_IDLE_END_SIG :result= "CAMERA_EXECUTING_TO_IDLE_END_SIG"; break;
		case ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG :result= "ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG"; break;
		case CAMERA_EXIT_PREVIEW_SIG : result = "CAMERA_EXIT_PREVIEW_SIG"; break;
		case CAMERA_IDLE_TO_EXECUTING_SIG :result = "CAMERA_IDLE_TO_EXECUTING_SIG"; break;
        case CAMERA_EOS_VPB02_SIG: result="CAMERA_EOS_VPB02_SIG"; break;
		case GRAB_ABORTED_SIG: result="GRAB_ABORTED_SIG"; break;	
		case SW3A_STOP_ACK_SIG: result="SW3A_STOP_ACK_SIG"; break;
		case SHARED_READLISTPE_DEBUG_SIG :result = "SHARED_READLISTPE_DEBUG_SIG"; break;
		case SHARED_WRITELISTPE_SIG :  result = "SHARED_WRITELISTPE_SIG"; break;
		case Q_DEFERRED_EVENT_SIG :    result = "Q_DEFERRED_EVENT_SIG"; break;
		case SHARED_RETURN_SIG :       result = "SHARED_RETURN_SIG"; break;
		case UPDATE_PORT_FOR_ROTATION_SIG :  result = "UPDATE_PORT_FOR_ROTATION_SIG"; break;
		case Q_PONG_SIG :              result = "Q_PONG_SIG"; break;
		case Q_LOCK_OK_SIG:            result = "Q_LOCK_OK_SIG"; break;
		case Q_LOCK_KO_SIG:            result = "Q_LOCK_KO_SIG"; break;
		case Q_RELEASED_SIG:result=  "Q_RELEASED_SIG"; break;
		case Q_FINAL_SIG :result=  "Q_FINAL_SIG"; break;
		case SW3A_LOOP_CONTROL_SIG :result=  "SW3A_LOOP_CONTROL_SIG"; break;
		case SW3A_STARTED_SIG: result=  "SW3A_STARTED_SIG"; break;
		case SW3A_STOPPED_SIG: result=  "SW3A_STOPPED_SIG"; break;
		case ISPPROC_EXIT_CAPTURE_SIG :result=  "ISPPROC_EXIT_CAPTURE_SIG"; break;
        case CAMERA_FILLBUFFERDONE_SIG: result="CAMERA_FILLBUFFERDONE_SIG"; break;
		case ISPPROC_STRIPE_SIG: result="ISPPROC_STRIPE_SIG"; break;
		case NONE_SIG :result=  "NONE_SIG"; break;
		case TIMEOUT_SIG :result=  "TIMEOUT_SIG"; break;
		case CAMERA_EXECUTING_TO_PAUSE_SIG: result = "CAMERA_EXECUTING_TO_PAUSE_SIG"; break;
		case CAMERA_PAUSE_TO_EXECUTING_SIG: result = "CAMERA_PAUSE_TO_EXECUTING_SIG"; break;
		case LOWPOWER_SIG: result = "LOWPOWER_SIG"; break;
		case CAMERA_BUFFERED_SIG: result = "CAMERA_BUFFERED_SIG";break;
		default: result = "*** signal ID not found ***";break;
		}
	return(result);
}


const char * CError::stringRSRequesterId(t_requesterID index)
{
	   const char *result;
	   switch(index) {
	     case CAMERA_RAW:result=  "CAMERA_RAW"; break;
	     case ISPPROC_SINGLE_STILL	:result=  "ISPPROC_SINGLE_STILL"; break;
	     case ISPPROC_SINGLE_SNAP:result=  "ISPPROC_SINGLE_SNAP"; break;
	     case ISPPROC_BURST:result=  "ISPPROC_BURST"; break;
	     case CAMERA_VF:result=  "CAMERA_VF"; break;
		 default: result = ("*** e_requesterID ID not found ***");break;
	}
 return(result);
}

const char * CError::stringISPStateid(t_isp_state index)
{
	const char *result;
	switch(index) 
	{
		case ISP_STATE_NONE:result=  "ISP_STATE_NONE"; break;
		case ISP_STATE_BOOTED	:result=  "ISP_STATE_BOOTED"; break;
		case ISP_STATE_STOP_STREAMING_STOP3A:result=  "ISP_STATE_STOP_STREAMING_STOP3A"; break;
		case ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE:result=  "ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE"; break;
		case ISP_STATE_STOP_STREAMING_SENDIDLE:result=  "ISP_STATE_STOP_STREAMING_SENDIDLE"; break;
		case ISP_STATE_STREAMING:result=  "ISP_STATE_STREAMING"; break;
		case ISP_STATE_SLEPT:result=  "ISP_STATE_SLEPT"; break;
		case ISP_STATE_PAUSED:result=  "ISP_STATE_PAUSED"; break;
		case ISP_STATE_WAKEUP:result=  "ISP_STATE_WAKEUP"; break;
		default: result = ("*** ISPStateid not found ***");break;
	}
	return(result);
}

const char * CError::stringSW3AStatusid(t_sw3a_stop_req_status index)
{
	const char *result;
	switch(index) 
	{
		case SW3A_STOP_NOT_REQUESTED:result=  "SW3A_STOP_NOT_REQUESTED"; break;
		case SW3A_STOP_REQUESTED	:result=  "SW3A_STOP_REQUESTED"; break;
		case SW3A_STOP_ACKNOWLEDGED:result=  "SW3A_STOP_ACKNOWLEDGED"; break;
		case SW3A_STOP_REQ_SERVICED:result=  "SW3A_STOP_REQ_SERVICED"; break;
		default: result = ("*** SW3AStatusid not found ***");break;
	}
	return(result);
}

const char * CError::stringGrabStatusid(t_grab_abort_status index)
{
	const char *result;
	switch(index) 
	{
		case GRAB_ABORT_NOT_REQUESTED:result=  "GRAB_ABORT_NOT_REQUESTED"; break;
		case GRAB_ABORT_REQUESTED	:result=  "GRAB_ABORT_REQUESTED"; break;
		case GRAB_ABORT_SERVICED:result=  "GRAB_ABORT_SERVICED"; break;
		default: result = ("*** GrabStatusid not found ***");break;
	}
	return(result);
}


const char * CError::stringIspctlCtrlType(t_controling_isp_ctrl_type index)
{
    const char *result;
    switch(index)
    {
        case ControlingISP_SendBoot_WaitBootComplete:result=  "ControlingISP_SendBoot_WaitBootComplete"; break;
        case ControlingISP_SendRun_WaitIspStreaming:result=  "ControlingISP_SendRun_WaitIspStreaming"; break;
        case ControlingISP_SendRun_WaitIspStreaming_Start3A:result=  "ControlingISP_SendRun_WaitIspStreaming_Start3A"; break;
        case ControlingISP_SendRun_WaitIspStreaming_SendExecuting:result=  "ControlingISP_SendRun_WaitIspStreaming_SendExecuting"; break;
        case ControlingISP_SendRun_WaitIspStreaming_SendPause:result=  "ControlingISP_SendRun_WaitIspStreaming_SendPause"; break;
        case ControlingISP_SendRun_WaitIspLoadReady:result=  "ControlingISP_SendRun_WaitIspLoadReady"; break;
        case ControlingISP_SendStop_WaitIspSensorStopStreaming:result=  "ControlingISP_SendStop_WaitIspSensorStopStreaming"; break;
        case ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A:result=  "ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A"; break;
        case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle:result=  "ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle"; break;
        case ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A:result=  "ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A"; break;
        case ControlingISP_SendStop_WaitIspStopStreaming:result=  "ControlingISP_SendStop_WaitIspStopStreaming"; break;
        case ControlingISP_SendSleep_WaitIspSleeping:result=  "ControlingISP_SendSleep_WaitIspSleeping"; break;
        case ControlingISP_SendWakeUp_WaitIspWorkenUp:result=  "ControlingISP_SendWakeUp_WaitIspWorkenUp"; break;
        case ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle:result=  "ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle"; break;
        case ControlingISP_GetState_GoRunning_SendExecuting:result=  "ControlingISP_GetState_GoRunning_SendExecuting"; break;
        case ControlingISP_GetState_GoRunning_SendExecuting_Start3A:result=  "ControlingISP_GetState_GoRunning_SendExecuting_Start3A"; break;
        case ControlingISP_GetState_GoRunning_SendPause:result=  "ControlingISP_GetState_GoRunning_SendPause"; break;
        case ControlingISP_GetState_GoRunning_SendPause_Start3A:result=  "ControlingISP_GetState_GoRunning_SendPause_Start3A"; break;
        case ControlingISP_GetState_GoRunning:result=  "ControlingISP_GetState_GoRunning"; break;
        case ControlingISP_GetState_GoRunning_Start3A:result=  "ControlingISP_GetState_GoRunning_Start3A"; break;
        case ControlingISP_GetState_GoSlept:result=  "ControlingISP_GetState_GoSlept"; break;
        case ControlingISP_GetState_GoSlept_Stop3A:result=  "ControlingISP_GetState_GoSlept_Stop3A"; break;
        default: result = ("*** ControlingISPid not found ***");break;
	}
	return(result);
}

const char * CError::stringOMXstate(OMX_STATETYPE index)
{
   const char *result;

    switch(index)
    {
        case OMX_StateLoaded:result=  "OMX_StateLoaded"; break;
        case OMX_StateIdle:result=  "OMX_StateIdle"; break;
        case OMX_StateExecuting:result=  "OMX_StateExecuting"; break;
        case OMX_StatePause:result=  "OMX_StatePasue"; break;
        default: result = ("*** stringOMXstate not found ***");break;
    }

   return(result);
}
