#include "b2r2/STECamB2R2Utils.h"

#define DBGT_LAYER 0
#define DBGT_PREFIX "B2R2Utils"

#include "STECamTrace.h"
#include <string.h>

/*static*/ int B2R2Utills::crop_resize(TImageData* aSrcImage, TImageData* aIntImage, TImageData* aDstImage, TRect* aCropRect, int aPass, bool aInPlace) {
    DBGT_PROLOG("");
    int ret = 0;
    if(aPass == 1) {
        ret = crop_resize(aSrcImage, aIntImage, aCropRect);
        if(0 != ret) {
            DBGT_CRITICAL("crop_resize failed ret = %d", ret);
            DBGT_EPILOG("");
            return ret;
        }

        if(aInPlace && (aDstImage!=NULL)) {
            size_t copySize = aDstImage->mBufSize;
            if(copySize > aIntImage->mBufSize) {
                copySize = aIntImage->mBufSize;
            }
            memcpy(aDstImage->mLogAddr, aIntImage->mLogAddr, copySize);
        }
    } else {
        ret = crop(aSrcImage, aIntImage, aCropRect);
        if(0 != ret) {
            DBGT_CRITICAL("crop failed ret = %d", ret);
            DBGT_EPILOG("");
            return ret;
        }

        ret = resize(aIntImage, aDstImage);
        if(0 != ret) {
            DBGT_CRITICAL("resize failed ret = %d", ret);
            DBGT_EPILOG("");
            return ret;
        }
    }
    DBGT_EPILOG("");
    return ret;
}

/*static*/ int B2R2Utills::crop(TImageData* aSrcImage, TImageData* aDstImage, TRect* aCropRect) {
    DBGT_PROLOG("");
    int ret = process(aSrcImage, aDstImage, aCropRect, 0);
    if(0 != ret) {
        DBGT_CRITICAL("crop failed ret = %d", ret);
    }
    DBGT_EPILOG("");
    return ret;
}

/*static*/ int B2R2Utills::resize(TImageData* aSrcImage, TImageData* aDstImage) {
    DBGT_PROLOG("");
    int ret = process(aSrcImage, aDstImage, NULL, 0);
    if(0 != ret) {
        DBGT_CRITICAL("resize failed ret = %d", ret);
    }
    DBGT_EPILOG("");
    return ret;
}

/*static*/ int B2R2Utills::crop_resize(TImageData* aSrcImage, TImageData* aDstImage, TRect* aCropRect) {
    DBGT_PROLOG("");
    int ret = process(aSrcImage, aDstImage, aCropRect, 0);
    if(0 != ret) {
        DBGT_CRITICAL("crop_resize failed ret = %d", ret);
    }
    DBGT_EPILOG("");
    return ret;
}

/*static*/ blt_fmt B2R2Utills::blt_fmt_from_omx_color_format(OMX_COLOR_FORMATTYPE aOmxColorFmt) {
    DBGT_PROLOG("");
    blt_fmt bltfmt = BLT_FMT_UNUSED;

    switch((uint32_t)aOmxColorFmt) {
        case OMX_COLOR_Format16bitRGB565:
            bltfmt = BLT_FMT_16_BIT_RGB565;
            break;
        case OMX_COLOR_FormatCbYCrY:
            bltfmt = BLT_FMT_CB_Y_CR_Y;
            break;
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            bltfmt = BLT_FMT_YUV420_PACKED_SEMIPLANAR_MB_STE;
            break;
        case OMX_COLOR_FormatYUV420SemiPlanar:
            bltfmt = BLT_FMT_YUV420_PACKED_SEMI_PLANAR;
            break;
        default:
            bltfmt = BLT_FMT_UNUSED;
    }

    DBGT_EPILOG("");
    return bltfmt;
}

/*static*/ unsigned int B2R2Utills::align16bit(unsigned int aValue) {
    return (((aValue + 0xf) >> 4) << 4);
}

/*static*/ int B2R2Utills::process(TImageData* aSrcImage, TImageData* aDstImage, TRect* aCropRect, int aRotation) {
    DBGT_PROLOG("");
    int ret = -1;

    int blt_handle = blt_open();

    DBGT_PTRACE("blt_open blt_handle : %d",blt_handle);
    if(blt_handle < 0) {
        DBGT_CRITICAL("blt_open failed err = %d", ret);
        DBGT_EPILOG("");
        return ret;
    }

    blt_req bltreq;

    memset(&bltreq,0x0,(sizeof(bltreq)));

    bltreq.size = sizeof(bltreq);

    if(aRotation == 0) {
        bltreq.transform = BLT_TRANSFORM_NONE;
    } else if(aRotation == 90) {
        bltreq.transform = BLT_TRANSFORM_CCW_ROT_90;
    } else if(aRotation == 180) {
        bltreq.transform = BLT_TRANSFORM_CCW_ROT_180;
    } else if(aRotation == 270) {
        bltreq.transform = BLT_TRANSFORM_CCW_ROT_270;
    }

    /********************Start Set Up src_img & dst_img********************/
    /*set the source*/
    bltreq.src_img.fmt = blt_fmt_from_omx_color_format(aSrcImage->mColorFmt);

#if 0
    bltreq.src_img.buf.type = BLT_PTR_FD_OFFSET;
    bltreq.src_img.buf.fd = aSrcImage->mFD;
    bltreq.src_img.buf.offset = aSrcImage->mOffset;
    bltreq.src_img.buf.len = aSrcImage->mBufSize;
#endif
    bltreq.src_img.buf.type = BLT_PTR_PHYSICAL;
    bltreq.src_img.buf.offset = (uint32_t)aSrcImage->mPhyAddr;
    bltreq.src_img.buf.len = aSrcImage->mBufSize;

    size_t srcAlignedWidth = aSrcImage->mWidth;
    size_t srcAlignedHeight = aSrcImage->mHeight;

    if(bltreq.src_img.fmt == BLT_FMT_YUV420_PACKED_SEMIPLANAR_MB_STE) {
        srcAlignedWidth = align16bit(srcAlignedWidth);
        srcAlignedHeight = align16bit(srcAlignedHeight);
    }

    bltreq.src_img.width = srcAlignedWidth;
    bltreq.src_img.height = srcAlignedHeight;
    bltreq.src_img.pitch = 0;

    /*set the destination*/
    bltreq.dst_img.fmt = blt_fmt_from_omx_color_format(aDstImage->mColorFmt);

#if 0
    bltreq.dst_img.buf.type = BLT_PTR_FD_OFFSET;
    bltreq.dst_img.buf.fd = aDstImage->mFD;
    bltreq.dst_img.buf.offset = aDstImage->mOffset;
    bltreq.dst_img.buf.len = aDstImage->mBufSize;
#endif
    bltreq.dst_img.buf.type = BLT_PTR_PHYSICAL;
    bltreq.dst_img.buf.offset = (uint32_t)aDstImage->mPhyAddr;
    bltreq.dst_img.buf.len = aDstImage->mBufSize;

    size_t dstAlignedWidth = aDstImage->mWidth;
    size_t dstAlignedHeight = aDstImage->mHeight;

    if(bltreq.src_img.fmt == BLT_FMT_YUV420_PACKED_SEMIPLANAR_MB_STE) {
        dstAlignedWidth = align16bit(dstAlignedWidth);
        dstAlignedHeight = align16bit(dstAlignedHeight);
    }

    bltreq.dst_img.width = dstAlignedWidth;
    bltreq.dst_img.height = dstAlignedHeight;
    bltreq.dst_img.pitch = 0;
    /********************End Set Up src_img & dst_img********************/

    /********************Start Set Up src_rect & dst_rect********************/
    if(aCropRect != NULL) {
        bltreq.src_rect.x  = aCropRect->mX;
        bltreq.src_rect.y  = aCropRect->mY;
        bltreq.src_rect.width  = aCropRect->mWidth;
        bltreq.src_rect.height  = aCropRect->mHeight;
    } else {
        bltreq.src_rect.x  = 0;
        bltreq.src_rect.y  = 0;
        bltreq.src_rect.width  = aSrcImage->mWidth;
        bltreq.src_rect.height  = aSrcImage->mHeight;
    }

    bltreq.dst_rect.x  = 0;
    bltreq.dst_rect.y  = 0;
    bltreq.dst_rect.width  = aDstImage->mWidth;
    bltreq.dst_rect.height  = aDstImage->mHeight;
    /********************End Set Up src_rect & dst_rect********************/

    /********************Start Set Up dst_clip_rect********************/
//    bltreq.dst_clip_rect.x = 0;
//    bltreq.dst_clip_rect.y = 0;
//    bltreq.dst_clip_rect.width = aDstImage->mWidth;
//    bltreq.dst_clip_rect.height = aDstImage->mHeight;
    /********************End Set Up dst_clip_rect********************/

    bltreq.global_alpha = 255;
    bltreq.prio = 4;

    DBGT_PTRACE("bltreq.size : %d, bltreq.flags : %#x, bltreq.transform : %#x, bltreq.prio : %d",
          bltreq.size,bltreq.flags,bltreq.transform,bltreq.prio);

    DBGT_PTRACE("bltreq.src_img.fmt : %#x, bltreq.src_img.width : %d, bltreq.src_img.height : %d, bltreq.src_img.pitch : %d",
          bltreq.src_img.fmt,bltreq.src_img.width,bltreq.src_img.height,bltreq.src_img.pitch);

    DBGT_PTRACE("bltreq.src_img.buf.type : %d, bltreq.src_img.buf.fd : %d, bltreq.src_img.buf.offset : %#x, bltreq.src_img.buf.len : %d",
          bltreq.src_img.buf.type,bltreq.src_img.buf.fd,bltreq.src_img.buf.offset,bltreq.src_img.buf.len);

    DBGT_PTRACE("bltreq.src_rect.x : %d, bltreq.src_rect.y : %d, bltreq.src_rect.width : %d, bltreq.src_rect.height : %d",
          bltreq.src_rect.x,bltreq.src_rect.y,bltreq.src_rect.width,bltreq.src_rect.height);

    DBGT_PTRACE("bltreq.dst_img.fmt : %#x, bltreq.dst_img.width : %d, bltreq.dst_img.height : %d, bltreq.dst_img.pitch : %d",
          bltreq.dst_img.fmt,bltreq.dst_img.width,bltreq.dst_img.height,bltreq.dst_img.pitch);

    DBGT_PTRACE("bltreq.dst_img.buf.type : %d, bltreq.dst_img.buf.fd : %d, bltreq.dst_img.buf.offset : %#x, bltreq.dst_img.buf.len : %d",
          bltreq.dst_img.buf.type,bltreq.dst_img.buf.fd,bltreq.dst_img.buf.offset,bltreq.dst_img.buf.len);

    DBGT_PTRACE("bltreq.dst_rect.x : %d, bltreq.dst_rect.y : %d, bltreq.dst_rect.width : %d, bltreq.dst_rect.height : %d",
          bltreq.dst_rect.x,bltreq.dst_rect.y,bltreq.dst_rect.width,bltreq.dst_rect.height);

    DBGT_PTRACE("bltreq.dst_clip_rect.x : %d, bltreq.dst_clip_rect.y : %d, bltreq.dst_clip_rect.width : %d, bltreq.dst_clip_rect.height : %d",
          bltreq.dst_clip_rect.x,bltreq.dst_clip_rect.y,bltreq.dst_clip_rect.width,bltreq.dst_clip_rect.height);

    /********************Start Conversion now********************/

    DBGT_PTRACE("Setup Done blt_handle : %d",blt_handle);

    ret = blt_request(blt_handle, &(bltreq));

    if(ret >= 0) {
        ret = blt_synch(blt_handle, ret);
        if(ret < 0) {
            blt_close(blt_handle);
            if(0 != ret) {
                DBGT_CRITICAL("blt_synch failed ret = %d", ret);
                DBGT_EPILOG("");
                return ret;
            }
        }
    } else {
        blt_close(blt_handle);
        if(0 != ret) {
            DBGT_CRITICAL("blt_request failed ret = %d", ret);
            DBGT_EPILOG("");
            return ret;
        }
    }
    blt_close(blt_handle);

    DBGT_EPILOG("");
    return ret;
}
