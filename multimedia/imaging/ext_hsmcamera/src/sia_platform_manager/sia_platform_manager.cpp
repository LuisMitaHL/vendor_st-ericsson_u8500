/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_sia_platform_manager.h"
#include "MMIO_Camera.h"

#ifndef MMIO_ENABLE_NEW_IF
#include <mmio_linux_api.h>
#endif //MMIO_ENABLE_NEW_IF

#undef    DBGT_LAYER
#define   DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "PLTM"
#include "debug_trace.h"

CSiaPlatformManager::CSiaPlatformManager()
{

}


/**
 *  Do MMIO initializations.
 *  @param slot indicates which camera to de-init
 *  @return OMX_ErrorNone or other OMX error codes
 */
OMX_ERRORTYPE CSiaPlatformManager::init(
        enumExtCameraSlot slot)
{
    OMX_ERRORTYPE error=OMX_ErrorNone;

#ifdef MMIO_ENABLE_NEW_IF
    //-------------------------------------------------------
    // Select camera
    //-------------------------------------------------------
    if(slot == ePrimaryExtCamera)
        MMIO_Camera::selectCamera(1, MMIO_Camera::eYUV);
    else
        MMIO_Camera::selectCamera(2, MMIO_Camera::eYUV);

    //-------------------------------------------------------
    // Init driver
    //-------------------------------------------------------
    error = MMIO_Camera::initDriver();
    if (error) {
        DBGT_PTRACE(  "ERROR in MMIO_Camera::initDriver!");
        return error;
    }

    //-------------------------------------------------------
    // Init board
    //-------------------------------------------------------
    error = MMIO_Camera::initHardware();
    if (error) {
        DBGT_PTRACE(  "ERROR in MMIO_Camera::initHardware!");
        return error;
    }

    //-------------------------------------------------------
    // PowerOn the sensor
    //-------------------------------------------------------
    error = MMIO_Camera::powerSensor(OMX_TRUE);
    if (error) {
        DBGT_PTRACE(  "ERROR in MMIO_Camera::powerSensor(ON)!");
        return error;
    }
#else
    if(slot == ePrimaryExtCamera) {
        error = MMIO_Camera::initBoard(MMIO_Camera::ePrimaryXSD);
        if (error)
        {
            DBGT_PTRACE(  "ERROR in MMIO_Camera::initBoard for primary camera!");
            return error;
        }
    } else {
        error = MMIO_Camera::initBoard(MMIO_Camera::eSecondaryXSD);
        if (error)
        {
            DBGT_PTRACE(  "ERROR in MMIO_Camera::initBoard for secondary camera!");
            return error;
        }
    }

    //-------------------------------------------------------
    // Trigger xShutdown
    //-------------------------------------------------------
    if(slot == ePrimaryExtCamera) {
        error = enable_xshutdown_from_host(OMX_TRUE, OMX_TRUE);
        if (error) {
            DBGT_PTRACE(  "ERROR in enable_xshutdown_from_host(ON) !");
            return error;
        }
    } else {
        error = enable_xshutdown_from_host(OMX_TRUE, OMX_FALSE);
        if (error) {
            DBGT_PTRACE(  "ERROR in enable_xshutdown_from_host(ON) !");
            return error;
        }
    }

    //-------------------------------------------------------
    // PowerOn the sensor
    //-------------------------------------------------------
    error = MMIO_Camera::powerSensor(OMX_TRUE);
    if (error) {
        DBGT_PTRACE(  "ERROR in MMIO_Camera::powerSensor(ON) !");
        return error;
    }

    // Select Primary/Secondary Camera
    //-------------------------------------------------------
    if(slot ==ePrimaryExtCamera){
        error = MMIO_Camera::setPrimaryCameraHWInterface();
        if (error) {
            DBGT_PTRACE(  "ERROR in MMIO_Camera::setPrimaryCameraHWInterface !");
            return error;
        }
    }
    else
    {
        error = MMIO_Camera::setSecondaryCameraHWInterface();
        if (error) {
            DBGT_PTRACE(  "ERROR in MMIO_Camera::setSecondaryCameraHWInterface !");
            return error;
        }
    }

    //-------------------------------------------------------
    // Switch ON extClk
    //-------------------------------------------------------
    error = MMIO_Camera::setExtClk(OMX_TRUE);
    if (error) {
        DBGT_PTRACE( "ERROR in MMIO_Camera::setExtClk(ON) !");
        return error;
    }
#endif //MMIO_ENABLE_NEW_IF

    return error;
}

OMX_ERRORTYPE CSiaPlatformManager::config(
        enumExtCameraSlot slot)
{
    //-------------------------------------------------------
    // Init MMDSP Timer
    //-------------------------------------------------------
    MMIO_Camera::initMMDSPTimer();
    return OMX_ErrorNone;
}

/**
 *  Undo MMIO initializations.
 *  @param slot indicates which camera to de-init
 *  @return OMX_ErrorNone or other OMX error codes
 */
OMX_ERRORTYPE CSiaPlatformManager::deinit(
        enumExtCameraSlot slot)
{
    OMX_ERRORTYPE error=OMX_ErrorNone;

    //-------------------------------------------------------
    // PowerOff the sensor
    //-------------------------------------------------------
    error = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error) {
        DBGT_PTRACE("ERROR in MMIO_Camera::powerSensor(OFF) !");
        return error;
    }

#ifndef MMIO_ENABLE_NEW_IF
    //-------------------------------------------------------
    // Switch OFF extClk
    //-------------------------------------------------------
    error = MMIO_Camera::setExtClk(OMX_FALSE);
    if (error) {
        DBGT_PTRACE("ERROR in MMIO_Camera::setExtClk(OFF) !");
        return error;
    }
#endif //MMIO_ENABLE_NEW_IF

    //-------------------------------------------------------
    // Desinit board
    //-------------------------------------------------------
    error = MMIO_Camera::desinitBoard();
    if (error) {
        DBGT_PTRACE("ERROR in MMIO_Camera::deinitBoard !");
        return error;
    }

    //-------------------------------------------------------
    // Release driver
    //-------------------------------------------------------
#ifdef MMIO_ENABLE_NEW_IF
    error = MMIO_Camera::releaseDriver();
    if (error) {
        DBGT_CRITICAL("ERROR in MMIO_Camera::releaseDriver!");
    }
#else
    if(slot ==ePrimaryExtCamera)
        error = MMIO_Camera::releasePrimaryCameraHWInterface();
    else
        error = MMIO_Camera::releaseSecondaryCameraHWInterface();
    if (error) {
        DBGT_PTRACE("ERROR in MMIO_Camera::releasePrimary/SecondaryCameraHWInterface !");
    }
#endif

    return error;
}


void CSiaPlatformManager::setExtIspctlCfg(
        Iext_ispctl_api_cfg cfg)
{
    mExtIspctlConfigure = cfg;

}
