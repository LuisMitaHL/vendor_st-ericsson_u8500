/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "ite_platform.h"
#include "MMIO_Camera.h"
#include "los/api/los_api.h"

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_main.h"
#include "ite_testenv_utils.h"

#include "primary_sensor.h"
#include "secondary_sensor.h"

extern ts_sia_usecase   usecase;

#ifdef __ARM_SYMBIAN
#   include "mmio_osal.h"
#endif
t_uint32                prcmu_base_address;
t_uint32                smia_base_address;
t_uint32                ssp_base_address;

CSensor                 *iSensor = ( CSensor * ) NULL;

/* temporary here!!!!!! */
#define SIA_MEM_BASE_ADDR   0xA0200000
#define SIA_MEM_END_ADDR    0xA02FFFFF

// ***************************************************************************************
Result_te
ITE_InitBoard(void)
{
    OMX_ERRORTYPE   error = OMX_ErrorNone;

    LOS_Log("\n >> ITE_InitBoard");

#ifndef MMIO_ENABLE_NEW_IF
    if (0 == usecase.sensor)
    {
        if (iSensor == NULL)
        {
            iSensor = new CSensorPrimary();
        }


        LOS_Log("\n Selecting primary sensor for Initboard");
        error = MMIO_Camera::initBoard(MMIO_Camera::ePrimaryXSD);
        if (error != OMX_ErrorNone)
        {
            LOS_Log("\n MMIO_Camera::initBoard FAILED Primary Camera %d", error);
            goto ERR_ERROR1;
        }
    }
    else
    {
        if (iSensor == NULL)
        {
            iSensor = new CSensorSecondary();
        }


        LOS_Log("\n Selecting secondary sensor for Initboard");
        error = MMIO_Camera::initBoard(MMIO_Camera::eSecondaryXSD);
        if (error != OMX_ErrorNone)
        {
            LOS_Log("\n MMIO_Camera::initBoard FAILED Secondary Camera %d", error);
            goto ERR_ERROR1;
        }
    }
    return Result_e_Success;

#else //MMIO_ENABLE_NEW_IF
    //-------------------------------------------------------
    // Select camera
    //-------------------------------------------------------
    if (0 == usecase.sensor)
    {
        if (iSensor == NULL)
        {
            iSensor = new CSensorPrimary();
        }


        error = MMIO_Camera::selectCamera(1, MMIO_Camera::eRawBayer);
        if (error != OMX_ErrorNone)
        {
            LOS_Log("\n MMIO_Camera::selectCamera FAILED Primary Camera %d", error);
            goto ERR_ERROR1;
        }
    }
    else
    {
        if (iSensor == NULL)
        {
            iSensor = new CSensorSecondary();
        }


        error = MMIO_Camera::selectCamera(2, MMIO_Camera::eRawBayer);
        if (error != OMX_ErrorNone)
        {
            LOS_Log("\n MMIO_Camera::selectCamera FAILED Secondary Camera %d", error);
            goto ERR_ERROR1;
        }
    }


    //-------------------------------------------------------
    // Init driver
    //-------------------------------------------------------
    error = MMIO_Camera::initDriver();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("\n MMIO_Camera::initDriver ERROR %d", error);
        goto ERR_ERROR1;
    }


    //-------------------------------------------------------
    // Init hardware
    //-------------------------------------------------------
    error = MMIO_Camera::initHardware();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("\n MMIO_Camera::initHardware ERROR %d", error);
        goto ERR_ERROR2;
    }

    return Result_e_Success;

ERR_ERROR2:
    error = MMIO_Camera::releaseDriver();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:ERROR in MMIO_Camera::releaseDriver, error =  %d\n", error); 
    }

#endif //MMIO_ENABLE_NEW_IF


ERR_ERROR1:
    // If in any case code has jumped to ERR_ERROR1 we return failure
    return Result_e_Failure;



}


Result_te
ITE_InitPrimary(void)
{
    OMX_ERRORTYPE   error = OMX_ErrorNone;
#ifndef MMIO_ENABLE_NEW_IF
    error = MMIO_Camera::setPrimaryCameraHWInterface();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("\n MMIO_Camera::setPrimaryCameraHWInterface ERROR %d", error);
        return Result_e_Failure;
    }
#endif

#if (defined(ANDROID) || defined(__ARM_SYMBIAN))
    error = MMIO_Camera::powerSensor(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("MMIO_Camera::powerSensor Error %d", error);
        goto ERR_POWERSENSOR;
    }

    error = MMIO_Camera::setExtClk(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("MMIO_Camera::initPrimary FAILED Primary Camera %d", error);
        goto ERR_SETCLK;
    }


    return Result_e_Success;

ERR_SETCLK:
    error = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("Can't switch off sensor, error =  %d\n", error);
    }

ERR_POWERSENSOR:
#ifndef MMIO_ENABLE_NEW_IF
    error = MMIO_Camera::releasePrimaryCameraHWInterface();

    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't release Primary Camera HW Interface error %d\n", error);
    }
#endif
    return Result_e_Failure;

#endif

}


Result_te
ITE_InitSecondary(void)
{
    OMX_ERRORTYPE   error = OMX_ErrorNone;
#ifndef MMIO_ENABLE_NEW_IF
    error = MMIO_Camera::setSecondaryCameraHWInterface();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("\n MMIO_Camera::setSecondaryCameraHWInterface ERROR %d", error);
        return Result_e_Failure;
    }
#endif

#if (defined(ANDROID) || defined(__ARM_SYMBIAN))
    error = MMIO_Camera::powerSensor(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("MMIO_Camera::powerSensor Error %d", error);
        goto ERR_POWERSENSOR;
    }


    error = MMIO_Camera::setExtClk(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("MMIO_Camera::initSecondary FAILED Secondary Camera %d", error);
        goto ERR_SETCLK;
    }


    return Result_e_Success;

ERR_SETCLK:
    error = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("Can't switch off sensor, error =  %d\n", error);
    }

ERR_POWERSENSOR:
#ifndef MMIO_ENABLE_NEW_IF
    error = MMIO_Camera::releaseSecondaryCameraHWInterface();

    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't release Secondary Camera HW Interface error %d\n", error);
    }
#endif
    return Result_e_Failure;

#endif

}


Result_te
ITE_DeInitSecondary(void)
{
    OMX_ERRORTYPE   error = OMX_ErrorNone;

    error = MMIO_Camera::setExtClk(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't switch of sensor clocks, error =  %d\n", error);
        goto ERR_DeInitSecondary;
    }


    error = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't switch off sensor, error =  %d\n", error);
        goto ERR_DeInitSecondary;
    }


#ifndef MMIO_ENABLE_NEW_IF
    LOS_Log("ite_DeInit: release Secondary Camera HW Interface\n");
    error = MMIO_Camera::releaseSecondaryCameraHWInterface();

    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't release Secondary Camera HW Interface error %d\n", error);
        goto ERR_DeInitSecondary;
    }


#else
    error = MMIO_Camera::desinitBoard();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:ERROR in MMIO_Camera::desinitBoard, error =  %d\n", error);
        goto ERR_DeInitSecondary;
    }


    error = MMIO_Camera::releaseDriver();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:ERROR in MMIO_Camera::releaseDriver, error =  %d\n", error);
        goto ERR_DeInitSecondary;
    }
#endif
    return Result_e_Success;

ERR_DeInitSecondary:
    return Result_e_Failure;

}


Result_te
ITE_DeInitPrimary(void)
{
    OMX_ERRORTYPE   error = OMX_ErrorNone;

    error = MMIO_Camera::setExtClk(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't switch of sensor clocks, error =  %d\n", error);
        goto ERR_DeInitPrimary;
    }


    error = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't switch off sensor, error =  %d\n", error);
        goto ERR_DeInitPrimary;
    }


#ifndef MMIO_ENABLE_NEW_IF
    LOS_Log("ite_DeInit: release Primary Camera HW Interface\n");
    error = MMIO_Camera::releasePrimaryCameraHWInterface();

    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:Can't release Primary Camera HW Interface error %d\n", error);
        goto ERR_DeInitPrimary;
    }


#else
    error = MMIO_Camera::desinitBoard();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:ERROR in MMIO_Camera::desinitBoard, error =  %d\n", error);
        goto ERR_DeInitPrimary;
    }


    error = MMIO_Camera::releaseDriver();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_Term:ERROR in MMIO_Camera::releaseDriver, error =  %d\n", error);
        goto ERR_DeInitPrimary;
    }
#endif
    return Result_e_Success;

ERR_DeInitPrimary:
    return Result_e_Failure;

}


int
IspRegWrite_Wrapper(
unsigned long   t1_dest,
const long int  *logical_addr,
int             count)
{
    int ret = 0;
    ret = ( int ) MMIO_Camera::ispVectorWrite(t1_dest, logical_addr, count);
    return (ret);
}


int
IspRegRead_Wrapper(
unsigned long   t1_dest,
const long int  *logical_addr,
int             count)
{
    UNUSED(t1_dest);
    UNUSED(logical_addr);
    UNUSED(count);

    // No ispVectorRead option
    return (0);
}


t_uint32
IspRegRead_Value(
unsigned long   t1_dest,
const long int  *logical_addr,
int             count)
{
    UNUSED(t1_dest);
    UNUSED(logical_addr);
    UNUSED(count);

    // No ispVectorRead option
    return (0);
}


Result_te
ITE_PlatformMmioTest(
int choice)
{
    OMX_ERRORTYPE   error = OMX_ErrorNone;
    char            mess[256],
                    filename[50],
                    pathname[255],
                    sensorname[20];
    int             status;
    snprintf(mess, sizeof(mess), IMAGING_PATH "/ite_nmf/test_results_nreg/SensorOutputMode_tests");
    status = mkdir(mess, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status == -1)
    {
        if (EEXIST != errno)
        {
            LOS_Log("\nERROR FOLDER NOT CREATED, ERRORNO:%d\n", errno);
        }
    }
    else
    {
        LOS_Log("\nFOLDER %s CREATED !!\n", mess);
    }


    if (usecase.sensor == 0)
    {
        sprintf(sensorname, "Cam0_");
    }
    else
    {
        sprintf(sensorname, "Cam1_");
    }


    snprintf(filename, sizeof(filename), "TestMMIOAPI%s", sensorname);
    snprintf(pathname, sizeof(pathname), IMAGING_PATH "/ite_nmf/test_results_nreg/SensorOutputMode_tests");
    strcat(g_out_path, pathname);

    mmte_testStart(filename, "Test Platform Test for MMIO API", g_out_path);
    sprintf(mess, "Testing for MMIO API ");
    mmte_testNext(mess);

    LOS_Log("\nite_mmio test command %d\n", choice);

#ifdef __ARM_SYMBIAN
    LOS_Log("ite_mmio test: MMIO_Camera::initDriver\n");
    LOS_Sleep(10);
    error = MMIO_Camera::initDriver();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Warning Initialize Driver returned error %d\n", error);
        goto ERR_ERROR1;
        // Ignoring since driver may already have been initialized
    }
#endif
    LOS_Log("ite_mmio test: MMIO_Camera::initBoard\n");
    LOS_Sleep(10);

#if 1
    if (0 == usecase.sensor)
    {
        LOS_Log("\n Selecting primary sensor for Initboard");
        error = MMIO_Camera::initBoard(MMIO_Camera::ePrimaryXSD);
    }
    else
    {
        LOS_Log("\n Selecting secondary sensor for Initboard");
        error = MMIO_Camera::initBoard(MMIO_Camera::eSecondaryXSD);
    }
#else
    error = MMIO_Camera::initBoard();
#endif
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Warning to Initialize Board %d\n", error);
        LOS_Log("ite_mmio test: The Board may already be initialized\n");
        goto ERR_ERROR1;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::setPrimaryCameraHWInterface\n");
    LOS_Sleep(10);
    error = MMIO_Camera::setPrimaryCameraHWInterface();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to setPrimaryCameraHWInterface %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR1;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::setSecondaryCameraHWInterface\n");
    LOS_Sleep(10);
    error = MMIO_Camera::setSecondaryCameraHWInterface();
    if (error != OMX_ErrorNotReady)
    {
        LOS_Log("ite_mmio test: WARNING setSecondaryCameraHWInterface Expected OMX_ErrorNotReady, Got %d\n", error);
        goto ERR_ERROR1;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::powerSensor\n");
    LOS_Sleep(10);
    error = MMIO_Camera::powerSensor(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to Power Sensor %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR1;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::setExtClk\n");
    LOS_Sleep(10);
    error = MMIO_Camera::setExtClk(OMX_TRUE);
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to setExtClk %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR2;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::releaseSecondaryCameraHWInterface\n");
    error = MMIO_Camera::releaseSecondaryCameraHWInterface();
    if (error != OMX_ErrorNotReady)
    {
        LOS_Log("ite_mmio test: Failed to Release Primary Camera HW Interface%d\n", error);
        goto ERR_ERROR1;
    }

    LOS_Log("ite_mmio test: MMIO_Camera::desinitBoard\n");
    error = MMIO_Camera::desinitBoard();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to desinitBoard Error- %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR1;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::desinitBoard\n");
#if 1
    if (0 == usecase.sensor)
    {
        LOS_Log("\n Selecting primary sensor for Initboard");
        error = MMIO_Camera::initBoard(MMIO_Camera::ePrimaryXSD);
    }
    else
    {
        LOS_Log("\n Selecting secondary sensor for Initboard");
        error = MMIO_Camera::initBoard(MMIO_Camera::eSecondaryXSD);
    }


#else
    error = MMIO_Camera::initBoard();
#endif
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to MMIO_Camera::initBoard Error- %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR1;
    }


    LOS_Log("ite_mmio test: MMIO_Camera::desinitBoard -2\n");
    error = MMIO_Camera::desinitBoard();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to desinitBoard -2 Error- %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR1;
    }


#ifdef __ARM_SYMBIAN
    LOS_Log("ite_mmio test: MMIO_Camera::releaseDriver\n");
    LOS_Sleep(10);
    error = MMIO_Camera::releaseDriver();
    if (error != OMX_ErrorNone)
    {
        LOS_Log("ite_mmio test: Failed to releaseDriver %d\n", error);
        mmte_testResult(TEST_FAILED);
        mmte_testEnd();
        goto ERR_ERROR1;
    }


#endif
    mmte_testResult(TEST_PASSED);
    mmte_testEnd();
    return Result_e_Success;

ERR_ERROR2:
        error = MMIO_Camera::powerSensor(OMX_FALSE);
        if (error != OMX_ErrorNone)
        {
            LOS_Log("Can't switch off sensor, error =  %d\n", error);
        }

ERR_ERROR1:
    return Result_e_Failure;

}


Result_te
DoPowerAction(
t_uint32    u32_power_command)
{
    OMX_ERRORTYPE   error_id = OMX_ErrorNone;
    LOS_Log(">> DoPowerAction");
    if (PowerCommand_e_voltage_on == u32_power_command)
    {
        LOS_Log("turning on sensor power");
        error_id = MMIO_Camera::powerSensor(OMX_TRUE);
        if (error_id != OMX_ErrorNone)
        {
            LOS_Log("Sensor poweron failure,  error = %d", error_id);
            goto ERR_ERROR1;
        }
    }
    else if (PowerCommand_e_ext_clk_on == u32_power_command)
    {
        LOS_Log("turning on external clock");
        error_id = MMIO_Camera::setExtClk(OMX_TRUE);
        if (error_id != OMX_ErrorNone)
        {
            LOS_Log("external clock poweron failure,  error = %d", error_id);
            goto ERR_ERROR2;
        }
    }
    else if (PowerCommand_e_voltage_off == u32_power_command)
    {
        LOS_Log("turning off sensor power");
        error_id = MMIO_Camera::powerSensor(OMX_FALSE);
        if (error_id != OMX_ErrorNone)
        {
            LOS_Log("Sensor power-off failure,  error = %d", error_id);
            goto ERR_ERROR1;
        }
    }
    else if (PowerCommand_e_ext_clk_off == u32_power_command)
    {
        LOS_Log("turning off external clock");
        error_id = MMIO_Camera::setExtClk(OMX_FALSE);
        if (error_id != OMX_ErrorNone)
        {
            LOS_Log("external clock power-off failure,  error = %d", error_id);
            goto ERR_ERROR1;
        }
    }
    LOS_Log("<< DoPowerAction");
    return Result_e_Success;

ERR_ERROR2:
    error_id = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error_id != OMX_ErrorNone)
    {
        LOS_Log("Can't switch off sensor, error =  %d\n", error_id);
    }

ERR_ERROR1:
    return Result_e_Failure;



}

