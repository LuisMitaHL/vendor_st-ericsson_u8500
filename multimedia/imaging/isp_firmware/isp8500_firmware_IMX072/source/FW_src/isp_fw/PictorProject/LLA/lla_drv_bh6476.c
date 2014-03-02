/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/
#include "lla_common_config.h"
#include "lla_drv_bh6476.h"
#include "lla_drv_imx072.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_LLA_lla_drv_bh6476Traces.h"
#endif

/* =====================
        AF driver - APIs
   =====================*/

/*Local Function Declaration*/
CAM_DRV_FN_RETVAL_E BH6476_AFDriver_MovetoPos (uint16_t u16_TargetDac) TO_EXT_PRGM_MEM;

void
BH6476_InitializeCallbackFn(
CAM_DRV_AF_DRIVER_SPECIFIC_FN_INIT_ts    *p_CallbackFn)
{
    p_CallbackFn->GetLensDetail = BH6476_GetLensDetail;
    p_CallbackFn->AFDriver_Init = BH6476_AFDriver_Init;
    p_CallbackFn->AFDriver_MeasurePos = BH6476_AFDriver_MeasurePos;
    p_CallbackFn->LensMovToPos = BH6476_LensMovToPos;
}


/*LLA_IMX072_AFDriver_MovetoPos:-
    AF Driver to be wil be used in Step mode..
    This function will set a new value as target DAC code ...
*/
CAM_DRV_FN_RETVAL_E
BH6476_AFDriver_MovetoPos(
uint16_t    u16_TargetDac)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    uint16_t            u16_Val = 0;
    uint8_t             u8_LTargetDac,
                        u8_HTargetDac;
#ifdef U5500_PLATFORM_PORTING
    uint16_t            u16_index;
#else
    uint8_t             u8_index;
#endif
    u8_LTargetDac = (u16_TargetDac & 0xFF);
    u8_HTargetDac = (u16_TargetDac & 0xFF00) >> 8;

    /*Set a new DAC value*/
#ifdef U5500_PLATFORM_PORTING
    /*Grouping address and data together for a single write*/
    u16_index = 0xC8 | (u8_HTargetDac & 0x3);
    u16_index = (u16_index << 8) | (u8_LTargetDac);

    retVal = LLA_IMX072_AFDriver_I2C_Write(u16_index, 0x1, ( uint8 * ) &u16_Val);
#else
    /*Create a index byte*/
    u8_index = 0xC8 | (u8_HTargetDac & 0x3);

    u16_Val = u8_LTargetDac << 8;
    u16_Val = BSWAP_16(u16_Val);

    retVal = LLA_IMX072_AFDriver_I2C_Write(u8_index, 0x2, ( uint8 * ) &u16_Val);
#endif
    return (retVal);
}


/* LLA_IMX072_AFDriver_IsLensMoving:
  Check if lens is moving or not ...
  As, currently driving AF driver in step mode.. so its possible to know lens moving status

  API, will return status as output variable, and will return error if I2C fails/or any other error etc...
  Not to rely on output variable value, if API itself return an error
*/
CAM_DRV_FN_RETVAL_E
BH6476_AFDriver_IsLensMoving(
Flag_te *isLenMoving)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    uint16_t            u16_Val = 0;
    uint8_t             u8_HVal;

    /*Read status from AF driver*/
    retVal = LLA_IMX072_AFDriver_I2C_Read(0xC8, 0x2, ( uint8 * ) &u16_Val);
    u16_Val = BSWAP_16(u16_Val);

    u8_HVal = u16_Val >> 8;

    /*Check the 'Finished' flag*/
    if (u8_HVal & 0x10)
    {
        /*Lens is moving*/
        *isLenMoving = Flag_e_TRUE;
    }
    else
    {
        /*Lens is not-moving*/
        *isLenMoving = Flag_e_FALSE;
    }


    /*Return error if above I2C failed*/
    if (CAM_DRV_FN_OK != retVal)
    {
        return (CAM_DRV_FN_FAIL);
    }
    else
    {
        return (CAM_DRV_FN_OK);
    }
}


/* LLA_GetCamDrvLensDetail:
Update the lens specific details to be passed on to client of low level driver
*/
CAM_DRV_RETVAL_E
BH6476_GetLensDetail(
CAM_DRV_LENS_DETAILS_T  *p_lens_details)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    uint16_t            u16_AfVal = 0;

    // OstTraceInt0(TRACE_DEBUG, "LLA_IMX072_GetLensDetail!!");

    /*Need to extend this API, when AF support is included in the LLD*/
    g_CamDrvConfig.camLensDetail.type = CAM_DRV_LENS_AF;
    g_CamDrvConfig.camLensDetail.af_lens_pos_measure_ability = FALSE;
    g_CamDrvConfig.camLensDetail.movement_times.horizontal_us = 1000;   /*Dummy value*/

    /*Need to read these values from NVM map*/
    g_CamDrvConfig.camLensDetail.positions.far_end = 0x0;
    g_CamDrvConfig.camLensDetail.positions.hyperfocal = 0x0;
    g_CamDrvConfig.camLensDetail.positions.infinity = 0x0;
    g_CamDrvConfig.camLensDetail.positions.macro = 0x0;
    g_CamDrvConfig.camLensDetail.positions.near_end = 0x0;
    g_CamDrvConfig.camLensDetail.positions.rest = 0x0;

    /*AF driver will be run in step mode, so can measure lens position*/
    p_lens_details->af_lens_pos_measure_ability = TRUE;

    /*TODO:CN: not sure about this setting*/

    /*AF drivier set with time interval (step time)as 0x14h*/
    p_lens_details->movement_times.horizontal_us = LLA_IMX072_AF_ONE_STEP_TIME; /*As per AF data sheet*/ ;

    /*Read focus data from the NVM*/

    /*infinity*/
    retVal = LLA_NVMMAP_Read(LLA_IMX072_AF_DATA_ADDR_INF, 2, ( uint8 * ) &u16_AfVal);
    u16_AfVal = BSWAP_16(u16_AfVal);
    p_lens_details->positions.infinity = u16_AfVal;

    //OstTraceInt1(TRACE_DEBUG, "infinity :%d",p_lens_details->positions.infinity);

    /*macro*/
    retVal = LLA_NVMMAP_Read(LLA_IMX072_AF_DATA_ADDR_MACRO, 2, ( uint8 * ) &u16_AfVal);
    u16_AfVal = BSWAP_16(u16_AfVal);
    p_lens_details->positions.macro = u16_AfVal;

    // OstTraceInt1(TRACE_DEBUG, "macro :%d",p_lens_details->positions.macro);

    /*1M*/
    retVal = LLA_NVMMAP_Read(LLA_IMX072_AF_DATA_ADDR_1M, 2, ( uint8 * ) &u16_AfVal);
    u16_AfVal = BSWAP_16(u16_AfVal);
    p_lens_details->positions.hyperfocal = u16_AfVal;

    // OstTraceInt1(TRACE_DEBUG, "hyperfocal :%d",p_lens_details->positions.hyperfocal);

    /*Read focus range from NVM*/

    /*Read focus range for infinity*/
    retVal = LLA_NVMMAP_Read(LLA_IMX072_AF_RANG_ADDR_INF, 2, ( uint8 * ) &u16_AfVal);
    p_lens_details->positions.far_end = BSWAP_16(u16_AfVal);

    // OstTraceInt1(TRACE_DEBUG, "far_end :%d", p_lens_details->positions.far_end);

    /*Read focus range for macro*/
    retVal = LLA_NVMMAP_Read(LLA_IMX072_AF_RANG_ADDR_MACRO, 2, ( uint8 * ) &u16_AfVal);
    p_lens_details->positions.near_end = BSWAP_16(u16_AfVal);

    //OstTraceInt1(TRACE_DEBUG, "near_end :%d", p_lens_details->positions.near_end);

    /*Rest position*/
    p_lens_details->positions.rest = p_lens_details->positions.far_end;

    //OstTraceInt1(TRACE_DEBUG, "rest :%d", p_lens_details->positions.rest);
    return (retVal);
}


/*
   =====================
        AF driver - APIs
   =====================
*/

/*LLA_IMX072_AFDriver_Init:-

    AF Driver to be wil be used in Step mode..
    So init function will be just setting uo step width ...
*/
CAM_DRV_FN_RETVAL_E
BH6476_AFDriver_Init(void)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    uint16_t            u16_regVal = 0x00;

    //OstTraceInt0(TRACE_DEBUG, "LLA_IMX072_AFDriver_Init!!");

    /*Put the AF driver in the init state*/
#ifdef U5500_PLATFORM_PORTING
    retVal = LLA_IMX072_AFDriver_I2C_Write((0x80 << 8), 0x1, ( uint8 * ) &u16_regVal);
#else
    retVal = LLA_IMX072_AFDriver_I2C_Write(0x80, 0x2, ( uint8 * ) &u16_regVal);
#endif

    /*Do the StepMode settings*/
    u16_regVal = (LLA_IMX072_AF_STEP_WIDTH << 8) | LLA_IMX072_AF_STEP_INTVAL;
    u16_regVal = BSWAP_16(u16_regVal);

#ifdef U5500_PLATFORM_PORTING
    retVal = LLA_IMX072_AFDriver_I2C_Write(((0xCC << 8) | LLA_IMX072_AF_STEP_WIDTH), 0x1, ( uint8 * ) &u16_regVal);
#else
    retVal = LLA_IMX072_AFDriver_I2C_Write(0xCC, 0x2, ( uint8 * ) &u16_regVal);
#endif
    return (retVal);
}


/*LLA_IMX072_AFDriver_MeasurePos:
  Measure the current possition of the Lens... by reading the 10bit curr DAC code
  As, currently driving AF driver in step mode.. so its possible to measure the position

  API, will return current Dac as output variable, and will return error if I2C fails/or any other error etc...
  Not to rely on output variable value, if API itself return an error
*/
CAM_DRV_FN_RETVAL_E
BH6476_AFDriver_MeasurePos(
uint16_t    *u16_CurrentDac)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    //OstTraceInt0(TRACE_DEBUG, "LLA_IMX072_AFDriver_MeasurePos!!");

    /*Read DAC from AF driver*/
    retVal = LLA_IMX072_AFDriver_I2C_Read(0xC8, 0x2, ( uint8 * ) u16_CurrentDac);

    *u16_CurrentDac = BSWAP_16(*u16_CurrentDac);

    /*10-lsb will be the 10bit dac, */
    *u16_CurrentDac = *u16_CurrentDac & 0x3FF;

    /*Return error if above I2C failed*/
    if (CAM_DRV_FN_OK != retVal)
    {
        return (CAM_DRV_FN_FAIL);
    }
    else
    {
        return (CAM_DRV_FN_OK);
    }
}


/*LLA_IMX072_LensMovToPos:-
    Move lens to position, intermidiate fn actual implementation depends on the AF driver used by the
    module,

*/
CAM_DRV_RETVAL_E
BH6476_LensMovToPos(
int32   position,
uint32  *pu32_MoveTime_us)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    CAM_DRV_FN_RETVAL_E status = CAM_DRV_FN_OK;
    uint16_t            u16_currDac,
                        u16_reqPos = 0,
                        u16_MovStep,
                        u16_speed,
                        u16_regVal;

    // OstTraceInt0(TRACE_DEBUG, "LLA_IMX072_LensMovToPos!!");

    /*Actual implementaion of lens movement depend on the actual AF driver used by the module*/
    u16_reqPos = (uint16_t) position;
    *pu32_MoveTime_us = 0;

    /*Check if the position passed is in the permissible range*/
    if (u16_reqPos <= LLA_IMX072_AF_MIN_DAC_CODE)
    {
        u16_reqPos = LLA_IMX072_AF_MIN_DAC_CODE;
    }
    else if (u16_reqPos >= LLA_IMX072_AF_MAX_DAC_CODE)
    {
        u16_reqPos = LLA_IMX072_AF_MAX_DAC_CODE;
    }


    /*Read the current position of the lens, may be the lens is already moving*/
    status = BH6476_AFDriver_MeasurePos(&u16_currDac);

    if (CAM_DRV_FN_OK != status)
    {
        /*error in reading current position of the lens*/
        retVal = CAM_DRV_ERROR_HW;
    }


    if (CAM_DRV_OK == retVal)
    {
        if (u16_currDac != u16_reqPos)
        {
            if (u16_currDac > u16_reqPos)
            {
                u16_MovStep = u16_currDac - u16_reqPos;
            }
            else
            {
                u16_MovStep = u16_reqPos - u16_currDac;
            }

            if (u16_MovStep >= LLA_IMX072_AF_SlewRate_Threshold)
            {
                u16_speed = LLA_IMX072_AF_SlewRate_High_TIME;
            }
            else
            {
                u16_speed = LLA_IMX072_AF_SlewRate_Low_TIME;
            }

            u16_regVal = (0x01 << 8) | ((u16_speed * 1000 / (256 * u16_MovStep) / 50) & 0xff);
            u16_regVal = BSWAP_16(u16_regVal);

#ifdef U5500_PLATFORM_PORTING
            retVal = LLA_IMX072_AFDriver_I2C_Write(((0xCC << 8) | 0x01), 0x1, ( uint8 * ) &u16_regVal); // M = 1, W = 1
#else
            retVal = LLA_IMX072_AFDriver_I2C_Write(0xCC, 0x2, ( uint8 * ) &u16_regVal); // M = 1, W = 1
#endif

            /*Time required to move the lens*/
            *pu32_MoveTime_us = u16_speed * 1000 / 256;

            /*Start moving the lens, timer/message handling will be done by higher layer code*/
            status = BH6476_AFDriver_MovetoPos(u16_reqPos);
            if (CAM_DRV_FN_OK != status)
            {
                /*Error occur in lens movement API*/
                retVal = CAM_DRV_ERROR_HW;
            }
        }


        // No change in position so no need to do anything
        if (*pu32_MoveTime_us < LLA_IMX072_AF_ONE_STEP_TIME)
        {
            *pu32_MoveTime_us = LLA_IMX072_AF_ONE_STEP_TIME;
        }
    }


    if (CAM_DRV_OK != retVal)
    {
        OstTraceInt0(TRACE_DEBUG,"BH6476_LensMovToPos Error in lens movement!!");
        /* For non-debug release, set the lens state to error */
        g_CamDrvConfig.camLensState = CamLensState_e_Error;
        /* AFDriver API return error, failed to move lens */
        LLA_ASSERT_XP70();
    }

    return (retVal);
}
uint8_t BH6476_SupportLensAbort()
{
	return FOCUS_LENS_MOVEMENT_ABORT;
}

