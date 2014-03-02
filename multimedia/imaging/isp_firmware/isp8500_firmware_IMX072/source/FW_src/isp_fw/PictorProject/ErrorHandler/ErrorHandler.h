/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef ERROR_HANDLER_H_
#define ERROR_HANDLER_H_

#   include "Platform.h"

/**
\enum Error_EOF_Control_te
\brief Enum for Error EOF Control Disabled (0), Rest (>1)
\ingroup
*/
typedef enum
{
    Error_DISABLED,
    Error_ENABLED,
    Error_ENABLED_ABORT,
    Error_ENABLED_RECOVER
} Error_Control_te;


//----------------------------------------Error Handlers -----------------------------------------------
//Error Handler - Status and Control for Error / EOF Events
/**
 \struct    ErrorHandler_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// Counter for total CSI EOF
    /// [DEFAULT]: 0 
    /// Value : Total Number of CSI EOF during streaming = Total frames streamed
    uint16_t  u16_CSI_EOF_Counter;  //Count Total Number of CSI EOF

    /// Error_Control
    /// [DEFAULT]: 0 
    /// 0 : Error and EOF Handling disabled.
    /// 1 : Error Handling enabled
    /// 2 : Error Handling enabled with Abort current frame and enter error state
    /// 3 : Error Handling enabled with Recovery
    uint8_t  e_Error_Control;  //Control Byte to Handle Different Error Settings

    /// Internal flag for abort in case of fatal front end streaming error. 
    /// [DEFAULT] : Flag_e_FALSE
    /// Flag_e_TRUE to signal ISP FW state machine to abort smia rx, stop sensor and enter error state
    uint8_t  e_Flag_Error_Abort;     

    /// Internal flag for forcing smia-rx abort, and ISP reset. 
    /// [DEFAULT] : Flag_e_FALSE
    /// Flag_e_TRUE to signal Host Interface to first attempt recovery, else abort currnet frame till smia-rx is restarted
    uint8_t  e_Flag_Error_Recover;     

    /// CSI Error's Counter
    /// [DEFAULT] : 0
    /// Values are the number of CSI errors
    uint8_t  u8_CSI_Error_Counter;     //Total Number of CSI errors.

    /// CCP Error 0 Counter
    /// [DEFAULT] : 0
    /// Values is the total number of CCP errors
    uint8_t  u8_CCP_Error_Counter;     //Total Number of CSI errors.

} ErrorHandler_ts;


extern ErrorHandler_ts   g_ErrorHandler;
extern void     ErrorEOF_ISR (void)TO_EXT_DDR_PRGM_MEM;

#endif // #ifndef ERROR_HANDLER_H_
