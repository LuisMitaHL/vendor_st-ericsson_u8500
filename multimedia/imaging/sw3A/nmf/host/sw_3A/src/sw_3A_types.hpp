/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __SW_3A_TYPES_HPP__
#define __SW_3A_TYPES_HPP__

#define SW3A_FILE_TRACE_ON_BY_DEFAULT   0
#define SW3A_FILE_TRACE_USE_STDIO
//#define SW3A_FILE_TRACE_USE_LOS

/** Enforcement of basic C types */
typedef unsigned char   sw3A_uint8_t;
typedef unsigned short  sw3A_uint16_t;
typedef unsigned long   sw3A_uint32_t;
typedef signed short    sw3A_int16_t;
typedef float           sw3A_float_t;
typedef bool            sw3A_bool_t;


/** Internal error codes of the 3A component */
typedef enum {
    SW3A_ERR_NONE = 0,
    SW3A_ERR_BAD_PARAM,
    SW3A_ERR_BAD_PRECONDITION,
    SW3A_ERR_NO_RESOURCE
} sw3A_error_t;

/** Internal state of the 3A component */
typedef enum {
    SW3A_stage_ViewFinder,
    SW3A_stage_Still_PreFlashMetering,
    SW3A_stage_Still_PreparationForStill,
    SW3A_stage_Still_Capture,
    SW3A_stage_Record,

    SW3A_state_Open,
    SW3A_state_Configured,
    SW3A_state_Running,
    SW3A_state_MAX
} sw3A_state_t;

#endif /* __SW_3A_TYPES_HPP__ */
