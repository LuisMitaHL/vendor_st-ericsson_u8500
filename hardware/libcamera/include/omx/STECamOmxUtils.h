/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMOMXUTILS_H
#define STECAMOMXUTILS_H

//System Include
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <semaphore.h>

//Multimedia includes
#include <ste_omxil_interface.h>

//internal include
#include "STECamTrace.h"
#include <OMX_Symbian_CameraExt_Ste.h>

namespace android {

class OmxUtils
    {
    public:
        enum
        {
            kVersionMajor = OMX_VERSION_MAJOR, /**< Version Major */
            kVersionMinor = OMX_VERSION_MINOR, /**< Version Minsr */
            kVersionRevision = OMX_VERSION_REVISION, /**< Version Revision */
            kVersionStep = OMX_VERSION_STEP, /**< Version Step */
            kVersion = ((kVersionStep<<24) | (kVersionRevision<<16) | (kVersionMinor<<8) | kVersionMajor ), /**< Complete version */
#ifdef CAM_DEBUG
            kFillStructDebugChar = 0xde
#else //!CAM_DEBUG
            kFillStructDebugChar = 0x00
#endif //CAM_DEBUG
        };

    public:
        /* Wrapper for OMX structs */
        template <class TStruct>
        class StructWrapper
            {
                public:
                   /* Memset all members and init size/version */
                   inline static void init(TStruct& aStruct, OMX_BOOL aMemset = OMX_TRUE, OMX_BOOL aInitWithZero = OMX_FALSE);
            };

        /* Container for OMX structs */
        template <class TStruct>
        class StructContainer
            {
                public:
                   /* CTOR */
                   inline StructContainer(OMX_BOOL aInitWithZero = OMX_FALSE);

                   /* ReInit */
                   inline void reInit(OMX_BOOL aInitWithZero = OMX_FALSE);

                   /* Reference */
                   inline TStruct& ref();

                   /* Pointer */
                   inline TStruct* ptr();

                 private:
                    TStruct mStruct;
            };

        /* To keep a track of Initialised OMX Components */
        enum OMXComponents {
            EOMXCamera = 0,
            EOMXISPProc,
            EOMXJPEGEnc,
            EOMXExifMixer,
            EOMXComponentsMax
        };

    public:
        /* Constructor */
        OmxUtils();

        /* Destructor */
        ~OmxUtils();

        /* Initialize */
        OMX_ERRORTYPE init();

    public:
        /* Get OMX core interface */
        inline OMXCoreInterface* interface();

    public:
        /**< To get error  name */
        static const char* name(OMX_ERRORTYPE aParam);

        /**< To get command  name */
        static const char* name(OMX_COMMANDTYPE aParam);

        /**< To get state  name */
        static const char* name(OMX_STATETYPE aParam);

        /**< To get event  name */
        static const char* name(OMX_EVENTTYPE aParam);

        /**< To get buffer supplier type  name */
        static const char* name(OMX_BUFFERSUPPLIERTYPE aParam);

        /**< To get dir type  name */
        static const char* name(OMX_DIRTYPE aParam);

        /**< To get  port domain type  name */
        static const char* name(OMX_PORTDOMAINTYPE aParam);

        /**< To get video coding type name */
        static const char* name(OMX_VIDEO_CODINGTYPE aParam);

        /**< To get image coding type name */
        static const char* name(OMX_IMAGE_CODINGTYPE aParam);

        /**< To get color format type name */
        static const char* name(OMX_COLOR_FORMATTYPE aParam);

        /**< To get Focus Range name */
        static const char* name(OMX_SYMBIAN_FOCUSRANGETYPE aParam);

        /**< To get Lock Type name */
        static const char* name(OMX_SYMBIAN_LOCKTYPE aParam);

        /**< To get Focus Status name */
        static const char* name(OMX_FOCUSSTATUSTYPE aParam);

        /**< To get OMX Component name */
        static const char* name(OMX_HANDLETYPE hComponent);

        /**< To get Focus Region name */
        static const char* name(OMX_SYMBIAN_FOCUSREGIONCONTROL aParam);

        /**< To get Colour Effect name */
        static const char* name(OMX_IMAGEFILTERTYPE aParam);

        /**< To get White balance type name */
        static const char* name(OMX_WHITEBALCONTROLTYPE aParam);

        /**< To get flash mode name */
        static const char* name(OMX_IMAGE_FLASHCONTROLTYPE aParam);

        /**< To get flicker mode name */
        static const char* name(OMX_SYMBIAN_FLICKERREMOVALTYPE aParam);

        /**< To get Metering mode name */
        static const char* name(OMX_METERINGTYPE aParam);

        /**< To set OMX Component name */
        static void setOMXComponentName(OMX_HANDLETYPE hComponent, char* aName, OMXComponents aOMXComponent);

        /**< To init OMX_PARAM_PORTDEFINITIONTYPE */
        static void initialize(OMX_PARAM_PORTDEFINITIONTYPE& aParam, OMX_PORTDOMAINTYPE aDomain, OMX_U32 aIndex);

        /**< To dump OMX_PARAM_PORTDEFINITIONTYPE */
        static void dump(OMX_PARAM_PORTDEFINITIONTYPE& aParam);

        /**< To dump OMX_AUDIO_PORTDEFINITIONTYPE */
        static void dump(OMX_AUDIO_PORTDEFINITIONTYPE& aParam);

        /**< To dump OMX_VIDEO_PORTDEFINITIONTYPE */
        static void dump(OMX_VIDEO_PORTDEFINITIONTYPE& aParam);

        /**< To dump OMX_IMAGE_PORTDEFINITIONTYPE */
        static void dump(OMX_IMAGE_PORTDEFINITIONTYPE& aParam);

        /**< To dump OMX_OTHER_PORTDEFINITIONTYPE */
        static void dump(OMX_OTHER_PORTDEFINITIONTYPE& aParam);

        /**< Get efective bits per pixel for given format */
        static int bpp(OMX_COLOR_FORMATTYPE aOmxColorFmt);

        /**< To return application status corresponding to OMX errors*/
        static status_t convertOmxErrorToStatus(OMX_ERRORTYPE aOmxErr);
        /**<  Flush Port safely after checking if its enabled or not
              This is not supported for OMX_ALL Port config*/

        static OMX_ERRORTYPE flushPort(OMX_HANDLETYPE hComponent, OMX_U32 port,
               void *pContext,sem_t* semaphore);
        /**<  Enable/Disable port safely after checking if its enabled or disabled
              This is not supported for OMX_ALL Port config*/

        static OMX_ERRORTYPE setPortState(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE cmd,
               OMX_U32 port, void *pContext, sem_t* semaphore );
        /**<free the port buffer after checking if its enabled or not
            This is not supported for OMX_ALL Port config*/
        static  OMX_ERRORTYPE freePortBuffer(OMX_HANDLETYPE hComponent, OMX_U32 portIndex,
                OMX_BUFFERHEADERTYPE *pBuffer);

    private:
        void                *mDlHandle; /**< DL handle */
        OMXCoreInterface    *mInterface; /**< OMXcore interface */
    };

#include "STECamOmxUtils.inl"

}

#endif
