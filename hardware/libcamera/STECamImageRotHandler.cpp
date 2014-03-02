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
#define DBGT_PREFIX "ImgRot"

#include "STECamTrace.h"
#include "STECamera.h"
#include "STECamOmxUtils.h"
#include "STECamImageRotHandler.h"
#include "STECamThumbnailHandler.h"

#define IFDtag_Orientation 274

#define ENDIANCONVERT16(a) ((a >> 8) | ((a & 0xff) << 8))
#define ENDIANCONVERT32(a) ((a >> 24) | ((a & 0x00FF0000) >> 8) | ((a & 0xFF00) << 8) | ((a & 0xff) << 24))

#define EXIF_SIZE_HEADER_SIZE 8
#define EXIF_APP_HEADER_SIZE 10
#define EXIF_TIFF_HEADER_SIZE 8

namespace android
{

class ThumbnailHandler;
class EnableDataFlow;

extern OMX_PARAM_PORTDEFINITIONTYPE paramPortVPB1;
extern OMX_PARAM_PORTDEFINITIONTYPE paramISPoutput;
extern OMX_PARAM_PORTDEFINITIONTYPE paramArmIVoutput;

typedef struct {
   OMX_U16 tag;
   OMX_U16 type;
   OMX_U32 count;
   OMX_U32 valueOffset;
} IFDfield_t;

#undef _CNAME_
#define _CNAME_ ImageRotHandler

ImageRotHandler::ImageRotHandler (STECamera *aSTECamera) : mSTECamera(aSTECamera)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

ImageRotHandler::~ImageRotHandler()
{
    DBGT_PROLOG("");

    mSTECamera = NULL;

    DBGT_EPILOG("");
};

OMX_ERRORTYPE ImageRotHandler::configAutoRotate()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ImageRotHandler::doConfiguration()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ImageRotHandler::handleBuffer(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr)
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ImageRotHandler::setupRotationProcessing()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ImageRotHandler::doConfigAutoRotate(OMX_BOOL aEnaleAutoRotate)
{
    DBGT_PROLOG("aEnaleAutoRotate: %d", aEnaleAutoRotate);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = setAutoRotate(mSTECamera->mISPProc, paramISPoutput.nPortIndex, aEnaleAutoRotate);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("SetAutoRotate failed for mISPProc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = mSTECamera->mEnableDataFlow->setAutoRotate(aEnaleAutoRotate);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("SetAutoRotate failed for mISPProc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE ImageRotHandler::updateExtraDataOrientation(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr)
{

    DBGT_PROLOG("aOmxBuffHdr: %p", aOmxBuffHdr);

    DBGT_ASSERT(NULL != aOmxBuffHdr, "aOmxBuffHdr is null");
    DBGT_ASSERT(NULL != aOmxBuffHdr->pBuffer, "aOmxBuffHdr->pBuffer is null");
    DBGT_ASSERT(0 != aOmxBuffHdr->nFilledLen, "aOmxBuffHdr->nFilledLen is 0");

    OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *pCaptureParameters = NULL;
    char *pExifParameters = NULL;

    /* get extradata start pointer */
    OMX_U8* pCur = aOmxBuffHdr->pBuffer + aOmxBuffHdr->nOffset + aOmxBuffHdr->nFilledLen;
    OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pCur + 3) & ~0x3);

    /* check that there is enough space for an extradata header*/
    if(((OMX_U8*)aOmxBuffHdr->pBuffer + aOmxBuffHdr->nAllocLen) <=
      ((OMX_U8*)pCurExtra + sizeof(OMX_OTHER_EXTRADATATYPE))) {
        DBGT_CRITICAL("not enough space for xtradata header OMX_ErrorUndefined");
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }

    /* search if field already exists */
    while (OMX_ExtraDataNone != pCurExtra->eType) {
        /* if we have found our capture parameters field*/
        if (OMX_SYMBIAN_CameraExtraDataCaptureParameters == (OMX_SYMBIAN_CAMERA_EXTRADATATYPE)pCurExtra->eType) {
            /* check that there is enough space for data*/
            if(((OMX_U8*)aOmxBuffHdr->pBuffer + aOmxBuffHdr->nAllocLen) <=
                (((OMX_U8*)pCurExtra + sizeof(OMX_OTHER_EXTRADATATYPE))
                + (sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE) - sizeof(OMX_U8)))) {
                DBGT_CRITICAL("not enough space for data OMX_ErrorUndefined");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
            }
            pCaptureParameters = (OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *)pCurExtra->data;
        /* if we have found our exif parameters field*/
        } else if (OMX_SYMBIAN_CameraExtraDataEXIFAppMarker == (OMX_SYMBIAN_CAMERA_EXTRADATATYPE)pCurExtra->eType) {
            /* check that there is enough space for data*/
            if(((OMX_U8*)aOmxBuffHdr->pBuffer + aOmxBuffHdr->nAllocLen) <=
                ((OMX_U8*)pCurExtra + sizeof(OMX_OTHER_EXTRADATATYPE) + pCurExtra->nDataSize - sizeof(OMX_U8))) {
                DBGT_CRITICAL("not enough space for data OMX_ErrorUndefined");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
            }
            pExifParameters = (char *)pCurExtra->data;
            break;
        }

        /* check that there is enough space for an other extradata header */
        if(((OMX_U8*)aOmxBuffHdr->pBuffer + aOmxBuffHdr->nAllocLen) <=
                     ((OMX_U8*)pCurExtra + sizeof(OMX_OTHER_EXTRADATATYPE) + pCurExtra->nSize)) {
            DBGT_CRITICAL("not enough space for xtradata header OMX_ErrorUndefined");
            DBGT_EPILOG("");
            return OMX_ErrorUndefined;
        }
        pCurExtra = (OMX_OTHER_EXTRADATATYPE*)((OMX_U8*)pCurExtra + pCurExtra->nSize);
    }
    DBGT_ASSERT(NULL != pCaptureParameters, "pCaptureParameters is null");
    DBGT_ASSERT(NULL != pExifParameters, "pExifParameters is null");

    doUpdateExtraDataOrientation(pCaptureParameters, pExifParameters);

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

void ImageRotHandler::updateExifParamsOrientation(char *aBuffer, int aOrientation)
{
    DBGT_PROLOG("aBuffer: %p aOrientation: %d", aBuffer, aOrientation);

    DBGT_ASSERT(NULL != aBuffer, "aBuffer is NULL");

    /* CPU endianness retrieval */
    OMX_U16 val = 0xAABB;
    bool bigEndian = ((char*)&val)[0] == 0xAA;
    uint32_t exif_data_size = 0;
    uint32_t ifd0_off = 0;
    uint32_t ifd0_field_off = 0;
    uint16_t ifd0_items = 0;
    IFDfield_t ifd0_field;
    uint16_t temp = 0;

    /* get the ifd0 offset*/
    memcpy(&ifd0_off, &aBuffer[EXIF_SIZE_HEADER_SIZE + EXIF_APP_HEADER_SIZE + EXIF_TIFF_HEADER_SIZE - sizeof(OMX_U32)], sizeof(OMX_U32));
    if (!bigEndian)
        ENDIANCONVERT32(ifd0_off);
    ifd0_off += (EXIF_SIZE_HEADER_SIZE + EXIF_APP_HEADER_SIZE);

    /* get no of fields in the ifd0 directory*/
    memcpy(&ifd0_items, &aBuffer[ifd0_off], sizeof(OMX_U16));
    if (!bigEndian)
        ENDIANCONVERT16(ifd0_items);

    ifd0_field_off = ifd0_off + sizeof(OMX_U16);

    /*locate orientation tag*/
    while (ifd0_items--) {
        memcpy(&temp, &aBuffer[ifd0_field_off], sizeof(OMX_U16));
        if (!bigEndian)
            ENDIANCONVERT16(temp);
        if (IFDtag_Orientation == temp) {
            break;
        }
        ifd0_field_off += sizeof(IFDfield_t);
    }

    DBGT_ASSERT(IFDtag_Orientation == temp, "Could not found Orientation Tag");

    /*update orientation tag*/
    memcpy(&ifd0_field, &aBuffer[ifd0_field_off], sizeof(IFDfield_t));
    ifd0_field.valueOffset = aOrientation;
    memcpy(&aBuffer[ifd0_field_off], &ifd0_field, sizeof(IFDfield_t));

    DBGT_EPILOG("");
}

OMX_ERRORTYPE ImageRotHandler::setAutoRotate(OMX_HANDLETYPE aHandle, int aPort, OMX_BOOL aEnaleAutoRotate)
{
    DBGT_PROLOG("aHandle: %p aPort: %d aEnaleAutoRotate: %d", aHandle, aPort, aEnaleAutoRotate);

    DBGT_ASSERT(NULL != aHandle, "aHandle is NULL");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    OMX_INDEXTYPE index;

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_BOOLEANTYPE> autoRotate;

    err = OMX_GetExtensionIndex(aHandle, (char *)OMX_SYMBIAN_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL_NAME, &index);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Still Auto Rotation OMX_GetExtensionIndex failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    autoRotate.ptr()->nPortIndex = aPort;

    err = OMX_GetConfig(aHandle, index, autoRotate.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Auto Rotation OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    autoRotate.ptr()->bEnabled = aEnaleAutoRotate;

    err = OMX_SetConfig(aHandle, index, autoRotate.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Auto Rotation OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

void ImageRotHandler::doUpdateExtraDataOrientation(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *aCaptureParameters, char *aExifParameters)
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
}

void ImageRotHandler::updateImageInfo(int aRotation)
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
}

#undef _CNAME_
#define _CNAME_ DeviceRotation

DeviceRotation::DeviceRotation (STECamera *aSTECamera) : ImageRotHandler(aSTECamera)
{
    DBGT_PROLOG("");

    aSTECamera->mPictureInfo.enableSwapParams();
    aSTECamera->mThumbnailInfo.enableSwapParams();

    DBGT_EPILOG("");
}


OMX_ERRORTYPE DeviceRotation::configAutoRotate()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    // Configure ISPproc and ARMIV for auto-rotation:on
    err = doConfigAutoRotate(OMX_TRUE);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("doConfigAutoRotate failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE DeviceRotation::doConfiguration()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if(mSTECamera->mPictureInfo.swapParams()) {
        err = mSTECamera->getThumbnailHandler()->configure(mSTECamera->mThumbnailInfo,
                                                           mSTECamera->mPreviewInfo);
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("configure() failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        err = mSTECamera->ReconfigureStill(mSTECamera->mPictureInfo, false);
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("ReconfigureStill() failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }
    err = setupRotationProcessing();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("setupRotationProcessing() err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mSTECamera->mPictureInfo.resetSwapParams();
    mSTECamera->mThumbnailInfo.resetSwapParams();

    DBGT_EPILOG("");
    return err;
}

void DeviceRotation::updateImageInfo(int aRotation)
{
    DBGT_PROLOG("aRotation %d", aRotation);

    mSTECamera->mPictureInfo.setRotation(aRotation);
    mSTECamera->mThumbnailInfo.setRotation(aRotation);

    DBGT_EPILOG("");
}

#undef _CNAME_
#define _CNAME_ ExifRotation

ExifRotation::ExifRotation(STECamera *aSTECamera) : ImageRotHandler(aSTECamera)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_ERRORTYPE ExifRotation::doConfiguration()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = setupRotationProcessing();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("setupRotationProcessing() err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

void ExifRotation::updateImageInfo(int aRotation)
{
    DBGT_PROLOG("aRotation %d", aRotation);

    mSTECamera->mPictureInfo.setRotation(aRotation);
    mSTECamera->mThumbnailInfo.setRotation(aRotation);

    DBGT_EPILOG("");
}


OMX_ERRORTYPE ExifRotation::setupRotationProcessing()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mSTECamera->mCam, "Camera Handle is Null");

    int rotation = mSTECamera->mPictureInfo.getRotation();

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /*Setup Hardware Processing*/
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE> stillOrientationConfig;

    stillOrientationConfig.ptr()->nPortIndex = paramPortVPB1.nPortIndex;

    err = OMX_GetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EOrientationScene), stillOrientationConfig.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Orientation Scene OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (0 == rotation) {
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
    } else if (90 == rotation) {   /*< Frame is 90 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowRightColumnTop;
    } else if (180 == rotation) {   /*< Frame is 90 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowBottomColumnRight;
    } else if (270 == rotation) {  /*< Frame is 270 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowLeftColumnBottom;
    } else
        DBGT_CRITICAL("Cant Happen");

    err = OMX_SetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EOrientationScene), stillOrientationConfig.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE ExifRotation::handleBuffer(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr)
{
    DBGT_PROLOG("aOmxBuffHdr: %p", aOmxBuffHdr);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    int rotation = mSTECamera->mPictureInfo.getRotation();

    DBGT_PROLOG("rotation = %d", rotation);

    if (180 == rotation) {
        err = updateExtraDataOrientation(aOmxBuffHdr);
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("updateExtraDataOrientation failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }
    DBGT_EPILOG("");
    return err;
}

void ExifRotation::doUpdateExtraDataOrientation(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *aCaptureParameters, char *aExifParameters)
{
    DBGT_PROLOG("aCaptureParameters->eSceneOrientation: %d", aCaptureParameters->eSceneOrientation);

    DBGT_ASSERT(NULL != aCaptureParameters, "pCaptureParameters is null");
    DBGT_ASSERT(NULL != aExifParameters, "aExifParameters is null");

    updateExifParamsOrientation(aExifParameters, aCaptureParameters->eSceneOrientation);

    DBGT_EPILOG("");
}

#undef _CNAME_
#define _CNAME_ DeviceHwRotation

DeviceHwRotation::DeviceHwRotation(STECamera *aSTECamera) : DeviceRotation(aSTECamera)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_ERRORTYPE DeviceHwRotation::setupRotationProcessing()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mSTECamera->mCam, "Camera Handle is Null");

    int rotation = mSTECamera->mPictureInfo.getRotation();
    DBGT_ASSERT(180 != rotation, "Invalid rotation for Hw Processing");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /*Setup Hardware Processing*/
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE> stillOrientationConfig;

    stillOrientationConfig.ptr()->nPortIndex = paramPortVPB1.nPortIndex;

    err = OMX_GetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EOrientationScene), stillOrientationConfig.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Orientation Scene OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (0 == rotation) {
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
    } else if (90 == rotation) {   /*< Frame is 90 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowRightColumnTop;
    } else if (270 == rotation) {  /*< Frame is 270 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowLeftColumnBottom;
    } else
        DBGT_CRITICAL("Cant Happen");

    err = OMX_SetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EOrientationScene), stillOrientationConfig.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Orientation Scene OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    /*Setup Software Processing*/
    mSTECamera->mISPImageOmxBuffInfo.mSwRotation = NULL;

    DBGT_EPILOG("");
    return err;
}

#undef _CNAME_
#define _CNAME_ DeviceSwRotation

DeviceSwRotation::DeviceSwRotation(STECamera *aSTECamera) : DeviceRotation(aSTECamera)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_ERRORTYPE DeviceSwRotation::setupRotationProcessing()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mSTECamera->mCam, "Camera Handle is Null");

    int rotation = mSTECamera->mPictureInfo.getRotation();
    DBGT_ASSERT(rotation == 180, "Invalid rotation for Sw Processing");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /*Setup Hardware Processing*/
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE> stillOrientationConfig;

    stillOrientationConfig.ptr()->nPortIndex = paramPortVPB1.nPortIndex;

    err = OMX_GetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EOrientationScene), stillOrientationConfig.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Orientation Scene OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowBottomColumnRight;

    err = OMX_SetConfig(mSTECamera->mCam, mSTECamera->mOmxILExtIndex->getIndex(OmxILExtIndex::EOrientationScene), stillOrientationConfig.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Orientation Scene OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    /*Setup Software Processing*/
    mSTECamera->mISPImageOmxBuffInfo.mSwRotation = CamSwRoutines::YUV420MB_180_rotation_inplace;

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE DeviceSwRotation::handleBuffer(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr)
{
    DBGT_PROLOG("aOmxBuffHdr: %p", aOmxBuffHdr);

    mSTECamera->doPictureSwRotProcessing(aOmxBuffHdr, mSTECamera->mISPImageOmxBuffInfo.m_pMMHwBuffer);

    OMX_ERRORTYPE err = updateExtraDataOrientation(aOmxBuffHdr);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("updateExtraDataOrientation failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

void DeviceSwRotation::doUpdateExtraDataOrientation(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *pCaptureParameters, char *pExifParameters)
{
    DBGT_PROLOG("pCaptureParameters->eSceneOrientation: %d", pCaptureParameters->eSceneOrientation);

    DBGT_ASSERT(NULL != pCaptureParameters, "pCaptureParameters is null");
    DBGT_ASSERT(NULL != pExifParameters, "pExifParameters is null");

    pCaptureParameters->eSceneOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;

    updateExifParamsOrientation(pExifParameters, pCaptureParameters->eSceneOrientation);

    DBGT_EPILOG("");
}
}; //namespace android
