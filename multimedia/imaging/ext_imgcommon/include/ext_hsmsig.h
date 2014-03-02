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

    SCF_USER_SIG,         /**< From here start camera signals. */

    // Signals coming from DSP
    EVT_ISPCTL_INFO_SIG ,
    EVT_ISPCTL_LIST_INFO_SIG,
    EVT_ISPCTL_ERROR_SIG,
    EVT_ISPCTL_DEBUG_SIG,
    EVT_GRABCTL_CONFIGURED_SIG, // ie Loaded->Idle OK
    EVT_GRAB_INFO_SIG,
    EVT_GRAB_ERROR_SIG,
    EVT_GRAB_DEBUG_SIG,
    FOCUS_STABLE_SIG,
    FOCUS_UNSTABLE_SIG,

    // Tuning Loader related
    TUNING_LOADER_OPERATION_DONE_SIG,
    TUNING_LOADER_OPERATION_FAILED_SIG,

    // Configuration related

    CAMERA_INDEX_CONFIG_ZOOM_SIG,
    CAMERA_INDEX_CONFIG_FOCUS_SIG,
    CAMERA_INDEX_CONFIG_WHITE_BALANCE_SIG,
    CAMERA_INDEX_CONFIG_BRIGHTNESS_SIG,
    CAMERA_INDEX_CONFIG_LIGHTNESS_SIG,
    CAMERA_INDEX_CONFIG_EXPOSUREVALUE_SIG,
    CAMERA_INDEX_CONFIG_GET_EXPOSUREVALUE_SIG,	// reading signal
    CAMERA_INDEX_CONFIG_EXPOSURECONTROL_SIG,
    CAMERA_INDEX_CONFIG_CONTRAST_SIG,
    CAMERA_INDEX_CONFIG_SATURATION_SIG,
    CAMERA_INDEX_CONFIG_GAMMA_SIG,
    CAMERA_INDEX_CONFIG_IMGFILTER_SIG,
    CAMERA_INDEX_CONFIG_STATUS_FOCUS_SIG,
    CAMERA_INDEX_CONFIG_TEST_MODE_SIG,
    CAMERA_INDEX_CONFIG_SELFTEST_SIG,
    CAMERA_INDEX_CONFIG_MIRROR_SIG,
    CAMERA_INDEX_CONFIG_FLICKER_REMOVAL_SIG,

    /* transition related */
    CONFIGURE_SIG,

    CONFIGURE_ABORT_SIG,

    SEND_COMMAND_SIG,
    EXECUTING_TO_IDLE_START_SIG,
    CAMERA_EXECUTING_TO_IDLE_END_SIG,
    ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG,
    CAMERA_EXIT_PREVIEW_SIG,
    CAMERA_ENTER_PREVIEW_SIG,
    CAMERA_IDLE_TO_EXECUTING_SIG,
    CAMERA_EOS_VPB02_SIG,
    SHARED_READLISTPE_DEBUG_SIG,
    SHARED_WRITELISTPE_SIG,
    Q_DEFERRED_EVENT_SIG,
    SHARED_RETURN_SIG,
    //Q_FREED_SIG,
    Q_PONG_SIG,
    Q_FINAL_SIG,
    Q_LOCK_OK_SIG,
    Q_LOCK_KO_SIG,
    Q_RELEASED_SIG,
    SW3A_STARTED_SIG,
    SW3A_STOPPED_SIG,
    SW3A_LOOP_CONTROL_SIG,
    ISPPROC_EXIT_CAPTURE_SIG,
    CAMERA_FILLBUFFERDONE_SIG,
    NONE_SIG,

    TIMEOUT_SIG
} e_scf_signal;

#endif // _SCF_SIGNALS_H_
