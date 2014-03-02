/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_error.h"

CError::CError()
{
}

CError::~CError()
{
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
    case ISP_SLEEPING:result=  "ISP_SLEEPING"; break;						//= 0x9,	/**< Means ISP has transitioned to the sleep state following a host SLEEP command*/
    case ISP_WOKEN_UP:result=  "ISP_WOKEN_UP"; break;						//= 0xA,	/**< Means ISP has transitioned out of the sleep state to stop state following a host WAKE_UP command*/
    case ISP_STREAMING:result=  "ISP_STREAMING"; break;						//= 0xB,	/**< Means ISP has started the streaming operation following a host RUN command*/
    case ISP_STOP_STREAMING:result=  "ISP_STOP_STREAMING"; break;					//= 0xC,	/**< Means ISP has stopped streaming, following a host ISP STOP or STOP command*/
    case ISP_SENSOR_START_STREAMING	:result=  "ISP_SENSOR_START_STREAMING"; break;		//= 0xD,	/**< Means SENSOR has started the streaming following a host RUN command*/
    case ISP_SENSOR_STOP_STREAMING:result=  "ISP_SENSOR_STOP_STREAMING"; break;			//= 0xE,	/**< Means SENSOR has stopped streaming, following a host STOP command*/
    case ISP_HOST_TO_SENSOR_ACCESS_COMPLETE:result=  "ISP_HOST_TO_SENSOR_ACCESS_COMPLETE"; break;	//= 0xF,	/**< Raised in context of a host to master i2c access operation initiated by the host.*/
    case ISP_LOAD_READY:result=  "ISP_LOAD_READY"; break;						//= 0x10,
    case ISP_ZOOM_CONFIG_REQUEST_DENIED:result=  "ISP_ZOOM_CONFIG_REQUEST_DENIED"; break;		//= 0x11,
    case ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED:result=  "ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED"; break;	//= 0x12,
    case ISP_ZOOM_STEP_COMPLETE:result=  "ISP_ZOOM_STEP_COMPLETE"; break;				//= 0x13,
    case ISP_ZOOM_SET_OUT_OF_RANGE:result=  "ISP_ZOOM_SET_OUT_OF_RANGE"; break;			//= 0x14,
    case ISP_RESET_COMPLETE:result=  "ISP_RESET_COMPLETE"; break;					//= 0x15,
    case ISP_GLACE_STATS_READY:result=  "ISP_GLACE_STATS_READY"; break;				//= 0x16,
    case ISP_HISTOGRAM_STATS_READY:result=  "ISP_HISTOGRAM_STATS_READY"; break;			//= 0x17,
    case ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED:result=  "ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED"; break;  //= 0x18,
    case ISP_AUTOFOCUS_STATS_READY:               result = "ISP_AUTOFOCUS_STATS_READY";               break; //= 0x19
    case ISP_FLADRIVER_LENS_STOP:                 result = "ISP_FLADRIVER_LENS_STOP";                 break; //= 0x20
    case ISP_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY:  result = "ISP_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY";  break; //= 0x21
    case ISP_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE: result = "ISP_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE"; break; //= 0x22
    case ISP_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE: result = "ISP_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE"; break; //= 0x23
    case ISP_SDL_UPDATE_READY:                    result = "ISP_SDL_UPDATE_READY";                    break; //= 0x23
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
    case FOCUS_STABLE_SIG :result=  "FOCUS_STABLE_SIG"; break;
    case FOCUS_UNSTABLE_SIG :result=  "FOCUS_UNSTABLE_SIG"; break;
    case TUNING_LOADER_OPERATION_DONE_SIG: result = "TUNING_LOADER_OPERATION_DONE_SIG";
    case TUNING_LOADER_OPERATION_FAILED_SIG: result = "TUNING_LOADER_OPERATION_FAILED_SIG";
    case CAMERA_INDEX_CONFIG_ZOOM_SIG :result=  "CAMERA_INDEX_CONFIG_ZOOM_SIG"; break;
    case CAMERA_INDEX_CONFIG_FOCUS_SIG :result=  "CAMERA_INDEX_CONFIG_FOCUS_SIG"; break;
    case CAMERA_INDEX_CONFIG_WHITE_BALANCE_SIG :result=  "CAMERA_INDEX_CONFIG_WHITE_BALANCE_SIG"; break;
    case CAMERA_INDEX_CONFIG_BRIGHTNESS_SIG :result=  "CAMERA_INDEX_CONFIG_BRIGHTNESS_SIG"; break;
    case CAMERA_INDEX_CONFIG_LIGHTNESS_SIG :result=  "CAMERA_INDEX_CONFIG_LIGHTNESS_SIG"; break;
    case CAMERA_INDEX_CONFIG_EXPOSUREVALUE_SIG :result=  "CAMERA_INDEX_CONFIG_EXPOSUREVALUE_SIG"; break;
    case CAMERA_INDEX_CONFIG_GET_EXPOSUREVALUE_SIG :result=  "CAMERA_INDEX_CONFIG_GET_EXPOSUREVALUE_SIG"; break;
    case CAMERA_INDEX_CONFIG_EXPOSURECONTROL_SIG :result=  "CAMERA_INDEX_CONFIG_EXPOSURECONTROL_SIG"; break;
    case CAMERA_INDEX_CONFIG_CONTRAST_SIG :result=  "CAMERA_INDEX_CONFIG_CONTRAST_SIG"; break;
    case CAMERA_INDEX_CONFIG_SATURATION_SIG :result=  "CAMERA_INDEX_CONFIG_SATURATION_SIG"; break;
    case CAMERA_INDEX_CONFIG_GAMMA_SIG :result=  "CAMERA_INDEX_CONFIG_GAMMA_SIG"; break;
    case CAMERA_INDEX_CONFIG_IMGFILTER_SIG :result=  "CAMERA_INDEX_CONFIG_IMGFILTER_SIG"; break;
    case CAMERA_INDEX_CONFIG_STATUS_FOCUS_SIG :result=  "CAMERA_INDEX_CONFIG_STATUS_FOCUS_SIG"; break;
    case CAMERA_INDEX_CONFIG_TEST_MODE_SIG :result=  "CAMERA_INDEX_CONFIG_TEST_MODE_SIG"; break;
    case CAMERA_INDEX_CONFIG_SELFTEST_SIG :result=  "CAMERA_INDEX_CONFIG_SELFTEST_SIG"; break;
    case CAMERA_INDEX_CONFIG_FLICKER_REMOVAL_SIG :result=  "CAMERA_INDEX_CONFIG_FLICKER_REMOVAL_SIG"; break;
    case CONFIGURE_SIG :result=  "CONFIGURE_SIG"; break;
    case CONFIGURE_ABORT_SIG :result=  "CONFIGURE_ABORT_SIG"; break;
    case SEND_COMMAND_SIG :result=  "SEND_COMMAND_SIG"; break;
    case EXECUTING_TO_IDLE_START_SIG :result=  "EXECUTING_TO_IDLE_START_SIG"; break;
    case CAMERA_EXECUTING_TO_IDLE_END_SIG :result=  "CAMERA_EXECUTING_TO_IDLE_END_SIG"; break;
    case ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG :result=  "ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG"; break;
    case CAMERA_EXIT_PREVIEW_SIG : result = "CAMERA_EXIT_PREVIEW_SIG"; break;
    case CAMERA_ENTER_PREVIEW_SIG :result=  "CAMERA_ENTER_PREVIEW_SIG"; break;
    case CAMERA_IDLE_TO_EXECUTING_SIG :result = "CAMERA_IDLE_TO_EXECUTING_SIG"; break;
    case CAMERA_EOS_VPB02_SIG: result="CAMERA_EOS_VPB02_SIG"; break;
    case SHARED_READLISTPE_DEBUG_SIG :result = "SHARED_READLISTPE_DEBUG_SIG"; break;
    case SHARED_WRITELISTPE_SIG :  result = "SHARED_WRITELISTPE_SIG"; break;
    case Q_DEFERRED_EVENT_SIG :    result = "Q_DEFERRED_EVENT_SIG"; break;
    case SHARED_RETURN_SIG :       result = "SHARED_RETURN_SIG"; break;
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
    case NONE_SIG :result=  "NONE_SIG"; break;
    case TIMEOUT_SIG :result=  "TIMEOUT_SIG"; break;
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
