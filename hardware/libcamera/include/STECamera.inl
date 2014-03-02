/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMERA_INL
#define STECAMERA_INL

inline OMX_BOOL STECamera::getCamSensorMode()
{
    DBGT_PROLOG("");

    OMX_BOOL sensor_mode = OMX_FALSE; //SFRC

    if (0 == mCamMode) {
        sensor_mode = OMX_TRUE; //ASRC
    }

	DBGT_EPILOG("sensor_mode - %d", sensor_mode);
	return sensor_mode;
}

inline const CameraFormatInfo& STECamera::cameraFormatInfo() const
{
    //check index valid
    DBGT_ASSERT(((mCameraFormatInfoIndex >= 0) &&
                 mCameraFormatInfoIndex < ZARRAYCOUNT(g_CameraFormatInfo)),
               "mCameraFormatInfoIndex: %d is invalid", mCameraFormatInfoIndex);

    return g_CameraFormatInfo[mCameraFormatInfoIndex];
}

inline void STECamera::enableCallbacks(OmxBuffInfo* aOmxBuffInfo, uint32_t aCount)
{
    DBGT_ASSERT(0 != aCount, "Count is zero");

    for(uint32_t loop = 0; loop < aCount; loop++)
        aOmxBuffInfo[loop].mFlags |= OmxBuffInfo::EEnableClientCallback;
}

inline void STECamera::disableCallbacks(OmxBuffInfo* aOmxBuffInfo, uint32_t aCount)
{
    DBGT_ASSERT(0 != aCount, "Count is zero");

    for(uint32_t loop = 0; loop < aCount; loop++)
        aOmxBuffInfo[loop].mFlags &= ~OmxBuffInfo::EEnableClientCallback;
}

inline OMX_ERRORTYPE STECamera::updateCapturing(OMX_U32 aPortIndex, OMX_BOOL aEnable)
{
    DBGT_PROLOG("PortIndex: %ld Enable: %d", aPortIndex, aEnable);

    DBGT_ASSERT(NULL != mCam, "Camera handle NULL");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    //get current value
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_BOOLEANTYPE> extCapturing;
    extCapturing.ptr()->nPortIndex = aPortIndex;
    err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ECapturing), extCapturing.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_GetConfig OmxILExtIndex::ECapturing failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    if(extCapturing.ptr()->bEnabled != aEnable) {
        //set new value
        DBGT_PTRACE("setting new value for ECapturing");
        extCapturing.ptr()->nPortIndex = aPortIndex;
        extCapturing.ptr()->bEnabled = aEnable;
        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ECapturing), extCapturing.ptr());
		if(OMX_ErrorNone != err) {
			DBGT_CRITICAL("OMX_SetConfig OmxILExtIndex::ECapturing failed err = %d", err);
			DBGT_EPILOG("");
			return err;
		}
    }
	DBGT_EPILOG("");
	return err;
}

inline OMX_ERRORTYPE STECamera::updateAutoPause(OMX_BOOL aEnable)
{
    DBGT_PROLOG("Enable: %d", aEnable);

    DBGT_ASSERT(NULL != mCam, "Camera handle NULL");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    //get current value
    OmxUtils::StructContainer<OMX_CONFIG_BOOLEANTYPE> autoPause;
    err = OMX_GetConfig(mCam, OMX_IndexAutoPauseAfterCapture, autoPause.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_GetConfig OMX_IndexAutoPauseAfterCapture failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    //set new value
    autoPause.ptr()->bEnabled = aEnable;
    err = OMX_SetConfig(mCam, OMX_IndexAutoPauseAfterCapture, autoPause.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_SetConfig OMX_IndexAutoPauseAfterCapture failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

	DBGT_EPILOG("");
	return err;
}

inline OMX_ERRORTYPE STECamera::updateSmoothZoom(OMX_BOOL aEnable)
{
    DBGT_PROLOG("Enable: %d", aEnable);

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_CONFIG_BOOLEANTYPE> zoomMode;

#if 0 //added by dyron
    err = OMX_GetConfig(mCam,mOmxILExtIndex->getIndex(OmxILExtIndex::EZoomMode),zoomMode.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}
    zoomMode.ptr()->bEnabled = aEnable;

    err = OMX_SetConfig(mCam,mOmxILExtIndex->getIndex(OmxILExtIndex::EZoomMode),zoomMode.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

#endif
	DBGT_EPILOG("");
	return err;
}

inline OMX_ERRORTYPE STECamera:: synchCBData(MMHwBuffer::TSyncCacheOperation aOp,
                                             MMHwBuffer& aMMHwBuffer,
                                             OMX_U8* aBuffer,
                                             OMX_U32 aAllocLen)
{
    DBGT_PROLOG("Op: %d Buffer: %p AllocLen: %lu", aOp, aBuffer, aAllocLen);

    DBGT_ASSERT(NULL != aBuffer, "Buffer is NULL");
    DBGT_ASSERT(0 != aAllocLen, "AllocLen is 0");

    OMX_U32 phyAddr;
    OMX_ERRORTYPE err = aMMHwBuffer.CacheSync(aOp, (OMX_U32)aBuffer, aAllocLen, phyAddr);

	DBGT_EPILOG("");
	return err;

}

inline void STECamera::dispatchError(int32_t aInfo1, int32_t aInfo2 /*= 0*/)
{
    DBGT_PROLOG("Info1: %d Info2: %d", aInfo1, aInfo2);

    dispatchNotification(CAMERA_MSG_ERROR, aInfo1, aInfo2);

    DBGT_EPILOG("");
    return;
}

// The output resolution of Jpeg encoder is same as input. The input of Jpeg Encoder is 16-bit aligned
// both in case of JpegEnc and SwJpegEnc. So if the desired output resolution is a non-multiple of 16,
// the output of jpeg encoder is to be cropped .

inline OMX_ERRORTYPE STECamera::cropOutputSize(OMX_HANDLETYPE aHandle, const int aWidth, const int aHeight )
{
    DBGT_PROLOG("Handle: %p Width: %d Height %d", aHandle, aWidth, aHeight) ;

    DBGT_ASSERT(NULL != aHandle, "Handle is NULL" );
    DBGT_ASSERT((0 != aWidth) && (0 != aHeight), "Invalid output resolution");

    OmxUtils::StructContainer<OMX_CONFIG_RECTTYPE> qrect;
    qrect.ptr()->nPortIndex = 1;

    OMX_ERRORTYPE err = OMX_GetConfig(aHandle, OMX_IndexConfigCommonOutputCrop, qrect.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    qrect.ptr()->nLeft = 0;
    qrect.ptr()->nTop = 0;
    qrect.ptr()->nWidth = aWidth;
    qrect.ptr()->nHeight = aHeight;

    err = OMX_SetConfig(aHandle, OMX_IndexConfigCommonOutputCrop, qrect.ptr());
	if(OMX_ErrorNone != err) {
		DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

	DBGT_EPILOG("");
	return err;
}

inline uint32_t STECamera::previewCallbackSize(
        uint32_t aTotalSize,
        uint32_t aOldFrameSize,
        uint32_t aNewFrameSize) const
{
    DBGT_PROLOG("ActualSize: %u OldFrameSize: %d NewFrameSize : %d",
            aTotalSize, aOldFrameSize, aNewFrameSize);

    DBGT_ASSERT(0 != aTotalSize, "TotalSize is 0");
    DBGT_ASSERT(0 != aOldFrameSize, "OldFrameSize is 0");
    DBGT_ASSERT(0 != aNewFrameSize, "NewFrameSize is 0");

    uint32_t size = aNewFrameSize;

#ifdef SEND_PREVIEW_EXTRADATA
    if (mVideoStabEnabled) {
        //Newframe + extradata
        size = aNewFrameSize + (aTotalSize - aOldFrameSize);
    }
#endif//SEND_PREVIEW_EXTRADATA

	DBGT_EPILOG("size - %d", size);
	return size;
}

inline FocusHandler* STECamera::focusHandler()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mFocusHandler, "mFocusHandler is NULL");

	DBGT_EPILOG("mFocusHandler - %p", mFocusHandler);
	return mFocusHandler;
}

inline OMX_ERRORTYPE STECamera::pushPreviewBuffer(const OmxBuffInfo& aOmxBuffInfo)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    PreviewThreadData *data = static_cast<PreviewThreadData*>(aOmxBuffInfo.userData());
    DBGT_ASSERT(NULL != data, "PreviewThreadData is NULL");

    if((aOmxBuffInfo.mCamMemoryBase.get()!=NULL)&&
        (aOmxBuffInfo.mCamMemoryBase.get()->omxBufferHeaderType()!=NULL) ){
        // Push the buffer back to the camera if preview running
        err = OmxBuffInfo::fillBuffer(mCam, aOmxBuffInfo.mCamMemoryBase.get()->omxBufferHeaderType());
    }

	DBGT_EPILOG("");
	return err;
}

inline STECamera::PreviewThreadData::PreviewThreadData() : mSwProcessingThreadData(NULL)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

inline ThumbnailHandler* STECamera::getThumbnailHandler()
{
    DBGT_ASSERT(NULL != mThumbnailHandler, "invalid Thumbnail Handler");
    return mThumbnailHandler;
}

inline ImageRotHandler* STECamera::getRotationHandler()
{
    DBGT_ASSERT(NULL != mImageRotHandler, "invalid Rotation Handler");
    return mImageRotHandler;
}

inline void STECamera::selectThumbnailHandler(int aThumbnailWidth, int aThumbnailHeight)
{
    DBGT_PROLOG("aThumbnailWidth %d, aThumbnailHeight %d", aThumbnailWidth, aThumbnailHeight);

    if (0 == aThumbnailWidth || 0 == aThumbnailHeight)
        mThumbnailHandler = mDisableDataFlow;
    else
        mThumbnailHandler = mEnableDataFlow;

    DBGT_EPILOG("");
}

inline bool STECamera::checkPreviewSizeValid(const char* aPreviewSize)
{
    DBGT_PROLOG("Preview Size: %s", aPreviewSize);

    bool valid = true;

    if( (NULL != aPreviewSize) && //Preview is size present
        (NULL == strstr(CAM_SUPPORTED_PREVIEW_SIZES, aPreviewSize)) && //Invalid preview size
        (NULL == strstr(CAM_EXTENDED_PREVIEW_SIZES, aPreviewSize))) //Invalid extented preview size
        valid = false;

	DBGT_EPILOG("valid - %d", valid);
	return valid;
}

#ifdef ENABLE_FACE_DETECTION
inline bool STECamera::isFaceDetectionEnable(void)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("mFaceDetection - %d", (int)mFaceDetection);
    return mFaceDetection;
}
#endif  //ENABLE_FACE_DETECTION

#endif // STECAMERA_INL
