/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
#include "ssc.h"

#include "ite_boardinfo.h"
#include "ite_testenv_utils.h"
#include "ite_pageelements.h"
#include <los/api/los_api.h>
//For Linux
#include "MMIO_Camera.h"
#ifdef __ARM_LINUX
#   include "mmio_linux_api.h"
#endif //__ARM_LINUX
extern t_uint32 prcmu_base_address;

Result_te
ITE_switchBoardDirectI2CMode(void)
{
    //For Linux
    //Use of Linux MMIO Camera driver interface
    //MMIO_Camera::initBoard();
    //Enable XSHUTDOWN_FROM_HOST for communicating with Camera sensors from host
#ifdef __ARM_LINUX
    OMX_ERRORTYPE   error = enable_xshutdown_from_host(OMX_TRUE, OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardDirectI2CMode : enable_xshutdown_from_host %d", error);
        goto ERR_XSHUTDOWN_FAILED;
    }


    //Activate I2C2 : gpio 8 & 9 in alternate B
    error = activate_i2c2(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardDirectI2CMode : activate_i2c2 %d", error);
        goto ERR_ACT_I2C_FAILED;
    }

    LOS_Sleep(500);
    return Result_e_Success;

ERR_ACT_I2C_FAILED:
    //Disable XSHUTDOWN_FROM_HOST
    error = enable_xshutdown_from_host(OMX_FALSE, OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardIRPI2CMode : enable_xshutdown_from_host %d", error);
    }


ERR_XSHUTDOWN_FAILED:
    return Result_e_Failure;

#endif //__ARM_LINUX
}


Result_te
ITE_switchBoardIRPI2CMode(void)
{
    //For Linux
    //Disactivate I2C2 and reactivate IPI2C on gpio 8&9
#ifdef __ARM_LINUX
    OMX_ERRORTYPE   error = activate_i2c2(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardIRPI2CMode : activate_i2c2 %d", error);
        goto ERR_ERROR1;
    }

    //Disable XSHUTDOWN_FROM_HOST
    error = enable_xshutdown_from_host(OMX_FALSE, OMX_TRUE);

    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardIRPI2CMode : enable_xshutdown_from_host %d", error);
        goto ERR_ERROR1;
    }
    LOS_Sleep(500);
    return Result_e_Success;


ERR_ERROR1:
    return Result_e_Failure;

#endif //__ARM_LINUX
}


Result_te
ITE_switchBoardSensor0(void)
{

    OMX_ERRORTYPE   error = MMIO_Camera::setPrimaryCameraHWInterface();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardSensor : MMIO_Camera::setPrimary CameraHWInterface WARNING %d", error);
        return Result_e_Failure;
    }
    LOS_Sleep(500);
    return Result_e_Success;
}


Result_te
ITE_switchBoardSensor1(void)
{

    OMX_ERRORTYPE   error = MMIO_Camera::setSecondaryCameraHWInterface();

    if (error != OMX_ErrorNone)
    {
        LOS_Log("ITE_switchBoardSensor : MMIO_Camera::setSecondary CameraHWInterface WARNING %d", error);
        return Result_e_Failure;
    }
    LOS_Sleep(500);
    return Result_e_Success;
}


