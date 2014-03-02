/*
* Copyright (C) ST-Ericsson 2010
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/

/* 5MP(IMX072) specific implementation*/
#include "lla_common_config.h"
#include "lla_drv_imx072.h"
#include "smia_sensor_memorymap_defs.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_LLA_lla_drv_imx072Traces.h"
#endif

#if (1 == LLA_INCLUDE_IMX072)

uint8_t  g_u8GetRawNVMData = Flag_e_FALSE;
uint8_t  g_u8SensorModelId = LLA_IMX072_MODEL_ID_OTH;

// For HDR
uint32_t Frame_count_timemodel = 0;

/*Sensor current configurations and Sensor capability register*/
CamGlbConfig_ts                      g_IMX072_CamGlbConfig;

/*Local Macro definition--- */
#define INC_PTR_COUNT()                                                   \
    {                                                                     \
        ptr++;                                                            \
        count++;                                                          \
        if (count == 4)                                                   \
        {                                                                 \
            count = 0;                                                    \
            if (CAM_DRV_SENS_FORMAT_RAW10 == g_IMX072_CamGlbConfig.camDrvMiscDetails.csiRawFormat) \
            {                                                             \
                ptr++;                                                    \
            }                                                             \
        }                                                                 \
    }


/*NVM Read interface*/
#   define LLA_READ_NVM_BYTES(devAddr, regAddr, NoOfBytes, ptrBuffer)   \
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_read_fn(   \
            devAddr,                                                    \
            regAddr,                                                    \
            CAM_DRV_I2C_REG_SIZE_8BIT,                                  \
            NoOfBytes,                                                  \
            CAM_DRV_I2C_BYTE_ORDER_NORMAL,                              \
            ptrBuffer)

#if (LLA_FLASH_LED == 0) // If Xenon Flash
   uint8_t         g_u8StrobeLength = 0;
#endif /*(LLA_FLASH_LED == 0)*/


/*PowerOn settings for IMX072 - from "IMX072ES_RegisterSetting_I2C_MIPI_2lane_def_rev2.0_ST-EMP.xls"*/
CAM_DRV_CAMERA_CONFIGS_T    g_LLA_IMX072_PowerOnSettings[] =
{
    //{0x03, 0x07, 0x12}, //PLL Multiplier will be set at the time of VF
    //{0x01, 0x01, 0x03}, //orientation will be set at the time of VF
    { 0x01, 0x03, 0x01 },

    // Software reset - Added from SEMC I2C Dump
    { 0x30, 0x0A, 0x80 },
    { 0x30, 0x14, 0x08 },
    { 0x30, 0X15, 0x37 },
    { 0x30, 0x1C, 0x01 },
    { 0x30, 0x31, 0x28 },
    { 0x30, 0x40, 0x00 },
    { 0x30, 0x41, 0x60 },
    { 0x30, 0x51, 0x24 },
    { 0x30, 0x53, 0x34 },
    { 0x30, 0x55, 0x3B },
    { 0x30, 0x57, 0xC0 },
    { 0x30, 0x60, 0x30 },
    { 0x30, 0x65, 0x00 },
    { 0x30, 0xAA, 0x88 },
    { 0x30, 0xAB, 0x1C },
    { 0x30, 0xB0, 0x32 },
    { 0x30, 0xB2, 0x83 },
    { 0x30, 0xD3, 0x04 },
    { 0x31, 0x0E, 0xDD },
    { 0x31, 0xA4, 0xD8 },
    { 0x31, 0xA6, 0x17 },
    { 0x31, 0xAC, 0xCF },
    { 0x31, 0xAE, 0xF1 },
    { 0x31, 0xB4, 0xD8 },
    { 0x31, 0xB6, 0x17 },

    //  Begin : MIPI Global Settings
    { 0x33, 0x04, 0x05 },
    { 0x33, 0x05, 0x04 },
    { 0x33, 0x06, 0x15 },
    { 0x33, 0x07, 0x02 },
    { 0x33, 0x08, 0x11 },
    { 0x33, 0x09, 0x04 },

    //ER#317473 <sudeep>old value 0x07
    { 0x33, 0x0A, 0x05 },
    { 0x33, 0x0B, 0x02 },

    //ER#317473 <sudeep>old value 0x04
    { 0x33, 0x0C, 0x05 },
    { 0x33, 0x0D, 0x04 },
    { 0x33, 0x0E, 0x01 },

    // End : MIPI Global Settings
    //
    // BEGIN: Added for for flash
    { 0x30, 0x90, 0x02 },

    //Pll Oscillation Stable Wait Time
    { 0x30, 0x2B, 0x1E },
    { 0x80, 0x00, 0x00 },
    { 0xCC, 0x10, 0x14 },

    // [NON_SMIA_ISL_SUPPORT]
#if (0 == NO_OF_ISL_FROM_SENSOR)
    { 0x30, 0xD8, 0x20},
#endif

    { 0x00, 0x00, 0x00 }
};


///PLL Settings corresponding to mode 0
const CAM_DRV_CAMERA_CONFIGS_T    g_LLA_IMX072_GlobalPLL_Settings_P0[] =
{
    //Number Of Output channels
    { 0x30, 0x17, 0x40 },

    // Pre PLL Divider
    { 0x03, 0x04, 0x00 },
    { 0x03, 0x05, 0x01 },

    // PLL Multiplier
    { 0x03, 0x06, 0x00 },
    { 0x03, 0x07, 0x43 },

    // End of writes
    { 0x00, 0x00, 0x00 }
};

///PLL Settings corresponding to mode 0
const CAM_DRV_CAMERA_CONFIGS_T    g_LLA_IMX072_GlobalPLL_Settings_P1[] =
{
    //Number Of Output channels
    { 0x30, 0x17, 0xC0 },

    // Pre PLL Divider
    { 0x03, 0x04, 0x00 },
    { 0x03, 0x05, 0x01 },

    // PLL Multiplier
    { 0x03, 0x06, 0x00 },
    { 0x03, 0x07, 0x43 },

    // End of writes
    { 0x00, 0x00, 0x00 }
};

//PLL Settings (P0)
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_PLL_SettingsMode0_P0[] = { { 0x00, 0x00, 0x00 } };// End of writes

//Mode 0 FFOV
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE0_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x00 },
    { 0x03, 0x45, 0x00 },

    //y_addr_start
    { 0x03, 0x46, 0x00 },
    { 0x03, 0x47, 0x00 },

    //x_addr_end
    { 0x03, 0x48, 0x0A },
    { 0x03, 0x49, 0x2F },

    //y_addr_end
    { 0x03, 0x4A, 0x07 },
    { 0x03, 0x4B, 0xA7 },

    //x_output_size
    { 0x03, 0x4C, 0x0A },
    { 0x03, 0x4D, 0x30 },

    //y_output_size
    { 0x03, 0x4E, 0x07},
    { 0x03, 0x4F, 0xA8 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

//Mode 1 SubSampled Output
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE1_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x00 },
    { 0x03, 0x45, 0x00 },

    //y_addr_start
    { 0x03, 0x46, 0x00 },
    { 0x03, 0x47, 0x00 },

    //x_addr_end
    { 0x03, 0x48, 0x0A },
    { 0x03, 0x49, 0x2F },

    //y_addr_end
    { 0x03, 0x4A, 0x07 },
    { 0x03, 0x4B, 0xA7 },

    //x_output_size
    { 0x03, 0x4C, 0x05 },
    { 0x03, 0x4D, 0x18 },

    //y_output_size
    { 0x03, 0x4E, 0x03 },
    { 0x03, 0x4F, 0xD4 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x03 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x03 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x86 },

   // frameblanking time with lane select reg
    { 0x33, 0x01, 0x84 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

//Mode 2 FFOV 16:9
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE2_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x00 },
    { 0x03, 0x45, 0x00 },

    //y_addr_start
    { 0x03, 0x46, 0x00 },
    { 0x03, 0x47, 0xF3 },

    // <Sudeep> ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x0A },
    { 0x03, 0x49, 0x2F },

    //y_addr_end
    { 0x03, 0x4A, 0x06 },
    { 0x03, 0x4B, 0xBC }, // 0xBC changed to B8 from B4: to support 1478 instead of 1474

    // <Sudeep> ER:370300 Incremented start address by 1 fix for purplecast
    //x_output_size
    { 0x03, 0x4C, 0x0A },
    { 0x03, 0x4D, 0x30 },

    //y_output_size
    { 0x03, 0x4E, 0x05 },
    { 0x03, 0x4F, 0xCA }, // CA from C6 Changed to C6 from C4: to support 1478 instead of 1474

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

  // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE3_P0[] =
{
    //x_addr_start
    { 0x03, 0x44, 0x01 },
    { 0x03, 0x45, 0xC6 },

    //y_addr_start
    { 0x03, 0x46, 0x01 },
    { 0x03, 0x47, 0x55 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x08 },
    { 0x03, 0x49, 0x69 },

    //y_addr_end
    { 0x03, 0x4A, 0x06 },
    { 0x03, 0x4B, 0x50 },

    //x_output_size
    { 0x03, 0x4C, 0x06 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x04 },
    { 0x03, 0x4F, 0xFC },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE4_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x01 },
    { 0x03, 0x45, 0xC6 },

    //y_addr_start
    { 0x03, 0x46, 0x01 },
    { 0x03, 0x47, 0xF3 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x08 },
    { 0x03, 0x49, 0x69 },

    //y_addr_end
    { 0x03, 0x4A, 0x05 },
    { 0x03, 0x4B, 0xB2 },

    //x_output_size
    { 0x03, 0x4C, 0x06 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x03 },
    { 0x03, 0x4F, 0xC0 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE5_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x02 },
    { 0x03, 0x45, 0x16 },

    //y_addr_start
    { 0x03, 0x46, 0x01 },
    { 0x03, 0x47, 0x93 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x08 },
    { 0x03, 0x49, 0x15 },

    //y_addr_end
    { 0x03, 0x4A, 0x06},
    { 0x03, 0x4B, 0x12},

    //x_output_size
    { 0x03, 0x4C, 0x06},
    { 0x03, 0x4D, 0x00},

    //y_output_size
    { 0x03, 0x4E, 0x04},
    { 0x03, 0x4F, 0x80},

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE6_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0x44 },

    //y_addr_start
    { 0x03, 0x46, 0x02 },
    { 0x03, 0x47, 0x75 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0xE7 },

    //y_addr_end
    { 0x03, 0x4A, 0x05 },
    { 0x03, 0x4B, 0x30 },

    //x_output_size
    { 0x03, 0x4C, 0x03 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x02 },
    { 0x03, 0x4F, 0xBC },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE7_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0x44 },

    //y_addr_start
    { 0x03, 0x46, 0x02 },
    { 0x03, 0x47, 0xCD },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0xE7 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0xDA },

    //x_output_size
    { 0x03, 0x4C, 0x03 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x02 },
    { 0x03, 0x4F, 0x0E },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE8_P0[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0x66 },

    //y_addr_start
    { 0x03, 0x46, 0x02 },
    { 0x03, 0x47, 0xE1 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0xC9 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0xC4 },

    //x_output_size
    { 0x03, 0x4C, 0x03 },
    { 0x03, 0x4D, 0x64 },

    //y_output_size
    { 0x03, 0x4E, 0x01 },
    { 0x03, 0x4F, 0xE4 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};



const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE9_P0[] =
{

    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0xF4 },

    //y_addr_start
    { 0x03, 0x46, 0x03 },
    { 0x03, 0x47, 0x31 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0x37 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0x76 },

    //x_output_size
    { 0x03, 0x4C, 0x02 },
    { 0x03, 0x4D, 0x44 },

    //y_output_size
    { 0x03, 0x4E, 0x01 },
    { 0x03, 0x4F, 0x46 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE10_P0[] =
{

    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x04 },
    { 0x03, 0x45, 0x8A },

    //y_addr_start
    { 0x03, 0x46, 0x03 },
    { 0x03, 0x47, 0x6B },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x05 },
    { 0x03, 0x49, 0xA1 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0x3C },

    //x_output_size
    { 0x03, 0x4C, 0x01 },
    { 0x03, 0x4D, 0x18 },

    //y_output_size
    { 0x03, 0x4E, 0x00 },
    { 0x03, 0x4F, 0xD2 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x04 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};


/* <Sudeep> Single Data Lane Support*/

//Mode 0 FFOV
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE0_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x00 },
    { 0x03, 0x45, 0x00 },

    //y_addr_start
    { 0x03, 0x46, 0x00 },
    { 0x03, 0x47, 0x00 },

    //x_addr_end
    { 0x03, 0x48, 0x0A },
    { 0x03, 0x49, 0x2F },

    //y_addr_end
    { 0x03, 0x4A, 0x07 },
    { 0x03, 0x4B, 0xA7 },

    //x_output_size
    { 0x03, 0x4C, 0x0A },
    { 0x03, 0x4D, 0x30 },

    //y_output_size
    { 0x03, 0x4E, 0x07},
    { 0x03, 0x4F, 0xA8 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

//Mode 1 SubSampled Output
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE1_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x00 },
    { 0x03, 0x45, 0x00 },

    //y_addr_start
    { 0x03, 0x46, 0x00 },
    { 0x03, 0x47, 0x00 },

    //x_addr_end
    { 0x03, 0x48, 0x0A },
    { 0x03, 0x49, 0x2F },

    //y_addr_end
    { 0x03, 0x4A, 0x07 },
    { 0x03, 0x4B, 0xA7 },

    //x_output_size
    { 0x03, 0x4C, 0x05 },
    { 0x03, 0x4D, 0x18 },

    //y_output_size
    { 0x03, 0x4E, 0x03 },
    { 0x03, 0x4F, 0xD4 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x03 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x03 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x86 },

   // frameblanking time with lane select reg
    { 0x33, 0x01, 0x85 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

//Mode 2 FFOV 16:9
const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE2_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x00 },
    { 0x03, 0x45, 0x00 },

    //y_addr_start
    { 0x03, 0x46, 0x00 },
    { 0x03, 0x47, 0xF3 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x0A },
    { 0x03, 0x49, 0x2F },

    //y_addr_end
    { 0x03, 0x4A, 0x06 },
    { 0x03, 0x4B, 0xBC }, // 0xBC changed to B8 from B4: to support 1478 instead of 1474

    // <Sudeep>ER:370300 Incremented end address by 1
    //x_output_size
    { 0x03, 0x4C, 0x0A },
    { 0x03, 0x4D, 0x30 },

    //y_output_size
    { 0x03, 0x4E, 0x05 },
    { 0x03, 0x4F, 0xCA },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

  // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE3_P1[] =
{
    //x_addr_start
    { 0x03, 0x44, 0x01 },
    { 0x03, 0x45, 0xC6 },

    //y_addr_start
    { 0x03, 0x46, 0x01 },
    { 0x03, 0x47, 0x55 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x08 },
    { 0x03, 0x49, 0x69 },

    //y_addr_end
    { 0x03, 0x4A, 0x06},
    { 0x03, 0x4B, 0x50},

    //x_output_size
    { 0x03, 0x4C, 0x06},
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x04},
    { 0x03, 0x4F, 0xFC },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE4_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x01 },
    { 0x03, 0x45, 0xC6},

    //y_addr_start
    { 0x03, 0x46, 0x01 },
    { 0x03, 0x47, 0xF3 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x08 },
    { 0x03, 0x49, 0x69 },

    //y_addr_end
    { 0x03, 0x4A, 0x05 },
    { 0x03, 0x4B, 0xB2 },

    //x_output_size
    { 0x03, 0x4C, 0x06 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x03 },
    { 0x03, 0x4F, 0xBF },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE5_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x02 },
    { 0x03, 0x45, 0x16 },

    //y_addr_start
    { 0x03, 0x46, 0x01 },
    { 0x03, 0x47, 0x93 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x08 },
    { 0x03, 0x49, 0x15 },

    //y_addr_end
    { 0x03, 0x4A, 0x06},
    { 0x03, 0x4B, 0x12},

    //x_output_size
    { 0x03, 0x4C, 0x06},
    { 0x03, 0x4D, 0x00},

    //y_output_size
    { 0x03, 0x4E, 0x04},
    { 0x03, 0x4F, 0x80},

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE6_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0x44 },

    //y_addr_start
    { 0x03, 0x46, 0x02 },
    { 0x03, 0x47, 0x75 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0xE7 },

    //y_addr_end
    { 0x03, 0x4A, 0x05 },
    { 0x03, 0x4B, 0x30 },

    //x_output_size
    { 0x03, 0x4C, 0x03 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x02 },
    { 0x03, 0x4F, 0xBC },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE7_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0x44 },

    //y_addr_start
    { 0x03, 0x46, 0x02 },
    { 0x03, 0x47, 0xCD },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0xE7 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0xDA },

    //x_output_size
    { 0x03, 0x4C, 0x03 },
    { 0x03, 0x4D, 0xA4 },

    //y_output_size
    { 0x03, 0x4E, 0x02 },
    { 0x03, 0x4F, 0x0E },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE8_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0x66 },

    //y_addr_start
    { 0x03, 0x46, 0x02 },
    { 0x03, 0x47, 0xE1 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0xC9 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0xC4 },

    //x_output_size
    { 0x03, 0x4C, 0x03 },
    { 0x03, 0x4D, 0x64 },

    //y_output_size
    { 0x03, 0x4E, 0x01 },
    { 0x03, 0x4F, 0xE4 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};


const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE9_P1[] =
{
    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x03 },
    { 0x03, 0x45, 0xF4 },

    //y_addr_start
    { 0x03, 0x46, 0x03 },
    { 0x03, 0x47, 0x31 },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x06 },
    { 0x03, 0x49, 0x37 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0x76 },

    //x_output_size
    { 0x03, 0x4C, 0x02 },
    { 0x03, 0x4D, 0x44 },

    //y_output_size
    { 0x03, 0x4E, 0x01 },
    { 0x03, 0x4F, 0x46 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

const CAM_DRV_CAMERA_CONFIGS_T  g_IMX072_FD_MODE10_P1[] =
{

    // FDM Settings
    //x_addr_start
    { 0x03, 0x44, 0x04 },
    { 0x03, 0x45, 0x8A },

    //y_addr_start
    { 0x03, 0x46, 0x03 },
    { 0x03, 0x47, 0x6B },

    // <Sudeep>ER:370300 Incremented start address by 1 fix for purplecast
    //x_addr_end
    { 0x03, 0x48, 0x05 },
    { 0x03, 0x49, 0xA1 },

    //y_addr_end
    { 0x03, 0x4A, 0x04 },
    { 0x03, 0x4B, 0x3C },

    //x_output_size
    { 0x03, 0x4C, 0x01 },
    { 0x03, 0x4D, 0x18 },

    //y_output_size
    { 0x03, 0x4E, 0x00 },
    { 0x03, 0x4F, 0xD2 },

    //x even inc
    { 0x03, 0x80, 0x00 },
    { 0x03, 0x81, 0x01 },

    //x odd inc
    { 0x03, 0x82, 0x00 },
    { 0x03, 0x83, 0x01 },

    //y even inc
    { 0x03, 0x84, 0x00 },
    { 0x03, 0x85, 0x01 },

    //y odd inc
    { 0x03, 0x86, 0x00 },
    { 0x03, 0x87, 0x01 },

    //Misc Settings. As they are mode specific, so write these to sensors here along with other mode specific settings
    // MIPI Size Setting
    { 0x30, 0x16, 0x06 },
    { 0x30, 0xE8, 0x06 },

    // frameblanking time with lane select reg
    { 0x33, 0x01, 0x05 },
    { 0x30, 0x22, 0xC2 },
    { 0x30, 0x25, 0x09 },
    { 0x00, 0x00, 0x00 },
};

/*Suported Camera output modes for  IMX072 camera*/
CAM_DRV_SENS_OUTPUT_MODE_T  g_LLA_IMX072_OutputModes[] =
{


//    CAM_DRV_USAGE_MODE_VF,                    0
//    CAM_DRV_USAGE_MODE_AF,                    1
//    CAM_DRV_USAGE_MODE_STILL_CAPTURE,         2
//    CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE,   3
//    CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE,     4
//    CAM_DRV_USAGE_MODE_VIDEO_CAPTURE,         5
//    CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE,   6
//    CAM_DRV_USAGE_MODE_HQ_VIDEO_CAPTURE,      7
//    CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE,      8// high frame rate video
//    HS:8 | HQ:7 | NIGHT_VIDEO:6 | Video:5 | Still_seq:4 |Still Night:3 | Still_capture:2|AF:1| VF:0,

    {/*FFoV*/
        {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
        {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_HQ_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF)|(1<<CAM_DRV_USAGE_MODE_VF) ,
        0
    },

    {/*FFoV - with - Elimination/Addition */
        {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
        {LLA_SENSOR_IMX072_FFOV_X/2, LLA_SENSOR_IMX072_FFOV_Y/2},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF),
        0
    },

    /*Next few modes are cropped mode,
    Have a look at IMX072 Addendum doc. for details of these mode
    */

    /*16:9 FFoV Crop from the sensor*/
    {
        {2608,1482}, // from 1478
        {2608,1482}, // from 1478
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF)|(1<<CAM_DRV_USAGE_MODE_VF), // 101011111 only HQ and Video.
        0
    },

    /*H1700xV1276*/
    { //    HS | HQ | NIGHT_VIDEO | Video | Still_seq |Still Night | Still_capture| AF| VF,
      { 1700, 1276 },
      { 1700, 1276 }, 
      CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF), //0x15A
      0
    },

    /*H1700xV960 */
    { //    HS | HQ | NIGHT_VIDEO | Video | Still_seq |Still Night | Still_capture| AF| VF,
      { 1700, 960 },
      { 1700, 960 }, //
      CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF), //0x15A
      0
    },

    /*4:3 Crop from the sensor*/
    {
        {1536,1152},
        {1536,1152},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF),
        0
    },

    /*4:3 Crop from the sensor*/
    {
        {932,700},
        {932,700},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF),
        0
    },

    /*16:9 Crop from the sensor*/
    {
        {932,526},
        {932,526},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF), //0x15E
        0
    },

    /*H868xV484 Cropping*/
    { //    HS | HQ | NIGHT_VIDEO | Video | Still_seq |Still Night | Still_capture| AF| VF,
      { 868, 484 },
      { 868, 484 }, //
      CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HQ_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF)|(1<<CAM_DRV_USAGE_MODE_VF), //0xFF
      0
    },

    /*16:9 FFoV Crop from the sensor, to support digital zoom for 720p and 1080p*/
    {
        {580,326},
        {580,326},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF), //0x15E
      0
    },

    /*4:3 Crop from the sensor*/
    {
        {280,210},
        {280,210},
        CAM_DRV_SENS_FORMAT_RAW10,
        (1<<CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_STILL_CAPTURE)|(1<<CAM_DRV_USAGE_MODE_AF), //0x15E
      0
    },
};


/* We use max frequency per lane as 648 when pixl clock is set at 129.6 for 2 datalanes from datasheet pg.no :91 */
const SensorModeSettings_ts g_IMX072_SensorMode0_P0 =
{
    {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
    {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
    129.6,                                            //f_VTPixelClockFrequency_Mhz
    129.6,                                            //f_OPPixelClockFrequency_Mhz
    0,                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/129.6),                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE0_P0,                             //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE0,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE0,                    //minimum frame blanking line
    EXTRA_LINES_MODE0,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode1_P0 =
{
    {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
    {LLA_SENSOR_IMX072_FFOV_X/2, LLA_SENSOR_IMX072_FFOV_Y/2},
    129.6,                                                                              //f_VTPixelClockFrequency_Mhz
    129.6,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                                // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE1_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE1,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE1,                    //minimum frame blanking line
    EXTRA_LINES_MODE1,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode2_P0 =
{
    {2608,1482}, // from 1478
    {2608,1482}, // from 1478
    129.6,                                                                              //f_VTPixelClockFrequency_Mhz
    129.6,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                        // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE2_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE2,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE2,                    //minimum frame blanking line
    EXTRA_LINES_MODE2,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode3_P0 =
{
    { 1700, 1276 },
    { 1700, 1276 },
    129.6,                                                                              //f_VTPixelClockFrequency_Mhz
    129.6,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE3_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE3,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE3,                    //minimum frame blanking line
    EXTRA_LINES_MODE3,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode4_P0 =
{
    { 1700, 960 },
    { 1700, 960 },
    129.6,                                                                              //f_VTPixelClockFrequency_Mhz
    129.6,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                               // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE4_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE4,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE4,                    //minimum frame blanking line
    EXTRA_LINES_MODE4,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode5_P0 =
{
    {1536,1152},
    {1536,1152},
    129.6,                                                                              //f_VTPixelClockFrequency_Mhz
    129.6,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                               // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE5_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE5,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE5,                    //minimum frame blanking line
    EXTRA_LINES_MODE5,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode6_P0 =
{    
    {932,700},
    {932,700},
    129.6,                                                                             //f_VTPixelClockFrequency_Mhz
    129.6,                                                                                //f_OPPixelClockFrequency_Mhz
    0,                                                                                 //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                         // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE6_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE6,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE6,                    //minimum frame blanking line
    EXTRA_LINES_MODE6,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode7_P0 =
{
    {932,526},
    {932,526},
    129.6,                                                                              //f_VTPixelClockFrequency_Mhz
    129.6,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1/129.6),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE7_P0,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE7,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE7,                    //minimum frame blanking line
    EXTRA_LINES_MODE7,                                //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode8_P0 =
{
    { 868, 484 },
    { 868, 484 }, //
    129.6,                          //f_VTPixelClockFrequency_Mhz
    129.6,                          //f_OPPixelClockFrequency_Mhz
    0,                              //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1 / 129.6),                    //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE8_P0,           //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE8,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE8,                    //minimum frame blanking line
    EXTRA_LINES_MODE8,                                 //extra lines
};


const SensorModeSettings_ts g_IMX072_SensorMode9_P0 =
{
    {580,326},
    {580,326},
    129.6,                          //f_VTPixelClockFrequency_Mhz
    129.6,                          //f_OPPixelClockFrequency_Mhz
    0,                              //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1 / 129.6),                    //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE9_P0,           //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE9,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE9,                    //minimum frame blanking line
    EXTRA_LINES_MODE9,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode10_P0 =
{
    {280,210},
    {280,210},
    129.6,                          //f_VTPixelClockFrequency_Mhz
    129.6,                          //f_OPPixelClockFrequency_Mhz
    0,                              //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1 / 129.6),                    //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE10_P0,           //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE10,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE10,                    //minimum frame blanking line
    EXTRA_LINES_MODE10,                                 //extra lines
};


const SensorModeSettings_ts *p_IMX072_SensorModesArray_P0[] =
{
    &g_IMX072_SensorMode0_P0,
    &g_IMX072_SensorMode1_P0,
    &g_IMX072_SensorMode2_P0,
    &g_IMX072_SensorMode3_P0,
    &g_IMX072_SensorMode4_P0,
    &g_IMX072_SensorMode5_P0,
    &g_IMX072_SensorMode6_P0,
    &g_IMX072_SensorMode7_P0,
    &g_IMX072_SensorMode8_P0,
    &g_IMX072_SensorMode9_P0,
    &g_IMX072_SensorMode10_P0,
};
/* We use max frequency per lane as 648 when pixl clock is set at 64.8 for 1 datalanes from datasheet pg.no :91 */
const SensorModeSettings_ts g_IMX072_SensorMode0_P1 =
{
    {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
    {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                                // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE0_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE0,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE0,                    //minimum frame blanking line
    EXTRA_LINES_MODE0,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode1_P1 =
{
    {LLA_SENSOR_IMX072_FFOV_X, LLA_SENSOR_IMX072_FFOV_Y},
    {LLA_SENSOR_IMX072_FFOV_X/2, LLA_SENSOR_IMX072_FFOV_Y/2},
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                                // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE1_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE1,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE1,                    //minimum frame blanking line
    EXTRA_LINES_MODE1,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode2_P1 =
{
    {2608,1482}, // from 1478
    {2608,1482}, // from 1478
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                        // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE2_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE2,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE2,                    //minimum frame blanking line
    EXTRA_LINES_MODE2,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode3_P1 =
{
    { 1700, 1276 },
    { 1700, 1276 },
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE3_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE3,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE3,                    //minimum frame blanking line
    EXTRA_LINES_MODE3,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode4_P1 =
{
    { 1700, 960 },
    { 1700, 960 },
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                               // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE4_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE4,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE4,                    //minimum frame blanking line
    EXTRA_LINES_MODE4,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode5_P1 =
{
    {1536,1152},
    {1536,1152},
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
   (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                               // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE5_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE5,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE5,                    //minimum frame blanking line
    EXTRA_LINES_MODE5,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode6_P1 =
{
    {932,700},
    {932,700},
    64.8,                                                                             //f_VTPixelClockFrequency_Mhz
    64.8,                                                                                //f_OPPixelClockFrequency_Mhz
    0,                                                                                 //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                         // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,                    ///TODO: p_PllModes Reusing this dummy settings array for all modes, as this sensor does not need Mode Specific Pll Settings
    g_IMX072_FD_MODE6_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE6,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE6,                    //minimum frame blanking line
    EXTRA_LINES_MODE6,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode7_P1 =
{
    {932,526},
    {932,526},
    64.8,                                                                              //f_VTPixelClockFrequency_Mhz
    64.8,                                                                              //f_OPPixelClockFrequency_Mhz
    0,                                                                                //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1/64.8),                                                         //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                                                                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE7_P1,                                         //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE7,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE7,                    //minimum frame blanking line
    EXTRA_LINES_MODE7,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode8_P1 =
{
    { 868, 484 },
    { 868, 484 }, //
    64.8,                           //f_VTPixelClockFrequency_Mhz
    64.8,                           //f_OPPixelClockFrequency_Mhz
    0,                              //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1 / 64.8),                     //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE8_P1,           //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE8,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE8,                    //minimum frame blanking line
    EXTRA_LINES_MODE8,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode9_P1 =
{
    {580,326},
    {580,326},
    64.8,                           //f_VTPixelClockFrequency_Mhz
    64.8,                           //f_OPPixelClockFrequency_Mhz
    0,                              //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1 / 64.8),                     //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE9_P1,           //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE9,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE9,                    //minimum frame blanking line
    EXTRA_LINES_MODE9,                                 //extra lines
};

const SensorModeSettings_ts g_IMX072_SensorMode10_P1 =
{
    {280,210},
    {280,210},
    64.8,                           //f_VTPixelClockFrequency_Mhz
    64.8,                           //f_OPPixelClockFrequency_Mhz
    0,                              //f_OPSystemClockFrequency_Mhz Not preSent in IMX072
    (1 / 64.8),                     //f_VTPixelClockPeriod_us = 1 / f_VTPixelClockFrequency_Mhz
    0,                              // u16_MiniLineLengthModeSpecific: Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    g_IMX072_PLL_SettingsMode0_P0,
    g_IMX072_FD_MODE10_P1,           //p_FDModes

    MIN_LINE_BLANKING_PCK_MODE10,                      //minimum line blanking pck
    MIN_FRAME_BLANKING_LINE_MODE10,                    //minimum frame blanking line
    EXTRA_LINES_MODE10,                                 //extra lines
};


const SensorModeSettings_ts *p_IMX072_SensorModesArray_P1[] =
{
    &g_IMX072_SensorMode0_P1,
    &g_IMX072_SensorMode1_P1,
    &g_IMX072_SensorMode2_P1,
    &g_IMX072_SensorMode3_P1,
    &g_IMX072_SensorMode4_P1,
    &g_IMX072_SensorMode5_P1,
    &g_IMX072_SensorMode6_P1,
    &g_IMX072_SensorMode7_P1,
    &g_IMX072_SensorMode8_P1,
    &g_IMX072_SensorMode9_P1,
    &g_IMX072_SensorMode10_P1,
};


SensorSettings_ts           g_IMX072_SensorModeSettings[] =
{
{
        //Settings for 9.6 MHz as Input Clock.
        EXT_SENSOR_CLOCK,                                                                  //u32_extClkFreqx100
        MAX_CSI_PLL_OUT_FREQ_IMX072_RAW10,                                                 //u32_CSI2LinkBitRate
        CAM_DRV_SENS_CSI2_LANE_MODE_2,                                                     //CSI2_lane_mode
        g_LLA_IMX072_GlobalPLL_Settings_P0,                                                //p_GlobalPllModes
        p_IMX072_SensorModesArray_P0,                                          ///TODO: REUSING FROM P0 - p_SensorModeSetting Pointer to sensor mode
},
{
        //Settings for 9.6 MHz as Input Clock.
        EXT_SENSOR_CLOCK,                                                                  //u32_extClkFreqx100
        MAX_CSI_PLL_OUT_FREQ_IMX072_RAW10,                                                 //u32_CSI2LinkBitRate
        CAM_DRV_SENS_CSI2_LANE_MODE_1,                                                     //e_CSI2_lane_mode
        g_LLA_IMX072_GlobalPLL_Settings_P1,                                                //p_GlobalPllModes
        p_IMX072_SensorModesArray_P1,                                          ///TODO: REUSING FROM P0 - p_SensorModeSetting Pointer to sensor mode
},
};

/*Local Function dec*/
LOCAL uint16_t                       LLA_IMX072_ComputeImpliedMinimumFrameLength(uint16_t u16_OPYSize)TO_EXT_PRGM_MEM;
LOCAL uint16_t LLA_IMX072_ComputeImpliedMinimumLineLength
    (uint16_t u16_OPXSize, uint32 u32_MinReqLineLenPck) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetOutputModes()TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetDataFormatCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetAnGainCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetDiGainCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetFrameFormatCap ( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetFrameDimensionCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetOrientationPixelCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetExposureCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetCsiSignallingCap( void ) TO_EXT_PRGM_MEM;
LOCAL void                           LLA_IMX072_GetTestPatternCap( void ) TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_GetCameraCap(void) TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_GetShutterDetail (void) TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_GetNdfDetail (void) TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_GetNVMDetail(void)TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_GetApertureDetail (void) TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_GetFlashDetail(void) TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_ApplyPLLSettings(void)TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E  LLA_IMX072_ApplyGlobalPLLSettings(void)TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E LLA_IMX072_WriteSettingsArray
    ( const CAM_DRV_CAMERA_CONFIGS_T *p_I2CSettingsArray ) TO_EXT_PRGM_MEM;

LOCAL uint8_t                        LLA_IMX072_GetRegLen (uint16_t regAddr)TO_EXT_PRGM_MEM;
LOCAL void                            LLA_IMX072_SetRegLen (uint16_t u16_RegAddr, uint16_t u16_RegVal)TO_EXT_PRGM_MEM;
LOCAL CAM_DRV_RETVAL_E   LLA_IMX072_ParseEmbeddedDataLine(const uint8_t   *p_ancillary_lines)TO_EXT_PRGM_MEM;
LOCAL void      LLA_IMX072_UpdateIdString (uint8_t *u8_ModelInfo, uint8_t *IdString);

/* LLA_SensorInitializeCallbackFn:
This function will initilize passed function pointer with sensor specific functions
*/
void
LLA_IMX072_InitializeCallbackFn(
CAM_DRV_SENSOR_SPECIFIC_FN_INIT_ts   *p_CallbackFn)
{
    p_CallbackFn->FnApplyMiscSettings = LLA_IMX072_ApplyMiscSettings;
    p_CallbackFn->FnSensorInitSequence = LLA_IMX072_SensorInitSequence;
    p_CallbackFn->FnModulePowerOn = LLA_IMX072_ModulePowerOn;
    p_CallbackFn->FnModulePowerOff = LLA_IMX072_ModulePowerOff;
    p_CallbackFn->FnStartStopSensor = LLA_IMX072_StartStopSensor;
    p_CallbackFn->FnGetCameraTypeDetail = LLA_IMX072_GetCameraTypeDetail;
    p_CallbackFn->FnGetCameraDetail = LLA_IMX072_GetCameraDetail;
    p_CallbackFn->FnInterpretSensorSettings = LLA_IMX072_InterpretSensorSettings;
    p_CallbackFn->FnComputeFrameTimeForExposure = LLA_IMX072_ComputeFrameTimeForExposure;
    p_CallbackFn->FnUpdateTestPicture = LLA_IMX072_UpdateTestPicture;
    p_CallbackFn->FnUpdateFlash = LLA_IMX072_UpdateFlash;
    p_CallbackFn->FnTriggerFlash = LLA_IMX072_TriggerFlash;
    p_CallbackFn->FnUpdateFrameRate = LLA_IMX072_UpdateFrameRate;
    p_CallbackFn->FnUpdateGainAndExposure = LLA_IMX072_UpdateGainAndExposure;
    p_CallbackFn->FnUpdateFrameDimensionParameters = LLA_IMX072_UpdateFrameDimensionParameters;
    p_CallbackFn->FnUpdateFeedback = LLA_IMX072_UpdateFeedback;
    p_CallbackFn->FnGetCurrSensorConfig = LLA_IMX072_GetCurrSensorConfig;
    p_CallbackFn->FnApplyFrameRateSettings = LLA_IMX072_ApplyFrameRateSettings;
    p_CallbackFn->FnApplyFrameDimensionSettings = LLA_IMX072_ApplyFrameDimensionSettings;
    p_CallbackFn->FnApplyExposureAndGainSettings = LLA_IMX072_ApplyExposureAndGainSettings;
    p_CallbackFn->FnCheckModeConfigurations = LLA_IMX072_CheckModeConfigurations;
    p_CallbackFn->FnGroupHoldRegSet = LLA_IMX072_GroupHoldRegSet;
    p_CallbackFn->FnGroupHoldRegReset = LLA_IMX072_GroupHoldRegReset;
    return;
}


/*LLA_IMX072_WriteSettingsArray
Write settings array passed as argument
*/
CAM_DRV_RETVAL_E
LLA_IMX072_WriteSettingsArray(
const CAM_DRV_CAMERA_CONFIGS_T  *p_I2CSettingsArray)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    int                 i = 0;
    uint16_t            regAddress;
    uint8_t             regVal;

    while
    (
        !((p_I2CSettingsArray[i].reg_addr_hi == 0)
        &&  (p_I2CSettingsArray[i].reg_addr_lo == 0)
        &&  (p_I2CSettingsArray[i].reg_value == 0)
        )
    )
    {
        regAddress = (p_I2CSettingsArray[i].reg_addr_hi << 8) | (p_I2CSettingsArray[i].reg_addr_lo);
        regVal = p_I2CSettingsArray[i].reg_value;

        if (CAM_DRV_FN_OK != LLA_WRITE_I2C_BYTES_16BitRegSize(regAddress, 1, &(regVal)))
        {
            retVal = CAM_DRV_ERROR_HW;
            break;
        }


        /*increment to access next entry*/
        i++;
    }


    return (retVal);
}


/* LLA_IMX072_SensorPowerOnSequence:
This function will perform IMX072 specific power on sequence. e.g. setting up PLL stable wait time
and some other intial setings
*/
CAM_DRV_RETVAL_E
LLA_IMX072_SensorInitSequence(void)
{
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;

    retVal = LLA_IMX072_WriteSettingsArray(g_LLA_IMX072_PowerOnSettings);

    retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) LLA_IMX072_ApplyGlobalPLLSettings());

    if (CAM_DRV_OK != retVal)
    {
#if LLA_ASSERT_LOG_EN
        /*error has occured, assert in debug mode*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_imx072_SensorInitSequence - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    return (CAM_DRV_OK);
}


/*LLA_IMX072_CheckModeConfigurations
Write settings array passed as argument
*/
CAM_DRV_RETVAL_E
LLA_IMX072_CheckModeConfigurations(
const CAM_DRV_CONFIG_ON_T   *p_config_on)
{
    uint8 u8_iter = 0;

    for
    (
        u8_iter = 0;
        u8_iter < (sizeof(g_IMX072_SensorModeSettings) / sizeof(g_IMX072_SensorModeSettings[0]));
        u8_iter++
    )
    {
        if
        (
           (g_IMX072_SensorModeSettings[u8_iter].u32_extClkFreqx100 == p_config_on->ext_clock_freq_x100)
           && (g_IMX072_SensorModeSettings[u8_iter].u32_CSI2LinkBitRate >= p_config_on->requested_link_bit_rate_mbps)
           &&  (g_IMX072_SensorModeSettings[u8_iter].e_CSI2_lane_select == p_config_on->image_data_interface_settings.CSI2_lane_mode)
          )
          {
            // Select the current Iteration as the profile requested
                g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected = u8_iter;

#if LLA_ASSERT_LOG_EN
            OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_CheckModeConfigurations: Profile Selected: %d ", g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected);
#endif
            return (CAM_DRV_OK);
          }
    }


    //If no PLL settings matches, then return error. Else code flow wont reach here.
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_CheckModeConfigurations - CAM_DRV_ERROR_PARAM!!");
#endif
    return (CAM_DRV_ERROR_PARAM);
}

/*LLA_IMX072_GroupHoldRegSet
Group Hold reg set
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GroupHoldRegSet(void)
{
 CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;
 uint8_t          u8_Value;

 //AcquireGrpHldReg
  u8_Value = 1;
  retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__GROUPED_PARAMETER_HOLD, 1, &u8_Value));
  return (CAM_DRV_OK);
}


/*LLA_IMX072_GroupHoldRegReset
Group Hold reg reset
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GroupHoldRegReset()
{
 CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;
 uint8_t          u8_Value;

 //ReleaseGrpHldReg
  u8_Value = 0;
  retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__GROUPED_PARAMETER_HOLD, 1, &u8_Value));
  return (CAM_DRV_OK);
}

CAM_DRV_RETVAL_E
LLA_IMX072_ApplyGlobalPLLSettings(void)
{
    /*Write Global array on sensor*/
    const CAM_DRV_CAMERA_CONFIGS_T  *SensorModeSelectedConf = NULL;

    /*Write mode specific array on sensor if there is such constraint, else make Null Arrays*/
    SensorModeSelectedConf = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_GlobalPllModes;

    if (CAM_DRV_OK != LLA_IMX072_WriteSettingsArray(SensorModeSelectedConf))
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_ApplyGlobalPLLSettings - CAM_DRV_ERROR_HW!!");
#endif

        /*error has occured, asser in debug mode*/
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    return (CAM_DRV_OK);
}


CAM_DRV_RETVAL_E
LLA_IMX072_ApplyPLLSettings(void)
{
    const CAM_DRV_CAMERA_CONFIGS_T  *SensorModeSelectedConf = NULL;

    /*Write mode specific array on sensor if there is such constraint, else make Null Arrays*/
    SensorModeSelectedConf = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->p_PllModes;

    if (CAM_DRV_OK != LLA_IMX072_WriteSettingsArray(SensorModeSelectedConf))
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_ApplyPLLSettings - CAM_DRV_ERROR_HW!!");
#endif

        /*error has occured, asser in debug mode*/
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    return (CAM_DRV_OK);
}


/*Exports the output modes supported by IMX072 sensor*/
void
LLA_IMX072_GetOutputModes(void)
{
    float_t     f_PixleClkFreq;
    uint8_t     index,
                u8_OutputModes,
                u8_SubSampledFactor;
    uint16_t    u16_CsiRawFormat;
    uint16_t    u16_LineLen = 0,
                u16_FrameLen = 0;

    switch (g_IMX072_CamGlbConfig.camDrvMiscDetails.csiRawFormat)
    {
        case CAM_DRV_SENS_FORMAT_RAW10:
            u16_CsiRawFormat = 0x0A0A;
            break;

        case CAM_DRV_SENS_FORMAT_RAW8:
            u16_CsiRawFormat = 0x0808;
            break;

        case CAM_DRV_SENS_FORMAT_RAW8_DPCM:
            u16_CsiRawFormat = 0x0A08;
            break;

        default:
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_GetOutputModes - CAM_DRV_ERROR_PARAM!!");
#endif
            LLA_ASSERT_XP70();
            break;
    }

    /* Number of supported output modes */
    u8_OutputModes = sizeof(g_LLA_IMX072_OutputModes)/sizeof(CAM_DRV_SENS_OUTPUT_MODE_T);
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.number_of_modes  =
        u8_OutputModes;

    /* populate output modes */
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes =
        (CAM_DRV_SENS_OUTPUT_MODE_T*)&g_LLA_IMX072_OutputModes;

    /* Update the max frame rate for each mode */
    for(index = 0; index < u8_OutputModes; index++)
    {
        /* Calculate sub-sampling factor required for this mode */
        /*Assuming here that sampling factor in H and V direction will be same for each mode*/
        u8_SubSampledFactor = g_LLA_IMX072_OutputModes[index].woi_res.width /
            g_LLA_IMX072_OutputModes[index].output_res.width;

        /* Calculate line length for this output mode */
        /* [CR - 445315] Since the "g_IMX072_MinLineBlankingPck" array is removed
           from the code, we will be using mode specific value of minimum line blanking
           pck which is defined in the mode specific structure "SensorModeSettings_ts".
        */
        u16_LineLen = g_LLA_IMX072_OutputModes[index].output_res.width +
                      GenericFunctions_CeilingOfMByN(LLA_SOL_SYNC_CODE_WIDTH_BITS, (uint8_t)u16_CsiRawFormat) +
                      GenericFunctions_CeilingOfMByN(LLA_EOL_SYNC_CODE_WIDTH_BITS, (uint8_t)u16_CsiRawFormat) +
                      g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[index]->u16_MinLineBlankingPck;
        u16_LineLen = max(u16_LineLen, IMX072_MIN_LINE_LEN_PCK);

        /* Calculate frame len for this mode */
        /* [CR - 445315] Since "g_IMX072_MinFrameBlankingLine" array and
           LLA_IMX072_GetExtraLines function are removed from the code,
           we will be using mode specific value of minimum frame blanking lines
           and extra lines which are defined in the mode specific structure
           "SensorModeSettings_ts".
        */
        u16_FrameLen = g_LLA_IMX072_OutputModes[index].output_res.height +
                       g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[index]->u16_MinFrameBlankingLines +
                       g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[index]->u16_ExtraLines;
        u16_FrameLen = max(u16_FrameLen,
                           g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines);

       /* Take the VTPixelClk for the given mode to calculate the max fps value */
       f_PixleClkFreq =
            g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[index]->f_VTPixelClockFrequency_Mhz;

        /* Calculate max fps possible */
        g_LLA_IMX072_OutputModes[index].max_frame_rate_x100 =
            (
                (float_t) (f_PixleClkFreq * 1000000.0) /
                (u16_LineLen * u16_FrameLen)
            ) *
            100;
    }

    return;
}


/*IMX072 module power is supplied using a another CHIP, PMIC
so need to programmed PMIC*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_ModulePowerOn(
const CAM_DRV_CONFIG_ON_T   *p_config_on)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    //retVal |= g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_read_fn(0x28,0x2,LLA_I2C_REG_ADDRESS_SIZE,1,LLA_I2C_BYTE_ORDER,&u8_Value);
#if IMX072_ENABLE_PMIC_WRITES
#ifndef U5500_PLATFORM_PORTING
    uint8_t u8_Value;
    u8_Value = 0x8;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
        0x9E,
        0x1,
        CAM_DRV_I2C_REG_SIZE_8BIT,
        1,
        LLA_I2C_BYTE_ORDER,
        &u8_Value);

    u8_Value = 0xC8;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x2,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x0E;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x3,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x4,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x5,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x6,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x01;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x0,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    LLA_blocking_delay(1000000);

    u8_Value = 0x09;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x0,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    LLA_blocking_delay(1000000);

    u8_Value = 0x0B;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x0,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    LLA_blocking_delay(1000000);

    u8_Value = 0x0F;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x0,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    LLA_blocking_delay(1000000);
#else //U5500_PLATFORM_PORTING
    u8_Value = 0x8;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x1,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    u8_Value = 0xC8;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x2,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    u8_Value = 0x0E;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x3,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    u8_Value = 0x00;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x4,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    u8_Value = 0x00;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x5,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);


    u8_Value = 0x00;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x6,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    u8_Value = 0x01;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x0,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    LLA_blocking_delay(1000000);

    u8_Value = 0x09;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x0,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    LLA_blocking_delay(1000000);

    u8_Value = 0x0B;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x0,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    LLA_blocking_delay(1000000);

    u8_Value = 0x0F;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(LLA_I2C_DEVICE_ADDRESS_PMIC,0x0,CAM_DRV_I2C_REG_SIZE_8BIT,1,LLA_I2C_BYTE_ORDER,&u8_Value);

    LLA_blocking_delay(1000000);

#endif //U5500_PLATFORM_PORTING

#endif

//sudeep

/* populate sensor information */
        switch (p_config_on->image_data_interface_settings.CSI2_lane_mode)
        {
        case CAM_DRV_SENS_CSI2_LANE_MODE_1: /**< Use 1 lane */
            g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_NoCSILane = 1;
            break;

        case CAM_DRV_SENS_CSI2_LANE_MODE_2: /**< Use 2 lane */
            g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_NoCSILane = 2;
            break;

        case CAM_DRV_SENS_CSI2_LANE_MODE_3: /**< Use 3 lane */
            g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_NoCSILane = 3;
            break;

        case CAM_DRV_SENS_CSI2_LANE_MODE_4: /**< Use 4 lane */
            g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_NoCSILane = 4;
            break;

            default:
#if LLA_ASSERT_LOG_EN
                OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_on - CAM_DRV_ERROR_PARAM!!");
#endif
                LLA_ASSERT_XP70();
            return (CAM_DRV_FN_FAIL);
        }


    /*Save required information from 'p_config_on*/
    g_IMX072_CamGlbConfig.camDrvMiscDetails.u16_CCP2SpeedMaxMbps = p_config_on->image_data_interface_settings.CCP2_speed_max_Mbps;
    g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ChannelIdentifier = p_config_on->image_data_interface_settings.channel_identifier;
    g_IMX072_CamGlbConfig.camDrvMiscDetails.selectedCamera = p_config_on->selected_cam;

    /*Power on the selected camera*/

    /*
       All power-up delays should be as mentioned in Power on_off_DMS_01061702.docx document
       present in attachement secion of FIDO ER # 366658
       FIDO ER # 366658 : Camera power up/down sequence is wrong
    */

    /*Raise Analog and digital Voltages*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn(
        p_config_on->selected_cam,
        CAM_DRV_PHYSICAL_PIN_VOLTAGES,
        CAM_DRV_PIN_ACTION_ACTIVATE);
    LLA_blocking_delay(1);

    /*Raise the XSHUTDOWN signal*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn(
        p_config_on->selected_cam,
        CAM_DRV_PHYSICAL_PIN_XSHUTDOWN,
        CAM_DRV_PIN_ACTION_ACTIVATE);
    LLA_blocking_delay(15);

    /*Raise the ExtClk Signal signal*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn(
        p_config_on->selected_cam,
        CAM_DRV_PHYSICAL_PIN_CLOCK,
        CAM_DRV_PIN_ACTION_ACTIVATE);
    LLA_blocking_delay(2);

    /*Need to wait before first IIC transaction can be done*/

    /*Second argument is number of EXTCLK cycle to wait for*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_timer_start_fn(CAM_DRV_OS_TIMER_GENERAL, XshutdownDelay_extclk_start);

    return (retVal);
}

/*IMX072 module specific power On*/

/*It would be the responsibility of ARM side SW to control the PMIC reset*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_ModulePowerOff(void)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

#if IMX072_ENABLE_PMIC_WRITES
    uint8_t u8_Value;
    /*Disable all power output*/
    u8_Value = 0x00;
    retVal = g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
        0x9E,
        0x0,
        CAM_DRV_I2C_REG_SIZE_8BIT,
        1,
        LLA_I2C_BYTE_ORDER,
        &u8_Value);

    LLA_blocking_delay(1000000);

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x1,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x2,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x3,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x4,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x5,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );

    u8_Value = 0x00;
    retVal = (CAM_DRV_RETVAL_E)
        (
            (uint8_t) retVal | (uint8_t) g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn(
                0x9E,
                0x6,
                CAM_DRV_I2C_REG_SIZE_8BIT,
                1,
                LLA_I2C_BYTE_ORDER,
                &u8_Value)
        );
#endif

    /*
       All power-up delays should be as mentioned in Power on_off_DMS_01061702.docx document
       present in attachement secion of FIDO ER # 366658
       FIDO ER # 366658 : Camera power up/down sequence is wrong
    */

    /*Deactivate the ExtClk Signal signal*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn(
        g_IMX072_CamGlbConfig.camDrvMiscDetails.selectedCamera,
        CAM_DRV_PHYSICAL_PIN_CLOCK,
        CAM_DRV_PIN_ACTION_DEACTIVATE);

    /*Deactivate the XSHUTDOWN signal*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn(
        g_IMX072_CamGlbConfig.camDrvMiscDetails.selectedCamera,
        CAM_DRV_PHYSICAL_PIN_XSHUTDOWN,
        CAM_DRV_PIN_ACTION_DEACTIVATE);
    LLA_blocking_delay(1);

    /*Deactivate Analog and digital Voltages*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn(
        g_IMX072_CamGlbConfig.camDrvMiscDetails.selectedCamera,
        CAM_DRV_PHYSICAL_PIN_VOLTAGES,
        CAM_DRV_PIN_ACTION_DEACTIVATE);

    return (retVal);
}


uint8_t
LLA_IMX072_GetRegLen(
uint16_t    regAddr)
{
    uint8_t regLen = 0;

    switch (regAddr)
    {
        case SENSOR_STATUS__FRAME_COUNT:                        /*Frame count is a single byte reg*/regLen = 1; break;
        case SENSOR_INTEGRATION__FINE_INTEGRATION_TIME_HI:      /*fine integration is 2 byte byte reg*/regLen = 2; break;
        case SENSOR_INTEGRATION__COARSE_INTEGRATION_TIME_HI:    /*fine integration is 2 byte byte reg*/regLen = 2; break;
        case SENSOR_INTEGRATION__ANALOGUE_GAIN_CODE_GLOBAL_HI:  /*analogue gain  is 2 byte byte reg*/regLen = 2; break;
        case SENSOR_VIDEO_TIMING__FRAME_LENGTH_LINES_HI:        /*frame length  is 2 byte byte reg*/regLen = 2; break;
        case SENSOR_VIDEO_TIMING__LINE_LENGTH_PCK_HI:           /*frame line len pck is 2 byte byte reg*/regLen = 2; break;
        default:                                                regLen = 0; break;
    }


    return (regLen);
}


void
LLA_IMX072_SetRegLen(
uint16_t    u16_RegAddr,
uint16_t    u16_RegVal)
{
    switch (u16_RegAddr)
    {
        case SENSOR_STATUS__FRAME_COUNT:
            g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.frameCount = u16_RegVal;
            break;

        case SENSOR_INTEGRATION__FINE_INTEGRATION_TIME_HI:
            g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.fineIntgPixel = u16_RegVal;
            break;

        case SENSOR_INTEGRATION__COARSE_INTEGRATION_TIME_HI:
            /*
            In IMX072, while streaming, if exposure is changed, ISL reflect updated exposure immediately in next frame
            but actual application happen on next to next frame i.e. there is one frame lag actual absorption and ISL reporting.
            Analog gain is in syncronization with ISL register and acutual application.
            It is not preferred to snoop ISL for IMX072 sensor due to the above contraints and hence timed-model should be used.
            */
                g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.coarseIntgLines = u16_RegVal;
            break;

        case SENSOR_INTEGRATION__ANALOGUE_GAIN_CODE_GLOBAL_HI:
            g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.analogGainCode = u16_RegVal;
            break;

        case SENSOR_VIDEO_TIMING__FRAME_LENGTH_LINES_HI:
            g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.frameLenLines = u16_RegVal;
            break;

        case SENSOR_VIDEO_TIMING__LINE_LENGTH_PCK_HI:
            g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.frameLineLenPck = u16_RegVal;
            break;

        default:
            break;
    }


    return;
}


CAM_DRV_RETVAL_E
LLA_IMX072_ParseEmbeddedDataLine(
const uint8_t   *p_ancillary_lines)
{
    uint8_t         *ptr = ( uint8_t * ) p_ancillary_lines;
    uint8_t         u8_byte,
                    u8_ValMsb,
                    u8_ValLsb,
                    u8_RegLen;
    static uint8_t  u8_RegMsb = 0,
                    u8_RegLsb = 0;
    uint16_t        u16_RegVal,
                    u16_RegAddr;
    Flag_te         regValid;
    uint8_t         count = 0;

    /*It is assumed that, client will always passed EDL in RAW8 format even if output dataformat is selected to be RAW10*/
    if (LLA_EDL_FORMAT_CODE != *ptr)
    {
        /*Not a embedded data line*/
        return (CAM_DRV_ERROR_PARAM);
    }


    INC_PTR_COUNT();                        //ptr++;

    /*Loop till emd of embedded data*/

    //while (!(LLA_EDL_END_DATA == *ptr && LLA_EDL_END_DATA == *(ptr+1)))
    while (!(LLA_EDL_END_DATA == *ptr))
    {
        u8_byte = u8_ValMsb = u8_ValLsb = 0;
        u8_byte = *ptr;
        u16_RegVal = 0, u16_RegAddr = 0, u8_RegLen = 0;
        regValid = Flag_e_FALSE;

        switch (u8_byte)
        {
            case LLA_EDL_CCI_MSB_TAG:
                {
                    /*Start of new register*/
                    INC_PTR_COUNT();        //ptr++;
                    u8_RegMsb = *ptr;

                    /*next byte will be 0xA5*/
                    INC_PTR_COUNT();        //ptr++;

                    /*next byte will be RegLsb*/
                    INC_PTR_COUNT();        //ptr++;
                    u8_RegLsb = *ptr;

                    u16_RegAddr = (u8_RegMsb << 8 | u8_RegLsb);
                    u8_RegLen = LLA_IMX072_GetRegLen(u16_RegAddr);

                    /*next byte will be 0x5A*/
                    INC_PTR_COUNT();        //ptr++;
                    if (1 == u8_RegLen)
                    {
                        /*next byte will be regVal_Msb*/
                        INC_PTR_COUNT();    //ptr++;
                        u8_ValMsb = *ptr;

                        u16_RegVal = u8_ValMsb;
                        u8_RegLsb += 1;

                        regValid = Flag_e_TRUE;
                    }
                    else                    /*2 byte register*/
                    {
                        /*next byte will be regVal_Msb*/
                        INC_PTR_COUNT();    //ptr++;
                        u8_ValMsb = *ptr;

                        /*read the another byte*/

                        /*next byte will be 0x5A*/
                        INC_PTR_COUNT();    //ptr++;

                        /*next byte will be regVal_Lsb*/
                        INC_PTR_COUNT();    //ptr++;
                        u8_ValLsb = *ptr;

                        u16_RegVal = (u8_ValMsb << 8) | u8_ValLsb;
                        u8_RegLsb += 2;
                        regValid = Flag_e_TRUE;
                    }
                }


                break;

            case LLA_EDL_INC_IDX_INVLD_TAG:
                {
                    /*increment the lsb address*/
                    u8_RegLsb++;

                    /*ignore the next byte*/
                    INC_PTR_COUNT();        //ptr++;
                }


                break;

            case LLA_EDL_INC_IDX_VLD_TAG:
                {
                    /*increment the lsb address*/

                    //u8_RegLsb++;
                    u16_RegAddr = (u8_RegMsb << 8 | u8_RegLsb);
                    u8_RegLen = LLA_IMX072_GetRegLen(u16_RegAddr);

                    if (1 == u8_RegLen)
                    {
                        /*next byte is valid register value*/
                        INC_PTR_COUNT();    //ptr++;
                        u8_ValMsb = *ptr;

                        u16_RegVal = u8_ValMsb;
                        u8_RegLsb += 1;
                        regValid = Flag_e_TRUE;
                    }
                    else
                    {
                        /*next byte will be regVal_Msb*/
                        INC_PTR_COUNT();    //ptr++;
                        u8_ValMsb = *ptr;

                        /*read the another byte*/

                        /*next byte will be 0x5A*/
                        INC_PTR_COUNT();    //ptr++;

                        /*next byte will be regVal_Lsb*/
                        INC_PTR_COUNT();    //ptr++;
                        u8_ValLsb = *ptr;

                        u16_RegVal = (u8_ValMsb << 8) | u8_ValLsb;
                        u8_RegLsb += 2;
                        regValid = Flag_e_TRUE;
                    }
                }


                break;

            default:
                break;
        }


        if (Flag_e_TRUE == regValid)
        {
            LLA_IMX072_SetRegLen(u16_RegAddr, u16_RegVal);
        }


        INC_PTR_COUNT();                    //ptr++;
    }


    return (CAM_DRV_OK);
}


void
LLA_IMX072_InterpretSensorSettingsUsingTimeModel(
CAM_DRV_SENS_SETTINGS_T *p_sensor_settings)
{
uint32_t frame_count = 0;



/*In this function we are sending values back to the high level API to check whether values of exposure and gain is applied or not.
   There are three cases
   1. For the first frame -> In this case, value to be send is the last configuration which is done before the start of streaming in the
       non trial mode.
   2. For second frame onwards (lets say N th frame)
       A. If the update request is received at N-2 frame -> In this case value to be send will be the feedback of N -2 frame which we have stored in
           the "FrameAppliedConfig" structure and will update the FrameCurrentConfig structure with the value of FrameAppliedConfig.
       B. If no update request is received at N-2 frame then we will send the current configuration value (means the previous configured
           value) present in FrameCurrentConfig structure.
*/
    if((g_TimeModelConfig.u32_FSCCount == 1))
    {
        p_sensor_settings->exposure_time_us = g_TimeModelConfig.FrameCurrentConfig.exposure_time_us;
        p_sensor_settings->analog_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000;
        p_sensor_settings->digital_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000;
    }
    else
    {
       // Iterate over all the stored request to find a match
       while (frame_count  < MAX_FRAME_COUNT)
      {
            if((g_TimeModelConfig.u32_FSCCount - LLA_DELAY_MODEL_FRAME_COUNT) == g_TimeModelConfig.u32_FrameFSC_ID[frame_count])
            {
                p_sensor_settings->exposure_time_us = g_TimeModelConfig.FrameAppliedConfig[frame_count].exposure_time_us;
                p_sensor_settings->analog_gain_x1000 = g_TimeModelConfig.FrameAppliedConfig[frame_count].analog_gain_x1000;
                p_sensor_settings->digital_gain_x1000 = g_TimeModelConfig.FrameAppliedConfig[frame_count].digital_gain_x1000;

                g_TimeModelConfig.FrameCurrentConfig.exposure_time_us = g_TimeModelConfig.FrameAppliedConfig[frame_count].exposure_time_us;
                g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000= g_TimeModelConfig.FrameAppliedConfig[frame_count].analog_gain_x1000;
                g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000 = g_TimeModelConfig.FrameAppliedConfig[frame_count].digital_gain_x1000;
                break;
           }
            else
            {
                frame_count ++;
            }
        }
        // If no match is found then no  request is pending
        if (MAX_FRAME_COUNT == frame_count)
        {
            p_sensor_settings->exposure_time_us = g_TimeModelConfig.FrameCurrentConfig.exposure_time_us;
            p_sensor_settings->analog_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000;
            p_sensor_settings->digital_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000;
        }

    }

#if 0 //This is for testing purpose to compare the values of ISL and the values that will be send if delay model is on
//*******************************************************************TESTING DM*****************************
    CAM_DRV_SENS_SETTINGS_T test_sensor_settings;
    if((g_TimeModelConfig.u32_FSCCount == 1))
    {
        test_sensor_settings.exposure_time_us = g_TimeModelConfig.FrameCurrentConfig.exposure_time_us;
        test_sensor_settings.analog_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000;
        test_sensor_settings.digital_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000;
    }
    else
    {
        if((g_TimeModelConfig.u32_FSCCount - LLA_DELAY_MODEL_FRAME_COUNT) == g_TimeModelConfig.u32_FrameFSC_ID)
        {
            test_sensor_settings.exposure_time_us = g_TimeModelConfig.FrameAppliedConfig.exposure_time_us;
            test_sensor_settings.analog_gain_x1000 = g_TimeModelConfig.FrameAppliedConfig.analog_gain_x1000;
            test_sensor_settings.digital_gain_x1000 = g_TimeModelConfig.FrameAppliedConfig.digital_gain_x1000;

            g_TimeModelConfig.FrameCurrentConfig.exposure_time_us = g_TimeModelConfig.FrameAppliedConfig.exposure_time_us;
            g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000= g_TimeModelConfig.FrameAppliedConfig.analog_gain_x1000;
            g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000;
        }
        else
        {
            test_sensor_settings.exposure_time_us = g_TimeModelConfig.FrameCurrentConfig.exposure_time_us;
            test_sensor_settings.analog_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000;
            test_sensor_settings.digital_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000;
        }

    }
    OstTraceInt4(TRACE_DEBUG, ">>>DM::Interprettest ET %d AG %d ID %d (%d)", test_sensor_settings.exposure_time_us, test_sensor_settings.analog_gain_x1000, g_TimeModelConfig.u32_FrameFSC_ID, g_TimeModelConfig.u32_FSCCount);

//*************************************************************TESTING DM END*********************************************
#endif
}


uint8
LLA_IMX072_InterpretSensorSettingsUsingISLData(
const uint8             *p_ancillary_lines,
CAM_DRV_SENS_SETTINGS_T *p_sensor_settings)
{
      /*Parse embedded data lines*/
    LLA_IMX072_ParseEmbeddedDataLine(p_ancillary_lines);

    /*Registers from embedded data lines has already been parsed*/
   if (g_u8GetRawNVMData == Flag_e_TRUE)
    {
      p_sensor_settings->analog_gain_x1000 = (uint16_t)
          (
              (
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                    g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.analogGainCode +
                  g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
              ) * 1000 /
                  (
                      g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                      g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.analogGainCode +
                      g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                    ) * (float_t) (g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.digitalGainGreenR) / 256
          );
    }
   else
    {
      p_sensor_settings->analog_gain_x1000 = (uint16_t)
           (
               (
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                    g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.analogGainCode +
                   g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
               ) * 1000 /
                   (
                       g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                       g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.analogGainCode +
                       g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                   )
           );
    }

    p_sensor_settings->digital_gain_x1000 = LLA_DEF_GAIN_x1000;
    p_sensor_settings->exposure_time_us = (uint32_t)((float_t)(g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.frameLineLenPck /
                    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].
                        p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->
                            f_VTPixelClockFrequency_Mhz
                ) * (g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.coarseIntgLines - IMX072_EXP_OFFSET_WO_VA)
     );

    p_sensor_settings->ms_used = FALSE;

    if(g_IMX072_CamGlbConfig.camDrvMiscDetails.g_CamISLReg.coarseIntgLines == g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrCoarseIntegrationLines)
    {
        return (Flag_e_TRUE);
    }
  else
   {
        return (Flag_e_FALSE);
   }
}

uint8
LLA_IMX072_InterpretSensorSettings(
const uint8             *p_ancillary_lines,
CAM_DRV_SENS_SETTINGS_T *p_sensor_settings)
{
    uint8 u8_RetVal = Flag_e_FALSE;

    if(Flag_e_FALSE == LLA_IS_NON_SMIA)
    {
        u8_RetVal = LLA_IMX072_InterpretSensorSettingsUsingISLData(p_ancillary_lines, p_sensor_settings);
    }
    else
    {
        LLA_IMX072_InterpretSensorSettingsUsingTimeModel(p_sensor_settings);
    }
    return (u8_RetVal);
}
/*
  LLA_IMX072_StartStopSensor
*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_StartStopSensor(
uint8_t u8_Value)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    retVal = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__MODE_SELECT, 1, &u8_Value);

    if (u8_Value)
    {
        /*IMX072 datasheet, need to wait for PLL lockup time > 200us, so adding a tight loop delay for */
        LLA_blocking_delay(250);
    }

    return (retVal);
}


/* LLA_IMX072_GetCameraTypeDetail:
Read sensor detail like ModelId, ManufId, and others from Sensor directly and populate these information
into passed structure
*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_GetCameraTypeDetail(
CAM_DRV_CAMERA_TYPE_T   *p_camera_type)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    uint8_t             u8_ModelInfo[5],
                        count;

    /*Read model ID information from Sensor*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_STATUS__MODEL_ID_HI, /*Model ID*/
        5,                          /*Read 5bytes in Big-endian order*/
        ( uint8_t * ) &u8_ModelInfo);

    if (CAM_DRV_FN_OK != retVal)
    {
        /*error occured in reading*/
        return (retVal);
    }


    /*update modelId at global struct*/
    LLA_IMX072_UpdateIdString(u8_ModelInfo, ( uint8_t * ) p_camera_type->id_string);

    // Fill unique serial no if module/camera has any
    for (count = 0; count < CAM_DRV_NUMBER_OF_SERIAL_NUMBERS; count++)
    {
        p_camera_type->camera_serial_number[count] = '\0';
    }

    p_camera_type->camera_register_map_version = u8_ModelInfo[4];

    // [NON_ISL_SUPPORT] - Override sensor version information from here for NON SMIA Sensors
    if(Flag_e_TRUE == LLA_IS_NON_SMIA)
    {
        p_camera_type->camera_register_map_version = SENSOR_VERSION;
    }

    p_camera_type->camera_model = CAM_DRV_CAMERA_MODEL_UNKNOWN;
    return (retVal);
}


/* LLA_IMX072_GetCameraCap
This function will get all sensor constraints like Frame dimension, Video timing and other sensor
capabilities to report back to client
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetCameraCap(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    /*Get sensor specific parameters like constraints, capabilities in global structures*/

    /*Get DataFormat capability of sensor*/
    LLA_IMX072_GetDataFormatCap();

    /*Get Analog gain capability of sensor*/
    LLA_IMX072_GetAnGainCap();

    /*Get Digital gain capability of sensor*/
    LLA_IMX072_GetDiGainCap();

    /*Read the frame format capability of sensor*/
    LLA_IMX072_GetFrameFormatCap();

    /*Read frame dimension(FD) constraints from sensor*/
    LLA_IMX072_GetFrameDimensionCap ();

    /*Read Orientation capability pixel order of the sensor*/
    LLA_IMX072_GetOrientationPixelCap();

    /*Read exposure capability of sensor*/
    LLA_IMX072_GetExposureCap();

    /*Read CSI signalling capabilities*/
    LLA_IMX072_GetCsiSignallingCap();

    /*Test pattern capability of sensor*/
    LLA_IMX072_GetTestPatternCap();

    /*Update the output modes supported by the sensor - it do change depending on speed*/
    LLA_IMX072_GetOutputModes();

    /*Get sensor specific parameters like constraints, capabilitis in global structures*/
    //  retVal = LLA_IMX072_GetCameraDetail();

        /*TODO:<CN>: What should be the proper values for these delays*/

        /*Value 2 is set for time being as specifed in  data sheet*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.delays.start_vf = 2;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.delays.capture_rolling = 2;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.delays.capture_global = 2;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.delays.config_woi = 2;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.delays.config_af = 2;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.delays.config_other = 2;

        /*TODO:<CN>:IMX072 supports both CPP modes, returning CLOCK*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.ccp_mode = CAM_DRV_SENS_CCP_MODE_CLOCK;

        /*TODO:<CN>:AWB is done in PIPE so setting it to 'Not Required'*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.awb_requirements = CAM_DRV_AWB_REQUIRED_NOT;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.zoom_roi_requirements = CAM_DRV_ZOOM_ROI_REQUIRED_NOT;

    return (retVal);
}


/*LLA_IMX072_GetShutterDetail:
Update the shutter specific details, for selected camera, to be passed on to client of low level driver
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetShutterDetail(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    /*Need to extend this for Module supporting shutter*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camShutterDetail.ms_continuous_capture_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camShutterDetail.ms_single_capture_supported = 0;

    return (retVal);
}


/*LLA_IMX072_GetNdfDetail:
Update the ND Filter specific details, for selected camera, to be passed on to client of low level driver
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetNdfDetail(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    /*Currently supported sensors/modules do not have ND Filter*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camNDFilterDetail.nd_filter_transparency_x_100 = 0;

    return (retVal);
}


/*LLA_IMX072_GetNVMDetail:
Update the NVM specific details, for selected camera, to be passed on to client of low level driver
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetNVMDetail(void)
{
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK ;
    uint8_t             size = 0,
                        nvm_temp;
    if (Flag_e_TRUE == g_u8GetRawNVMData)     // We will be exporting RAW NVM DATA from here.
    {
      g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.size = LLA_IMX072_NVM_SIZE;
    }
    else      // We will be exporting PARSED NVM DATA from here.
    {
    size = 1;
        retVal = LLA_NVMMAP_Read(LLA_IMX072_ADDR_LSC_TEMP_COUNT, size, ( uint8_t * ) &(nvm_temp));

    g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size            = sizeof( CAM_DRV_NVM_T);

      if (LLA_IMX072_MODEL_ID_STW == g_u8SensorModelId)
       {
          // Hardcode no of sensitivity colour temperatures at which sensor is calibrated... <AG>
         nvm_temp = LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED;
       }


    // This is just to check against a ill-programmed NVM
        if (LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED == nvm_temp || LLA_IMX072_MIN_COLOR_TEMP_CALIBRATED == nvm_temp)
    {
    // Add space for "uint8* p_ls_measured_lp_nums"
    if ( 0 != ( (sizeof(uint8_t) * nvm_temp) % 4) )
    {
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += (sizeof(uint8_t) * nvm_temp) +  (4-((sizeof(uint8_t) * nvm_temp)%4));
    }
    else
    {
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += (sizeof(uint8_t) * nvm_temp);
    }


    // Add space for "Tsmiapp_ls_table *p_ls_comTable"
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(CAM_DRV_NVM_LS_TABLE_T ) * nvm_temp;

    // Add space for "CAM_DRV_NVM_LSC_DATA_T *p_lsc_data"
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += nvm_temp * (sizeof(CAM_DRV_NVM_LSC_DATA_T) * ( LLA_IMX072_LSC_GRID_HORIZ_COUNT * LLA_IMX072_LSC_GRID_VERTI_COUNT));
    }
    else
    {
    // Invalid NVM data for Number of Color temperatures for Lens Shading Correction data.
    //
    // Add space for "uint8* p_ls_measured_lp_nums"
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size +=
                (
                    sizeof(uint8_t) *
                    LLA_IMX072_INVALID_COLOR_TEMP_CALIBRATED
                );

    // Add space for "Tsmiapp_ls_table *p_ls_comTable"
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(CAM_DRV_NVM_LS_TABLE_T ) * LLA_IMX072_INVALID_COLOR_TEMP_CALIBRATED;

    // Add space for "CAM_DRV_NVM_LSC_DATA_T *p_lsc_data"
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size +=
                    LLA_IMX072_INVALID_COLOR_TEMP_CALIBRATED *
                (sizeof(CAM_DRV_NVM_LSC_DATA_T) * (LLA_IMX072_LSC_GRID_HORIZ_COUNT * LLA_IMX072_LSC_GRID_VERTI_COUNT));
    }


    // Add space for "uint16 *p_positions_horizontal "
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(uint16_t) * LLA_IMX072_AF_DATA_POSN_COUNT;

    // Add space for "CAM_DRV_NVM_SENSITIVITY_DATA_T *p_sens"
    size =1;
        retVal = (CAM_DRV_RETVAL_E)
            (
                (uint8_t) retVal | (uint8_t) LLA_NVMMAP_Read(
                    LLA_IMX072_ADDR_WB_TEMP_COUNT,
                    size,
                    ( uint8_t * ) &(nvm_temp))
            );  // in how many colour temperatures values are measured
    if (LLA_IMX072_MODEL_ID_STW == g_u8SensorModelId)
     {
        // Hardcode no of sensitivity colour temperatures at which sensor is calibrated... <AG>
       nvm_temp = LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED;
     }


    // This is just to check against a ill-programmed NVM
        if ((LLA_IMX072_MIN_COLOR_TEMP_CALIBRATED == nvm_temp) || (LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED == nvm_temp))
    {
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(CAM_DRV_NVM_SENSITIVITY_DATA_T ) * nvm_temp;
    }
    else
    {
       // Invalid NVM data for Number of Color temperatures for White Balance.
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(CAM_DRV_NVM_SENSITIVITY_DATA_T ) * LLA_IMX072_INVALID_COLOR_TEMP_CALIBRATED;
    }


    // Add space for "CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T *p_couplet_type1_map"
    //Read number of Type 1 defects.
    size = 1 ;
        retVal = (CAM_DRV_RETVAL_E)
            (
                ((uint8_t) retVal) | ((uint8_t) LLA_NVMMAP_Read(
                    LLA_IMX072_ADDR_DEFECT_PIXEL_COUNT,
                    size,
                    ( uint8_t * ) &(nvm_temp)))
            );

    if ((LLA_IMX072_MIN_DEFECT_PIXEL_COUNT < (nvm_temp)) && ((LLA_IMX072_MAX_DEFECT_PIXEL_COUNT > (nvm_temp)) || (LLA_IMX072_MAX_DEFECT_PIXEL_COUNT == (nvm_temp))))
    {
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T ) * nvm_temp;
    }
    else
    {
       // Invalid NVM data for Number of defect pixels
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size           += sizeof(CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T ) * (LLA_IMX072_INVALID_DEFECT_PIXEL_COUNT);
    }


    if ( CAM_DRV_OK != retVal)
    {
       // An error occured while reading from NVM
       g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size =0;
    }
  }


    return (retVal);
}


/*LLA_IMX072_GetApertureDetail:
Update the aperture details, for selected camera, to be passed on to client of low level driver
*/
const uint16_t g_s16_f_number_x_100 = FNUMBER_IMX072;

CAM_DRV_RETVAL_E
LLA_IMX072_GetApertureDetail(void)
{
  CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

  g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvApertureDetail.number_of_supported_apertures = 1;
  g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvApertureDetail.p_f_number_x_100 = &g_s16_f_number_x_100;

  return (retVal);
}


/*LLA_IMX072_GetFlashDetail
Update the Flash details, for selected camera, to be passed on to client of low level driver
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetFlashDetail(void)
{
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK ;
#if LLA_FLASH_LED == 0
    float_t             f_PixleClkFreq,
                        f_SysFreq;
    uint8_t             i,
                        u8_OutChannels;
    uint16_t    u16_CsiRawFormat;
    u8_OutChannels = g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_NoCSILane;

    switch (g_CamDrvConfig.csiRawFormat)
    {
        case CAM_DRV_SENS_FORMAT_RAW10:     u16_CsiRawFormat = 0x0A0A; break;
        case CAM_DRV_SENS_FORMAT_RAW8:      u16_CsiRawFormat = 0x0808; break;
        case CAM_DRV_SENS_FORMAT_RAW8_DPCM: u16_CsiRawFormat = 0x0A08; break;
        default:                            /*No other modes are supported as of now*/LLA_ASSERT_XP70();
    }


    /*CK_CSI2 freq*/
    f_SysFreq = min(
        g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].u32_CSI2LinkBitRate,
        MAX_CSI_PLL_SDL_FREQ_IMX072_RAW10);

    /*CK_PIXEL freq
    Relation betwwn CK_CSI2 and CK_PIXEL
    CK_CSI2 = CK_PIXEL * BPP / Number of OutChannels
    */
    f_PixleClkFreq = (float_t) (f_SysFreq * u8_OutChannels) / (u16_CsiRawFormat & 0x00FF);
    f_PixleClkFreq = min(f_PixleClkFreq, MAX_PIXEL_CLK_FREQ);

    /* {datasheet 5-2-2-4. Flash_PL_STEP, FLASH_PL_STEP_GAIN}

       Pulse Width = (1/CK_PIXEL) * 128 * (2 Pow FLASH_PL_STEP_GAIN) * (FLASH_PL_STEP + 1)

    */

    /* To maximize pulse width minimize CK_PIXEL, and maximize  FLASH_PL_STEP_GAIN, FLASH_PL_STEP
     * To minimize pulse width maximize CK_PIXEL, and minimize  FLASH_PL_STEP_GAIN, FLASH_PL_STEP
    */

    /* Datasheet 5-2-2-4. Flash_PL_STEP, FLASH_PL_STEP_GAIN
       Maximum FLASH_PL_STEP = 63 , Minimum FLASH_PL_STEP = 0
       Maximum FLASH_PL_STEP_GAIN = 3 , Minimum FLASH_PL_STEP_GAIN = 0
    */
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.max_strobe_length = (128 * 8 * 64) / ((uint32) (f_PixleClkFreq));

    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.min_strobe_length = (128 * 1 * 1) / ((uint32) (f_PixleClkFreq));

    // strobe_length_step is an issue, as step value is expected to be an integer variable in Firmware whereas IMX072 sensor can have step value as a float value
    g_CamDrvConfig.camDrvSensorDetails.camDrvFlashDetail.strobe_length_step = (128 * 1 * 1) / ((uint32) (f_PixleClkFreq));
#else /* LLA_FLASH_LED == 0 */
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.max_strobe_length  = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.min_strobe_length = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.strobe_length_step = 0;
#endif
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.number_of_strobe_per_frame = 0x1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail.strobe_modulation_support = FALSE;

    return (retVal);
}


/*LLA_IMX072_GetCameraDetail
Get camera module detail for SMIA sensor
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetCameraDetail(
CAM_DRV_CAMERA_DETAILS_T    *p_camera_details)
{
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;

    /*Update camera driver sensor detail*/
    retVal = LLA_IMX072_GetCameraCap();

  // OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_GetCameraCap:%d",retVal);

    /*Update camera driver shutter detail*/
    retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) LLA_IMX072_GetShutterDetail());

   //OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_GetShutterDetail:%d",retVal);

    /*Update camera driver NDF detail*/
    retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) LLA_IMX072_GetNdfDetail());

   //OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_GetNdfDetail:%d",retVal);

    /*Update camera driver NVM detail*/
    retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) LLA_IMX072_GetNVMDetail());

   //OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_GetNVMDetail:%d",retVal);

    /*Update aperture specific information for selected module*/
    retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) LLA_IMX072_GetApertureDetail());

   //OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_GetApertureDetail:%d",retVal);

    /*Update flash detail for selected module*/
    retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) LLA_IMX072_GetFlashDetail());

   //OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_GetFlashDetail:%d",retVal);
    p_camera_details->p_nd_details = NULL;

    /*TODO:<CN>:Copy of this structure is maintained at both LLA and client ... good to remove one copy from any one place*/

    //TODO: Need to fix in generic FW
    p_camera_details->p_sensor_details = ( CAM_DRV_SENS_DETAILS_T * ) &(g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail);
    p_camera_details->p_shutter_details = ( CAM_DRV_SHUTTER_DETAILS_T * ) &(g_IMX072_CamGlbConfig.camDrvSensorDetails.camShutterDetail);

    //p_camera_details->p_nd_details = ( CAM_DRV_ND_DETAILS_T * ) &(g_IMX072_CamGlbConfig.camDrvSensorDetails.camNDFilterDetail);
    p_camera_details->p_nd_details = NULL;
    p_camera_details->p_nvm_details = ( CAM_DRV_NVM_DETAILS_T * ) &(g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail);
    p_camera_details->p_aperture_details = ( CAM_DRV_APERTURE_DETAILS_T * ) &(g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvApertureDetail);
    p_camera_details->p_flash_strobe_details = ( CAM_DRV_FLASH_STROBE_DETAILS_T * ) &(g_IMX072_CamGlbConfig.camDrvSensorDetails.camDrvFlashDetail);

    return (retVal);
}


/*This function will update the data-format capability of the sensor module
 * I.e. Whether sensor support RAW8, RAW10, RAW10to8 etc ...
 * */
void
LLA_IMX072_GetDataFormatCap(void)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    uint8_t             u8_NumberOfDataFormat;
    uint16_t            u16_DataFormatDescriptor,
                        u16_DiscriptorAddress;

    /*Fetch data-format model subtype register*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(SENSOR_STATUS__DATA_FORMAT_MODEL_SUBTYPE, 1, &u8_NumberOfDataFormat);

    /*Check each data-format one by one*/
    u16_DiscriptorAddress = SENSOR_STATUS__DATA_FORMAT_DESCRIPTOR_0_HI;
    while (u8_NumberOfDataFormat)
    {
        //Fetch data-format descriptor register
        retVal = (CAM_DRV_FN_RETVAL_E)
            (
                (uint8_t) retVal | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
                    u16_DiscriptorAddress,
                    2,
                    (uint8 *) (&(u16_DataFormatDescriptor)))
            );

        u16_DataFormatDescriptor = BSWAP_16(u16_DataFormatDescriptor);

        if ((0x08 == (u16_DataFormatDescriptor >> 8)) && (0x08 == (u16_DataFormatDescriptor & 0x00FF)))
        {
            /*Raw8 supported*/
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.format_capability.raw8 = 0;
        }
        else if ((0x0A == (u16_DataFormatDescriptor >> 8)) && (0x0A == (u16_DataFormatDescriptor & 0x00FF)))
        {
            /*Raw10 supported*/
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.format_capability.raw10 = 1;
        }
        else if ((0x0A == (u16_DataFormatDescriptor >> 8)) && (0x08 == (u16_DataFormatDescriptor & 0x00FF)))
        {
            /*Raw10to8 supported*/
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.format_capability.raw8dpcm = 0;
        }


        /*TBD:<CN>:There are more data format that can be supported by SMIA sensor - LLA Extension reqd*/

        /*Read next descriptor*/
        u16_DiscriptorAddress += 2;
        u8_NumberOfDataFormat -= 1;
    }


    return ;
}


/*LLA_IMX072_CamAnGainCap
This function will get the analog gain capability of the sensor module and update the global structure
accordingly.
Analog gain capability depends on sensor to sensor
*/
void
LLA_IMX072_GetAnGainCap(void)
{
    uint16              u16_AnalogGainCap[5];
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    uint16_t            analogGainMin,
                        analogGainMax,
                        analogGainStepSize,
                        temp = 0;

    /*analog_gain_capability, analogue_gain_code_min, analogue_gain_code_max, analogue_gain_code_step*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_STATUS__ANALOGUE_GAIN_CAPABILITY_HI,
        2,
        ( uint8 * ) &u16_AnalogGainCap);

    // This has been split into two lines as 0x82 and 0x83 are reserved registers so we should not read them.
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_STATUS__ANALOGUE_GAIN_CAPABILITY_HI + 4,
        6,
        ( uint8 * ) &(u16_AnalogGainCap[2]));

    if (BSWAP_16(u16_AnalogGainCap[0]) == 0)
    {
        /*Global analog gain is supported only*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.analog_gain_capability.gain_type = CAM_DRV_SENS_GAIN_GLOBAL;
    }
    else
    {
        /*Per channel analog gain is supported*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.analog_gain_capability.gain_type = CAM_DRV_SENS_GAIN_SEPARATE;
    }


    analogGainMin = BSWAP_16(u16_AnalogGainCap[2]);
    analogGainMax = BSWAP_16(u16_AnalogGainCap[3]);

    analogGainStepSize = BSWAP_16(u16_AnalogGainCap[4]);

   // OstTraceInt3(TRACE_FLOW, "GAIN: min: %X, MAX: %X, STEP: %X",analogGainMin, analogGainMax, analogGainStepSize);
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_min = analogGainMin;
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_max = analogGainMax;
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_step = analogGainStepSize;

    /*Read extra parameters that are not passed to client*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(SENSOR_STATUS__ANALOGUE_GAIN_TYPE_HI, 10, ( uint8 * ) &u16_AnalogGainCap);

    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_type = BSWAP_16(u16_AnalogGainCap[0]);
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 = BSWAP_16(u16_AnalogGainCap[1]);
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0 = BSWAP_16(u16_AnalogGainCap[2]);
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 = BSWAP_16(u16_AnalogGainCap[3]);
    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1 = BSWAP_16(u16_AnalogGainCap[4]);

  //  OstTraceInt1(TRACE_FLOW, "GAIN: Type: %X ",g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_type);
  //  OstTraceInt4(TRACE_FLOW, "GAIN: m0: %X, c0: %X, m1: %X, c1: %X",g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0, g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0, g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1,g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1);

    /*
      Calculate minimum and maximum gain
      Analog gain = (m0 * x + c0) / (m1 * x + c1)
    */

    /*Min gain*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.analog_gain_capability.min_gain_x1000 = (uint16_t)
        (
            (
                g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                analogGainMin +
                g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
            ) * 1000 /
                (
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                    analogGainMin +
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                )
        );

    /*Max gain */
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.analog_gain_capability.max_gain_x1000 = (uint16_t)
        (
            (
                g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                analogGainMax +
                g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
            ) * 1000 /
                (
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                    analogGainMax +
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                )
        );

    /* Step size */

    /*Calculate actual gain step, for single step size*/

    /*Analogue gain value for single step*/
    temp = (uint16_t)
        (
            (
                g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                analogGainStepSize /*for single step in analog gain code*/ +
                g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
            ) * 1000 /
                (
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                    analogGainStepSize /*for single step in analog gain code*/ +
                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                )
        );

    // adding +1 as step calculated here is 3.96 but due to integer reporting, it round of to 3 which is wrong. @ host side, step reporting is done in units
    // of x256 which finally lead to 0 i.e. total wrong value. Adding +1 will make it 4 and hence in x256 units, value will be 1.
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.analog_gain_capability.gain_step_x1000 = temp -
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.analog_gain_capability.min_gain_x1000 +
        1;

    if (retVal != CAM_DRV_FN_OK)
    {
        LLA_ASSERT_XP70();
    }


    return;
}


/* LLA_IMX072_GetDiGainCap
This function will read digital gain capability of the sensor and update the global structures accordingly
*/
void
LLA_IMX072_GetDiGainCap(void)
{
    uint16              u16_DigitalGain[3];
    uint16_t            u16_DigitalGainCap;

    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

   // OstTraceInt0(TRACE_FLOW, "LLA_IMX072_GetDiGainCap");

    /*Read digital gain capability*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_INTEGAIN_LIMIT__DIGITAL_GAIN_CAPABILITY_HI,
        2,
        ( uint8 * ) &u16_DigitalGainCap);

    u16_DigitalGainCap = BSWAP_16(u16_DigitalGainCap);

    /*Update SMIA global configuration structure with values read*/
    g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_cap = u16_DigitalGainCap;

    if (CAM_DRV_SENS_GAIN_NONE == u16_DigitalGainCap)
    {
        /*Digital gain none*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.gain_type = CAM_DRV_SENS_GAIN_NONE;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.min_gain_x1000 = LLA_DEF_GAIN_x1000;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.max_gain_x1000 = LLA_DEF_GAIN_x1000;
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.gain_step_x1000 = LLA_DEF_GAIN_x1000;
    }
    else
    {
        /*Separate digital gain*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.gain_type = CAM_DRV_SENS_GAIN_SEPARATE;

        /*Read min, max and step size for digital gain*/
        retVal = (CAM_DRV_FN_RETVAL_E)
            (
                (uint8_t) retVal | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGAIN_LIMIT__DIGITAL_GAIN_MIN_HI,
            6,
                    ( uint8 * ) &u16_DigitalGain)
            );

        /*Update SMIA global configuration structure with values read*/
        g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_min = BSWAP_16(u16_DigitalGain[0]);
        g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_max = BSWAP_16(u16_DigitalGain[1]);
        g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_step = BSWAP_16(u16_DigitalGain[2]);

        /*Digital gain registers are in 8.8 unsigned format, so the corresponding gain is
                        Gain = (UpperByte  +  LowerByte / 256 )
                */

        /*Min digital gain*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.min_gain_x1000 = (uint16_t)
            (
                (g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_min >> 8) +
                    ((g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_min & 0x00FF) / 256.0)
            ) * 1000;

        /*Max digital gain*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.max_gain_x1000 = (uint16_t)
            (
                (g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_max >> 8) +
                    ((g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_max & 0x00FF) / 256.0)
            ) * 1000;

        /*actual gain step*/
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.gain_step_x1000 = (uint16) ((g_IMX072_CamGlbConfig.digitalGainCap.digital_gain_step * 1000UL) >> 8);
    }


    /*Asserting if error*/
    if (retVal != CAM_DRV_FN_OK)
    {
        LLA_ASSERT_XP70();
    }


    return;
}


/*LLA_IMX072_GetFrameFormatCap
This function will update the frame format capability (i.e num of status line etc ..) of the sensor
*/
void
LLA_IMX072_GetFrameFormatCap(void)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    uint8               u8_FrameFormatModelSubtype,
                        u8_NumOfColDescriptors,
                        u8_NumOfRowDescriptors,
                        u8_PixelCode,
                        e_Flag_StartOfVisibleLinesReached = 0;

    uint16              u16_DescriptorAddress,
                        u16_FrameFormatDescriptor;

    uint8               e_Flag_StartOfVisibleColReached = 0,
                        dummyPixelFound = 0;

    /*Fetch frame_format_model_subtype*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_STATUS__FRAME_FORMAT_MODEL_SUBTYPE,
        1,
        (uint8_t *) (&(u8_FrameFormatModelSubtype)));

    /* First get number of column descriptors.*/
    u8_NumOfColDescriptors = (u8_FrameFormatModelSubtype >> 4);
    u16_DescriptorAddress = SENSOR_STATUS__FRAME_FORMAT_DESCRIPTOR_0_HI;

    /*Initialize before fetching from sensor*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_left = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_right = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_left = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_right = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_capability = CAM_DRV_SENS_NO_BLACK_PIXELS;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixel_position = CAM_DRV_SENS_DUMMY_PIXELS_NONE;

    while (u8_NumOfColDescriptors)
    {
        //Fetch the descriptor value
        retVal = (CAM_DRV_FN_RETVAL_E)
            (
                (uint8_t) retVal | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
                    u16_DescriptorAddress,
                    2,
                    (uint8_t *) (&(u16_FrameFormatDescriptor)))
            );

        u16_FrameFormatDescriptor = BSWAP_16(u16_FrameFormatDescriptor);

        u8_PixelCode = (u16_FrameFormatDescriptor & 0xF000) >> 12;

        /*TODO:<CN>:Currently only taking care of Black and dummy pixels only*/
        if
        (
            u8_PixelCode == LLA_PIXEL_CODE_BLACK        /*Black pixel*/
        ||  u8_PixelCode == LLA_PIXEL_CODE_DARK         /*Dark pixel data*/
        )
        {
            if (!e_Flag_StartOfVisibleColReached)
            {
                g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_left = u16_FrameFormatDescriptor & 0x0FFF;
            }
            else
            {
               g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_right = u16_FrameFormatDescriptor & 0x0FFF;
            }


            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_capability = CAM_DRV_SENS_ALWAYS_BLACK_PIXELS;
        }
        else if (u8_PixelCode == LLA_PIXEL_CODE_DUMMY /*Dummy pixel*/ )
        {
            dummyPixelFound = 1;
            if (!e_Flag_StartOfVisibleColReached)
            {
               g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_left = u16_FrameFormatDescriptor & 0x0FFF;
            }
            else
            {
                g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_right = u16_FrameFormatDescriptor & 0x0FFF;
            }
        }
        else if (u8_PixelCode == LLA_PIXEL_CODE_VISIBLE /*visible data*/ )
        {
            e_Flag_StartOfVisibleColReached = 1;
        }


        u16_DescriptorAddress += 2;
        u8_NumOfColDescriptors -= 1;
    }


    /// Extract the number of row descriptors.
    u8_NumOfRowDescriptors = (u8_FrameFormatModelSubtype & 0x0F);
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_top = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_bottom = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_top = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_bottom = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_top = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_bottom = 0;

    while (u8_NumOfRowDescriptors)
    {
        /// Get the value of the descriptor.
        retVal = (CAM_DRV_FN_RETVAL_E)
            (
                (uint8_t) retVal | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
                    u16_DescriptorAddress,
                    2,
                    (uint8_t *) (&(u16_FrameFormatDescriptor)))
            );

        u16_FrameFormatDescriptor = BSWAP_16(u16_FrameFormatDescriptor);

        u8_PixelCode = (u16_FrameFormatDescriptor & 0xF000) >> 12;

        if (LLA_PIXEL_CODE_EMBEDDED /*Embedded data*/ == u8_PixelCode)
        {
            if (!(e_Flag_StartOfVisibleLinesReached))
            {
                g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_top = (u16_FrameFormatDescriptor & 0x0FFF);
            }
            else
            {
               g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_bottom = (u16_FrameFormatDescriptor & 0x0FFF);
            }
        }
        else if
            (
                u8_PixelCode == LLA_PIXEL_CODE_BLACK    /*Black pixel*/
            ||  u8_PixelCode == LLA_PIXEL_CODE_DARK     /*Dark pixels*/
            )
        {
            if (!e_Flag_StartOfVisibleLinesReached)
            {
                g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_top = u16_FrameFormatDescriptor & 0x0FFF;
            }
            else
            {
               g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_bottom = u16_FrameFormatDescriptor & 0x0FFF;
            }


           g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_capability = CAM_DRV_SENS_ALWAYS_BLACK_PIXELS;
        }
        else if (u8_PixelCode == LLA_PIXEL_CODE_DUMMY /*Dummy pixel*/ )
        {
            dummyPixelFound = 1;
            if (!e_Flag_StartOfVisibleLinesReached)
            {
                g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_top = u16_FrameFormatDescriptor & 0x0FFF;
            }
            else
            {
               g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_bottom = u16_FrameFormatDescriptor & 0x0FFF;
            }
        }
        else if (u8_PixelCode == LLA_PIXEL_CODE_VISIBLE /*visible data*/ )
        {
            // Use a flag to indicate that the start of active region has been reached...
            e_Flag_StartOfVisibleLinesReached = 1;
        }


        u16_DescriptorAddress += 2;
        u8_NumOfRowDescriptors -= 1;
    }


    if (dummyPixelFound)
    {
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixel_position = CAM_DRV_SENS_DUMMY_PIXELS_OUTSIDE;
    }
    else
    {
        g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixel_position = CAM_DRV_SENS_DUMMY_PIXELS_NONE;
    }


    /*For IMX072, frame format changes with the vertical sub-sampling registers, Effetive OB lines changes with vertical sub-sampling
       This can be avoided via setting manf-specific register - 0x3015 with different values depending upon modes, this make EOB lines as 0 (constant).

       Thus overriding some of values read from sensor
    */
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_top = DEFAULT_DUMMY_PIXEL_TOP;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_top = DEFAULT_BLACK_PIXELS_TOP;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_bottom = DEFAULT_BLACK_PIXELS_BOTTOM;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_bottom = DEFAULT_DUMMY_PIXEL_BOTTOM;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_top = NO_OF_ISL_FROM_SENSOR;

    /*Error has occured somewhere, asserting */
    if (retVal != CAM_DRV_FN_OK)
    {
        LLA_ASSERT_XP70();
    }


    return;
}


/*LLA_IMX072_GetFrameDimensionCap
This fuction will get the frame dimension sensor constraints
*/
void
LLA_IMX072_GetFrameDimensionCap(void)
{
uint16_t            u16_ImpliedMaximum;
    CAM_DRV_FN_RETVAL_E e_Flag_Status = CAM_DRV_FN_OK;

     /*Read*/
    e_Flag_Status = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_VTIMING_LIMIT__X_ADDR_MIN_HI,
        12,
        (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMin)));

    g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMin = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMin);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTYAddrMin = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTYAddrMin);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMax = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMax);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTYAddrMax = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTYAddrMax);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPXOutputSize = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPXOutputSize);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPYOutputSize = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPYOutputSize);

    /*Read max_x_output_size and max_y_output_size */
    e_Flag_Status = (CAM_DRV_FN_RETVAL_E)
        (
            (uint8_t) e_Flag_Status | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_VTIMING_LIMIT__MAX_X_OUTPUT_SIZE_HI,
        4,
                (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize)))
        );
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize);

    e_Flag_Status = (CAM_DRV_FN_RETVAL_E)
        (
            (uint8_t) e_Flag_Status | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_VTIMING_LIMIT__MIN_FRAME_LENGTH_LINES_HI,
        12,
                (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines)))
        );

    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTFrameLengthLines = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTFrameLengthLines);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTLineLengthPck = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTLineLengthPck);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineBlankingPck = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineBlankingPck);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameBlanking = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameBlanking);

    /*Read scaling capability of sensor*/
    e_Flag_Status = (CAM_DRV_FN_RETVAL_E)
        (
            (uint8_t) e_Flag_Status | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_SCALING_LIMIT__SCALING_CAPABILITY_LO,
        1,
                (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.e_SensorProfile)))
        );

    /*Read scalar min/max*/
    e_Flag_Status = (CAM_DRV_FN_RETVAL_E)
        (
            (uint8_t) e_Flag_Status | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_SCALING_LIMIT__SCALE_M_MIN_HI,
        4,
                (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.u16_ScalerMMin)))
        );
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_ScalerMMin = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_ScalerMMin);
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_ScalerMMax = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_ScalerMMax);

    /*Read max_odd_inc*/
    e_Flag_Status = (CAM_DRV_FN_RETVAL_E)
        (
            (uint8_t) e_Flag_Status | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_VTIMING_LIMIT__MAX_ODD_INC_HI,
        2,
                (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOddInc)))
        );
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOddInc = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOddInc);

    //OstTraceInt1(TRACE_DEBUG, "g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOddInc : %d",g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOddInc);

    /*Read max_even_inc*/
    e_Flag_Status = (CAM_DRV_FN_RETVAL_E)
        (
            (uint8_t) e_Flag_Status | (uint8_t) LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_VTIMING_LIMIT__MAX_EVEN_INC_HI,
        2,
                (uint8_t *) (&(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxEvenInc)))
        );
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxEvenInc = BSWAP_16(g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxEvenInc);

    /*Compute the implied maximum VT x output size*/
    u16_ImpliedMaximum = g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMax -
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTXAddrMin +
        1;

    /*Compute the actual maximum VT x output size*/
    if (g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize)
    {
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize = min(
            g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize,
            u16_ImpliedMaximum);
    }
    else
    {
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize = u16_ImpliedMaximum;
    }


    /*Compute the implied maximum y output size*/
    u16_ImpliedMaximum = g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTYAddrMax -
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_VTYAddrMin +
        1;

    /*Compute the actual maximum VT y output size*/
    if (g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize)
    {
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize = min(
            g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize,
            u16_ImpliedMaximum);
    }
    else
    {
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize = u16_ImpliedMaximum;
    }


    /*As per video-timing application notes pg27*/
    if (0 == g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPXOutputSize)
    {
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPXOutputSize = LLA_MIN_X_SIZE;
    }


    if (0 == g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPYOutputSize)
    {
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinOPYOutputSize = LLA_MIN_Y_SIZE;
    }


    /*Minimum line length pck for IMX072 is fixed*/
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck = IMX072_MIN_LINE_LEN_PCK;

    /*minimum blanking pck, set to same values as for full h-mode*/
    //[CR - 445315] - MinVTLineBlankingPck varriable will be initialized to minimum line blanking pck for mode 0.
    //g_IMX072_MinLineBlankingPck array is now removed from the code
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineBlankingPck = MIN_LINE_BLANKING_PCK_MODE0;

    /*minimum frame blanking lines, set to same values as for full v-mode*/
    //[CR - 445315] - MinVTFrameLengthLines varriable will be initialized to minimum frame blanking lines for mode 0.
    //g_IMX072_MinFrameBlankingLine array is now removed from the code
    g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines = MIN_FRAME_BLANKING_LINE_MODE0;

  if (CAM_DRV_FN_OK != e_Flag_Status)
    {
#if LLA_ASSERT_LOG_EN
        //OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_GetFrameDimensionCap  - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();
    }


    return;
}


/*LLA_IMX072_GetOrientationPixelCap:
Read orientation capability and pixel order of the sensor
*/
void
LLA_IMX072_GetOrientationPixelCap(void)
{
    /*image-orientation: Support both flip and mirrot*/

    /*TODO:<CN>: for sensor not supporting any of these need to be handled seprately*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.orientation_capability.flip = 1;     /*1 to indicate that sensor support flip*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.orientation_capability.mirror = 1;   /*1 to indicate that sensor support mirroring*/

    /*Pixel order*/
   g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.pixel_orders.normal = CAM_DRV_SENS_PIXEL_ORDER_BGbGrR;
   g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.pixel_orders.mirrored = CAM_DRV_SENS_PIXEL_ORDER_GbBRGr;
   g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.pixel_orders.flipped = CAM_DRV_SENS_PIXEL_ORDER_GrRBGb;
   g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.pixel_orders.mirrored_and_flipped = CAM_DRV_SENS_PIXEL_ORDER_RGrGbB;

    return;
}


/*LLA_IMX072_GetExposureCap:
Read exposure capability registers from sensor
*/
void
LLA_IMX072_GetExposureCap(void)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    uint16_t u16_Array[2];
    uint16_t u16_IntgTimeCap;

     //OstTraceInt0(TRACE_FLOW, "LLA_IMX072_GetExposureCap !!");

     /*Read integration time, capability from sensor*/

    /* integration_time_capability*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_INTEGAIN_LIMIT__INTEGRATION_TIME_CAPABILITY_HI,
        2,
        ( uint8 * ) &u16_IntgTimeCap);

    u16_IntgTimeCap = BSWAP_16(u16_IntgTimeCap);

    //Cnanda: Fine integration control is not supported in LLCD
    u16_IntgTimeCap = 0;
    g_IMX072_CamGlbConfig.exposureCap.e_IntegrationCapability = u16_IntgTimeCap;

//  OstTraceInt1(TRACE_FLOW, "LLA_IMX072_GetExposureCap e_IntegrationCapability :%d",u16_IntgTimeCap);
    if (0 == u16_IntgTimeCap)
    {
        /*Coarse integration, no fine integration*/
        g_IMX072_CamGlbConfig.exposureCap.u16_MinimumFineIntegrationPixels = 0;
        g_IMX072_CamGlbConfig.exposureCap.u16_FineIntegrationMaxMargin = 0;
    }
    else if (1 == u16_IntgTimeCap)
    {
        /*Fine integration available*/

        /*Read fine integration values*/
        retVal = LLA_READ_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGAIN_LIMIT__FINE_INTEGRATION_TIME_MIN_HI,
            4,
            ( uint8_t * ) &u16_Array);

        g_IMX072_CamGlbConfig.exposureCap.u16_MinimumFineIntegrationPixels = BSWAP_16(u16_Array[0]);
        g_IMX072_CamGlbConfig.exposureCap.u16_FineIntegrationMaxMargin = BSWAP_16(u16_Array[1]);

      //  OstTraceInt2(TRACE_FLOW, "LLA_IMX072_GetExposureCap u16_MinimumFineIntegrationPixels:%d u16_FineIntegrationMaxMargin:%d",u16_Array[0],u16_Array[1]);
    }
    else
    {
      //  OstTraceInt0(TRACE_FLOW, "Wrong value for integration capability !!");

        /*Wrong value for integration capability*/
        LLA_ASSERT_XP70();
    }


    /*Read coarse integration values*/
    retVal = LLA_READ_I2C_BYTES_16BitRegSize(
        SENSOR_INTEGAIN_LIMIT__COARSE_INTEGRATION_TIME_MIN_HI,
        4,
        ( uint8 * ) &u16_Array);

    g_IMX072_CamGlbConfig.exposureCap.u16_MinimumCoarseIntegrationLines = BSWAP_16(u16_Array[0]);
    g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin = BSWAP_16(u16_Array[1]);

    // OstTraceInt2(TRACE_FLOW, "LLA_IMX072_GetExposureCap u16_MinimumCoarseIntegrationLines:%d u16_CoarseIntegrationMaxMargin:%d",u16_Array[0],u16_Array[1]);
   return;
}


/*LLA_IMX072_GetCsiSignallingCap:
CSI Signalling capability of a selected SMIA sensor
*/
void
LLA_IMX072_GetCsiSignallingCap(void)
{
    /*TODO:<CN>:No mapping found from SMIA registers*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.dphy_ctrl_automatic_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.dphy_ctrl_UI_based_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.dphy_ctrl_manual_time_and_UI_1_register_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.dphy_ctrl_manual_time_and_UI_2_register_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.dphy_ctrl_manual_time_register_supported = 0;

    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.raw_8_dt = 0x2A;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.raw_10_dt = 0x2B;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.dpcm_10_to_8_dt = 0x30;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.dpcm_10_to_6_dt = 0;

    /* Update CSI CCP capabilitis, to be exported to client */

    /*LLA_IMX072_UpdateCsiCcpCap()*/

    /*IMX072 module support 2 lane CSI , no support for CCP*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_1_lane_supported = 1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_2_lane_supported = 1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_3_lane_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_4_lane_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi_signalling_ccp2_data_clock_mode_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi_signalling_ccp2_data_strobe_mode_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi_signalling_csi2_mode_supported = 1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.fast_standby_frame_truncation_supported = 0;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.sw_changeable_CCI_address_supported = 0;

    /* Documentation Source for codes
        -Kanna Datasheet: IMX072PQH5-C    Page 58, Table 17: Sync Code Settings <SS>*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.ancillary_data_dt = 0x12;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.black_pixels_dt = 0x13;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.csi_signalling_options.csi2_data_type_ids.dummy_pixels_dt = 0x10;
    return;
}


/*Update the test patter capability of sensor*/
void
LLA_IMX072_GetTestPatternCap(void)
{

  /*IMX072 support SolidColorMode, 100% solid color bar, and PN9*/
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.test_picture_capability.solid_colour_mode = 1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.test_picture_capability.colour_bar_solid_mode = 1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.test_picture_capability.pseudo_random_mode = 1;
    g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.test_picture_capability.fix_data_mode = 0;

    return;

}


/*LLA_IMX072_UpdateTestPicture:
Enable test pattern mode in the sensor
*/
CAM_DRV_RETVAL_E
LLA_IMX072_UpdateTestPicture
(CAM_DRV_SENS_CONFIG_TEST_PICTURE_T *p_config_test_picture)
{
    uint16_t    u16_Value;
    uint8_t u8_Value;
        CAM_DRV_FN_RETVAL_E status = CAM_DRV_FN_OK;
#if 0
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;
    /*Disable the test pattern mode*/
        u16_Value = 0x2;
    u16_Value = BSWAP_16(u16_Value);
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, &u16_Value);
#endif

#if 0
#if 0
    /*Set the default values for solid color*/
    /*Need to update this code when LLA API extended to provide this information*/
    u16_Value = 0;
    u16_Value = BSWAP_16(u16_Value);
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_RED_HI, 2, &u16_Value);

    u16_Value = 0x03FF;
    u16_Value = BSWAP_16(u16_Value);
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_GREENR_HI, 2, &u16_Value);

    u16_Value = 0;
    u16_Value = BSWAP_16(u16_Value);
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_BLUE_HI, 2, &u16_Value);

    u16_Value = 0;
    u16_Value = BSWAP_16(u16_Value);
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_GREENB_HI, 2, &u16_Value);
#endif
    /*Enable test pattern generation*/
    u16_Value = 2;
    u16_Value = BSWAP_16(u16_Value);
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, &u16_Value);

    u8_Value = 1;
    retVal |= LLA_WRITE_I2C_BYTES_16BitRegSize(0x301a, 1, &u8_Value);
#else
    switch(p_config_test_picture->test_picture_mode)
    {
    case CAM_DRV_SENS_TEST_PICTURE_MODE_NONE:
        /*Disable the test pattern mode*/
        u16_Value = 0;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, (uint8*)&u16_Value);
        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_SOLID_COLOUR:

        /*Set the default values for solid color*/
        /*Need to update this code when LLA API extended to provide this information*/
        u16_Value = LLA_TEST_SOLID_COLOR_RED;
        u16_Value = BSWAP_16(p_config_test_picture->test_picture_colour.red);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_RED_HI, 2, (uint8*)&u16_Value);

        u16_Value = LLA_TEST_SOLID_COLOR_GrR;
        u16_Value = BSWAP_16(p_config_test_picture->test_picture_colour.greenR);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_GREENR_HI, 2, (uint8*)&u16_Value);

        u16_Value = LLA_TEST_SOLID_COLOR_BLU;
        u16_Value = BSWAP_16(p_config_test_picture->test_picture_colour.blue);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_BLUE_HI, 2, (uint8*)&u16_Value);

        u16_Value = LLA_TEST_SOLID_COLOR_GrB;
        u16_Value = BSWAP_16(p_config_test_picture->test_picture_colour.greenB);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_GREENB_HI, 2, (uint8*)&u16_Value);

        /*Enable test pattern generation*/
        u16_Value = 1;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, (uint8*)&u16_Value);

        u8_Value = 1;
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(0x301a, 1, &u8_Value);

        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_SOLID:
        /*Enable test pattern generation*/
        u16_Value = 2;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, (uint8*)&u16_Value);

        u8_Value = 1;
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(0x301a, 1, &u8_Value);

        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_FADE:
        /*Enable test pattern generation*/
        u16_Value = 3;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, (uint8*)&u16_Value);

        u8_Value = 1;
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(0x301a, 1, &u8_Value);

        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_PSEUDO_RANDOM:
        /*Enable test pattern generation*/
        u16_Value = 4;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, (uint8*)&u16_Value);

        u8_Value = 1;
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(0x301a, 1, &u8_Value);

        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_SCALE:
        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_RAMP:
        break;

    case CAM_DRV_SENS_TEST_PICTURE_MODE_FIX_DATA:

        /*Set the default values for solid color*/
        /*Need to update this code when LLA API extended to provide this information*/
        u16_Value = LLA_TEST_SOLID_COLOR_RED;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_RED_HI, 2, (uint8*)&u16_Value);

        u16_Value = LLA_TEST_SOLID_COLOR_GrR;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_GREENR_HI, 2, (uint8*)&u16_Value);

        u16_Value = LLA_TEST_SOLID_COLOR_BLU;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_BLUE_HI, 2, (uint8*)&u16_Value);

        u16_Value = LLA_TEST_SOLID_COLOR_GrB;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_DATA_GREENB_HI, 2, (uint8*)&u16_Value);

        /*Enable test pattern generation*/
        u16_Value = 1;
        u16_Value = BSWAP_16(u16_Value);
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_TEST_PATTERN__TEST_PATTERN_MODE_HI, 2, (uint8*)&u16_Value);

        u8_Value = 1;
            status = LLA_WRITE_I2C_BYTES_16BitRegSize(0x301a, 1, &u8_Value);
        break;
    default:
        break;
    }

        if(CAM_DRV_FN_OK != status)
    {
        /*Error has occured*/
        return CAM_DRV_ERROR_HW;
    }

    return CAM_DRV_OK;
#endif

}


/*LLA_IMX072_UpdateFrameDimensionParameters
This function calculate frame dimension parameters for sensor using WOI and Output resolution passed
*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_UpdateFrameDimensionParameters(
CAM_DRV_SENS_CONFIG_WOI_T   *p_config_woi,
uint32 u32_MinReqLineLenPck,
uint8 trial_only)
{
    uint8_t                     u8_iter = 0;
	uint8_t                     u8_iter1 = 0;
    CAM_DRV_FN_RETVAL_E         retVal = CAM_DRV_FN_OK;

    CamFrameDimension_ts       *currFrameDimensionParam;


    /*If in trial mode, then initlize pointer with local struct, else with global struct*/
    if (Flag_e_TRUE == trial_only)
    {
        currFrameDimensionParam = &g_IMX072_CamGlbConfig.FrameDimensionParam_TrialMode;
    }
    else
    {
        currFrameDimensionParam = &g_IMX072_CamGlbConfig.currFrameDimensionParam;;
    }

    while (u8_iter < g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.number_of_modes)
    {
       //OstTraceInt4(TRACE_USER1, "Curr Mode woi w: %x, H: %x, OP w: %x, H: %x", p_config_woi->woi_res.width, p_config_woi->woi_res.height, p_config_woi->output_res.width, p_config_woi->output_res.height);
       //OstTraceInt4(TRACE_USER1, "Check -woi w: %x, H: %x, OP w: %x, H: %x", g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].woi_res.width, g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].woi_res.height, g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].output_res.width, g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].output_res.height);
        if
        (
              (p_config_woi->woi_res.width == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].woi_res.width)
        &&  (p_config_woi->woi_res.height == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].woi_res.height)
        &&  (p_config_woi->output_res.width == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].output_res.width)
        &&  (p_config_woi->output_res.height == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].output_res.height)
        )
        {
            g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected = u8_iter;
            break;
        }

        u8_iter++;
    }
    if(u8_iter == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.number_of_modes)
    {
        //OstTraceInt0(TRACE_DEBUG, "Corresponding output mode settings not found");
        LLA_ASSERT_XP70();
    }

    /* Find the appropriate sensor mode settings*/	 
    while (u8_iter1 < g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.number_of_modes)
        {

            if
             (
                (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[u8_iter1]->woi_res.width == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].woi_res.width)
             &&  (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[u8_iter1]->woi_res.height == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].woi_res.height)
             &&  (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[u8_iter1]->output_res.width == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].output_res.width)
             &&  (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[u8_iter1]->output_res.height == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].output_res.height)
             )
             {
                g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected = u8_iter1;
                break;
             }

               u8_iter1++;
            }

    if(u8_iter1 == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.number_of_modes)
    {
        OstTraceInt0(TRACE_DEBUG, "Corresponding mode settings not found");
        LLA_ASSERT_XP70();
    }


    currFrameDimensionParam->u16_WOIXSize =  g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].woi_res.width;
    currFrameDimensionParam->u16_WOIYSize =  g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].woi_res.height;
    currFrameDimensionParam->u16_OPXSize  =  g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].output_res.width;
    currFrameDimensionParam->u16_OPYSize  =  g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[u8_iter].output_res.height;
    currFrameDimensionParam->u16_VTFrameLengthLines = LLA_IMX072_ComputeImpliedMinimumFrameLength(currFrameDimensionParam->u16_OPYSize);
    currFrameDimensionParam->u16_VTLineLengthPck = LLA_IMX072_ComputeImpliedMinimumLineLength(currFrameDimensionParam->u16_OPXSize, u32_MinReqLineLenPck);

    return (retVal);
}


/*LLA_IMX072_UpdateFrameRate:
Change frame rate for SMIA sensor
*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_UpdateFrameRate(
uint16_t frameRate, uint8 trial_only)
{

 CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    float_t             f_RequestedFrameRate,
                        f_CalculatedFrameRate = 0.0;
    uint16_t            u16_TargetFrameLengthLine,
                        u16_MinimumFrameLengthLine,
                        u16_MinimumFrameBlankingLine;
    uint8_t             u8_ExtraRows;

    uint32_t            u32_FrameLengthFlickerPeriod;

    CamFrameDimension_ts   *currFrameDimensionParam;

    uint8_t             u8_SubSamplingFactor;

    //<Sudeep>  To calculate u16_MinimumFrameLengthLine
     u8_SubSamplingFactor = ( (g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].woi_res.width)
                              /(g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].output_res.width)
                             );

     OstTraceInt3(TRACE_DEBUG, "LLA_IMX072_UpdateFrameRate: u8_SubSamplingFactor:%x u16_WOIXSize :%x u16_OPXSize:%x",u8_SubSamplingFactor,g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].woi_res.width,g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].output_res.width);

     //[CR - 445315] - The LLA_IMX072_GetExtraLines function is removed from the code and extra lines are added to the
     //mode specific structure "SensorModeSettings_ts".
     u8_ExtraRows = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected]->u16_ExtraLines;


    /*If in trial mode, then initlize pointer with local struct, else with global struct*/
    if (Flag_e_TRUE == trial_only)
    {
        currFrameDimensionParam = &g_IMX072_CamGlbConfig.FrameDimensionParam_TrialMode;
    }
    else
    {
        currFrameDimensionParam = &g_IMX072_CamGlbConfig.currFrameDimensionParam;;
    }


   // u8_ExtraRows = LLA_FrameDimension_GetSensorConstantRows();

    /*For sensor frame rate is updated by modifying frameLengthLines.
            **Line length is not modified as it results in re-calculation of Exposure parameters
            */
    f_RequestedFrameRate = (float_t) frameRate / 100.0;

   // OstTraceInt1(TRACE_DEBUG, "f_RequestedFrameRate: %f", f_RequestedFrameRate);

    /*Client requested a new frame rate.
            **For changing the frame rate.. frame_length will be modified and frame_line_length will not be modified
            */

    /*First find the frame length in lines required for the given frame rate at current configurations*/

    /*
                                                      1000000
                As frame_rate =     -------------------------       [Application notes pg55]
                                                     vt_frame_length_us

                vt_frame_length_us = vt_frame_length_lines * vt_line_length_us;


                                                            1000000
                vt_frame_length_line =  ---------------------
                                                       vt_line_length_us * wanted_frame_rate
            */

    /*Calculate FrameLengthLine required for achieiving the requested frame rate*/
    u16_TargetFrameLengthLine = (uint16_t)
        (
            (float_t) 1000000.0 /
                (
                    currFrameDimensionParam->u16_VTLineLengthPck *
                    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us *
                    f_RequestedFrameRate
                )
        );

   //  OstTraceInt1(TRACE_DEBUG, "u16_TargetFrameLengthLine: %f", u16_TargetFrameLengthLine);

    f_CalculatedFrameRate = (float_t)
        (
            (float_t) 1000000.0 / (float_t)
                (
                    currFrameDimensionParam->u16_VTLineLengthPck *
                    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us *
                    u16_TargetFrameLengthLine
                )
        );

     OstTraceInt1(TRACE_DEBUG, "f_CalculatedFrameRate: %f", f_CalculatedFrameRate);

    if (f_CalculatedFrameRate < f_RequestedFrameRate)
    {
        u16_TargetFrameLengthLine--;
    }

      //<sudeep> for imx072 read from I2C
     u16_MinimumFrameBlankingLine = g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameBlanking;
     OstTraceInt1(TRACE_DEBUG, "u16_MinimumFrameBlankingLine: %d",u16_MinimumFrameBlankingLine);



   // u16_MinimumFrameBlankingLine = g_IMX072_MinFrameBlankingLine[u8_SubSamplingFactor - 1];

    /*Compute minimum frame length required for the current configuration*/
    u16_MinimumFrameLengthLine = currFrameDimensionParam->u16_OPYSize +
        u16_MinimumFrameBlankingLine +
        u8_ExtraRows;

    u16_MinimumFrameLengthLine = max(
        u16_MinimumFrameLengthLine,
        g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines);

    /*Compute maximum line length possible for current configuration and frame rate*/
    currFrameDimensionParam->u16_MaxLineLenPckInCurrConf = (uint16)
        (
            (    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz * 1000000.0) /
                (u16_MinimumFrameLengthLine * f_RequestedFrameRate)
        );

   currFrameDimensionParam->u16_MaxLineLenPckInCurrConf =  clip(currFrameDimensionParam->u16_MaxLineLenPckInCurrConf, g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck, g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTLineLengthPck);

    /*Compute maximum line length possible for current configuration and frame rate*/
 /*   currFrameDimensionParam->u16_MaxLineLenPckInCurrConf = (uint16)(
                                                                       (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz * 1000000.0)
                                                                     / (u16_MinimumFrameLengthLine * f_RequestedFrameRate)
                                                                    );
    if(currFrameDimensionParam->u16_MaxLineLenPckInCurrConf <  g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck)
    {
        currFrameDimensionParam->u16_MaxLineLenPckInCurrConf =  g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck;
    }
    else if (currFrameDimensionParam->u16_MaxLineLenPckInCurrConf >  g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTLineLengthPck)
    {
        currFrameDimensionParam->u16_MaxLineLenPckInCurrConf =  g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTLineLengthPck;
    }
*/

    /*If target frame length lines is less than frame length lines required for current configuration. In that case requested frame rate can't be achieved
        in current configuration.
        */
    OstTraceInt2(TRACE_DEBUG, "u16_MinimumFrameLengthLine: %d, calcFrLen %d", u16_MinimumFrameLengthLine, u16_TargetFrameLengthLine);
    if (u16_TargetFrameLengthLine < u16_MinimumFrameLengthLine)
    {
        /*Requested frame rate not possible under current configuration*/
        u16_TargetFrameLengthLine = u16_MinimumFrameLengthLine;
    }
    else if (u16_TargetFrameLengthLine > g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTFrameLengthLines)
    {
        u16_TargetFrameLengthLine = g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTFrameLengthLines;
    }


    /*Update the currentFrameLengthlines*/
    currFrameDimensionParam->u16_VTFrameLengthLines = u16_TargetFrameLengthLine;

    /*Modify line length for flicker fre calculations*/

    /*Application notes, pg53- Now we have calculated all frequencies, adjust frame length time based on flicker period*/

    /*Calculate flicker free priod*/
    if ((Flag_e_TRUE == LLA_FLICKER_COMPATIABLE_TIMING) && (0 != g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_Acfrequency))
    {
        /*If AC frequency is defined, and flicker free calculation is enabled*/
        currFrameDimensionParam->f_FlickerPeriod_us = 1000000.0 / (2.0 * g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_Acfrequency);

        /*
            The number of flicker periods is rounded up to the nearest integer value to ensure that the minimum
            frame length time is always respected.
            */
        u32_FrameLengthFlickerPeriod = GenericFunctions_Ceiling(
            (float_t)
                (   /*frame time us*/
                    currFrameDimensionParam->u16_VTFrameLengthLines *
                    currFrameDimensionParam->u16_VTLineLengthPck *
                    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us
                ) / currFrameDimensionParam->f_FlickerPeriod_us);

        /*The target frame length in us is*/
        currFrameDimensionParam->f_FrameLength_us = u32_FrameLengthFlickerPeriod * currFrameDimensionParam->f_FlickerPeriod_us;

        /*Calculate the frame length in terms of lines and us*/
        currFrameDimensionParam->u16_VTFrameLengthLines = GenericFunctions_Ceiling(
            (float_t) currFrameDimensionParam->f_FrameLength_us /
                (
                    currFrameDimensionParam->u16_VTLineLengthPck *
                    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us
                ));
    }
    else
    {
        currFrameDimensionParam->f_FlickerPeriod_us = currFrameDimensionParam->u16_VTLineLengthPck * g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us;
    }


    currFrameDimensionParam->f_FrameLength_us = currFrameDimensionParam->u16_VTFrameLengthLines *
        currFrameDimensionParam->u16_VTLineLengthPck *
        g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us;

    return (retVal);

}


/* LLA_IMX072_UpdateGainAndExposure:
This function will update gain and exposure time of sensor
*/
CAM_DRV_FN_RETVAL_E    LLA_IMX072_UpdateGainAndExposure(CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T * p_config_gain_and_exp_time,uint8 trial_only)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    CamFrameDimension_ts                   * ptr_currFrameDimensionParam = NULL;
    CamSensorGainControl_ts                * ptr_currGainConfigParam = NULL;

    uint16_t            u16_AnalogGainCode;     /*will be programmed in sensor*/
    uint16_t            u16_TargetAnalogGain;

    uint16_t            u16_DigitalGainCode;    /*will be programmed in sensor*/
    uint16_t            u16_TargetDigitalGain;

    uint16_t            u16_CoarseIntegrationLines,
                            u16_FineIntegrationPixels;
    uint16_t            u16_numerator = 0, u16_denominator = 0; //for delay model
    uint32_t            u32_TargetIntegrationTime;

    uint32_t            u32_exposure_time_us = 0;
    uint16_t            u16_analog_gain_x1000 = 0;

    float_t             digitalGainAdjustment;

    /*Parameter validations*/
    if (NULL == p_config_gain_and_exp_time)
    {
        LLA_ASSERT_XP70();
    }

  /*If in trial mode, then initlize pointer with local struct, else with global struct*/
    if (Flag_e_TRUE == trial_only)
    {
        ptr_currFrameDimensionParam  = &g_IMX072_CamGlbConfig.FrameDimensionParam_TrialMode;
        ptr_currGainConfigParam = &g_IMX072_CamGlbConfig.SensorGainControl_TrialMode;
    }
    else
    {
        ptr_currFrameDimensionParam  = &g_IMX072_CamGlbConfig.currFrameDimensionParam;;
        ptr_currGainConfigParam = &g_IMX072_CamGlbConfig.currGainConfigParam;
    }

    u16_TargetAnalogGain = p_config_gain_and_exp_time->analog_gain_x1000;
    u16_TargetDigitalGain = p_config_gain_and_exp_time->digital_gain_x1000;

    /*No need to stop streaming for absorbing gain related parameters*/

    /*1. Calculate Analog gain code from analog gain passed by client*/
    {
        /*
                    analog_gain_x1000           m0 * x + c0
                ----------------------  =  ----------------
                        1000                    m1 * x + c1

                                            1000 * c0 - analog_gain_x1000 * c1
                Thus x (analog gain code) =  -------------------------------------
                                            analog_gain_x1000 * m1 - m0 * 1000

            */
        if (u16_TargetAnalogGain < 1000)
        {
            u16_TargetAnalogGain = 1000;
        }


        u16_AnalogGainCode = (uint16_t)
            (
                (float_t)
                    (
                        1000 *  g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0 -
                        u16_TargetAnalogGain *  g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                    ) /
                        (
                            u16_TargetAnalogGain * g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 -
                            g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                            1000
                        )
            );

        /*Analog gain code must be with in max and min analog gain code*/
        u16_AnalogGainCode = clip(
            u16_AnalogGainCode,
            g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_min,
            g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_max);
    }


    /*2. Calculate digital gain code*/
    {
       //<Sudeep>   Digital gain implementation in LLCD if RAW NVM is selected
       if (g_u8GetRawNVMData == Flag_e_TRUE)
        {
           digitalGainAdjustment = u16_TargetAnalogGain /
                                            ((float_t)( g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
                                                        u16_AnalogGainCode +
                                                        g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
                                                      ) *1000 /
                                                      (
                                                        g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                                                        u16_AnalogGainCode +
                                                        g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                                                 ));

            u16_DigitalGainCode = (uint16_t)(digitalGainAdjustment * 256);

        }

       else
        {

          if (CAM_DRV_SENS_GAIN_NONE == g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.gain_type)
          {
              /*Digital gain not suported by sensor, ignore the parameter*/
          }
          else
          {
              if (u16_TargetDigitalGain < g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.min_gain_x1000)
              {
                  u16_TargetDigitalGain = g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.min_gain_x1000;
              }
              else if (u16_TargetDigitalGain > g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.max_gain_x1000)
              {
                  u16_TargetDigitalGain = g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.max_gain_x1000;
              }


              /*Digital gain code*/
              u16_DigitalGainCode = (u16_TargetDigitalGain / 1000) << 8;
          }
        }
      }


    /*3. Integration time calculation*/
    {
        float_t     f_ActualFlickerFreeBunches;
        uint16_t    u16_FlickerFreeBunches, u16_MaxFlickerFreeBunches;

        u32_TargetIntegrationTime = p_config_gain_and_exp_time->exposure_time_us;

        /*Do anti flickering adjustment only if exposure time is more than flicker period*/
        if(u32_TargetIntegrationTime > (uint32_t)g_IMX072_CamGlbConfig.currFrameDimensionParam.f_FlickerPeriod_us && Flag_e_TRUE == LLA_FLICKER_COMPATIABLE_TIMING)
        {
            /*calculate the number of flicker free bunches from the desired exposure time*/
            f_ActualFlickerFreeBunches = (float_t)(u32_TargetIntegrationTime / g_IMX072_CamGlbConfig.currFrameDimensionParam.f_FlickerPeriod_us);
            if(f_ActualFlickerFreeBunches < 1.0)
            {
                f_ActualFlickerFreeBunches = 1.0;
            }
            u16_FlickerFreeBunches = (uint16_t) f_ActualFlickerFreeBunches;
            u16_MaxFlickerFreeBunches = (uint16_t)
            (
              (float_t)
                (
                   (
                      ptr_currFrameDimensionParam ->u16_VTFrameLengthLines -
                      g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin
                    ) *
                       (
                         ptr_currFrameDimensionParam ->u16_VTLineLengthPck *
                         g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us
                        )
                 )
                          / ptr_currFrameDimensionParam ->f_FlickerPeriod_us
              );

            /*Clip the value of u16_FlickerFreeBunches*/
            u16_FlickerFreeBunches = min(u16_FlickerFreeBunches, u16_MaxFlickerFreeBunches);

            /*Target integration gain*/
            u32_TargetIntegrationTime = u16_FlickerFreeBunches * ptr_currFrameDimensionParam ->f_FlickerPeriod_us;
        }


        u16_CoarseIntegrationLines = (uint16_t)
            (
                (float_t)
                    (
                        u32_TargetIntegrationTime *
                            g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz /
                        ptr_currFrameDimensionParam ->u16_VTLineLengthPck
                    )+IMX072_EXP_OFFSET_WO_VA
            );

       u16_FineIntegrationPixels = (uint16_t)
            (u32_TargetIntegrationTime *     g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz) %
                (ptr_currFrameDimensionParam ->u16_VTLineLengthPck);


        if (g_IMX072_CamGlbConfig.exposureCap.e_IntegrationCapability == 0 /*Coarse integration gain only*/ )
        {
            u16_FineIntegrationPixels = 0;

            /*Clip the target integration time */
            if
            (
               u16_CoarseIntegrationLines > ptr_currFrameDimensionParam ->u16_VTFrameLengthLines -
                    g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin
            )
            {
                u16_CoarseIntegrationLines = ptr_currFrameDimensionParam ->u16_VTFrameLengthLines - g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin;
            }
            else if (u16_CoarseIntegrationLines < g_IMX072_CamGlbConfig.exposureCap.u16_MinimumCoarseIntegrationLines)
            {
                u16_CoarseIntegrationLines = g_IMX072_CamGlbConfig.exposureCap.u16_MinimumCoarseIntegrationLines;
            }
        }
        else
        {
            /*TODO:<CN>:Need to impelement this case for sensor with fine integration gain also */
            u16_FineIntegrationPixels = g_IMX072_CamGlbConfig.exposureCap.u16_MinimumFineIntegrationPixels;
        }
    }


    if (g_u8GetRawNVMData == Flag_e_TRUE)
    {
         ptr_currGainConfigParam->u16_CurrDigitalGainCode = u16_DigitalGainCode;
    }
    else
    {
        /*in current FW design, unity digital gain is applied to sensor actual DG is programmed in pipe */
        ptr_currGainConfigParam->u16_CurrDigitalGainCode = 0x0100;
    }

    /*Update the Global structure holding these values*/
    ptr_currGainConfigParam->u16_CurrAnalogGainCode = u16_AnalogGainCode;
    ptr_currGainConfigParam->u16_CurrCoarseIntegrationLines = u16_CoarseIntegrationLines;
    ptr_currGainConfigParam->u16_CurrFineIntegrationPixels = u16_FineIntegrationPixels;

    if (g_u8GetRawNVMData == Flag_e_TRUE)
    {
         ptr_currGainConfigParam->u16_CurrDigitalGainCode = u16_DigitalGainCode;
    }
    else
    {
        /*in current FW design, unity digital gain is applied to sensor actual DG is programmed in pipe */
        ptr_currGainConfigParam->u16_CurrDigitalGainCode = 0x0100;
    }

    /*
    In IMX072, while streaming, if exposure is changed, ISL reflect updated exposure immediately in next frame
    but actual application happen on next to next frame i.e. there is one frame lag actual absorption and ISL reporting.
    Analog gain is in syncronization with ISL register and acutual application.
    It is not preferred to snoop ISL for IMX072 sensor due to the above contraints and hence timed-model should be used.
    */

    if(Flag_e_TRUE == LLA_IS_NON_SMIA)
    {
        if(Flag_e_FALSE == trial_only)
        {

            u16_numerator = ((uint16_t) g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 * ptr_currGainConfigParam->u16_CurrAnalogGainCode +
                                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0);

            u16_denominator = ((uint16_t) g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 * ptr_currGainConfigParam->u16_CurrAnalogGainCode +
                                    g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1);

            u16_analog_gain_x1000 = (uint16_t) (((float_t) u16_numerator / u16_denominator) * 1000);

            u32_exposure_time_us = (uint32_t)
                    (
                        (float_t)
                        (
                            (ptr_currGainConfigParam->u16_CurrCoarseIntegrationLines - IMX072_EXP_OFFSET_WO_VA) *
                             ptr_currFrameDimensionParam->u16_VTLineLengthPck
                        ) / g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz
                    );

            if((Flag_e_FALSE == g_CamDrvConfig.flagIsStreaming)/* && (0 == g_TimeModelConfig.u32_FSCCount)*/)
            {
                /*This is case when streaming is not started. Here our main objective is to store the the last configured value in the "FrameAppliedConfig"
                structure so that is can be send in the ISL of the first frame.The non trial mode configuration is stored
                */
                Frame_count_timemodel = 0;
                g_TimeModelConfig.FrameCurrentConfig.exposure_time_us = u32_exposure_time_us;
                g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000 = u16_analog_gain_x1000;
                g_TimeModelConfig.FrameCurrentConfig.digital_gain_x1000 = LLA_DEF_GAIN_x1000;

                g_TimeModelConfig.u32_FrameFSC_ID[Frame_count_timemodel] = g_TimeModelConfig.u32_FSCCount;
                g_TimeModelConfig.FrameAppliedConfig[Frame_count_timemodel].exposure_time_us = g_TimeModelConfig.FrameCurrentConfig.exposure_time_us ;

                /*Update actual analog gain*/
                g_TimeModelConfig.FrameAppliedConfig[Frame_count_timemodel].analog_gain_x1000 = g_TimeModelConfig.FrameCurrentConfig.analog_gain_x1000;
                g_TimeModelConfig.FrameAppliedConfig[Frame_count_timemodel].digital_gain_x1000 = LLA_DEF_GAIN_x1000;
            }
            else
            {
                //DM
                /*In this case whenever the update request is coming (Lets say for N th frame, we are storing its FSC count and exposure, AG and DG
                in FrameAppliedConfig structure so that is can be send in the ISL of the N+2 frame.*/
                Frame_count_timemodel = (Frame_count_timemodel+1)%MAX_FRAME_COUNT;

                g_TimeModelConfig.u32_FrameFSC_ID[Frame_count_timemodel] = g_TimeModelConfig.u32_FSCCount;
                g_TimeModelConfig.FrameAppliedConfig[Frame_count_timemodel].exposure_time_us = u32_exposure_time_us;

                /*Update actual analog gain*/
                g_TimeModelConfig.FrameAppliedConfig[Frame_count_timemodel].analog_gain_x1000 = u16_analog_gain_x1000;
                g_TimeModelConfig.FrameAppliedConfig[Frame_count_timemodel].digital_gain_x1000 = LLA_DEF_GAIN_x1000;
            }
        }
    }

    return (retVal);
}


/*LLA_IMX072_UpdateFeedback:
This function will get the feedback from the sensor about the latest settings
*/
CAM_DRV_RETVAL_E        LLA_IMX072_UpdateFeedback(CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback, uint8 trial_only)

{
    CamFrameDimension_ts       *currFrameDimensionParam = NULL;
    CamSensorGainControl_ts    *currGainConfigParam = NULL;
    CAM_DRV_RETVAL_E            retVal = CAM_DRV_OK;
    uint8_t                     u8_SubSampleFactor;

    /*If in trial mode, then initlize pointer with local struct, else with global struct*/
    if (Flag_e_TRUE == trial_only)
    {
        currFrameDimensionParam = &g_IMX072_CamGlbConfig.FrameDimensionParam_TrialMode;
        currGainConfigParam = &g_IMX072_CamGlbConfig.SensorGainControl_TrialMode;
    }
    else
    {
        currFrameDimensionParam = &g_IMX072_CamGlbConfig.currFrameDimensionParam;;
        currGainConfigParam = &g_IMX072_CamGlbConfig.currGainConfigParam;
    }


    /*Update WOI and OutputResolution*/
    p_config_feedback->woi_res.width = currFrameDimensionParam->u16_WOIXSize;
    p_config_feedback->woi_res.height = currFrameDimensionParam->u16_WOIYSize;
    p_config_feedback->output_res.width = currFrameDimensionParam->u16_OPXSize;
    p_config_feedback->output_res.height = currFrameDimensionParam->u16_OPYSize;
    /* Subsamling = (odd + even) /2;    odd = 2 * subsampling(WOI/OP) - even(1) */

 //   OstTraceInt4(TRACE_USER1, "FEEDBACK woi w: %d, H: %d, OUTPUT w: %d, h: %d", p_config_feedback->woi_res.width, p_config_feedback->woi_res.height, p_config_feedback->output_res.width, p_config_feedback->output_res.height);

   /* To Avoid divide by zero in arm*/
   if (p_config_feedback->output_res.width != 0)
   {

       p_config_feedback->sub_sampling_factor =
             (((p_config_feedback->woi_res.width/p_config_feedback->output_res.width) & 0xF) << 4)
         |     (((p_config_feedback->woi_res.width/p_config_feedback->output_res.width) & 0xF));
   }
   else
   {
       p_config_feedback->sub_sampling_factor = 0;
   }

   if (currFrameDimensionParam->u16_OPXSize !=0)
   {
   u8_SubSampleFactor = currFrameDimensionParam->u16_WOIXSize / currFrameDimensionParam->u16_OPXSize;
   }
   else
   {
      u8_SubSampleFactor = 1;
   }

    if(u8_SubSampleFactor < 1)
        u8_SubSampleFactor = 1;


    /*Update Exposure and gain related parameters*/

    /*Update actual analog gain*/
    p_config_feedback->analog_gain.numerator = (uint16_t) g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
        currGainConfigParam->u16_CurrAnalogGainCode +
        g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0;
    p_config_feedback->analog_gain.denominator = (uint16_t) g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
        currGainConfigParam->u16_CurrAnalogGainCode +
        g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1;
    p_config_feedback->analog_gain_x1000 = (uint16_t) (((float_t) p_config_feedback->analog_gain.numerator / p_config_feedback->analog_gain.denominator) * 1000);

    /*Update digita gain - Unity DG has been programmed in sensor*/

    //p_config_feedback->digital_gain_x1000 = (uint16_t) ((currFrameDimensionParam->u16_CurrDigitalGainCode * 1000) >> 8);
    p_config_feedback->digital_gain_x1000 = LLA_DEF_GAIN_x1000;
    p_config_feedback->digital_gain.numerator = 1;      /*Unity DG gain programmed in sensor*/
    p_config_feedback->digital_gain.denominator = 1;    /*Unity DG gain programmed in sensor*/

    /*Binning not used*/
    p_config_feedback->binning_type = CAM_DRV_BINNING_NONE;
    p_config_feedback->scaling_type = CAM_DRV_SCALING_NONE;


    p_config_feedback->vt_pix_clk_freq =   g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz;
    p_config_feedback->op_pix_clk_freq =  g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_OPPixelClockFrequency_Mhz / u8_SubSampleFactor;

    p_config_feedback->line_length_pck = currFrameDimensionParam->u16_VTLineLengthPck;
    p_config_feedback->max_line_length_pck = currFrameDimensionParam->u16_MaxLineLenPckInCurrConf / u8_SubSampleFactor;

    /*Update line readout time in ns*/
    p_config_feedback->line_readout_time_ns = (float_t) currFrameDimensionParam->u16_VTLineLengthPck *
        g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us *
        1000.0;

    /*Update lineBlanking time in bits*/
    p_config_feedback->line_blanking_bits = (uint32_t) currFrameDimensionParam->u16_VTLineLengthPck * (g_IMX072_CamGlbConfig.camDrvMiscDetails.csiRawFormat & 0xFF);

    /*Update CCP speed, OpFreqency*/
    p_config_feedback->ccp_speed_khz = (uint32_t) g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_OPSystemClockFrequency_Mhz * 1000; /*Convert into KHZ*/

    /*Update Exposure time*/
    p_config_feedback->exposure_time_us = (uint32_t)
        (
            (float_t)
                (
                    (currGainConfigParam->u16_CurrCoarseIntegrationLines - IMX072_EXP_OFFSET_WO_VA) *
                    currFrameDimensionParam->u16_VTLineLengthPck
                ) / g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz
        );


    p_config_feedback->exposure_time_margin_us = (uint16)
        (
            ( float )
                (
                    g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin *
                    currFrameDimensionParam->u16_VTLineLengthPck
                ) / g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz
        );

    /*Update Frame Rate
                                1000000
            As frame_rate =     -------------------------       [Application notes pg55]
                                vt_frame_length_us
            vt_frame_length_lines * vt_line_length_us
            */
    p_config_feedback->frame_rate_x100 = (uint16_t)
        (
            (float_t) 1000000.0 * 100.0 /
                (
                    currFrameDimensionParam->u16_VTFrameLengthLines *
                    currFrameDimensionParam->u16_VTLineLengthPck *
                    g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us
                )
        );

//   //OstTraceInt1(TRACE_DEBUG, "LLA_IMX072_UpdateFeedback: p_config_feedback->frame_rate_x100: %d",p_config_feedback->frame_rate_x100);

    //Updating frame format in feedback
    p_config_feedback->frame_format.anc_lines_top = NO_OF_ISL_FROM_SENSOR;
    p_config_feedback->frame_format.anc_lines_bottom = 0;
    p_config_feedback->frame_format.black_pixels_capability = (CAM_DRV_SENS_BLACK_PIXEL_E)1;
    p_config_feedback->frame_format.black_pixels_left = 0;
    p_config_feedback->frame_format.black_pixels_right = 0;
     p_config_feedback->frame_format.black_pixels_top = 0;
    p_config_feedback->frame_format.black_pixels_bottom = 0;
    p_config_feedback->frame_format.dummy_pixel_position = (CAM_DRV_SENS_DUMMY_PIXEL_POS_E)0;
    p_config_feedback->frame_format.dummy_pixels_left = 0;
    p_config_feedback->frame_format.dummy_pixels_right = 0;
    p_config_feedback->frame_format.dummy_pixels_top = 0;
    p_config_feedback->frame_format.dummy_pixels_bottom = 0;

     /* Updating Current Exposure time in feedback #ER 442024*/
    p_config_feedback->current_et_capability.exp_time_step = p_config_feedback->line_readout_time_ns / 1000;

    /* Updating max_exp_time_us  time in feedback #ER 446192*/
    p_config_feedback->current_et_capability.max_exp_time_us = (p_config_feedback->line_readout_time_ns * (currFrameDimensionParam->u16_VTFrameLengthLines - g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin))/1000;
    p_config_feedback->current_et_capability.min_exp_time_us = p_config_feedback->current_et_capability.exp_time_step;

    // initializing the focal length element
     p_config_feedback->focal_length_x100 = FOCAL_LENGTH_DEFAULT_VALUE_IMX072_X100;

    /*TODO: CN:
        config_line_number
        binning_factor
        test_picture_fix_pattern_data
        */
    return (retVal);

}

/* This function will update LLD state variables for IMX072 sensor
 * for Flash requests from the firmware
 */
CAM_DRV_RETVAL_E
LLA_IMX072_UpdateFlash(
CAM_DRV_CONFIG_FLASH_STROBES_T    *p_config_flash_strobe)
{
   CAM_DRV_RETVAL_E retVal = CAM_DRV_OK ;
#if (LLA_FLASH_LED == 0) // If Xenon Flash
   g_u8StrobeLength = p_config_flash_strobe->p_each_strobe_config->strobe_length;
#endif /* (LLA_FLASH_LED == 0) */

   return retVal;

}

CAM_DRV_RETVAL_E LLA_LowLevel_ActivateFlash(FlashState_te* p_e_FlashState, uint8_t FlashEn, uint32_t *pu32_FlashAfterDelayOf_us)
{
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK ;
    static bool_t sbFlashDelayTimerFired = FALSE;

    CamFrameDimension_ts       *currFrameDimensionParam = NULL;
    CamSensorGainControl_ts    *currGainConfigParam = NULL;

    currFrameDimensionParam = &g_IMX072_CamGlbConfig.currFrameDimensionParam;
    currGainConfigParam = &g_IMX072_CamGlbConfig.currGainConfigParam;

    if(FLASH_TYPE == FLASHTYPE_SENSORDRIVEN)
    {
        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize( REG_FLASH_CONTROL, 1, &FlashEn));
    }
    else
    {
        if((Flash_e_Requested == *p_e_FlashState) || (Flash_e_Fired == *p_e_FlashState))
        {
            if(FALSE == sbFlashDelayTimerFired)
            {
                // Update the timer value
                sbFlashDelayTimerFired = TRUE;

                // Calculate the delay duration after which the Flash is enabled
                *pu32_FlashAfterDelayOf_us =
                (uint32_t)((float_t)(currFrameDimensionParam->u16_VTFrameLengthLines -
                   currGainConfigParam->u16_CurrCoarseIntegrationLines) *
                    ( g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us));
            }
            else
            {
                *pu32_FlashAfterDelayOf_us = (uint32_t)NULL;
                g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_additional_physical_pins_fn( (CAM_DRV_SELECT_CAMERA_E)NULL, CAM_DRV_PHYSICAL_PIN_FLASH, CAM_DRV_PIN_ACTION_ACTIVATE);
            }
        }
        else if(Flash_e_ReportFrameLit == *p_e_FlashState)
        {
            g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_additional_physical_pins_fn((CAM_DRV_SELECT_CAMERA_E) NULL, CAM_DRV_PHYSICAL_PIN_FLASH, CAM_DRV_PIN_ACTION_DEACTIVATE);
            sbFlashDelayTimerFired = FALSE;
            *pu32_FlashAfterDelayOf_us = (uint32_t)NULL;
        }
    }

    return retVal;

}




/* This function will write the Flash registers for IMX072 sensor based on the Flash state variable
*/
CAM_DRV_RETVAL_E
LLA_LowLevel_TriggerrFlash(FlashState_te* p_e_FlashState, uint32_t* pu32_FlashAfterDelayOf_us)
{
   CAM_DRV_RETVAL_E retVal = CAM_DRV_OK ;
   uint8_t FlashEn = 0 ;

   //OstTraceInt1(TRACE_DEBUG,"LLA_LowLevel_TriggerrFlash p_e_FlashState: %d",*p_e_FlashState);

#if (LLA_FLASH_LED == 0) //XENON FLASH

   uint8_t FlashPlStep = 0;

#endif /*    (!(LLA_FLASH_LED == 1)) */

   if((Flash_e_Requested == *p_e_FlashState) || (Flash_e_Fired == *p_e_FlashState))
   {

#if (LLA_FLASH_LED == 1) //LED FLASH
       // write register
       FlashEn = ENABLE_FLASH_LED;

#else  // Xenon Flash
       FlashEn = ENABLE_FLASH_XENON ;

      if ( g_u8StrobeLength < (
                               ( 128 * 1 * 64 ) / ((uint32) g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz)
                              )
         )
      {
          FlashPlStep = (
                         ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz)) *  g_u8StrobeLength
                        )
                        /
                        (128 * 1 );
          FlashPlStep = FlashPlStep << 2;
          //FlashPlStep += 0;
      }
      else if ( g_u8StrobeLength < (
                                    ( 128 * 2 * 64 ) / ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz))
                                   )
              )
      {
          FlashPlStep = (
                         ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz)) *  g_u8StrobeLength
                        )
                        /
                        (128 * 2 );
          FlashPlStep = FlashPlStep << 2;
          FlashPlStep += 1;
      }
      else if ( g_u8StrobeLength < (
                                    ( 128 * 4 * 64 ) / ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz))
                                   )
              )
      {
          FlashPlStep = (
                         ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz)) *  g_u8StrobeLength
                        )
                        /
                        (128 * 4 );
          FlashPlStep = FlashPlStep << 2;
          FlashPlStep += 2;
      }
      else if ( g_u8StrobeLength < (
                                    ( 128 * 8 * 64 ) / ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz))
                                   )
              )
      {
          FlashPlStep = (
                         ((uint32) (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz)) *  g_u8StrobeLength
                        )
                        /
                        (128 * 8 );
          FlashPlStep = FlashPlStep << 2;
          FlashPlStep += 3;
      }


#endif /* (LLA_FLASH_LED == 1) */

      // Write Flash register
      //OstTraceInt0(TRACE_FLOW,"Write Flash register!!!");
      retVal  = LLA_LowLevel_ActivateFlash(p_e_FlashState, FlashEn, pu32_FlashAfterDelayOf_us);
   }

   else if(Flash_e_ReportFrameLit == *p_e_FlashState)
   {
        // Disable Flash
        //Write default value to Flash register
        //OstTraceInt0(TRACE_FLOW,"if((Flash_e_ReportFrameLit == *p_e_FlashState)");
        FlashEn = DISABLE_FLASH_REG;
        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_LowLevel_ActivateFlash(p_e_FlashState, FlashEn, pu32_FlashAfterDelayOf_us));
   }

   return retVal;
}



/* This function choose the appropriate path of execution for "handle message FSC"
 * based on If sensor is SMIA compliant
 * or not.
 */
CAM_DRV_RETVAL_E
LLA_IMX072_TriggerFlash(
FlashState_te   *p_e_FlashState)
{
    CAM_DRV_EVENT_INFO_U    event_info;
    CAM_DRV_RETVAL_E        retVal = CAM_DRV_OK;

 //OstTraceInt0(TRACE_FLOW,"LLA_IMX072_TriggerFlash!!");

    uint32_t u32_FlashAfterDelayOf_us = (uint32_t)0;
    retVal = LLA_LowLevel_TriggerrFlash(p_e_FlashState, &u32_FlashAfterDelayOf_us);

    /*Check if required to start the timer*/
    if(NULL != u32_FlashAfterDelayOf_us)
    {
        /*Start the timer, in granularity of 10us*/
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_int_timer_fn(CAM_DRV_INT_TIMER_1,
            u32_FlashAfterDelayOf_us / 10);
    }

    if (CAM_DRV_OK != retVal)
    {
        event_info.error_id = CAM_DRV_EVENT_ERROR_FLASH_STROBE;
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_EVENT_ERROR, &event_info, FALSE);
    }

    return (retVal);
}



/*Generic function to read from IMX072 EEPROM*/
CAM_DRV_RETVAL_E     LLA_NVMMAP_Read( uint16_t    addr,uint8_t     size,uint8_t     *u8_ptr)

{   CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    int                 count = size,
                        readCount = 0;
    uint16_t            nvmAddr = addr;
    uint8_t             devAddr = 0;

    /*Caller responsibility to make sure that passed parameters are correct*/
    do
    {
        /*At max NVM_MAX_READ_BYTE can be read from EEPROM*/
        if (count > NVM_MAX_READ_BYTE)
        {
            readCount = NVM_MAX_READ_BYTE;
            count = count - NVM_MAX_READ_BYTE;
        }
        else
        {
            readCount = count;
            count = 0;
        }


        /*Create DevAddr*/
        devAddr = 0;
        devAddr = (0xA << 4) | (((nvmAddr >> 8) & 0x7/*three most significant bits of nvm addr*/) << 1);

        /*Read 'readCount* from EEPROM*/
        retVal = LLA_READ_NVM_BYTES(devAddr,(nvmAddr & 0xFF),readCount,u8_ptr);

        if(CAM_DRV_FN_OK != retVal)
        {
            /*Error in reading NVM break from the loop*/
            break;
        }

        /*Increment the buffer pointer and address accordingly*/
        u8_ptr = u8_ptr + readCount;
        nvmAddr = nvmAddr + readCount;

    }while(count > 0);

    if(CAM_DRV_FN_OK != retVal)
        return CAM_DRV_ERROR_HW;
    else
        return CAM_DRV_OK;

}

/*module ID string for IMX072 is directly read from NVM*/
void
LLA_IMX072_NVMReadModuleIdString(char *idString, uint8_t size)
{
    /*ModuleIdInformation is at 0x000 in NVM*/
    LLA_NVMMAP_Read(0x000, size, (uint8_t *) idString);
}



/*Update the Id string to be return to the client*/
void
LLA_IMX072_UpdateIdString(uint8_t *u8_ModelInfo, uint8_t* IdString)
{
    char u8_NvmIdstring[3];

    /*For IMX072 module id_string used to identify the camera module in used, and read directly from NVM of camera module*/

    /*Read Manufacturer info from NVM*/
    LLA_IMX072_NVMReadModuleIdString(u8_NvmIdstring, 0x3 /*size of id-string*/);

    /*Update the IdString accordingly*/
    if(u8_NvmIdstring[0] == 'S' && u8_NvmIdstring[1] == 'T' && u8_NvmIdstring [2] == 'W')
    {
        IdString[0] = '0';
        IdString[1] = LLA_IMX072_MODEL_ID_STW;
        g_u8SensorModelId = LLA_IMX072_MODEL_ID_STW;
    }
    else if(u8_NvmIdstring[0] == 'K' && u8_NvmIdstring[1] == 'M' && u8_NvmIdstring [2] == 'O')
    {
        IdString[0] = '0';
        IdString[1] = LLA_IMX072_MODEL_ID_KMO;
        g_u8SensorModelId = LLA_IMX072_MODEL_ID_KMO;
    }
    else
    {
        IdString[0] = '0';
        IdString[1] = LLA_IMX072_MODEL_ID_OTH;
        g_u8SensorModelId = LLA_IMX072_MODEL_ID_OTH;
    }


    IdString[2] = '0' + ((u8_ModelInfo[0] & 0xF0) >> 4);
    IdString[3] = '0' + (u8_ModelInfo[0] & 0x0F);
    IdString[4] = '0' + ((u8_ModelInfo[1] & 0xF0) >> 4);
    IdString[5] = '0' + (u8_ModelInfo[1] & 0x0F);

    /*rev number*/
    IdString[6] = 0x0;
    IdString[7] = '0' + u8_ModelInfo[2];

    return;
}


/*LLA_IMX072_GetParsedNVM
    Returns the filled CAM_DRV_NVM_T structure with NVM data from IMX072
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetParsedNVM(
CAM_DRV_NVM_T   *p_nvm)
{
   CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;
    uint8_t             i,
                        size = 0;
   uint8_t g_GbGrCalibrated = Flag_e_FALSE;
   uint8_t *u8_nvmStartAddress=NULL;
    uint32_t            u32_NextFreeMemoryOffset = 0;

   u8_nvmStartAddress = (uint8 *) p_nvm;
    u32_NextFreeMemoryOffset = sizeof(CAM_DRV_NVM_T);

    // This is a check against over-writing the buffer
    if (u32_NextFreeMemoryOffset > g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size)
   {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "!!ERROR!!-> 1 LLA_IMX072_GetParsedNVM()");
#endif
        LLA_ASSERT_XP70();
   }


   /* Initialization */
   p_nvm->ls_data.ls_table_num = 0;

   /* Irrelevant Data */
   p_nvm-> checksum            = 0;      /**< stored checksum */
   p_nvm-> checksum_calculated = 0;  /**< calculated checksum */
   p_nvm-> number_of_errors    = 0;
   p_nvm-> checksum2           = 0;
   p_nvm-> checksum2_calculated= 0;
   p_nvm-> decoder_status      = 0;      /**< decoding err codes if any */

       /* Irrelevant Auto-focus Data */
   p_nvm->af_data.pos_unit_relation_to_cw=0;
   p_nvm->af_data.total_positions_u=0;
   p_nvm->af_data.availability_bitmask_u=0;
   p_nvm->af_data.total_positions_d=0;
   p_nvm->af_data.availability_bitmask_d=0;
   p_nvm->af_data.obj_dis_availability_bitmask=0;
   p_nvm->af_data.size_of_additional_af_data=0;

   /* End of Irrelevant Data */

    /* Maximum sensor resolution -----------------------------------------------------------------------------------------*/
    p_nvm->sens_resolution.width = g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPXOutputSize;
    p_nvm->sens_resolution.height = g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxOPYOutputSize;

    /* Focus lens related NVM data ---------------------------------------------------------------------------------------*/
    p_nvm->af_data.total_positions_h = 5;
    size = 2;

    /*Read focus range for infinity: infinity end*/
    retVal = LLA_NVMMAP_Read(
        LLA_IMX072_AF_RANG_ADDR_INF,
        size,
        ( uint8_t * ) &(p_nvm->af_data.positions_horizontal[0]));
    p_nvm->af_data.positions_horizontal[0] = BSWAP_16(p_nvm->af_data.positions_horizontal[0]);              // Swap (big endian -> little endian)

    /*Read focus range for macro: macro end*/
    retVal = LLA_NVMMAP_Read(
        LLA_IMX072_AF_RANG_ADDR_MACRO,
        size,
        ( uint8_t * ) &(p_nvm->af_data.positions_horizontal[4]));
    p_nvm->af_data.positions_horizontal[4] = BSWAP_16(p_nvm->af_data.positions_horizontal[4]);              // Swap (big endian -> little endian)

    // "p_positions_horizontal" is replaced with array "positions_horizontal". so we dont need to assign the free memory location
    //u32_NextFreeMemoryOffset  += (sizeof(uint16_t) * (LLA_IMX072_AF_DATA_POSN_COUNT));
    // This is a check against over-writing the buffer
    //if ( u32_NextFreeMemoryOffset >  g_CamDrvConfig.camNVMDetail.parsed_nvm_size )
    //{
    // return CAM_DRV_ERROR_MEMORY;
    //}
    for (i = 0; i < LLA_IMX072_AF_DATA_POSN_COUNT; i++)
    {
        /*
              switch(i)
              {
                 case 0:retVal |= LLA_NVMMAP_Read(LLA_IMX072_AF_DATA_ADDR_INF , size, (uint8_t *) &(p_nvm->p_positions_horizontal[i]));
                        break;
                 case 1:retVal |= LLA_NVMMAP_Read(LLA_IMX072_AF_DATA_ADDR_1M, size, (uint8_t *) &(p_nvm->p_positions_horizontal[i]));
                        break;
                 case 2:retVal |= LLA_NVMMAP_Read(LLA_IMX072_AF_DATA_ADDR_MACRO, size, (uint8_t *) &(p_nvm->p_positions_horizontal[i]));
                        break;
              }
              */
         retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(
            LLA_IMX072_AF_DATA_ADDR_INF + (i * 2),
            size,
            ( uint8_t * ) &(p_nvm->af_data.positions_horizontal[i + 1])));
        p_nvm->af_data.positions_horizontal[i + 1] = BSWAP_16(p_nvm->af_data.positions_horizontal[i + 1]);  // Swap (big endian -> little endian)
    }


    p_nvm->af_data.availability_bitmask_h = SOC_exist | INF_exist | C_exist | D_exist | EOC_exist;

    /* Module Information Region -----------------------------------------------------------------------------------------*/
   size = 1;
    retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read( LLA_IMX072_ADDR_GbGr_CALIBRATION, size, (uint8_t *) &(g_GbGrCalibrated)));

   // This is just to check against a ill-programmed NVM
   if ( g_GbGrCalibrated != LLA_IMX072_GbGr_CALIBRATED)
   {
      g_GbGrCalibrated = LLA_IMX072_GbGr_NOT_CALIBRATED;
   }


    /* Sensitivity related NVM data --------------------------------------------------------------------------------------*/
    CAM_DRV_NVM_SENSITIVITY_DATA_T  *p_sens;
    size = 1;

     retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(
        LLA_IMX072_ADDR_WB_TEMP_COUNT,
        size,
        ( uint8_t * ) &(p_nvm->sensitivity_data.sensitivity_colour_temps)));             // in how many colour temperatures values are measured

     // Hardcode no of sensitivity colour temperatures at which sensor is calibrated... <AG>
           p_nvm->sensitivity_data.sensitivity_colour_temps = LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED;

    /* Check if NVM has valid data for number of Color temperatures for which WB data is measured, if not we assume that there is no WB data */
    if
    (
        p_nvm->sensitivity_data.sensitivity_colour_temps < LLA_IMX072_MIN_COLOR_TEMP_CALIBRATED
    ||  p_nvm->sensitivity_data.sensitivity_colour_temps > LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED
    )
    {
        // Number of Color temperaures specified in the NVM has a invalid value so we assume no WB data is present in NVM
        p_nvm->sensitivity_data.sensitivity_colour_temps = LLA_IMX072_INVALID_CHANNEL_COUNT_CALIBRATED;
    }


    // Valid NVM data present for WB, so now it is read and parsed to be filled in data strucutures
    size = 4;

    if (p_nvm->sensitivity_data.sensitivity_colour_temps > 0)
    {
        p_sens = ( CAM_DRV_NVM_SENSITIVITY_DATA_T * ) &u8_nvmStartAddress[u32_NextFreeMemoryOffset];
        p_nvm->offset_sensitivity_data = u32_NextFreeMemoryOffset;
        p_nvm->sensitivity_data.sens_offset = (uint32) p_sens - (uint32) & p_nvm->sensitivity_data;
        u32_NextFreeMemoryOffset +=
            (
                sizeof(CAM_DRV_NVM_SENSITIVITY_DATA_T) *
                (p_nvm->sensitivity_data.sensitivity_colour_temps)
            );

        // This is a check against over-writing the buffer
        if (u32_NextFreeMemoryOffset > g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size)
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "!!ERROR!!-> 2 LLA_IMX072_GetParsedNVM()");
#endif
            LLA_ASSERT_XP70();
        }
    }


    for (i = 0; i < p_nvm->sensitivity_data.sensitivity_colour_temps; i++)
    {
        uint16_t    nvm_addr_wb_RedGreen = 0,
                    nvm_addr_wb_BlueGreen = 0,
                    nvm_addr_wb_GbGr = 0;
        uint32_t    nvm_data_wb_RedGreen = 0,
                    nvm_data_wb_BlueGreen = 0,
                    nvm_data_wb_GbGr = 0;

        switch (i)
        {
            case 0:
                p_sens[i].x_value_x1000 = LLA_IMX072_CIE1931_X_HIGH_TEMP * 1000;
                p_sens[i].y_value_x1000 = LLA_IMX072_CIE1931_Y_HIGH_TEMP * 1000;
                nvm_addr_wb_RedGreen = LLA_IMX072_WB_DATA_ADDR_HIGH_RED_GREEN;
                nvm_addr_wb_BlueGreen = LLA_IMX072_WB_DATA_ADDR_HIGH_BLUE_GREEN;
                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                    nvm_addr_wb_GbGr = LLA_IMX072_WB_DATA_ADDR_HIGH_Gb_Gr;
                }


                break;

            case 1:
                p_sens[i].x_value_x1000 = LLA_IMX072_CIE1931_X_LOW_TEMP * 1000;
                p_sens[i].y_value_x1000 = LLA_IMX072_CIE1931_Y_LOW_TEMP * 1000;
                nvm_addr_wb_RedGreen = LLA_IMX072_WB_DATA_ADDR_LOW_RED_GREEN;
                nvm_addr_wb_BlueGreen = LLA_IMX072_WB_DATA_ADDR_LOW_BLUE_GREEN;
                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                    nvm_addr_wb_GbGr = LLA_IMX072_WB_DATA_ADDR_LOW_Gb_Gr;
                }


                break;

            case 2:
                p_sens[i].x_value_x1000 = LLA_IMX072_CIE1931_X_FlUORESCENT_TEMP * 1000;
                p_sens[i].y_value_x1000 = LLA_IMX072_CIE1931_Y_FlUORESCENT_TEMP * 1000;
                nvm_addr_wb_RedGreen = LLA_IMX072_WB_DATA_ADDR_FlUORESCENT_RED_GREEN;
                nvm_addr_wb_BlueGreen = LLA_IMX072_WB_DATA_ADDR_FlUORESCENT_BLUE_GREEN;
                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                    nvm_addr_wb_GbGr = LLA_IMX072_WB_DATA_ADDR_FlUORESCENT_Gb_Gr;
                }


                break;

            default:
#if LLA_ASSERT_LOG_EN
                OstTraceInt0(TRACE_ERROR, "!!ERROR!!-> DEFAULT ONE!!! LLA_IMX072_GetParsedNVM()");
#endif
                LLA_ASSERT_XP70();
                break;
        }


         retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t) LLA_NVMMAP_Read(nvm_addr_wb_RedGreen, size, ( uint8_t * ) &(nvm_data_wb_RedGreen)));
         retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_wb_BlueGreen, size, ( uint8_t * ) &(nvm_data_wb_BlueGreen)));

        nvm_data_wb_RedGreen = BSWAP_32(nvm_data_wb_RedGreen);                          // Swap (big endian -> little endian)
        nvm_data_wb_BlueGreen = BSWAP_32(nvm_data_wb_BlueGreen);                        // Swap (big endian -> little endian)
        if (Flag_e_TRUE == g_GbGrCalibrated)
        {
            retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_wb_GbGr, size, ( uint8_t * ) &(nvm_data_wb_GbGr)));

            nvm_data_wb_GbGr = BSWAP_32(nvm_data_wb_GbGr);                              // Swap (big endian -> little endian)
        }
        else
        {
            nvm_data_wb_GbGr = LLA_IMX072_SENS_SRC_UNITY_I;
        }


        p_sens[i].sens.r = (uint16_t) ((nvm_data_wb_RedGreen / LLA_IMX072_SENS_SRC_UNITY_F) * LLA_IMX072_SENS_DST_REFERENCE + 0.5f);
        p_sens[i].sens.b = (uint16_t) ((nvm_data_wb_BlueGreen / LLA_IMX072_SENS_SRC_UNITY_F) * LLA_IMX072_SENS_DST_REFERENCE + 0.5f);

        p_sens[i].sens.gr = (uint16_t) ((2.0f / (1.0f + nvm_data_wb_GbGr / LLA_IMX072_SENS_SRC_UNITY_F)) * LLA_IMX072_SENS_DST_REFERENCE + 0.5f);
        p_sens[i].sens.gb = (uint16_t) ((2.0f / (1.0f + LLA_IMX072_SENS_SRC_UNITY_F / nvm_data_wb_GbGr)) * LLA_IMX072_SENS_DST_REFERENCE + 0.5f);
    }


    /* Lens shading related NVM data--------------------------------------------------------------------------------------*/
   uint8* p_ls_measured_lp_nums;
   CAM_DRV_NVM_LS_TABLE_T* p_ls_comTable;
   CAM_DRV_NVM_LSC_DATA_T *p_lsc_data;

   size = 1;
     retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(
        LLA_IMX072_ADDR_LSC_TEMP_COUNT,
        size,
        ( uint8_t * ) &(p_nvm->ls_data.ls_measured_temp_num)));

    if (LLA_IMX072_MODEL_ID_STW == g_u8SensorModelId)
    {
        // Hardcode no of Lens shading colour temperatures at which sensor is calibrated... <AG>
       p_nvm->ls_data.ls_measured_temp_num = LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED;
        }


   // This is just to check against a ill-programmed NVM
    if
    (
        p_nvm->ls_data.ls_measured_temp_num != LLA_IMX072_MIN_COLOR_TEMP_CALIBRATED
    &&  p_nvm->ls_data.ls_measured_temp_num != LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED
      )
   {
      // Number of Color temperaures specified in the NVM has a invalid value so we assume no LSC data is present in NVM
      p_nvm->ls_data.ls_measured_temp_num = LLA_IMX072_INVALID_CHANNEL_COUNT_CALIBRATED;
    }


   // Valid NVM data present for LSC, so now it is read and parsed to be filled in data strucutures
   else
   {
        p_ls_measured_lp_nums = ( uint8 * ) &u8_nvmStartAddress[u32_NextFreeMemoryOffset];
        p_nvm->offset_ls_no_of_lens_positions = u32_NextFreeMemoryOffset;
          p_nvm->ls_data.ls_measured_lp_nums_offset = (uint32)p_ls_measured_lp_nums-(uint32)&p_nvm->ls_data;

   //Only ONE lens position values are measured in each of temperatures
   for ( i = 0; i < p_nvm->ls_data.ls_measured_temp_num; i++)
   {
          p_ls_measured_lp_nums[i] = LLA_IMX072_LSC_LENS_POSN_COUNT;
          p_nvm->ls_data.ls_table_num += p_ls_measured_lp_nums[i];
   }


   // Choose the next free memory address in the buffer with 4byte alignment
   if (0 == ((p_nvm->ls_data.ls_measured_temp_num * sizeof(p_nvm->ls_data.ls_measured_temp_num)) % 4))
   {
            u32_NextFreeMemoryOffset +=
                (
                    p_nvm->ls_data.ls_measured_temp_num *
                    sizeof(p_nvm->ls_data.ls_measured_temp_num)
                );
   }
   else
   {
            u32_NextFreeMemoryOffset +=
                (
                    (p_nvm->ls_data.ls_measured_temp_num * sizeof(p_nvm->ls_data.ls_measured_temp_num)) -
                    ((p_nvm->ls_data.ls_measured_temp_num * sizeof(p_nvm->ls_data.ls_measured_temp_num)) % 4)
                ) +
                4;
   }

    if (p_nvm->ls_data.ls_table_num > 0)
    {
        p_ls_comTable = ( CAM_DRV_NVM_LS_TABLE_T * ) &u8_nvmStartAddress[u32_NextFreeMemoryOffset];
        p_nvm->ls_data.ls_comTable_offset = (uint32) p_ls_comTable - (uint32) & p_nvm->ls_data;
        p_nvm->offset_com_tables = u32_NextFreeMemoryOffset;
        u32_NextFreeMemoryOffset += ((p_nvm->ls_data.ls_table_num) * sizeof(CAM_DRV_NVM_LS_TABLE_T));

        // Offset of first LSC data
        p_nvm->offset_ls_lsc_data = u32_NextFreeMemoryOffset;
    }


    for (i = 0; i < p_nvm->ls_data.ls_table_num; i++)
    {
        uint8_t     k = 0;
        uint16_t    nvm_addr_lsc_red = 0,
                    nvm_addr_lsc_blue = 0,
                    nvm_addr_lsc_green = 0,
                    nvm_addr_lsc_GbGr = 0;

        p_ls_comTable[i].x_division = LLA_IMX072_LSC_GRID_HORIZ_COUNT;
        p_ls_comTable[i].y_division = LLA_IMX072_LSC_GRID_VERTI_COUNT;

        if (Flag_e_TRUE == g_GbGrCalibrated)
        {
            p_ls_comTable[i].ch = LLA_IMX072_RGBGrGb_CALIBRATED;
        }
        else
        {
            p_ls_comTable[i].ch = LLA_IMX072_RGB_CALIBRATED;
        }


        p_ls_comTable[i].center_bit_depth = LLA_IMX072_LSC_DST_ABS_BIT_DEPTH;
        p_ls_comTable[i].other_bit_depth = LLA_IMX072_LSC_DST_REL_BIT_DEPTH;

        switch (i)
        {
            case 0:
                p_ls_comTable[i].x_value_x1000 = LLA_IMX072_CIE1931_X_HIGH_TEMP * 1000;
                p_ls_comTable[i].y_value_x1000 = LLA_IMX072_CIE1931_Y_HIGH_TEMP * 1000;
                nvm_addr_lsc_red = LLA_IMX072_LSC_DATA_ADDR_HIGH_RED;
                nvm_addr_lsc_blue = LLA_IMX072_LSC_DATA_ADDR_HIGH_BLUE;
                nvm_addr_lsc_green = LLA_IMX072_LSC_DATA_ADDR_HIGH_GREEN;
                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                    nvm_addr_lsc_GbGr = LLA_IMX072_LSC_DATA_ADDR_HIGH_GbGr;
                }


                break;

            case 1:
                p_ls_comTable[i].x_value_x1000 = LLA_IMX072_CIE1931_X_LOW_TEMP * 1000;
                p_ls_comTable[i].y_value_x1000 = LLA_IMX072_CIE1931_Y_LOW_TEMP * 1000;
                nvm_addr_lsc_red = LLA_IMX072_LSC_DATA_ADDR_LOW_RED;
                nvm_addr_lsc_blue = LLA_IMX072_LSC_DATA_ADDR_LOW_BLUE;
                nvm_addr_lsc_green = LLA_IMX072_LSC_DATA_ADDR_LOW_GREEN;
                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                    nvm_addr_lsc_GbGr = LLA_IMX072_LSC_DATA_ADDR_LOW_GbGr;
                }


                break;

            case 2:
                p_ls_comTable[i].x_value_x1000 = LLA_IMX072_CIE1931_X_FlUORESCENT_TEMP * 1000;
                p_ls_comTable[i].y_value_x1000 = LLA_IMX072_CIE1931_Y_FlUORESCENT_TEMP * 1000;
                nvm_addr_lsc_red = LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_RED;
                nvm_addr_lsc_blue = LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_BLUE;
                nvm_addr_lsc_green = LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_GREEN;
                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                    nvm_addr_lsc_GbGr = LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_GbGr;
                }


                break;
        }


        // TODO Check this value
        p_ls_comTable[i].object_distance = LLA_IMX072_DEFAULT_OBJECT_DISTANCE;
        p_ls_comTable[i].zoom_factor = LLA_IMX072_DEFAULT_ZOOM_FACTOR;

        // Choose the next free memory address in the buffer with 4byte alignment
        p_lsc_data = ( CAM_DRV_NVM_LSC_DATA_T * ) &u8_nvmStartAddress[u32_NextFreeMemoryOffset];

        u32_NextFreeMemoryOffset +=
            (
                sizeof(CAM_DRV_NVM_LSC_DATA_T) *
                (LLA_IMX072_LSC_GRID_HORIZ_COUNT * LLA_IMX072_LSC_GRID_VERTI_COUNT)
            );

        // This is a check against over-writing the buffer
        if (u32_NextFreeMemoryOffset > g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size)
        {
#if LLA_ASSERT_LOG_EN
           OstTraceInt0(TRACE_ERROR, "!!ERROR!!-> 3 LLA_IMX072_GetParsedNVM()");
#endif
            LLA_ASSERT_XP70();
        }


        p_ls_comTable[i].lsc_data_offset = (uint32) p_lsc_data - (uint32) & p_ls_comTable[i];
        {
            uint8_t cenval_r;
            uint8_t cenval_g;
            uint8_t cenval_b;
            uint8_t cenval_i;

            float_t nrmval_r;
            float_t nrmval_gr;
            float_t nrmval_gb;
            float_t nrmval_b;
            {
            uint16_t    center_sample_offset;

            center_sample_offset = (LLA_IMX072_LSC_GRID_HORIZ_COUNT / 2) + LLA_IMX072_LSC_GRID_HORIZ_COUNT * (LLA_IMX072_LSC_GRID_VERTI_COUNT / 2);

            retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_red + center_sample_offset, 1, &cenval_r));
            retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_green + center_sample_offset, 1, &cenval_g));
             retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_blue + center_sample_offset, 1, &cenval_b));

            if (Flag_e_TRUE == g_GbGrCalibrated)
            {
                 retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_GbGr + center_sample_offset, 1, &cenval_i));
            }
            else
            {
                cenval_i = LLA_IMX072_LSC_SRC_UNITY_I;
            }
            }


                nrmval_r = cenval_r;
                nrmval_b = cenval_b;

                nrmval_gr = (2.0f * cenval_g) / (1.0f + cenval_i / LLA_IMX072_LSC_SRC_UNITY_F);
                nrmval_gb = (2.0f * cenval_g) / (1.0f + LLA_IMX072_LSC_SRC_UNITY_F / cenval_i);

                for (k = 0; k < (LLA_IMX072_LSC_GRID_HORIZ_COUNT * LLA_IMX072_LSC_GRID_VERTI_COUNT); k++)
    {
                    uint8_t curval_r;
                    uint8_t curval_g;
                    uint8_t curval_b;
                    uint8_t curval_i;
    {
                         retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_red + k, 1, &curval_r));
                         retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_green + k, 1, &curval_g));
                         retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_blue + k, 1, &curval_b));

                if (Flag_e_TRUE == g_GbGrCalibrated)
                {
                            retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(nvm_addr_lsc_GbGr + k, 1, &curval_i));
                }
                        else
                {
                            curval_i = LLA_IMX072_LSC_SRC_UNITY_I;
                }
                }


                    p_lsc_data[k].r = (uint16_t) ((((float_t) curval_r) / ((float_t) nrmval_r)) * LLA_IMX072_LSC_DST_REL_UNITY_I + 0.5f);
                    p_lsc_data[k].b = (uint16_t) ((((float_t) curval_b) / ((float_t) nrmval_b)) * LLA_IMX072_LSC_DST_REL_UNITY_I + 0.5f);

                    p_lsc_data[k].gr = (uint16_t)
                        (
                            (((2.0f * curval_g) / (1.0f + curval_i / LLA_IMX072_LSC_SRC_UNITY_F)) / nrmval_gr) *
                            LLA_IMX072_LSC_DST_REL_UNITY_I +
                            0.5f
                        );
                    p_lsc_data[k].gb = (uint16_t)
                        (
                            (((2.0f * curval_g) / (1.0f + LLA_IMX072_LSC_SRC_UNITY_F / curval_i)) / nrmval_gb) *
                            LLA_IMX072_LSC_DST_REL_UNITY_I +
                            0.5f
                        );
                }
        }


            // overwriting the central sample with absolute data
            {
                uint16_t    center_sample_index;

                center_sample_index = (LLA_IMX072_LSC_GRID_HORIZ_COUNT / 2) + LLA_IMX072_LSC_GRID_HORIZ_COUNT * (LLA_IMX072_LSC_GRID_VERTI_COUNT / 2);

                if (p_nvm->sensitivity_data.sensitivity_colour_temps == p_nvm->ls_data.ls_table_num)
        {
                    // absolute data taken from the sensitivity section
                    // implying the same order comparing the array of LSC tables and sensitivities data sets
                    p_lsc_data[center_sample_index].r = p_sens[i].sens.r;
                    p_lsc_data[center_sample_index].b = p_sens[i].sens.b;

                    p_lsc_data[center_sample_index].gr = p_sens[i].sens.gr;
                    p_lsc_data[center_sample_index].gb = p_sens[i].sens.gb;
        }
        else
        {
                    // not possible to associate data from the sensitivity section (should never happen)
                    p_lsc_data[center_sample_index].r = 0;
                    p_lsc_data[center_sample_index].b = 0;

                    p_lsc_data[center_sample_index].gr = 0;
                    p_lsc_data[center_sample_index].gb = 0;
                }
            }
        }
    }


    /* Defect pixels related NVM dataDefect pixels related NVM data ------------------------------------------------------*/

    /* IMX072 NVM does not say anything about couplet defects, it only gives X,Y co-ordinates of single defective Pixels
        * So basically concept of couplet is not being implemented here, we are just specifying each single pixel defect as a Type 1
        * couplet to stay in coherence with SMIA++. Sol type 2 defects cannot be specified in IMX072 even if they exist.
        */
    CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T  *p_couplet_type1_map;

    p_nvm->defect_data.num_of_couplet_type2_defects = 0;
    p_nvm->defect_data.num_of_line_defects = 0;
    p_nvm->defect_data.couplet_type2_map_offset = 0;
    p_nvm->defect_data.line_map_offset = 0;

    p_nvm->offset_defect_couplet_type2 = 0;
    p_nvm->offset_defect_line = 0;

    //Read number of Type 1 defects.
    size = 1;
     retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(
        LLA_IMX072_ADDR_DEFECT_PIXEL_COUNT,
        size,
        ( uint8_t * ) &(p_nvm->defect_data.num_of_couplet_type1_defects)));

    // Create map of defects
    // First Check if NVM value for Defect Pixel count is valid data.
    if
    (
        (LLA_IMX072_MIN_DEFECT_PIXEL_COUNT < (p_nvm->defect_data.num_of_couplet_type1_defects))
    &&  (LLA_IMX072_MAX_DEFECT_PIXEL_COUNT > (p_nvm->defect_data.num_of_couplet_type1_defects))
    )
    {
        // Choose the next free memory address in the buffer with 4byte alignment
        p_couplet_type1_map = ( CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T * ) &u8_nvmStartAddress[u32_NextFreeMemoryOffset];
        p_nvm->defect_data.couplet_type1_map_offset = (uint32) p_couplet_type1_map - (uint32) & p_nvm->defect_data;
        p_nvm->offset_defect_couplet_type1 = u32_NextFreeMemoryOffset;
        u32_NextFreeMemoryOffset +=
            (
                sizeof(CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T) *
                (p_nvm->defect_data.num_of_couplet_type1_defects)
            );

        // This is a check against over-writing the buffer
        if (u32_NextFreeMemoryOffset > g_IMX072_CamGlbConfig.camDrvSensorDetails.camNVMDetail.parsed_nvm_size)
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "!!ERROR!!-> 4 LLA_IMX072_GetParsedNVM()");
#endif
            LLA_ASSERT_XP70();
        }


        //used inside for loop, NVM data being read is of 4bytes
        size = 4;

        for (i = 0; i < (uint8) (p_nvm->defect_data.num_of_couplet_type1_defects); i++)
        {
            uint32_t    defect_pixel_data = 0;
            retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(
                (LLA_IMX072_DATA_ADDR_DEFECT_PIXEL + (i * size)),
                size,
                ( uint8_t * ) &(defect_pixel_data)));

            // Split data into X[Bit0 to Bit 12]  and Y[Bit 16 to Bit 29] co-ordinates ; Page 28 of EEPROM map
            p_couplet_type1_map[i].x = (uint16_t) (defect_pixel_data & 0x1FFF);                             // 0x1FFF is equal to 13 1's
            p_couplet_type1_map[i].y = (uint16_t) ((defect_pixel_data >> 16) & 0x3FFF);                     // 0x3FFF is equal to 14 1's
        }
    }
    else
    {
        // There are no defects or the NVM does not say anything about pixel defects
        // Defect Pixel data is specified only in case of 16kbit EEPROM, in case of 4,8 kbit EEPROM Defect Pixel data is not supplied.
        p_nvm->defect_data.couplet_type1_map_offset = 0;
        p_nvm->defect_data.num_of_couplet_type1_defects = LLA_IMX072_MIN_DEFECT_PIXEL_COUNT;
        p_nvm->offset_defect_couplet_type1 = 0;
    }


    /* ND filter and Shutter related NVM data ----------------------------------------------------------------------------*/
    p_nvm->nd_ms_data.shut_delay_us = 0;
    p_nvm->nd_ms_data.ND_trans_x100 = 0;

    /* < spatial pedestal data tables ------------------------------------------------------------------------------------*/
    p_nvm->spatial_pedestal_data.number_of_tables = 0;

    return (retVal);
}


/*LLA_IMX072_GetRawNVM
    Returns the filled CAM_DRV_NVM_T structure with NVM data from IMX072
*/
CAM_DRV_RETVAL_E
LLA_IMX072_GetRawNVM(
uint8   *p_nvm)
{
  CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;
  uint8 u8_Count= 0;
  uint8 *Current_add = p_nvm;
  uint16 u16_AddressToRead, u16_ReadSizeFromCurrAddress;
  #if 0
  uint16 u16_ValidAddRange[][2] = {/*List of all the data gaps, reserved and undefined both.*/
                                                            {0x0,63},
                                                            {0x40,16},
                                                            {0x60,12},
                                                            {0x70,16},
                                                            {0x98,8},
                                                            {0xBC,12},
                                                            {0xD0,6},
                                                            {0xD8,2},
                                                            {0xDC,2},
                                                            {0xE0,6},
                                                            {0xF0,2},
                                                            {0x100,769},
                                                            {0x404,1020},
                                                            {0,0}
                                                        };
  #else
    uint16 u16_ValidAddRange[][2] = {/*List of all the data gaps, reserved and undefined both.*/
                                                            {0x0, LLA_IMX072_NVM_SIZE},
                                                            {0,0}
                                                        };
  #endif
    u8_Count = 0;
    while(u16_ValidAddRange[u8_Count][1])
    {
         u16_AddressToRead = u16_ValidAddRange[u8_Count][0];
         u16_ReadSizeFromCurrAddress = u16_ValidAddRange[u8_Count][1];

        while(u16_ReadSizeFromCurrAddress / NVM_MAX_READ_BYTE)
        {
           retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(u16_AddressToRead, NVM_MAX_READ_BYTE, Current_add));
          Current_add += NVM_MAX_READ_BYTE;

          u16_AddressToRead +=  NVM_MAX_READ_BYTE;
          u16_ReadSizeFromCurrAddress -=NVM_MAX_READ_BYTE;
         }

         if(u16_ReadSizeFromCurrAddress % NVM_MAX_READ_BYTE)
         {
          retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_NVMMAP_Read(u16_AddressToRead, (u16_ReadSizeFromCurrAddress % NVM_MAX_READ_BYTE), Current_add));
         }
         u8_Count++;
         Current_add  = p_nvm + u16_ValidAddRange[u8_Count][0]; // Add the offset of the next valid address to pointer to the memory where NVM data will be stored.
    }

  return (retVal);
}


/*Apply any other Misc settings to be done for VF mode, any sensor specific setting to be done*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_ApplyMiscSettings(
CAM_DRV_SENS_ORIENTATION_T  orientation)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    uint8_t u8_Orientation = 0;

    uint8_t u8_Size, u8_ptrArray[6];
    uint16_t u16_CsiRawFormat;
    //CAM_DRV_SENS_IMAGE_DATA_INTERFACE_OPTIONS_E mode = g_IMX072_CamGlbConfig.dataInterfaceMode ;

    u8_Size = 0;
    switch (g_IMX072_CamGlbConfig.camDrvMiscDetails.csiRawFormat)
    {
        case CAM_DRV_SENS_FORMAT_RAW10:     u16_CsiRawFormat = 0x0A0A;break;
        case CAM_DRV_SENS_FORMAT_RAW8:      u16_CsiRawFormat = 0x0808;break;
        case CAM_DRV_SENS_FORMAT_RAW8_DPCM: u16_CsiRawFormat = 0x0A08;break;
        default:                            /*No other modes are supported as of now*/LLA_ASSERT_XP70();
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_ApplyMiscSettings - CAM_DRV_ERROR_PARAM!!");
#endif
            LLA_ASSERT_XP70();
            break;
    }


    //sudeep
    // CCP Signalling mode
       if (CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CCP2_DATA_STROBE == g_IMX072_CamGlbConfig.dataInterfaceMode)
       {
           // for parallel sensor and DS mode we set csi_signalling_mode to 1
           u8_ptrArray[u8_Size++] = 0x01;
       }
       else if ( CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CCP2_DATA_CLOCK == g_IMX072_CamGlbConfig.dataInterfaceMode)
       {
           // for data strobe mode we setup the csi_signalling_mode to 0
           u8_ptrArray[u8_Size++] = 0x00;
       }


    u8_ptrArray[u8_Size++] = (uint8_t) (u16_CsiRawFormat >> 8);     // MSB CCP RAW FORMAT
    u8_ptrArray[u8_Size++] = (uint8_t) (u16_CsiRawFormat & 0xFF);   // MSB CCP RAW FORMAT


    if ( CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CSI2 == g_IMX072_CamGlbConfig.dataInterfaceMode ) //sudeep
       {
          // it is CSI so do not program ccp2_signalling_mode
            retVal  =(CAM_DRV_FN_RETVAL_E)((uint8_t)retVal |(uint8_t) LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__CSI_SIGNALLING_MODE+1, u8_Size, u8_ptrArray));
       }
       else
       {
        retVal  =(CAM_DRV_FN_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__CSI_SIGNALLING_MODE, u8_Size, u8_ptrArray));
       }



    /*Commit orientation information*/
    {

        if (1 == orientation.mirror)
        {
            u8_Orientation = u8_Orientation | 0x01;
        }


        if (1 == orientation.flip)
        {
            u8_Orientation = u8_Orientation | 0x02;
        }

         retVal  =(CAM_DRV_FN_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__IMAGE_ORIENTATION, 1, ( uint8_t * ) &u8_Orientation));
    }

    /*any sensor specific setting if required*/
    if(CAM_DRV_FN_OK != retVal)
    {
        return (CAM_DRV_FN_FAIL);
    }

    return (CAM_DRV_FN_OK);
}

/*LLA_ApplyFrameDimensionSettings
This function will commit current FD parameters to sensor
*/
CAM_DRV_RETVAL_E
LLA_IMX072_ApplyFrameDimensionSettings(void)
{

    CAM_DRV_RETVAL_E         e_Flag_Status = CAM_DRV_OK;
    const CAM_DRV_CAMERA_CONFIGS_T    *SensorModeSelectedConf = NULL;

    uint16_t                    u16_Array1[] =
    {
        BSWAP_16(g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTFrameLengthLines),
                                BSWAP_16(g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTLineLengthPck)
    };


    // Apply Pll settings specific to mode selected. If sensor has no such constraints, then make Null array.
    e_Flag_Status = LLA_IMX072_ApplyPLLSettings();


    /*Commit calculated values for frame length and line length, Note these values may get updated again depending
        on the frame rate and exposure settings
        */
    e_Flag_Status  =(CAM_DRV_RETVAL_E)((uint8_t)e_Flag_Status |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
        SENSOR_VIDEO_TIMING__FRAME_LENGTH_LINES_HI,
        4,
        (uint8_t *) (u16_Array1)));

   // OstTraceInt1(TRACE_USER3, "APPLY_FDM g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected: %d", g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected);

    SensorModeSelectedConf = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->p_FDModes;

    e_Flag_Status  =(CAM_DRV_RETVAL_E)((uint8_t)e_Flag_Status |(uint8_t)LLA_IMX072_WriteSettingsArray(SensorModeSelectedConf));

    /*Check for any error so far*/
    if (CAM_DRV_OK != e_Flag_Status)
    {
#if LLA_ASSERT_LOG_EN
        /*error has occured, asser in debug mode*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_ApplyFrameDimensionSettings - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();

        return (CAM_DRV_ERROR_HW);
    }


    return (CAM_DRV_OK);

    }



/*LLA_IMX072_ApplyExposureAndGainSettings:
Apply gain and exposure settings to sensor
*/
CAM_DRV_RETVAL_E
LLA_IMX072_ApplyExposureAndGainSettings(void)
{
    CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;
    volatile uint16_t   u16_TempValue = 0;


    //AcquireGrpHldReg
   // u8_Value = 1;
   // retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__GROUPED_PARAMETER_HOLD, 1, &u8_Value));


    /*Commit the analog gain code*/

    /*TODO:*/

    //g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrAnalogGainCode = 0x00f0;
    u16_TempValue = BSWAP_16(g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrAnalogGainCode);
    retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
        SENSOR_INTEGRATION__ANALOGUE_GAIN_CODE_GLOBAL_HI,
        2,
        ( uint8_t * ) &u16_TempValue));

    u16_TempValue = 0;

    {
        /*Commit the integration time*/
        u16_TempValue = BSWAP_16(g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrCoarseIntegrationLines);
        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGRATION__COARSE_INTEGRATION_TIME_HI,
            2,
            ( uint8_t * ) &u16_TempValue));
    }

    /*Check if sensor do support fine integration time*/
    if (0 != g_IMX072_CamGlbConfig.exposureCap.e_IntegrationCapability)
    {
        u16_TempValue = BSWAP_16(g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrFineIntegrationPixels);
        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGRATION__FINE_INTEGRATION_TIME_HI,
            2,
            ( uint8_t * ) &u16_TempValue));
    }


    /*Commit digital gain*/
    if (g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.digital_gain_capability.gain_type == CAM_DRV_SENS_GAIN_SEPARATE)
    {
        u16_TempValue = BSWAP_16(g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrDigitalGainCode);
        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGRATION__DIGITAL_GAIN_GREENR_HI,
            2,
            ( uint8_t * ) &u16_TempValue));

        retVal =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGRATION__DIGITAL_GAIN_RED_HI,
            2,
            ( uint8_t * ) &u16_TempValue));

        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGRATION__DIGITAL_GAIN_BLUE_HI,
            2,
            ( uint8_t * ) &u16_TempValue));

        retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
            SENSOR_INTEGRATION__DIGITAL_GAIN_GREENB_HI,
            2,
            ( uint8_t * ) &u16_TempValue));
    }



 ////ReleaseGrpHldReg
  //  u8_Value = 0;
  //  retVal  =(CAM_DRV_RETVAL_E)((uint8_t)retVal |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__GROUPED_PARAMETER_HOLD, 1, &u8_Value));

    if (CAM_DRV_OK != retVal)
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    return (CAM_DRV_OK);
}



/*LLA_IMX072_ApplyFrameRateSettings
Commit frame dimension parameters for Smia sensor
Any change in frame rate will result in change in the number of lines from the sensor*/
CAM_DRV_FN_RETVAL_E
LLA_IMX072_ApplyFrameRateSettings()
{

    CAM_DRV_FN_RETVAL_E e_Flag_Status = CAM_DRV_FN_OK;
    uint16_t            u16_VTFrameLengthLines =0;

   //<Sudeep> AcquireGrpHldReg
   // u8_Value = 1;
   // e_Flag_Status = LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__GROUPED_PARAMETER_HOLD, 1, &u8_Value);

   u16_VTFrameLengthLines = BSWAP_16(g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTFrameLengthLines);

   /*Commit Frame length */
    e_Flag_Status  =(CAM_DRV_FN_RETVAL_E)((uint8_t)e_Flag_Status |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(
        SENSOR_VIDEO_TIMING__FRAME_LENGTH_LINES_HI,
        2,
        (uint8_t *)&u16_VTFrameLengthLines));

 //<Sudeep>RemoveGrpHldReg
  //  u8_Value = 0;
  //  e_Flag_Status  =(CAM_DRV_FN_RETVAL_E)((uint8_t)e_Flag_Status |(uint8_t)LLA_WRITE_I2C_BYTES_16BitRegSize(SENSOR_SETUP__GROUPED_PARAMETER_HOLD, 1, &u8_Value));


    if (CAM_DRV_FN_OK != e_Flag_Status)
    {

#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_ApplyFrameRateSettings - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_FN_FAIL);
    }

    return (CAM_DRV_FN_OK);

}


/* This function will return current configuration of sensor */

void
LLA_IMX072_GetCurrSensorConfig
(CamDrvCurrConfig_ts *pcurrConfig)
{
    /*current WOI and OutputResolution*/
    pcurrConfig->camCurrWoiRes.width = g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_WOIXSize;
    pcurrConfig->camCurrWoiRes.height = g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_WOIYSize;
    pcurrConfig->camCurrOutputRes.width = g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_OPXSize;
    pcurrConfig->camCurrOutputRes.height = g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_OPYSize;

    /*Exposure and gain setting*/
    pcurrConfig->camCurrExpGain.exposure_time_us = (uint32_t)((float_t)
    (g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTLineLengthPck / g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockFrequency_Mhz) *
    (g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrCoarseIntegrationLines - IMX072_EXP_OFFSET_WO_VA /*specific to IMX072*/));

    pcurrConfig->camCurrExpGain.analog_gain_x1000 = (uint16_t)
       (
           (
               g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m0 *
               g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrAnalogGainCode +
               g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c0
           ) * 1000 /
               (
                   g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_m1 *
                   g_IMX072_CamGlbConfig.currGainConfigParam.u16_CurrAnalogGainCode  +
                   g_IMX072_CamGlbConfig.analogGainCap.analogue_gain_c1
                )
       );

/*    pcurrConfig->camCurrFrameRate.frame_rate_x100 = (uint16_t)
       (
           (float_t) 1000000.0 /
               (
                     g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTFrameLengthLines *
                     g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTLineLengthPck *
                     g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us
               )
        ) * 100;
 */
    pcurrConfig->camCurrExpGain.digital_gain_x1000 = LLA_DEF_GAIN_x1000;
    pcurrConfig->camCurrLineLenPck = g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTLineLengthPck;
    pcurrConfig->camCurrFrameLenLine = g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTFrameLengthLines;

    return;
}


/*LLA_IMX072_CalculateFrameTimeForExposure
This will convert exposure time to requested frame time us
*/
uint32_t
LLA_IMX072_ComputeFrameTimeForExposure(uint32_t exposure_time_us)
{
    uint32_t    u32_FrameTime_us;

    /*First calculate the total frame time required for achieving the requested exposure time
    Total Frame Time = requested exposure time + exposure max-margin time
    */
     u32_FrameTime_us = exposure_time_us + (uint32_t)
        (
            (float_t) (g_IMX072_CamGlbConfig.exposureCap.u16_CoarseIntegrationMaxMargin + 1) *
            g_IMX072_CamGlbConfig.currFrameDimensionParam.u16_VTLineLengthPck *
            g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->f_VTPixelClockPeriod_us
        );

    return u32_FrameTime_us;
}




/**
\if INCLUDE_IN_HTML_ONLY
\fn uint16_t ComputeImpliedMinimumLineLength( uint16_t u16_VTXOutputSize )
\brief This function computes the implied minimum VT line length corresponding
to the given VT X output size
\param u16_VTXOutputSize : The video timing X output size.
\return The implied minimum VT line length.
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
*/uint16_t
LLA_IMX072_ComputeImpliedMinimumLineLength(uint16_t u16_OPXSize, uint32 u32_MinReqLineLenPck)
 {
    /*~~~~~~~~~~~~~~~~~~*/
    uint16_t    u16_Value;
    uint16_t    u16_CsiRawFormat,u16_MinimumValue;

    /*~~~~~~~~~~~~~~~~~~*/

    switch (g_IMX072_CamGlbConfig.camDrvMiscDetails.csiRawFormat)
    {
        case CAM_DRV_SENS_FORMAT_RAW10:     u16_CsiRawFormat = LLA_DATA_FORMAT_RAW10; break;
        case CAM_DRV_SENS_FORMAT_RAW8:      u16_CsiRawFormat = LLA_DATA_FORMAT_RAW8; break;
        case CAM_DRV_SENS_FORMAT_RAW8_DPCM: u16_CsiRawFormat = LLA_DATA_FORMAT_DPCM10TO8; break;
        default:                         /*No other modes are supported as of now*/
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_IMX072_ComputeImpliedMinimumLineLength - CAM_DRV_ERROR_PARAM!!");
#endif
            LLA_ASSERT_XP70();
            break;
    }

    /// If mode specific minimum line length is specified, then calc mini VT line blanking with respect to that. Otherwise use as read in capabilities register.
    if(g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->u16_MiniLineLengthModeSpecific)
    {
        u16_MinimumValue = (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->u16_MiniLineLengthModeSpecific
            - g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.output_mode_capability.p_modes[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected].output_res.width
            - LLA_FrameDimension_GetSensorConstantCols());
    }
    else
    {
        //[CR - 445315] - Here the mode specific value of minimum line blanking pck will be used
        u16_MinimumValue = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->u16_MinLineBlankingPck;
    }


    /// Compute the implied minimum video timing line length.\n
    /// Implied_min_video_timing_line_length = VTXOutputSize + additional_cols + minimum VT line blanking pixel clocks.\n
    /// min_vt_line_blanking_pck = sav_code_pck + eav_code_pck + checksum_pck + min_line_blanking_pck(from g_SensorFrameConstraints.u16_MinVTLineBlankingPck).\n
    /// sav_code_pck => Start of active video code pixel clocks.\n
    /// eav_code_pck => End of active video code pixel clocks.\n
    if (CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CSI2 == g_IMX072_CamGlbConfig.dataInterfaceMode)
    {
        u16_Value = GenericFunctions_CeilingOfMByN(LLA_SOL_SYNC_CODE_WIDTH_BITS, (uint8_t) u16_CsiRawFormat) +
            GenericFunctions_CeilingOfMByN(LLA_EOL_SYNC_CODE_WIDTH_BITS, (uint8_t) u16_CsiRawFormat) + u16_MinimumValue;
    }
    else
    {
        u16_Value = GenericFunctions_CeilingOfMByN(LLA_SYNC_CODE_WIDTH_BITS, (uint8_t) u16_CsiRawFormat) +
            GenericFunctions_CeilingOfMByN((LLA_SYNC_CODE_WIDTH_BITS + LLA_CHECKSUM_WIDTH_BITS), (uint8_t) u16_CsiRawFormat) + u16_MinimumValue;
    }

    u16_Value += u16_OPXSize + LLA_FrameDimension_GetSensorConstantCols();

    if (u32_MinReqLineLenPck)
    {
        /*Client has requested for a min line length to support up-scaling in pipe*/
        if (u16_Value < u32_MinReqLineLenPck)
        {
            /*client has requested for more line length*/
            /*It is the responsibility of client to ensure that requested line length is indeed possible in given configuration while maintaining frame rate*/
            u16_Value = u32_MinReqLineLenPck;
        }
    }

    /// The calculated line length must be with in range specified
    /// If Mode specific value for minimum line lenght is specified, then use that to clip the calculated line length. else use whatever is read in capability register.
    if (g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->u16_MiniLineLengthModeSpecific)
    {
        u16_MinimumValue = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMModeSelected]->u16_MiniLineLengthModeSpecific;
    }
    else
    {
        u16_MinimumValue = g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTLineLengthPck;
    }
    u16_Value = clip(u16_Value, u16_MinimumValue, g_IMX072_CamGlbConfig.frameDimensionCap.u16_MaxVTLineLengthPck);

    return (u16_Value);


}


/*
************************************************************************************************
\if INCLUDE_IN_HTML_ONLY
\fn uint16_t ComputeImpliedMinimumFrameLength( uint16_t u16_VTYOutputSize )
\brief This function computes the implied minimum vt frame length
corresponding to the given vt y output size.
\param u16_VTYOutputSize : The video timing y output size.
\return The implied minimum VT frame length.
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
************************************************************************************************
*/
uint16_t
LLA_IMX072_ComputeImpliedMinimumFrameLength(uint16_t u16_OPYSize)
{
    uint16_t    u16_MinimumFrameLength;
    uint8_t     u8_ExtraRows;

    //[CR - 445315] - The LLA_IMX072_GetExtraLines function is removed from the code and extra lines are added to the
    //mode specific structure "SensorModeSettings_ts".
    u8_ExtraRows = g_IMX072_SensorModeSettings[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_ProfileSelected].p_SensorModeSetting[g_IMX072_CamGlbConfig.camDrvMiscDetails.u8_FDMOutputModeSelected]->u16_ExtraLines;


    /*frame length lines = y_op_size + extra_rows + min_frame_blanking*/
    u16_MinimumFrameLength = u16_OPYSize + u8_ExtraRows + g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameBlanking;
    u16_MinimumFrameLength = max( u16_MinimumFrameLength, g_IMX072_CamGlbConfig.frameDimensionCap.u16_MinVTFrameLengthLines);

    return (u16_MinimumFrameLength);

}

#endif //#if (1 == LLA_INCLUDE_IMX072)

