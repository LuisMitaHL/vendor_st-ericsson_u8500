/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#undef OMXCOMPONENT
//#define OMXCOMPONENT "SIA_PLATF_MGR"
#define DBGT_PREFIX "SIA_PLATF_MGR"


#include "sia_platform_manager.h"
#include "OMX_Types.h"
#include "VhcElementDefs.h"

#include "MMIO_Camera.h"
#include <mmio_linux_api.h>
#include <unistd.h>

#include "ImgConfig.h"

//extern "C"{
//#include <cm/inc/cm_macros.h>
#include <cm/inc/cm.hpp>
//}

#include "osi_trace.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_sia_platform_manager_sia_platform_managerTraces.h"
#endif

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#define DEACTIVATE_POWER_DELAY      1
#define DEACTIVATE_EXT_CLK_DELAY    200
#define DEACTIVATE_XSHUTDOWN_DELAY  54
#if (IMG_CONFIG == 501)
#define DEACTIVATE_XSHUTDOWN_DELAY  14
#endif


CSiaPlatformManager::CSiaPlatformManager(TraceObject *traceobj): mTraceObject(traceobj)
{
}


//*************************************************************************
/**
 *  Do MMIO initializations.
 *  @param slot indicates which camera to init
 *  @return OMX_ErrorNone or other OMX error codes
 */
//*************************************************************************
OMX_ERRORTYPE CSiaPlatformManager::init(enumCameraSlot slot) {
	IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CSiaPlatformManager::init", (mTraceObject));
    OMX_ERRORTYPE error=OMX_ErrorNone;

#ifdef MMIO_ENABLE_NEW_IF
    //-------------------------------------------------------
    // Select camera
    //-------------------------------------------------------
    if(slot == ePrimaryCamera)
        MMIO_Camera::selectCamera(1, MMIO_Camera::eRawBayer);
    else
        MMIO_Camera::selectCamera(2, MMIO_Camera::eRawBayer);

    //-------------------------------------------------------
    // Init driver
    //-------------------------------------------------------
    error = MMIO_Camera::initDriver();
    if (error) {
        MSG0(  "ERROR in MMIO_Camera::initDriver!\n");
        return error;
    }

    //-------------------------------------------------------
    // Init hardware
    //-------------------------------------------------------
    error = MMIO_Camera::initHardware();
    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::initHardware !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::initHardware !", (mTraceObject));
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CSiaPlatformManager::init (%d)", (mTraceObject), error);
        return error;
    }
#else
    //-------------------------------------------------------
    // Init board
    //-------------------------------------------------------
    if (slot == ePrimaryCamera)
        error = MMIO_Camera::initBoard(MMIO_Camera::ePrimaryXSD);
    else
        error = MMIO_Camera::initBoard(MMIO_Camera::eSecondaryXSD);
    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::initBoard !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::initBoard !", (mTraceObject));
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CSiaPlatformManager::init (%d)", (mTraceObject), error);
        return error;
    }

    //-------------------------------------------------------
    // Select Primary/Secondary Camera
    //-------------------------------------------------------
	if(slot ==ePrimaryCamera){
	    error = MMIO_Camera::setPrimaryCameraHWInterface();
	    if (error) {
	        DBGT_ERROR("ERROR in MMIO_Camera::setPrimaryCameraHWInterface !\n");
	        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setPrimaryCameraHWInterface !", (mTraceObject));
	        OstTraceFiltStatic1(TRACE_FLOW, "Exit CSiaPlatformManager::init (%d)", (mTraceObject), error);
	        return error;
	   }
	}
    else
	{
	    error = MMIO_Camera::setSecondaryCameraHWInterface();
	    if (error) {
	       DBGT_ERROR("ERROR in MMIO_Camera::setSecondaryCameraHWInterface !\n");
	       OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setSecondaryCameraHWInterface !", (mTraceObject));
	        OstTraceFiltStatic1(TRACE_FLOW, "Exit CSiaPlatformManager::init (%d)", (mTraceObject), error);
	       return error;
	    }
	}

    //-------------------------------------------------------
    // PowerOn the sensor
    //-------------------------------------------------------
    //error = MMIO_Camera::powerSensor(OMX_TRUE);
    //if (error) {
    //    MSG0("ERROR in MMIO_Camera::powerSensor(ON) !\n");
    //    OstTraceFiltStatic0(TRACE_DEBUG, "ERROR in MMIO_Camera::powerSensor(ON) !", (mTraceObject));
    //    return error;
    //}
#endif //MMIO_ENABLE_NEW_IF

    //-------------------------------------------------------
    // Switch ON extClk commented to fix MMIO External clocks are on after exiting from cam application
    //-------------------------------------------------------
    //error = MMIO_Camera::setExtClk(OMX_TRUE);
    //if (error) {
    //    MSG0("ERROR in MMIO_Camera::setExtClk(ON) !\n");
    //   OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setExtClk(ON) !", (mTraceObject));
    //    OstTraceFiltStatic1(TRACE_FLOW, "Exit CSiaPlatformManager::init (%d)", (mTraceObject), error);
    //    return error;
    //  }

    //-------------------------------------------------------
    // Init MMDSP Timer
    //-------------------------------------------------------
    MMIO_Camera::initMMDSPTimer();

    OUTR(" ", error);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CSiaPlatformManager::init (%d)", (mTraceObject), error);
    return error;

}

OMX_ERRORTYPE CSiaPlatformManager::config(enumCameraSlot slot)
{
	//MSG0("Configure platform manager\n");
	//OstTraceFiltStatic0(TRACE_DEBUG, "Configure platform manager", (mTraceObject));
    return OMX_ErrorNone;
}

//*************************************************************************
/**
 *  Undo MMIO initializations.
 *  @param slot indicates which camera to de-init
 *  @return OMX_ErrorNone or other OMX error codes
 */
//*************************************************************************
OMX_ERRORTYPE CSiaPlatformManager::deinit(enumCameraSlot slot) {

    OMX_ERRORTYPE error = OMX_ErrorNone;
#ifndef MMIO_ENABLE_NEW_IF
    //Deactivate I2C2
    error = activate_i2c2(MMIO_DEACTIVATE_I2C);
    if (error) {
        DBGT_ERROR("ERROR in activate_i2c2(MMIO_DEACTIVATE_I2C) !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in activate_i2c2(MMIO_DEACTIVATE_I2C) !", (mTraceObject));
        return error;
    }

    //Delay after deactivating i2c
    usleep(DEACTIVATE_EXT_CLK_DELAY);
#endif //#ifndef MMIO_ENABLE_NEW_IF
    // Switch OFF extClk
    error = MMIO_Camera::setExtClk(OMX_FALSE);
    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::setExtClk(OFF) !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setExtClk(OFF) !", (mTraceObject));
        return error;
    }

    //Delay after stopping external clock
    usleep(DEACTIVATE_XSHUTDOWN_DELAY);
#ifndef MMIO_ENABLE_NEW_IF
    //Toggle xshutdown pin
    if(slot == ePrimaryCamera){
        error = enable_xshutdown_from_host(OMX_TRUE, OMX_TRUE);
        if (error) {
            MSG0(  "ERROR in enable_xshutdown_from_host(ON) !\n");
            return error;
        }
    }
    //Delay after toggling xshutdown pin
    usleep(DEACTIVATE_POWER_DELAY);
#endif
    // PowerOff the sensor
    error = MMIO_Camera::powerSensor(OMX_FALSE);
    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::powerSensor(OFF) !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::powerSensor(OFF) !", (mTraceObject));
        return error;
    }

    error = MMIO_Camera::desinitBoard();
    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::desinitBoard !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::desinitBoard !", (mTraceObject));
    }

    //release camera
#ifdef MMIO_ENABLE_NEW_IF
    error = MMIO_Camera::releaseDriver();
    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::releaseDriver!\n");
        OstTraceFiltStatic0(TRACE_ERROR, "MMIO_Camera::releaseDriver!", (mTraceObject));
    }
#else
    if(slot ==ePrimaryCamera)
        error = MMIO_Camera::releasePrimaryCameraHWInterface();
    else
        error = MMIO_Camera::releaseSecondaryCameraHWInterface();

    if (error) {
        DBGT_ERROR("ERROR in MMIO_Camera::releasePrimary/SecondaryCameraHWInterface !\n");
        OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::releasePrimary/SecondaryCameraHWInterface !", (mTraceObject));
    }
#endif // MMIO_ENABLE_NEW_IF

   return error;

}

void CSiaPlatformManager::setIspctlCfg(Iispctl_api_cfg cfg){
	mIspctlConfigure = cfg;
}
