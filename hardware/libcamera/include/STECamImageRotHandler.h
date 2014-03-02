/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * STECamFocusHandler.h: This class manages image rotation .
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef STECAMIMAGEROTHANDLER_H
#define STECAMIMAGEROTHANDLER_H


//Multimedia Includes
#include <OMX_Core.h>

namespace android {

//forward class declarations
class STECamera;

class ImageRotHandler {
    public:
        /**
         * set AutoRotationControl.
         * Incase AutoRotationControl = OFF, ISP Proc should update the
         * ExifParams orientation field with the value in CaptureParameters.
         * Incase AutoRotationControl = ON, ISP Proc should do the rotation for 90/180
         * and set the CaptureParameters\ExifParams to normal orientation.
         * Similarly for ARM IV Proc.
         */
        virtual OMX_ERRORTYPE configAutoRotate();
        /**
         * Do the configuration required for capturing image at any orientation
         */
        virtual OMX_ERRORTYPE doConfiguration();
        /**
         * Handles any SW processing required in case of,
         * Device 180 degree or EXIF rotation
         */
        virtual OMX_ERRORTYPE handleBuffer(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr);

        /* Setup Hardware or software processing*/
        virtual OMX_ERRORTYPE setupRotationProcessing();

        /* update the thumbnail and picture's ImageInfo */
        virtual void updateImageInfo(int aRotation);
    public:
        ImageRotHandler(STECamera *aSTECamera);
        virtual ~ImageRotHandler();

    protected:
        /* Turns on/off the Auto Rotation Control for ISPPROC and ARMIV*/
        OMX_ERRORTYPE doConfigAutoRotate(OMX_BOOL aEnaleAutoRotate);

        /**
         * Search the CaptureParammeters and ExifParameters
         * call doUpdateExtraDataOrientation to update the orientation
         */
        OMX_ERRORTYPE updateExtraDataOrientation(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr);

        /* update exif orientation*/
        void updateExifParamsOrientation(char *aBuffer, int aOrientation);

        virtual void doUpdateExtraDataOrientation(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *aCaptureParameters, char *aExifParameters);

    private:
        OMX_ERRORTYPE setAutoRotate(OMX_HANDLETYPE aHandle, int aPort, OMX_BOOL aEnaleAutoRotate);

    protected:
        STECamera *mSTECamera; /**< Camera */
};

class DeviceRotation : public ImageRotHandler {

    public:
        /* Set the AutoRotationControl = ON for on Device Rotation*/
        OMX_ERRORTYPE configAutoRotate();

        void updateImageInfo(int aRotation);
        /* do configuration specific to on device rotation, specially in setparameter call*/
        OMX_ERRORTYPE doConfiguration();
    public:
        DeviceRotation(STECamera *aSTECamera);

};

class ExifRotation : public ImageRotHandler {

    public:
        /**
         * Do the configuration required for capturing image at any orientation
         * setupRotationProcessing()
         */
        OMX_ERRORTYPE doConfiguration();
        /**
         * Update ExifParams Orientation tag with value of CaptureParameters scene orientation
         * in case of 180 degree orientation
         */
        OMX_ERRORTYPE handleBuffer(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr);

        /* Setup Hardware or software processing - set rotation in CaptureParameters*/
        OMX_ERRORTYPE setupRotationProcessing();

        void updateImageInfo(int aRotation);
    public:
        ExifRotation(STECamera *aSTECamera);

    protected:
        void doUpdateExtraDataOrientation(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *aCaptureParameters, char *aExifParameters);
};

class DeviceHwRotation : public DeviceRotation {

    public:
        /* Setup SIA_HW processing i.e. eOrientation.*/
        OMX_ERRORTYPE setupRotationProcessing();

    public:
        DeviceHwRotation(STECamera *aSTECamera);
};

class DeviceSwRotation : public DeviceRotation {

    public:
        /* Setup sw processing routine.*/
        OMX_ERRORTYPE setupRotationProcessing();

        /**
         * Do sw processing(rotation) orientation.
         * Set ExifParams Orientation tag and CaptureParameters orientation
         * to normal orientation(0 degree).
         */
        OMX_ERRORTYPE handleBuffer(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr);
    public:
        DeviceSwRotation(STECamera *aSTECamera);

    protected:
        void doUpdateExtraDataOrientation(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *aCaptureParameters, char *aExifParameters);
};
}; //namespace android
#endif //STECAMIMAGEROTHANDLER_H