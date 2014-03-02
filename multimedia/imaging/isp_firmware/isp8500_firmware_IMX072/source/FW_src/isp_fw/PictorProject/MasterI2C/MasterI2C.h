/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup MasterI2C Master CCI
 \brief Master I2C module manages communication interface between MasterI2C IP and
        Host with help of ISP firmware running STxP70. To manage communication with Host, the module
        provide page elements like MasterI2C_Access. To Manage communication with ISP firmware
        MasterI2C module provide Read/Write API's. MasterI2C module than interact with I2C master
        hardware which further control i2c bus. I2C bus can have I2C compliant slaves like sensor and flash.
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  MasterI2C.h
 \brief This file is a part of the release code. It contains the data
        structures, macros, enums and function declarations used by the module.
 \ingroup MasterI2C
 \endif
*/
#ifndef _MASTER_I2C_H_
#   define _MASTER_I2C_H_

#   include "MasterI2C_ip_interface.h"
#   include "MasterI2C_op_interface.h"
#   include "MasterI2C_PlatformSpecific.h"

/**
 \enum Resource_te
 \brief Availability of MasterI2C channel
 \ingroup MasterI2C
*/
typedef enum
{
    /// MasterI2C is available for Read/Write
    Resource_e_ResourceAvailable,

    /// MasterI2C is busy with some transaction
    Resource_e_ResourceBusy
} Resource_te;

/**
 \enum CCI_EERROR_MGMT_te
 \brief Error code returned by hardware
 \ingroup MasterI2C
*/
typedef enum
{
    /// 0x00 no-error
    CCI_EERROR_MGMT_e_No_Error,

    /// 0x01 device_id_lo_error
    CCI_EERROR_MGMT_e_device_id_lo_error,

    /// 0x02 device_id_hi_error
    CCI_EERROR_MGMT_e_device_id_hi_error,

    /// 0x03 index_hi_error
    CCI_EERROR_MGMT_e_index_hi_error,

    /// 0x04 index_lo_error
    CCI_EERROR_MGMT_e_index_lo_error,

    /// 0x05 data_error
    CCI_EERROR_MGMT_e_data_error
} CCI_EERROR_MGMT_te;

// macros for resetting and setting the MasterI2C error
#   define MasterI2C_GetTransactionError()     (g_MasterI2CStatus.e_CCI_EERROR_MGMT_Error)
#   define MasterI2C_ResetTransactionError()   (g_MasterI2CStatus.e_CCI_EERROR_MGMT_Error = Flag_e_FALSE)
#   define MasterI2C_SetTransactionError(x)    (g_MasterI2CStatus.e_CCI_EERROR_MGMT_Error = x)

// If resource is busy, the macro will return true, and while will not execute further
#   define MasterI2C_SetI2CResourceAvailable() (g_MasterI2CStatus.e_Resource_Status = Resource_e_ResourceAvailable)
#   define MasterI2C_SetI2CResourceBusy()      (g_MasterI2CStatus.e_Resource_Status = Resource_e_ResourceBusy)
#   define MasterI2C_IsI2C_ResourceBusy()      (Resource_e_ResourceBusy == g_MasterI2CStatus.e_Resource_Status)
#   define MasterI2C_IsI2C_ResourceAvialable() (Resource_e_ResourceAvailable == g_MasterI2CStatus.e_Resource_Status)

// If resource is busy, the macro will return true, and while will not execute further
#   define MasterI2C_SetI2CTransactionAvailable()      (g_MasterI2CStatus.e_Resource_I2C_TrasactionStatus = Resource_e_ResourceAvailable)
#   define MasterI2C_SetI2CTransactionBusy()           (g_MasterI2CStatus.e_Resource_I2C_TrasactionStatus = Resource_e_ResourceBusy)
#   define MasterI2C_IsI2C_TransactionBusy()           (Resource_e_ResourceBusy == g_MasterI2CStatus.e_Resource_I2C_TrasactionStatus)
#   define MasterI2C_IsI2C_TransactionAvailable()      (Resource_e_ResourceAvailable == g_MasterI2CStatus.e_Resource_I2C_TrasactionStatus)


// exported functions
// ==================
extern uint8_t  MasterI2C_GrabResource (void);
extern void     MasterI2C_FreeResource (void);

/***************************************************************/
extern uint8_t  MasterI2C_ReadBytesHW (
                uint16_t    u16_DeviceID,
                uint16_t    u16_RegisterAddress,
                uint8_t     u8_NoOfBytes,
                uint8_t     *ptru8_ReadData,
                uint8_t     e_DeviceAddress_Type,
                uint8_t     e_DeviceIndex_Type);

extern uint8_t  MasterI2C_WriteBytesHW (
                uint16_t    u16_DeviceID,
                uint16_t    u16_RegisterAddress,
                uint8_t     u8_NoOfBytes,
                uint8_t     *ptru8_ReadData,
                uint8_t     e_DeviceAddress_Type,
                uint8_t     e_DeviceIndex_Type,
                uint8_t     e_SendType_Type);

#endif // _MASTER_I2C_H_

