/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** @file scf_signals.h
    @brief This file defines all functional signals
    used by the state machines of the camera system
*/
// rename_file scf_signals.h

#ifndef _SCF_SIGNALS_H_
#define _SCF_SIGNALS_H_


typedef enum e_scf_signal {
	// Framework signals, should NOT be changed
    SCF_FIRST_SIG = -1, /**< dummy framework signal for separting framework for framework usage signals*/
	SCF_PARENT_SIG = 0,  /**< framework reserved signal used to request parent of a user sate */
	SCF_STATE_INIT_SIG = 1,  /**< framework signal used to trigger init phase of a state */
	SCF_STATE_ENTRY_SIG = 2, /**< framework signal used to trigger entry  phase of a state */
	SCF_STATE_EXIT_SIG = 3,  /**< framework signal used to trigger exit  phase of a state */

    SCF_LAST_SIG = 0xff, /**< dummy framework signal for separting framework signals from framework users sepcific signals*/

    // end of framworks signals, user signals here

    SCF_USER_SIG,         /**< From here start camera signals. */      /*256 */

	// Signals coming from DSP
	EVT_ISPCTL_INFO_SIG ,                                          /*257 */
	EVT_ISPCTL_LIST_INFO_SIG,                                      /*258 */
	EVT_ISPCTL_ERROR_SIG,                                          /*259 */
	EVT_ISPCTL_DEBUG_SIG,                                          /*260 */
	EVT_GRABCTL_CONFIGURED_SIG, // ie Loaded->Idle OK                                /*261 */
	EVT_GRAB_INFO_SIG,                                             /*262 */
	EVT_GRAB_ERROR_SIG,                                            /*263 */
	EVT_GRAB_DEBUG_SIG,                                            /*264 */

	// Tuning Loader related
	TUNING_LOADER_OPERATION_DONE_SIG,                              /*265 */
	TUNING_LOADER_OPERATION_FAILED_SIG,                            /*266 */

	// Flash related
	ASYNC_FLASH_OPERATION_DONE_SIG,                                /*267 */
	ASYNC_FLASH_OPERATION_FAILED_SIG,                              /*268 */

    // Configuration related
    CAMERA_INDEX_CONFIG_ZOOM_SIG,                                  /*269 */
    CAMERA_INDEX_CONFIG_TEST_MODE_SIG,                             /*270 */
    CAMERA_INDEX_CONFIG_SELFTEST_SIG,                              /*271 */
    CAMERA_INDEX_CONFIG_MIRROR_SIG,                                /*272 */
    CAMERA_INDEX_CONFIG_EXTCAPTUREMODE_SIG,                        /*273 */

    /* transition related */
    CONFIGURE_SIG,                                                 /*274 */
    GRAB_ABORTED_SIG,/*For Shutterlag*/	                           /*275 */
    CONFIGURE_ABORT_SIG,                                           /*276 */

    SEND_COMMAND_SIG,                                              /*277 */
    EXECUTING_TO_IDLE_START_SIG,                                   /*278 */
    CAMERA_EXECUTING_TO_IDLE_END_SIG,                              /*279 */
    ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG,                          /*280 */
    CAMERA_UNSUSPEND_RESOURCES_SIG,       						   /*281 */
    CAMERA_EXIT_PREVIEW_SIG,                                       /*282 */
    CAMERA_IDLE_TO_EXECUTING_SIG,                                  /*283 */
    CAMERA_EXECUTING_TO_PAUSE_SIG,                                 /*284 */
    CAMERA_PAUSE_TO_EXECUTING_SIG, 								   /*285 */
    CAMERA_EOS_VPB02_SIG,           							   /*286 */
    SHARED_READLISTPE_DEBUG_SIG, 								   /*287*/
    SHARED_WRITELISTPE_SIG,       								   /*288 */
    Q_DEFERRED_EVENT_SIG,       								   /*289 */
    SHARED_RETURN_SIG,                   						   /*290 */
    UPDATE_PORT_FOR_ROTATION_SIG,      							   /*291 */
    //Q_FREED_SIG,
    Q_PONG_SIG,	                  								   /*292 */
    Q_FINAL_SIG,                   								   /*293 */
    Q_LOCK_OK_SIG,            								       /*294 */
    Q_LOCK_KO_SIG,            								       /*295 */
    Q_RELEASED_SIG,           								       /*296 */
    SW3A_STARTED_SIG,         								       /*297 */
    SW3A_STOPPED_SIG,               							   /*298 */
    SW3A_LOOP_CONTROL_SIG,  								       /*299 */
    SW3A_STOP_ACK_SIG, /*For Shutterlag*/	 						   /*300 */
    SW3A_PRECAPTURE_FLASH_END,  								   /*301 */
    ISPPROC_EXIT_CAPTURE_SIG,  								       /*302 */
    CAMERA_FILLBUFFERDONE_SIG,  								   /*303 */
    CAMERA_BUFFERED_SIG, /*ZSL*/  								   /*304 */
    ISPPROC_STRIPE_SIG,  								           /*305 */
    NONE_SIG,  								                       /*306 */

    TIMEOUT_SIG,  								                   /*307 */
    BUFFER_AVAILABLE_SIG,  								           /*308 */
    LOWPOWER_SIG,  								                   /*309 */
    START_HIDDEN_BMS_SIG,                                          /* 310 */ 
    HIDDEN_BMS_COMPLETE_SIG,                                       /* 311 */
    HIDDEN_BMS_LAUNCHED_SIG,                                       /* 312 */
    START_HDR_SIG,                                                 /* 313 */
    RESUME_ZSL_SIG                                                 /* 314 */
} e_scf_signal;


/*For Shutterlag*/	
typedef enum
{	
	ISP_STATE_NONE,
	ISP_STATE_BOOTED,	
	ISP_STATE_STOP_STREAMING_STOP3A,
	ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE,
	ISP_STATE_STOP_STREAMING_SENDIDLE,
	ISP_STATE_STREAMING,	
	ISP_STATE_PAUSED,	
	ISP_STATE_SLEPT,	
	ISP_STATE_WAKEUP,
	ISP_STATE_STOP_STREAMING
}t_isp_state;


/*For Shutterlag */
typedef enum
{	SW3A_STOP_NOT_REQUESTED,
	SW3A_STOP_REQUESTED,
	SW3A_STOP_ACKNOWLEDGED,
	SW3A_STOP_REQ_SERVICED,	
}t_sw3a_stop_req_status;

typedef enum
{	GRAB_ABORT_NOT_REQUESTED,
	GRAB_ABORT_REQUESTED,
	GRAB_ABORT_SERVICED,	
}t_grab_abort_status;

typedef enum {

		ControlingISP_SendBoot_WaitBootComplete=0,

		ControlingISP_SendRun_WaitIspStreaming,
		ControlingISP_SendRun_WaitIspStreaming_Start3A,
		ControlingISP_SendRun_WaitIspStreaming_SendExecuting,
		ControlingISP_SendRun_WaitIspStreaming_SendPause,
		ControlingISP_SendRun_WaitIspLoadReady,

		ControlingISP_SendStop_WaitIspSensorStopStreaming,
		ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A,
		ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle,
		ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A,


		ControlingISP_SendStop_WaitIspStopStreaming,

		ControlingISP_SendSleep_WaitIspSleeping,

		ControlingISP_SendWakeUp_WaitIspWorkenUp,
		ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle,
		ControlingISP_GetState_GoRunning_SendExecuting,
		ControlingISP_GetState_GoRunning_SendExecuting_Start3A,
		ControlingISP_GetState_GoRunning_SendPause,
		ControlingISP_GetState_GoRunning_SendPause_Start3A,
		ControlingISP_GetState_GoRunning,
		ControlingISP_GetState_GoRunning_Start3A,

		ControlingISP_GetState_GoSlept,
		ControlingISP_GetState_GoSlept_Stop3A,

}t_controling_isp_ctrl_type;

/*Configuring ISP pipe related */
typedef enum {
	ConfiguringISPPipe_All=0,
	ConfiguringISPPipe_ResolutionOnly,
	ConfiguringISPPipe_StandardBML,
	ConfiguringISPPipe_TimeNudgeBML
}t_configuring_isp_pipe;





#endif // _SCF_SIGNALS_H_
