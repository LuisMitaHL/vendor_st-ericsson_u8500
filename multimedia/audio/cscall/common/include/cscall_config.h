/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file  cscall_config.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __cscall_config_h
#define __cscall_config_h


//////////////////////////////////////////
// Version number
/////////////////////////////////////////
#define CSCALL_MAJOR    5
#define CSCALL_MINOR    4
#define CSCALL_REVISION 0


//////////////////////////////////////////
// Component trace ID
/////////////////////////////////////////
#define CSCONTROLLER_ID         (10)
#define UPLINK_ID               (11)
#define DOWNLINK_ID             (12)
#define PH_ID                   (14)
#define LBMODEM_ID              (20)


//////////////////////////////////////////
//   Default Sampling Freq
//////////////////////////////////////////
#ifdef ISI_TD_PROTOCOL
#define CSCALL_DEFAULT_SAMPLING_RATE 16000
#else
#define CSCALL_DEFAULT_SAMPLING_RATE 8000
#endif


//////////////////////////////////////////
//   Framing Information
//////////////////////////////////////////
// Nmf input buffer size in ms (must be a divider of 20)
#define INPUT_BUFFER_DURATION 20

// OMX input buffer size in ms
#define OMX_INPUT_BUFFER_DURATION 20


// Nmf output buffer size in ms (must be a divider of 20)
#define OUTPUT_BUFFER_DURATION 20

// OMX output buffer size in ms
#define OMX_OUTPUT_BUFFER_DURATION 20


//////////////////////////////////////////
//   Internal buffers
//////////////////////////////////////////
// number of buffers between protocol_handler and audiomodedriver
#define NB_DL_MODEM_BUFFER 16
#define NB_UL_MODEM_BUFFER 16





#endif // __cscall_config_h

