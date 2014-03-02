/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file MasterI2C_access.c
 \brief Interface for host to access devices connected to I2C bus
 \ingroup MasterI2C
 \endif
*/
#include "MasterI2C.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_MasterI2C_MasterI2C_accessTraces.h"
#endif

HostToMasterI2CControl_ts       g_HostToMasterI2CAccessControl =
{
    MASTER_I2C_DEFAULT_DEVICE_ID,                   // Default device ID
    0x0,                                            // Default index
    Coin_e_Heads,
    HostToMasterI2CRequest_e_ReadNBytes,
    0x0,                                // No of bytes to be read or write
    DeviceAddress_e_7BitDeviceAddress,  // Default 7 bit address supported
    DeviceIndex_e_16BitDataIndex        // 16 bit data index
};

HostToMasterI2CAccessData_ts    g_HostToMasterI2CAccessData;

HostToMasterI2CAccessStatus_ts  g_HostToMasterI2CAccessStatus =
{
    Coin_e_Heads,
    Result_e_Success,
    0   // Error count set to zero
};

/**
 \fn void HostToMasterI2CAccess_Run ( void )
 \brief Check if host has requested any read or write access on MasterI2C
 \details Check if host has requested any read or write access on MasterI2C. The function should be called by state machine
          periodically. Host can request byte, word, DWord and N byte read/write
          access on MasterI2C.
          The value of N should be less than 16. For details refer g_HostToMasterI2CAccessControl and g_HostToMasterI2CAccessData
          page elements.
          After request completion, Module raises notification event to host.

 \return void
 \ingroup MasterI2C
 \callgraph
 \callergraph
*/
void
HostToMasterI2CAccess_Run(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t e_Flag_Status = Flag_e_TRUE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (g_HostToMasterI2CAccessControl.e_Coin_Command != g_HostToMasterI2CAccessStatus.e_Coin_Status)
    {
        switch (g_HostToMasterI2CAccessControl.e_HostToMasterI2CRequest_Request)
        {
            case HostToMasterI2CRequest_e_ReadNBytes:
                e_Flag_Status = MasterI2C_ReadNBytes(
                    g_HostToMasterI2CAccessControl.u16_DeviceID,
                    g_HostToMasterI2CAccessControl.u16_Index,
                    (uint8_t *) (g_HostToMasterI2CAccessData.u8_arrData),
                    g_HostToMasterI2CAccessControl.u8_NoBytesReadWrite,
                    g_HostToMasterI2CAccessControl.e_DeviceAddress_Type,
                    g_HostToMasterI2CAccessControl.e_DeviceIndex_Type);

                break;

            case HostToMasterI2CRequest_e_WriteNBytes:
                e_Flag_Status = MasterI2C_WriteNByte(
                    g_HostToMasterI2CAccessControl.u16_DeviceID,
                    g_HostToMasterI2CAccessControl.u16_Index,
                    (uint8_t *) (g_HostToMasterI2CAccessData.u8_arrData),
                    g_HostToMasterI2CAccessControl.u8_NoBytesReadWrite,
                    g_HostToMasterI2CAccessControl.e_DeviceAddress_Type,
                    g_HostToMasterI2CAccessControl.e_DeviceIndex_Type,
                    SendType_e_Synchronous);

                break;

            default:
                // Some Bad condition hit
                // ASSERT Macro should be used here
                OstTraceInt0(TRACE_ERROR, "<ERROR>!!HostToMasterI2CAccess_Run() - BAD default condition!!");
                ASSERT_XP70();
                break;
        }   // End of Switch

        if (Flag_e_FALSE == e_Flag_Status)
        {
            // there has been an error accessing the MasterI2C channel...
            g_HostToMasterI2CAccessStatus.u8_HostToMasterI2CAccessErrorCount++;
            g_HostToMasterI2CAccessStatus.e_Result_Driver = Result_e_Failure;
        }
        else
        {
            g_HostToMasterI2CAccessStatus.e_Result_Driver = Result_e_Success;
        }


        // at this point, either there has been an error, or the operation has completed
        // flip the status coin anyways...
        g_HostToMasterI2CAccessStatus.e_Coin_Status = g_HostToMasterI2CAccessControl.e_Coin_Command;

        /// Raise event to signal host that I2C transaction is complete
        MasterI2C_HostRequestComplete_Notify();
    }
}

