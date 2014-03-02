/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * SteExifMng.h : Exif Manager
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STEEXIFMNG_H
#define STEEXIFMNG_H

#include <camera/CameraParameters.h>

#ifndef ENABLE_EXIF_MIXER
/* ****************************JHEAD****************************** */
#define FILE_SYSTEM_INTERFACE

namespace android {

        class SteExifMng {
        public:
                SteExifMng();
                ~SteExifMng();
                int UpdateExifField(
                    const CameraParameters &params,
                    char* inBuffer,
                    int inBufSize,
                    char* outBuffer,
                    int outBufSize,
                    char* inThumbBuffer,
                    int inThumbBufSize,
                    int cameraId);

        private:

        };

}; // namespace android

#else
/* **************************EXIF_MIXER**************************** */
namespace android {

        class SteExifMng {
        public:
                sem_t mStateEXIF_sem;
                sem_t mExif_sem;
                sem_t mExif_eos_sem;
                SteExifMng(
                                char* inThumbBuffer,
                                int inThumbSize,
                                int inThumbWidth,
                                int inThumbHeight,
                                char* inPictBuffer,
                                int inPictSize,
                                int inPictWidth,
                                int inPictHeight,
                                const CameraParameters &aParams,
                                char* &outBuffer,
                                int* size);

                ~SteExifMng();

                OMX_BOOL convertToDegrees(
                                OMX_U64 &aDegrees,
                                OMX_U64 &aMinutes,
                                OMX_U64 &aSeconds,
                                char const *const aGPSKey);

                void convertUTCToTime(OMX_U64& aHours,
                                OMX_U64& aMinutes,
                                OMX_U64& aSeconds,
                                char const* const aGPSKey );
                float extractSubUnitLocation(float& aWholeValue);
                OMX_U64 extractSubUnitTime(OMX_U64 &tmpTime);

                OMX_ERRORTYPE setGPSTags(
                                const CameraParameters &aParams,
                                OMX_HANDLETYPE aHandle, int aPort);

        private:
                OmxUtils mOmxUtils;
                int mInThumbWidth;
                int mInThumbHeight;
                int mInThumbBufSize;
                int mInPictWidth;
                int mInPictHeight;
                int mInPictBufSize;
                int mOutputBufSize;
                OMX_HANDLETYPE  mExifMixer;
                OMX_CALLBACKTYPE mExifCallback;
                OMX_PARAM_PORTDEFINITIONTYPE paramEXIFinput1;
                OMX_PARAM_PORTDEFINITIONTYPE paramEXIFinput2;
                OMX_PARAM_PORTDEFINITIONTYPE paramEXIFoutput;
                OMX_BUFFERHEADERTYPE *pExifInThumbBuffer;
                OMX_BUFFERHEADERTYPE *pExifInPictBuffer;
                OMX_BUFFERHEADERTYPE *pExifOutBuffer;

        };

}; // namespace android

#endif //#ifndef ENABLE_EXIF_MIXER
#endif
