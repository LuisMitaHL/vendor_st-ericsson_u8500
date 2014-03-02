/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//Multimedia Includes
#include <IFM_Types.h>
#include <IFM_Index.h>
#include <OMX_Symbian_ComponentExt_Ste.h>

//Internal Includes
#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 1
#define DBGT_PREFIX "Focus "

#include "STECamTrace.h"
#include "STECamera.h"
#include "STECamFocusHandler.h"
#include "STECamOmxUtils.h"
#include "STECamAutoFocusThreadData.h"

namespace android
{

const FocusMode g_STECamFocusProp[] = {
        {0, CameraParameters::FOCUS_MODE_AUTO, OMX_IMAGE_FocusControlAuto, OMX_SYMBIAN_FocusRangeAuto},
        {0, CameraParameters::FOCUS_MODE_INFINITY, OMX_IMAGE_FocusControlOff,OMX_SYMBIAN_FocusRangeInfinity},
        {0, CameraParameters::FOCUS_MODE_FIXED, OMX_IMAGE_FocusControlOff,OMX_SYMBIAN_FocusRangeHyperfocal},
#ifdef ENABLE_CONTINUOUS_AUTOFOCUS
        {0, CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO,OMX_IMAGE_FocusControlAuto,OMX_SYMBIAN_FocusRangeAuto},
#endif //ENABLE_CONTINUOUS_AUTOFOCUS
        {1, CameraParameters::FOCUS_MODE_MACRO, OMX_IMAGE_FocusControlOn,OMX_SYMBIAN_FocusRangeMacro}
};

#undef _CNAME_
#define _CNAME_ FocusHandler

FocusHandler::FocusHandler (STECamera *aCam) : mSTECamera(aCam),
                                               mFocusModeIndex(kInvalidIndex)
{
    DBGT_PROLOG("");

    //Initializing Focus Status Parameters
    OmxUtils::StructWrapper<OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE>::init(mExtFocusStatus.mFocusStatus);
#ifdef PRINT_ALL_ROI
    mExtFocusStatus.mFocusStatus.nMaxAFAreas= OMX_SYMBIAN_MAX_NUMBER_OF_ROIS;
#else //!PRINT_ALL_ROI
    mExtFocusStatus.mFocusStatus.nMaxAFAreas= 1;
#endif //PRINT_ALL_ROI
    mExtFocusStatus.mFocusStatus.nSize = sizeof(RegionExtFocus);

    DBGT_EPILOG("");
}

FocusHandler::~FocusHandler()
{
    DBGT_PROLOG("");

    mSTECamera = NULL;

    DBGT_EPILOG("");
};

int FocusHandler::getFocusModeIndex(const char * const aFocus)
{
    DBGT_PROLOG("Focus: %s", aFocus);
    DBGT_ASSERT(NULL != aFocus, "Focus is NULL");

    int i = 0, value;
    do {
        value = strcmp(g_STECamFocusProp[i].focus, aFocus);
        if (!value)
            break;
        else
            i++;
    } while (g_STECamFocusProp[i - 1].iMarkendofdata != 1);

    if (value != 0)
        i = kInvalidIndex;

    DBGT_EPILOG("i = %d", i);
    return i;
}

/** Configures a given focus mode. While configuring a focus mode, it configures
 * 1. Range Control
 * 2. Focus Control
 * 3. Optionally, Focus Lock, if ENABLE_LOCK is enabled.
 */
OMX_ERRORTYPE FocusHandler::setUpFocus(const char* const aFocus)
{
    DBGT_PROLOG("Focus: %s", aFocus);
    DBGT_ASSERT(NULL != aFocus, "Focus is NULL");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mFocusModeIndex = getFocusModeIndex(aFocus);

    if (kInvalidIndex == mFocusModeIndex) {
        DBGT_EPILOG("OMX_ErrorBadParameter");
        return OMX_ErrorBadParameter;
    }

    //Setting Focus Range
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE> rangeControl;
    rangeControl.ptr()->nPortIndex = OMX_ALL;

    err = OMX_GetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), rangeControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    rangeControl.ptr()->eFocusRange = g_STECamFocusProp[mFocusModeIndex].eRangeType;

    DBGT_PTRACE("Setting Range %s",OmxUtils::name ((OMX_SYMBIAN_FOCUSRANGETYPE)rangeControl.ptr()->eFocusRange));
    err = OMX_SetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), rangeControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //Setting Focus lock
#ifdef ENABLE_LOCK
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_LOCKTYPE> focuslock;
    focuslock.ptr()->nPortIndex=OMX_ALL;

#ifndef DISABLE_GET_CONFIG_LOCK //Remove this workaround when ER 351601 is resolved
    err = OMX_GetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusLock), focuslock.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }
#endif //DISABLE_GET_CONFIG_LOCK

    focuslock.ptr()->eImageLock = getLockType();

    DBGT_PTRACE("Setting Lock %s",OmxUtils::name((OMX_SYMBIAN_LOCKTYPE) focuslock.ptr()->eImageLock));
    err = OMX_SetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusLock), focuslock.ptr() );
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }
#endif //ENABLE_LOCK

    //Setting Focus Control
    OmxUtils::StructContainer<OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE> focusControl;
    focusControl.ptr()->nPortIndex = OMX_ALL;

    err = OMX_GetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    focusControl.ptr()->eFocusControl = g_STECamFocusProp[mFocusModeIndex].eFocusType;
    DBGT_PTRACE("Setting Control %u", focusControl.ptr()->eFocusControl);

    err = OMX_SetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE FocusHandler::doFocus()
{
    DBGT_PROLOG("");

    mSTECamera->mAutoFocusThreadData->mEnabled = OMX_TRUE;

    OmxUtils::StructContainer<OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE> focusControl;
    focusControl.ptr()->nPortIndex = OMX_ALL;

    DBGT_ASSERT(OMX_IMAGE_FocusControlOff != g_STECamFocusProp[mFocusModeIndex].eFocusType,
                        "Focussing not permitted in this mode");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = mSTECamera->mOmxReqCbHandler.enable(OmxReqCbHandler::EAutoFocusLock);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Enable reqcb failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    focusControl.ptr()->eFocusControl = OMX_IMAGE_FocusControlOn;

    err = OMX_SetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE FocusHandler::handleAFEvent()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    bool focusReached = false;

    //get status
#ifdef ENABLE_AUTOFOCUS
        err = getFocusStatus();
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("getFocusStatus failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        if(OMX_TRUE == mExtFocusStatus.mFocusStatus.bFocused) {
            mSTECamera->mPerfManager.logAndDump(PerfManager::EAutoFocusReached);
            focusReached = true;
        }
    DBGT_PTRACE("Focus reached: %d", focusReached);
#endif

#ifdef ENABLE_CONTINUOUS_AUTOFOCUS
    if (mSTECamera->mMsgEnabled & CAMERA_MSG_FOCUS_MOVE) {
        int lensState;
        int focusStatus = (OMX_FOCUSSTATUSTYPE)mExtFocusStatus.mFocusStatus.sAFROIs[0].eFocusStatus;
        switch (focusStatus) {
            case OMX_FocusStatusRequest:
                lensState = ETrackingState;
                break;
            case OMX_FocusStatusUnableToReach:
                lensState = EFocusUnableToReach;
                break;
            case OMX_FocusStatusReached:
                lensState = EFocusReached;
                break;
            default:
                DBGT_PTRACE("Invalid focus status");
                break;
        }

        //If autofocus msg is enable, that means capture is in progress
        //no need to send callback
        //If lensState is same as previous then no need to send callback
        if (!(mSTECamera->mAutoFocusThreadData->mEnabled)
                && !(lensState == mSTECamera->mAutoFocusThreadData->mContinuousFocusState)) {
            mSTECamera->mLock.unlock();
            mSTECamera->mNotifyCb(CAMERA_MSG_FOCUS_MOVE, !(focusReached), 0, mSTECamera->mCallbackCookie);
            mSTECamera->mLock.lock();
            //Assign new state
            mSTECamera->mAutoFocusThreadData->mContinuousFocusState = lensState;
        }
    }
#endif

#ifdef ENABLE_AUTOFOCUS
    if (mSTECamera->mMsgEnabled & CAMERA_MSG_FOCUS) {
        mSTECamera->mLock.unlock();
        if (mSTECamera->mAutoFocusThreadData->mEnabled) {
            mSTECamera->mNotifyCb(CAMERA_MSG_FOCUS, focusReached, 0, mSTECamera->mCallbackCookie);
            mSTECamera->mAutoFocusThreadData->mEnabled = OMX_FALSE;
            mSTECamera->mMsgEnabled &= ~CAMERA_MSG_FOCUS;
        }
        mSTECamera->mLock.lock();
    }
#else //!ENABLE_AUTOFOCUS
        mSTECamera->mPerfManager.logAndDump(PerfManager::EAutoFocusReached);
#endif //ENABLE_AUTOFOCUS

#ifdef PRINT_ALL_ROI
    if(focusReached == true) {
            for (int i=0; i< (int)mExtFocusStatus.mFocusStatus.nAFAreas; i++){
                DBGT_PINFO("For RoI %d: Status is",i);
                DBGT_PINFO("Focus Status for ROI %s",OmxUtils::name((OMX_FOCUSSTATUSTYPE)mExtFocusStatus.mFocusStatus.sAFROIs[i].eFocusStatus ) );
                DBGT_PINFO("Ref-W%d Ref-H%d",(int)(mExtFocusStatus.mFocusStatus.sAFROIs[i].nReferenceWindow.nWidth),
                    (int) (mExtFocusStatus.mFocusStatus.sAFROIs[i].nReferenceWindow.nHeight) );
                DBGT_PINFO("Rectangle X%d,Y%d,W%d,H%d ",
                    (int)(mExtFocusStatus.mFocusStatus.sAFROIs[i].nRect.sTopLeft.nX),
                    (int)(mExtFocusStatus.mFocusStatus.sAFROIs[i].nRect.sTopLeft.nY) ,
                    (int)(mExtFocusStatus.mFocusStatus.sAFROIs[i].nRect.sSize.nWidth),
                    (int)(mExtFocusStatus.mFocusStatus.sAFROIs[i].nRect.sSize.nHeight) );
            }
        }
#endif //PRINT_ALL_ROI
    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE FocusHandler::cancelFocus()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    OmxUtils::StructContainer<OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE> focusControl;

    focusControl.ptr()->nPortIndex = OMX_ALL;

    err = OMX_GetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    setupFocusControl(focusControl.ref());

    err = OMX_SetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = getFocusStatus();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("getFocusStatus failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE FocusHandler::getFocusStatus()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if(OMX_IMAGE_FocusControlOff != g_STECamFocusProp[mFocusModeIndex].eFocusType)
        err = OMX_GetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusStatus), &mExtFocusStatus.mFocusStatus);
    else
        DBGT_PINFO("Autofocus not done in this mode: OMX_IMAGE_FocusControlOff");

    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

#undef _CNAME_
#define _CNAME_ AutoFocusHandler

AutoFocusHandler::AutoFocusHandler(STECamera *aCam) : FocusHandler(aCam)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_SYMBIAN_LOCKTYPE AutoFocusHandler::getLockType()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("OMX_SYMBIAN_LockAtCapture");
    return OMX_SYMBIAN_LockAtCapture;
}

void AutoFocusHandler::setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl)
{
    DBGT_PROLOG("");

    /* Reset the below fields to avoid manual focus configuration while sending
       OMX_IMAGE_FocusControlOff */
    aFocusControl.nFocusStepIndex = 0;
    aFocusControl.nFocusSteps = 0;
    aFocusControl.eFocusControl = OMX_IMAGE_FocusControlOff;

    DBGT_EPILOG("");
}

OMX_ERRORTYPE  AutoFocusHandler::moveLensToDefaultPos()
{
    DBGT_PROLOG("");

    /** Reset the Lens position to rest position */

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

#undef _CNAME_
#define _CNAME_ FixedFocusHandler
FixedFocusHandler::FixedFocusHandler(STECamera *aCam) : FocusHandler(aCam)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}


OMX_SYMBIAN_LOCKTYPE FixedFocusHandler::getLockType()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
    return (OMX_SYMBIAN_LOCKTYPE)NULL;
}

void FixedFocusHandler::setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl)
{
    DBGT_PROLOG("");

    aFocusControl.nFocusStepIndex = 0;
    aFocusControl.nFocusSteps = 100;
    aFocusControl.eFocusControl = OMX_IMAGE_FocusControlOff;

    DBGT_EPILOG("");
}

OMX_ERRORTYPE  FixedFocusHandler::moveLensToDefaultPos()
{
    DBGT_PROLOG("");

    /** Reset the Lens position to rest position */

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}
OMX_ERRORTYPE  FixedFocusHandler::doFocus()
{
    DBGT_PROLOG("");

   /* Do nothing in case of fixed focus mode*/

   DBGT_EPILOG("");
   return OMX_ErrorNone;
}

OMX_ERRORTYPE FixedFocusHandler::setUpFocus(const char* const aFocus)
{
    DBGT_PROLOG("Focus: %s", aFocus);
    DBGT_ASSERT(NULL != aFocus, "Focus is NULL");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    //Setting Focus Control
    OmxUtils::StructContainer<OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE> focusControl;
    focusControl.ptr()->nPortIndex = OMX_ALL;

    err = OMX_GetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    focusControl.ptr()->eFocusControl = OMX_IMAGE_FocusControlOff;
    focusControl.ptr()->nFocusStepIndex = 0;
    focusControl.ptr()->nFocusSteps = 100;
    DBGT_PTRACE("Setting Control %u", focusControl.ptr()->eFocusControl);

    err = OMX_SetConfig(mSTECamera->mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

#undef _CNAME_
#define _CNAME_ ContinuousAutoFocusHandler

ContinuousAutoFocusHandler::ContinuousAutoFocusHandler(STECamera *aCam) : FocusHandler(aCam)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_SYMBIAN_LOCKTYPE ContinuousAutoFocusHandler::getLockType()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("OMX_STE_LockLensBestPosition");
    return (OMX_SYMBIAN_LOCKTYPE)OMX_STE_LockLensBestPosition;
}

void ContinuousAutoFocusHandler::setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl)
{
    DBGT_PROLOG("");
    aFocusControl.eFocusControl = OMX_IMAGE_FocusControlAuto;
    DBGT_EPILOG("");
}

OMX_ERRORTYPE  ContinuousAutoFocusHandler::moveLensToDefaultPos()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

}; //namespace android
