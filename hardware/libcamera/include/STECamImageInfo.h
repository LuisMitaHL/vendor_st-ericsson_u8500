/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMIMAGEINFO_H
#define STECAMIMAGEINFO_H

namespace android {

class ImageInfo
    {

    public:
        // Constructor
        inline ImageInfo();
        inline ImageInfo(uint32_t aWidth,
                         uint32_t aHeight,
                         int32_t aQuality,
                         int32_t aRotation);
        inline ImageInfo(uint32_t aWidth,
                         uint32_t aHeight,
                         const char * const aQuality,
                         const char * const aRotation);
        inline ImageInfo(const char * const aWidth,
                         const char * const aHeight,
                         const char *const aQuality,
                         const char * const aRotation);
        // Destructor
        inline ~ImageInfo();
        //Member routines
        inline void setupImageInfo(uint32_t aWidth,
                                   uint32_t aHeight,
                                   int32_t aQuality,
                                   int32_t aRotation,
                                   uint32_t aWidthAlignment,
                                   uint32_t aHeightAlignment); // Required in initdefault()

        inline void setupImageInfo(uint32_t aWidth,
                                   uint32_t aHeight,
                                   const char * const aQuality,
                                   const char * const aRotation,
                                   uint32_t aWidthAlignment,
                                   uint32_t aHeightAlignment); // Required in setParameters

        inline void setupImageInfo(const char * const aWidth,
                                   const char * const aHeight,
                                   const char * const aQuality,
                                   const char * const aRotation,
                                   uint32_t aWidthAlignment,
                                   uint32_t aHeightAlignment);

        inline uint32_t getWidth() const;
        inline uint32_t getHeight() const;
        inline uint32_t getAlignedWidth() const;
        inline uint32_t getAlignedHeight() const;
        inline uint32_t getOverScannedWidth() const;
        inline uint32_t getOverScannedHeight() const;
        inline int32_t getQuality() const;
        inline int32_t getRotation() const;
        inline void setRotation(int32_t aRotation);
        inline void setWidthAlignment(uint32_t aWidthAlignment);
        inline void setHeightAlignment(uint32_t aHeightAlignment);
        inline void setOverScannedParams(uint32_t aWidth, uint32_t aHeight);
        inline int32_t invalidQuality() const;
        inline void enableSwapParams();
        inline bool getEnableSwapParams();
        inline bool swapParams() const;
        inline void resetSwapParams();
        inline void dumpValues();

        inline bool operator != (const ImageInfo&);
        inline bool operator == (const ImageInfo&);

    private:
        uint32_t mWidth;
        uint32_t mHeight;
        int32_t mQuality;
        int32_t mRotation;
        uint32_t mWidthAlignment;
        uint32_t mHeightAlignment;
        uint32_t mAlignedWidth;
        uint32_t mAlignedHeight;
        uint32_t mOverScannedWidth;
        uint32_t mOverScannedHeight;
        bool mSwapParams;
        bool isSwapParams ;
        static const int32_t kInvalidQuality = -1 ;
};

#include "STECamImageInfo.inl"

} //namespace android

#endif //STECAMIMAGEINFO_INL
