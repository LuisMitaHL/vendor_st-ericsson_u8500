#ifndef __STECAMB2R2UTILS_H__
#define __STECAMB2R2UTILS_H__

#include <IFM_Types.h>

#include <mmhwbuffer.h>
#include <mmhwbuffer_ext.h>
#include <mmhwbuffer_c_wrapper.h>

extern "C" {
    #include <blt_api.h>
}

/* Utility Class to use B2R2 for CRROP, RESIZE and ROTATION */
/* HWConversion class doesnot have crop functionality */

class B2R2Utills {
    public:
        struct TImageData {
            TImageData(): mWidth(0), mHeight(0), mBufSize(0),
                          mLogAddr(NULL), mPhyAddr(NULL),
                          mColorFmt(OMX_COLOR_FormatUnused) {
            };
            size_t mWidth;
            size_t mHeight;
            size_t mBufSize;
            void* mLogAddr;
            void* mPhyAddr;
            OMX_COLOR_FORMATTYPE mColorFmt;
        };

        struct TRect {
            TRect(): mX(0), mY(0),
                     mWidth(0), mHeight(0) {
            };
            int mX;
            int mY;
            size_t mWidth;
            size_t mHeight;
        };

    public:
        static unsigned int align16bit(unsigned int aValue);

        static int crop_resize(TImageData* aSrcImage, TImageData* aIntImage, TImageData* aDstImage, TRect* aCropRect, int aPass, bool aInPlace=false);

        static int crop(TImageData* aSrcImage, TImageData* aDstImage, TRect* aCropRect);

        static int resize(TImageData* aSrcImage, TImageData* aDstImage);

        static int crop_resize(TImageData* aSrcImage, TImageData* aDstImage, TRect* aCropRect);

    private:
        static int process(TImageData* aSrcImage, TImageData* aDstImage, TRect* aCropRect, int aRotation);

        static blt_fmt blt_fmt_from_omx_color_format(OMX_COLOR_FORMATTYPE aOmxColorFmt);
};

#endif