/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   time_align_common.h
 * \brief  Contains defines/enums used for both (ARM/DSP) implementation
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _time_align_common_h_
#define _time_align_common_h_

#include "speech_proc_config.h"

// all sizes are in sample unless specifically specified in the name

#define MAX_LOOP_COUNT 640 // stereo 20ms at 16khz (MUST be a number and not an expression)

// output buffer (only host version)
#define OUTPUT_BUFFER_SIZE      ((MAX_NB_REF_CHANNEL + MAX_NB_INPUT_CHANNEL) * (MAX_SAMPLE_FREQ * PROCESSING_BUFFER_DURATION))
#define OUTPUT_BUFFER_SIZE_BYTE (OUTPUT_BUFFER_SIZE * 2)

// timestamp to ms conversion
#define MS2TIMESTAMP                         1000 // timestamps are in us
#define INPUT_BUFFER_DURATION_TIMESTAMP     (UL_INPUT_BUFFER_DURATION  * MS2TIMESTAMP) // us
#define REFERENCE_BUFFER_DURATION_TIMESTAMP (REF_INPUT_BUFFER_DURATION * MS2TIMESTAMP) // us

// timestamp/us to samples conversion
#define us2samples_8khz     67109 //  8/1000 in Q23
#define us2samples_16khz   134218 // 16/1000 in Q23

#define samples2ms_8khz   1048576 // 1/8  in Q23
#define samples2ms_16khz   524288 // 1/16 in Q23
#define samples2ms_48khz   174763 // 1/48 in Q23

// number of input buffer on uplink port to create an output buffer
#define NB_OUTPUT_PART (PROCESSING_BUFFER_DURATION/UL_INPUT_BUFFER_DURATION)

// SRC mode
#define SRC_MODE 6  // low latency + constant buffer duration 

// TimeAlignment status
typedef enum {
  TIME_ALIGNMENT_NOT_STARTED,
  TIME_ALIGNMENT_ON_GOING,
  TIME_ALIGNMENT_NOT_AVAILABLE
} t_align_status;

// Reference port status
typedef enum {
  REFERENCE_INACTIVE,
  REFERENCE_ACTIVE_NO_TIMESTAMP,
  REFERENCE_ACTIVE_TIMESTAMP
}t_ref_status;

// port name of NMF component
typedef enum {
  INPUT_PORT     = 0,
  REFERENCE_PORT = 1,
  OUTPUT_PORT    = 2
} t_port_name;

#endif // _time_align_common_h_
