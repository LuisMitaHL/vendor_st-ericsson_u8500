/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STECAMIMAGEINFO_INL_
#define _STECAMIMAGEINFO_INL_

inline void ImageInfo::setupImageInfo(const char * const aWidth, const char * const aHeight, const char * const aQuality, const char * const aRotation, uint32_t aWidthAlignment = 16, uint32_t aHeightAlignment = 16)
{
    uint32_t tmpWidth, tmpHeight;

    DBGT_ASSERT(NULL != aWidth,"Input width is NULL");
    DBGT_ASSERT(NULL != aHeight,"Input height is NULL");

    tmpWidth = atoi(aWidth);
    tmpHeight = atoi(aHeight);

    setupImageInfo(tmpWidth, tmpHeight, aQuality, aRotation, aWidthAlignment, aHeightAlignment);

}

inline void ImageInfo::setupImageInfo(uint32_t aWidth,
                                      uint32_t aHeight,
                                      const char * const aQuality,
                                      const char * const aRotation,
                                      uint32_t aWidthAlignment = 16,
                                      uint32_t aHeightAlignment = 16)
{

    int32_t tmpQuality = 0;
    int32_t tmpRotation = 0;

    DBGT_ASSERT(0 != aWidth,"Input width is NULL");
    DBGT_ASSERT(0 != aHeight,"Input height is NULL");

    if (aQuality )
        tmpQuality = atoi(aQuality);

    if(aRotation)
        tmpRotation = atoi(aRotation);

    setupImageInfo(aWidth, aHeight, tmpQuality, tmpRotation, aWidthAlignment, aHeightAlignment);

}

inline void ImageInfo::setupImageInfo(uint32_t aWidth ,
                                      uint32_t aHeight,
                                      int32_t aQuality,
                                      int32_t aRotation,
                                      uint32_t aWidthAlignment = 16,
                                      uint32_t aHeightAlignment = 16)
{
    DBGT_PROLOG("Width  %d  Height  %d  Quality  %d  Rotation %d",aWidth, aHeight, aQuality, aRotation);

    /*assuming KEY_ROTATION can be negative*/
    DBGT_ASSERT(((-360 <= aRotation) && (360 >= aRotation)),"Invalid Rotation: %d", aRotation);

    mRotation = (aRotation + 360) % 360;

    mQuality = aQuality;

    if ((mRotation == 90 || mRotation == 270) && (true == mSwapParams)) {
        mWidth = aHeight;
        mHeight = aWidth;
    } else {
        mWidth = aWidth;
        mHeight = aHeight;
    }

    mAlignedWidth = mWidth;
    AlignPow2<uint32_t>::up(mAlignedWidth, aWidthAlignment);

    mAlignedHeight = mHeight;
    AlignPow2<uint32_t>::up(mAlignedHeight, aHeightAlignment);

    isSwapParams = false;

    DBGT_EPILOG("");
}

inline ImageInfo::ImageInfo() : mWidth(0),
                                mHeight(0),
                                mQuality(0),
                                mRotation(0),
                                mWidthAlignment(0),
                                mHeightAlignment(0),
                                mAlignedWidth(0),
                                mAlignedHeight(0),
                                mOverScannedWidth(0),
                                mOverScannedHeight(0),
                                mSwapParams(false),
                                isSwapParams(false)
{

}

inline ImageInfo::ImageInfo(uint32_t aWidth,
                            uint32_t aHeight,
                            int32_t aQuality,
                            int32_t aRotation) :
                            mWidthAlignment(0),
                            mHeightAlignment(0),
                            mSwapParams(false)

{
    setupImageInfo(aWidth, aHeight, aQuality ,aRotation);
}

inline ImageInfo::ImageInfo(uint32_t aWidth,
                            uint32_t aHeight,
                            const char * const aQuality,
                            const char * const aRotation):
                            mWidthAlignment(0),
                            mHeightAlignment(0),
                            mSwapParams(false)
{
    setupImageInfo(aWidth, aHeight, aQuality, aRotation);
}

inline ImageInfo::ImageInfo(const char * const aWidth,
                            const char * const aHeight,
                            const char * const aQuality,
                            const char * const aRotation) :
                            mWidthAlignment(0),
                            mHeightAlignment(0),
                            mSwapParams(false)
{
    setupImageInfo(aWidth, aHeight, aQuality, aRotation);
}

inline ImageInfo::~ImageInfo()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

inline bool ImageInfo::operator!= (const ImageInfo &param) {

    if((mWidth!= param.mWidth) || ( mHeight!= param.mHeight) ||
       (mQuality != param.mQuality) || (mRotation != param.mRotation))
        return true;
    else
        return false;
}

inline bool ImageInfo::operator== (const ImageInfo &param) {

    if(( mWidth == param.mWidth) && ( mHeight == param.mHeight) &&
       ( mQuality == param.mQuality) && (mRotation == param.mRotation))
        return true;
    else
        return false;
}

inline void ImageInfo::dumpValues()
{
    DBGT_PTRACE("mQuality %d mRotation %d",mQuality,mRotation);
    DBGT_PTRACE("mWidth %d  mHeight %d  mAlignedWidth %d  mAlignedHeight %d", mWidth, mHeight, mAlignedWidth, mAlignedHeight);
}

inline uint32_t ImageInfo::getWidth() const{
    return mWidth;
}

inline uint32_t ImageInfo::getHeight() const{
    return mHeight;
}

inline uint32_t ImageInfo::getOverScannedWidth() const{
    if (0 < mOverScannedWidth) {
        return mOverScannedWidth;
    }
    return mAlignedWidth;

    DBGT_EPILOG("");
}

inline uint32_t ImageInfo::getOverScannedHeight() const{
    if (0 < mOverScannedHeight) {
        return mOverScannedHeight;
    }
    return mAlignedHeight;

    DBGT_EPILOG("");
}

inline uint32_t ImageInfo::getAlignedWidth() const{
    return mAlignedWidth;
}

inline uint32_t ImageInfo::getAlignedHeight() const{
    return mAlignedHeight;
}

inline int32_t ImageInfo::getQuality() const{
    return mQuality;
}

inline int32_t ImageInfo::getRotation() const{
    return mRotation;
}

inline int32_t ImageInfo::invalidQuality() const{
    return kInvalidQuality;
}

inline void ImageInfo::enableSwapParams() {
    DBGT_PROLOG("");

    mSwapParams = true;

    DBGT_EPILOG("");
}

inline bool ImageInfo::getEnableSwapParams() {
    return  mSwapParams;
}

inline bool ImageInfo::swapParams() const{
    return  isSwapParams;
}

inline void ImageInfo::resetSwapParams(){
    isSwapParams = false;
}
inline void ImageInfo::setRotation(int32_t aRotation){
    DBGT_PROLOG("aRotation: %d", aRotation);

    if ((180 != abs(aRotation - mRotation)) && (0 != abs(aRotation - mRotation))) {
        if(true == mSwapParams && isSwapParams == false) {
            isSwapParams = true;
            uint32_t tmp = mWidth;
            mWidth = mHeight;
            mHeight = tmp;

            tmp =  mAlignedWidth;
            mAlignedWidth = mAlignedHeight;
            mAlignedHeight = tmp;
        }
    }
    mRotation = aRotation ;

    DBGT_EPILOG("");
}

inline void ImageInfo::setWidthAlignment(uint32_t aWidthAlignment)
{
    DBGT_PROLOG("aWidthAlignment: %d", aWidthAlignment);

    mWidthAlignment = aWidthAlignment;

    DBGT_EPILOG("");
}

inline void ImageInfo::setHeightAlignment(uint32_t aHeightAlignment)
{
    DBGT_PROLOG("aHeightAlignment: %d", aHeightAlignment);

    mHeightAlignment = aHeightAlignment;

    DBGT_EPILOG("");
}

inline void ImageInfo::setOverScannedParams(uint32_t aOverScannedWidth, uint32_t aOverScannedHeight)
{
    DBGT_PROLOG("aOverScannedWidth: %d aOverScannedHeight: %d", aOverScannedWidth, aOverScannedHeight);

    DBGT_ASSERT((0 < aOverScannedWidth) && (0 < aOverScannedHeight), "Invalid Parameters");

    mOverScannedWidth = aOverScannedWidth;
    mOverScannedHeight = aOverScannedHeight;

    DBGT_EPILOG("");
}
#endif // _STECAMIMAGEINFO_INL_
