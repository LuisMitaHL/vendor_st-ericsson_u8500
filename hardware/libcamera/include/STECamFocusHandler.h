/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * STECamFocusHandler.h: This class manages focus modes.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef STECAMFOCUSHANDLER_H
#define STECAMFOCUSHANDLER_H

//Multimedia Includes
#include <OMX_Core.h>

//Internal Includes
#include <STECamOmxILExtIndex.h>
#include "STECamProp.h"

namespace android {

//forward class declarations
class STECamera;

class FocusHandler {
    public:
        /**
         * Returns lock type for currently configured focus mode.
         * For Continuous Auto Focus mode, OMX_STE_LockLensBestPosition
         * is returned.
         * For Single Auto Focus mode, OMX_SYMBIAN_LockAtCapture is
         * returned.
         */
        virtual OMX_SYMBIAN_LOCKTYPE getLockType() = 0;
        /**
         * Used to restore the Focus Control to appropriate state
         * after Focus Control has been turned on during doFocus().
         * For continuous auto focus mode, focus control is restored
         * to OMX_IMAGE_FocusControlAuto, while in single auto focus mode,
         * focus control is restored to OMX_IMAGE_FocusControlOff.
         */
        virtual void setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl) = 0;

        /**
         * Reset the Lens position to default position
         * Set the Focus control to OMX_IMAGE_FocusControlOff and
         * range according to autofocus mode.
         */
        virtual OMX_ERRORTYPE moveLensToDefaultPos() = 0;

    public:
        FocusHandler(STECamera *aCam);
        virtual ~FocusHandler();
        /**
         * Sets up Focus Control, Range Control and optionally, Focus Lock.
         * Focus Control is set up for the focus modes which are not single
         * shot type, i.e. Focus Control type is not OMX_IMAGE_FocusControlOn.
         * Currently, OMX_STE_LockLensBestPosition is supported for continuous
         * autofocus mode and OMX_SYMBIAN_LockAtCapture for single AF cases.
         */
        OMX_ERRORTYPE setUpFocus(const char* const aFocus);
        /**
          * Turns the focus control to OMX_IMAGE_FocusControlOn and
          * requests lower layer to provide a notification/result of focus
          * operation.
          */
        OMX_ERRORTYPE doFocus();
        /**
         * Handles AF event handling. Camera client is notifed of focus status
         */
        OMX_ERRORTYPE handleAFEvent();
        /**
         * Restores the appropriate focus control (OMX_IMAGE_FocusControlOff/
         * OMX_IMAGE_FocusControlAuto) after a call from either STECamera::doAutoFocusProcessing()
         * or STECamera::cancelAutoFocus().
         */
        OMX_ERRORTYPE cancelFocus();

        /**
         * Return the current value of focal Distance from mFocusStatus
         */
        inline float getFocusDistance();

    private:
        /** Returns the g_STECamFocusProp index for supplied focus mode */
        int getFocusModeIndex(const char * const aFocus);

        /**
         * Get the focus distance from the
         * OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE index
         * Set the current value in mFocusStatus
         */
        OMX_ERRORTYPE getFocusStatus();
    protected:
        STECamera *mSTECamera; /**< Camera */

        RegionExtFocus mExtFocusStatus;
    private:
         enum {
            kInvalidIndex = -1,
         };
        int mFocusModeIndex; //contains currently configured focus property

};

class AutoFocusHandler : public FocusHandler {
    public:
        AutoFocusHandler(STECamera *aCam);
    public:
        /** Returns OMX_SYMBIAN_LockAtCapture */
        virtual OMX_SYMBIAN_LOCKTYPE getLockType();
        /** Setup Focus Control- OMX_IMAGE_FocusControlOff */
        virtual void setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl);

        /** Reset the Lens position to rest position */
        OMX_ERRORTYPE moveLensToDefaultPos();
};
class FixedFocusHandler : public FocusHandler {
    public:
        FixedFocusHandler(STECamera *aCam);
    public:
        /** Returns OMX_SYMBIAN_LockAtCapture */
        virtual OMX_SYMBIAN_LOCKTYPE getLockType();
        /** Setup Focus Control- OMX_IMAGE_FocusControlOff */
        virtual void setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl);
        /** Reset the Lens position to rest position */
       virtual OMX_ERRORTYPE moveLensToDefaultPos();
        /** Do nothing in setup pfics**/
        virtual OMX_ERRORTYPE setUpFocus(const char* const aFocus);
        virtual OMX_ERRORTYPE doFocus();
};
class ContinuousAutoFocusHandler : public FocusHandler {
    public:
        ContinuousAutoFocusHandler(STECamera *aCam);
    public:
        /** Returns OMX_STE_LockLensBestPosition */
        virtual OMX_SYMBIAN_LOCKTYPE getLockType();
        /**
         * Setup Focus Control OMX_IMAGE_FocusControlAuto.
         * Resume again in continuous auto focus mode after
         * takePicture() or cancelAutoFocus().
         */
        virtual void setupFocusControl(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE& aFocusControl);

        /** Do Nothing*/
        OMX_ERRORTYPE moveLensToDefaultPos();
};

#include "STECamFocusHandler.inl"

} //namespace android

#endif //STECAMFOCUSHANDLER_H
