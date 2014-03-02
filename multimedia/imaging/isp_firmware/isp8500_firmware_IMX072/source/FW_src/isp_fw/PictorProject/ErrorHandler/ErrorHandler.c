/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "Platform.h"
#include "HostInterface.h"
#include "IPPhwReg.h"

#include "ErrorHandler.h"
#include "ITM.h"

#define DEFAULT_ERROR_EOF_COUNTER               (0) // Initialize CSI EOF Counter to zero
#define DEFAULT_ERROR_CONTROL                   Error_DISABLED //Disabled by Default
#define DEFAULT_FLAG_ERROR_ABORT                Flag_e_FALSE
#define DEFAULT_FLAG_ERROR_ABORT_RECOVER        Flag_e_FALSE
#define DEFAULT_CSI_ERROR_COUNTER               (0) // Initialize CSI Error Counter to zero
#define DEFAULT_CCP_ERROR_COUNTER               (0) // Initialize CCP Error Counter to zero

//Initialization of g_TraceLogsControl page element
ErrorHandler_ts    g_ErrorHandler =
{
	DEFAULT_ERROR_EOF_COUNTER,
	DEFAULT_ERROR_CONTROL,
	DEFAULT_FLAG_ERROR_ABORT,
	DEFAULT_FLAG_ERROR_ABORT_RECOVER,
	DEFAULT_CSI_ERROR_COUNTER,
	DEFAULT_CCP_ERROR_COUNTER
};


void ErrorEOF_ISR(void)
{

    if(g_ErrorHandler.e_Error_Control)
    {
        if(Get_IPP_IPP_SD_ERROR_ITS_CCP_SHIFT_SYNC())
        {
            // Clearing the Error Status
            Set_IPP_IPP_SD_ERROR_ITS_BCLR_SD_ERROR_ITS_BCLR(0x01);
            g_ErrorHandler.u8_CCP_Error_Counter++;
        }


        if(Get_IPP_IPP_SD_ERROR_ITS_CCP_FALSE_SYNC())
        {
            // Clearing the Error Status
            Set_IPP_IPP_SD_ERROR_ITS_BCLR_SD_ERROR_ITS_BCLR(0x02);
            g_ErrorHandler.u8_CCP_Error_Counter++;
        }

        if(Get_IPP_IPP_SD_ERROR_ITS_CCP_CRC_ERROR())
        {
            // Clearing the Error Status
            Set_IPP_IPP_SD_ERROR_ITS_BCLR_SD_ERROR_ITS_BCLR(0x04);
            g_ErrorHandler.u8_CCP_Error_Counter++;
        }

        if(Get_IPP_IPP_SD_ERROR_ITS_SD_CSI2_PACKET_ERROR())
        {
            // Clearing the Error Status
            Set_IPP_IPP_SD_ERROR_ITS_BCLR_SD_ERROR_ITS_BCLR(0x0400);

        if(Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_PACKET_PAYLOAD_CRC_ERROR())
        {
            // Clearing the Error Status
            Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR_CSI_PACKET_ERROR_ITS_BCLR(0x01);
                g_ErrorHandler.u8_CSI_Error_Counter++;
        }

        if(Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_PACKET_HEADER_ECC_ERROR())
        {
            // Clearing the Error Status
            Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR_CSI_PACKET_ERROR_ITS_BCLR(0x02);
                g_ErrorHandler.u8_CSI_Error_Counter++;
        }

        if(Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_EOF())
        {
                 // Clearing the Error Status
            Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR_CSI_PACKET_ERROR_ITS_BCLR(0x04);
                 g_ErrorHandler.u16_CSI_EOF_Counter++;
             }
        }

        // Clear SD_Error Event
        ITM_Clear_EVT_SD_ERROR();

        if(Error_ENABLED_ABORT <= g_ErrorHandler.e_Error_Control)
         {
            if(g_ErrorHandler.u8_CSI_Error_Counter || g_ErrorHandler.u8_CCP_Error_Counter)
            {
                g_ErrorHandler.e_Flag_Error_Abort = Flag_e_TRUE;  // Fatal Error, going to Abort/Error
                if(Error_ENABLED_RECOVER == g_ErrorHandler.e_Error_Control)
                {
                    g_ErrorHandler.e_Flag_Error_Recover = Flag_e_TRUE; // Trying to recover by resetting IPP
                    // These are required here, because if CSI_EOF is enabled, we cannot wait to reset
                    // them till Error Loop is completed
                    g_ErrorHandler.u8_CSI_Error_Counter = 0;
                    g_ErrorHandler.u8_CCP_Error_Counter = 0;
                }
            }
         }
    }

    return;
}



