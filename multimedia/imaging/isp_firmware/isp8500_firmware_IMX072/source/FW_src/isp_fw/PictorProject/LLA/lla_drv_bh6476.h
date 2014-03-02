/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/
#ifndef __LLA_DRV_IMPL_H__
#   define __LLA_DRV_IMPL_H__

#include "lla_drv_imx072.h"

/*Macros definitions for AF driver*/
#define LLA_IMX072_AF_DRIVER_ADDR       0x18  /*7Bit address + 1 R/W bit for AF driver*/
#define LLA_IMX072_AF_CAPB_NVM_ADDR     0x038 /*NVM location for AF driver*/
#define LLA_IMX072_ZOOM_CAPB_NVM_ADDR   0x039 /*NVm location for Zoom driving method*/
#define LLA_IMX072_AF_MIN_DAC_CODE      0x0   /*Min value for 10bit DAC code*/
#define LLA_IMX072_AF_MAX_DAC_CODE      0x3FF /*Max value for 10bit DAC code*/


#define FOCUS_LENS_MOVEMENT_ABORT 0
/*Macro for I2C read/Write*/
#define LLA_IMX072_AFDriver_I2C_Read(index,NoOfBytes,ptrBuffer)                         \
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_read_fn( \
            LLA_IMX072_AF_DRIVER_ADDR,                              \
            index,                                                  \
            CAM_DRV_I2C_REG_SIZE_8BIT,                              \
            NoOfBytes,                                              \
            CAM_DRV_I2C_BYTE_ORDER_NORMAL,                      \
            ptrBuffer)

#define LLA_IMX072_AFDriver_I2C_Write(index,NoOfBytes,ptrBuffer)                            \
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn( \
            LLA_IMX072_AF_DRIVER_ADDR,                              \
            index,                                                  \
            CAM_DRV_I2C_REG_SIZE_8BIT,                              \
            NoOfBytes,                                              \
            CAM_DRV_I2C_BYTE_ORDER_NORMAL,                      \
            ptrBuffer)



/*
Forward function declarations
*/
void BH6476_InitializeCallbackFn (CAM_DRV_AF_DRIVER_SPECIFIC_FN_INIT_ts * p_CallbackFn) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E BH6476_AFDriver_Init ( void ) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E BH6476_AFDriver_MeasurePos (uint16_t * u16_CurrentDac) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E BH6476_GetLensDetail (CAM_DRV_LENS_DETAILS_T * p_lens_details) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E BH6476_LensMovToPos (int32 position, uint32 * pu32_MoveTime_us) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E BH6476_AFDriver_IsLensMoving (Flag_te * isLenMoving) TO_EXT_PRGM_MEM;
uint8_t BH6476_SupportLensAbort();
#endif /*__LLA_DRV_IMPL_H__*/

