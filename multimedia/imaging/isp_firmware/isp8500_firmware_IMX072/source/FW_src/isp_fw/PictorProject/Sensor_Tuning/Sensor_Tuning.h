/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if        INCLUDE_IN_HTML_ONLY
#include "Sensor_Tuning.h"

 \brief     This file exposes an interface to set the sensor_tuning parameters
            in the device.

 \ingroup   Sensor_Tuning
 \endif
*/
#ifndef _SENSOR_TUNING_H_
#define _SENSOR_TUNING_H_

#include "Platform.h"
#include "lla_abstraction.h"

typedef enum
{
    TypeNVMExport_e_Parsed  = 0,
    TypeNVMExport_e_Raw
} TypeNVMExport_te;

/**
 \struct  sensor_tuning_NVM_Control_ts
 \brief   Page for knowing sensor_tuning sub-blocks supported by the device
          and subsequently specifying the buffer containing sensor_tuning data
 \ingroup sensor_tuning
*/
typedef struct
{
    /// SDRAM (shared) Buffer Address, which holds the sensor_tuning config data
    /// [DEFAULT]:  0
    uint32_t                    u32_SubBlock_Data_Address;

    /// address of the memory location to which NVM data is to be published
    uint32_t                    u32_NVM_Data_Address;

    /// Index of selected configuration data sub-block ID
    /// [Range]: (0) to (u16_TotalSubBlockIdsCount - 1)
    /// To be used only after firmware boot
    /// [DEFAULT]:  0
    uint16_t                    u16_SelectedSubBlockIdIndex;

    /// Coin available to HOST for controlling the sensor_tuning functionality.
    /// To be used only after firmware boot
    /// [DEFAULT]:  Coin_e_Heads
    uint8_t                     e_Coin_SubBlock_Control;

    /// coin mechanism for checking status NVM details export
    uint8_t                     e_Coin_NVM__Control;

    /// Host should set it to TRUE when reading configuration is done. Host should set this PE only
    /// when e_Flag_ReadConfigBeforeBoot is programmed to Flag_E_TRUE i.e. host is capable to read and
    /// provide tunning data while booting.
    uint8_t                     e_Flag_ConfigurationDone;

    /// Host should program this PE to True/False to program tunning data while booting / After boot complete respectively
    /// Flag_e_FALSE: Tunning data/buffer for LLCD should be specified in stopped state
    /// Flag_e_TRUE: Tunning data/buffer should be programmed while ISP FW is in booting state.
    /// [DEFAULT]: Flag_e_FALSE

    uint8_t                     e_Flag_ReadConfigBeforeBoot;

    /// PE should be used to control b/w parsed and unparsed data
    /// TypeNVMExport_e_Parsed: provide parsed data
    /// TypeNVMExport_e_Raw: provide RAW data
    /// [DEFAULT]: TypeNVMExport_e_Parsed
    uint8_t                     e_TypeNVMExport;
}Sensor_Tuning_Control_ts;

/**
 \struct  sensor_tuning_NVM_Status_ts
 \brief   Page for knowing sensor_tuning sub-block Ids supported by the device
 \ingroup sensor_tuning
*/
typedef struct
{
    /// Total number of configuration data sub-block IDs
    /// Valid only after the firmware boot
    /// Its value should be > 0, if firmware supports sensor_tuning
    /// [DEFAULT]:  0
    uint16_t    u16_TotalSubBlockIdsCount;

    /// Value of current configuration data sub-block ID
    /// [DEFAULT]:  0
    uint16_t    u16_CurrentSubBlockIdValue;

    /// size of nvm data
    uint16_t    u16_NVM_Data_Size;

    /// Coin to inform HOST that sensor_tuning command has completed.
    /// [DEFAULT]:  Coin_e_Heads
    uint8_t     e_Coin_SubBlock_Status;

    /// coin mechanism for status of NVM details export
    uint8_t     e_Coin_NVM_Status;
}Sensor_Tuning_Status_ts;

extern volatile Sensor_Tuning_Control_ts    g_Sensor_Tuning_Control;
extern volatile Sensor_Tuning_Status_ts     g_Sensor_Tuning_Status;

void                                        sensor_tuning_Read_sensor_tuningConfig (void)TO_EXT_DDR_PRGM_MEM;
#endif // _SENSOR_TUNING_H_

