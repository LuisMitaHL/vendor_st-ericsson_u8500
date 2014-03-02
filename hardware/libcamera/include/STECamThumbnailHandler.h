/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * STECamFocusHandler.h: This class manages image rotation .
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * This class is responsible to handler the data flow from snapshot/preview image
 * till the generation of thumbnail
 *
 * EnableDataFlow: enable the generation of thumbnail from snapshot.
 * DisableDataFlow: disable the generation of thumbnail
 *                                                      Main Image
 *                                                           |
 *                                                           |
 *                                                           |
 *  SnapShot     ------             ------   Thumbnail    ------   Exif Image
 * ___________ | ArmIv  |_________| SwJpeg |____________|  Exif  |___________
 *             |        |         |  Enc   |            |  Mixer |
 *  Preview      ------             ------                ------
 *              Scaling           Conversion
 *              Rotaion            to Jpeg
 *
 *                 -------------------------------------------
 *                             THUMBNAIL DATAFLOW
 *                 -------------------------------------------
 */
#ifndef STECAMTHUMBNAILHANDLER_H
#define STECAMTHUMBNAILHANDLER_H

//Internal Includes
#include "STECamSemaphoreContainer.h"

namespace android {

//forward class declarations
class STECamera;

class ThumbnailHandler {
    public:
        virtual OMX_ERRORTYPE init(const ImageInfo &aThumbnailInfo,
                           const ImageInfo &aPreviewInfo) = 0;

        virtual OMX_ERRORTYPE release() = 0;

        virtual OMX_ERRORTYPE sendComponentToExecuting() = 0;

        virtual OMX_ERRORTYPE resetComponents() = 0;

        virtual OMX_ERRORTYPE freeBuffers() = 0;

        /*
         * Do configuration of thumbnail pipe.
         * All the thumbnail pipe ports should be disabled before calling
         */
        virtual OMX_ERRORTYPE configure(const ImageInfo &aThumbnailInfo,
                                        const ImageInfo &aPreviewInfo) = 0;

        /*
         * Method to request buffer to be processed by the thumbnail handler.
         */
        virtual OMX_ERRORTYPE handleBuffer(OMX_U32 aFilledLen, OMX_U32 aFlags) = 0;

        /*
         * Method could be used for receiving the processing event such
         * as EOS and do further processing.
         */
        virtual void handleEvent() = 0;

        /*
         * Method to configure thumbnail pipe input port parameters
         */
        virtual OMX_ERRORTYPE updateSnapShot(const ImageInfo &aPreviewInfo) = 0;
    public:
        /* DTOR */
        virtual ~ThumbnailHandler() = 0;
};

class EnableDataFlow : public ThumbnailHandler {

    public:
        /* Do the configuration and enable the thumbnail data path
         * The following components are part of thumbnail pipe:
         *    1) ArmIv(scaling and rotation)
         *    2) SwJpegEnc(conversion to jpeg)
         *    3) ExifMixerExif(embed thumbnail in exif header)
         */
        OMX_ERRORTYPE configure(const ImageInfo &aThumbnailInfo,
                                const ImageInfo &aPreviewInfo);

        /*
         * Disable ports and free resources.
         * Should be called before calling configure
         */
        OMX_ERRORTYPE release();

        /*
         * Get the input buffer header information.
         * Send the buffer to the ArmIv input port.
         */
        OMX_ERRORTYPE handleBuffer(OMX_U32 aFilledLen, OMX_U32 aFlags);

        /*
         * Wait for the Arm-Iv and SwJpeg encoder EOS.
         */
        void handleEvent();

        /*
         * Update the ArmIv input port parameters.
         * Should be called when the preview/snapshot parameters chanaged
         */
        OMX_ERRORTYPE updateSnapShot(const ImageInfo &aPreviewInfo);

        /*
         * Method to Send the ArmIv and SwJpegEnc from unloaded
         * to excecuting state with all the ports disabled.
         */
        OMX_ERRORTYPE init(const ImageInfo &aThumbnailInfo,
                           const ImageInfo &aPreviewInfo);

        /*
         * Send ArmIv and SwJpegEnc from Loaded to Executing state
         */
        OMX_ERRORTYPE sendComponentToExecuting();

        /*
         * Method to Send the ArmIv and SwJpegEnc from executing
         * to loaded state.
         * Should be called during cleanup.
         */
        OMX_ERRORTYPE resetComponents();

        /*
         * Free ArmIv input buffer.
         */
        OMX_ERRORTYPE freeBuffers();

        /*
         * Send ArmIv and SwJpegEnc to unloaded state.
         */
        OMX_ERRORTYPE freeHandles();

        /*
         * Configure AutoRotate:On for ArmIv.
         */
        OMX_ERRORTYPE setAutoRotate(OMX_BOOL aEnaleAutoRotate = OMX_FALSE);

    private:
        friend OMX_ERRORTYPE ArmIvEventHandler(OMX_HANDLETYPE hComponent,
                                               OMX_PTR pAppData,
                                               OMX_EVENTTYPE eEvent,
                                               OMX_U32 nData1,
                                               OMX_U32 nData2,
                                               OMX_PTR pEventData);
        friend OMX_ERRORTYPE SWJpegEventHandler(OMX_HANDLETYPE hComponent,
                                                OMX_PTR pAppData,
                                                OMX_EVENTTYPE eEvent,
                                                OMX_U32 nData1,
                                                OMX_U32 nData2,
                                                OMX_PTR pEventData);
    private:
        /*
         * Configure port parameters.
         */
        OMX_ERRORTYPE configPorts(const ImageInfo &aThumbnailInfo,
                                  const ImageInfo &aPreviewInfo);

        /*
         * Setup tunneling between:
         *     ArmIv(output)-SwJpegEnc(Input)
         *     SwJpegEnc(output)-ExifMixer(input)
         */
        OMX_ERRORTYPE setupTunneling();

        /*
         * Disble all the ports
         * Free Buffer aloocated for ArmIv input
         */
        OMX_ERRORTYPE disablePorts();

        /*
         * Enable all the Ports
         * Call usebuffer for ArmIv input
         */
        OMX_ERRORTYPE enablePorts();

        /*
         * Send input Buffer parameters to ArmIv component
         */
        OMX_ERRORTYPE useBuffer();

    public:
        /* CTOR */
        EnableDataFlow(OMX_HANDLETYPE &aExifMixer,
                       OmxBuffInfo &aArmIvOmxBuffInfo,
                       OMX_OSI_CONFIG_SHARED_CHUNK_METADATA &aChunkData_ArmIvProc,
                       OmxUtils &aOmxUtils,
                       const OMX_COLOR_FORMATTYPE &aPreviewOmxColorFormat,
                       PerfManager &aPerfManager,
                       sem_t &aStateExifSem);
        /* DTOR */
        ~EnableDataFlow();

    private:
        OMX_CALLBACKTYPE swjpegcallback ,
                         armivcallback; /**< ArmIv, SwJpegEnc Callbacks*/
        OMX_HANDLETYPE mArmIvProc; /**< ArmIv OMX Handle*/
        OMX_HANDLETYPE mSwJpegEnc; /**< SwJpegEnc OMX Handle*/
        OMX_HANDLETYPE &mExifMixer; /**< ExifMixer OMX Handle*/
        OMX_BUFFERHEADERTYPE *mArmIvInBuffer; /**< ArmIv OMX Buffer Header*/
        OmxBuffInfo &mArmIvOmxBuffInfo; /**< ArmIv Wrapper for Buffer Handling*/

        /**< Port Parameteters Structures*/
        OMX_PARAM_PORTDEFINITIONTYPE mParamArmIvInput, mParamArmIvOutput,
                                     mParamSwJpegInput, mParamSwJpegOutput,
                                     mParamExifInput;

        /**< ArmIv memory chunck*/
        OMX_OSI_CONFIG_SHARED_CHUNK_METADATA &mChunkData_ArmIvProc;
        OmxUtils &mOmxUtils;  /**< OMX utils*/
        const OMX_COLOR_FORMATTYPE &mPreviewOmxColorFormat; /**< ArmIv Color Format*/
        PerfManager &mPerfManager;  /**< PerfManager*/

        sem_t &mStateExifSem; /**< ExifMixer state Semaphore*/
        SemaphoreContainer mStateArmIvSem; /**< ArmIv state Semaphore*/
        SemaphoreContainer mStateSWJpegSem; /**< SwJpeg state Semaphore*/
        SemaphoreContainer mArmIvEosSem; /**< ArmIv EOS Semaphore*/
        SemaphoreContainer mSwJpegEosSem; /**< SwJpeg EOS Semaphore*/
        bool mPortsEnabled;
};

class DisableDataFlow : public ThumbnailHandler {

    public:
        OMX_ERRORTYPE configure(const ImageInfo &aThumbnailInfo,
                                const ImageInfo &aPreviewInfo);

        OMX_ERRORTYPE release();

        OMX_ERRORTYPE handleBuffer(OMX_U32 aFilledLen, OMX_U32 aFlags);

        void handleEvent();

        OMX_ERRORTYPE updateSnapShot(const ImageInfo &aPreviewInfo);

        /*
        */
        OMX_ERRORTYPE init(const ImageInfo &aThumbnailInfo,
            const ImageInfo &aPreviewInfo);
        /*
         */
        OMX_ERRORTYPE resetComponents();
        OMX_ERRORTYPE sendComponentToExecuting();
        OMX_ERRORTYPE freeBuffers();
    public:
        /* DTOR */
        ~DisableDataFlow();
};
} //namespace android
#endif //STECAMTHUMBNAILHANDLER_H