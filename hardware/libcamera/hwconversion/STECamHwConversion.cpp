/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 2
#define DBGT_PREFIX "HwConv"

//Internal includes
#include <blt_api.h>
#include <linux/hwmem.h>
#include <sys/ioctl.h>
#include <sys/time.h>

//Internal includes
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamOmxUtils.h"
#include "STEExtIspCamera.h"
#include "STECamMemoryHeapBase.h"

#include "hwconversion/STECamHwConversion.h"


namespace android {

#undef _CNAME_
#define _CNAME_ SteHwConv

    SteHwConv::SteHwConv(
            int rotation,
            int inputWidth,
            int inputHeight,
            OMX_COLOR_FORMATTYPE inputColorFormat,
            int outputWidth,
            int outputHeight,
            OMX_COLOR_FORMATTYPE outputColorFormat)
    {
        timestamp_begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
        DBGT_PROLOG("");

        mSrcRectWidth = inputWidth;
        mSrcRectHeight = inputHeight;
        mSrcRectX = 0;
        mSrcRectY = 0;

        mDstRectWidth = outputWidth;
        mDstRectHeight = outputHeight;
        mDstRectX = 0;
        mDstRectY = 0;

        mRotation          = rotation;
        mInputWidth        = inputWidth;
        mInputHeight       = inputHeight;
        mInputColorFormat  = inputColorFormat;
        mOutputWidth       = outputWidth;
        mOutputHeight      = outputHeight;
        mOutputColorFormat = outputColorFormat;

        DBGT_PTRACE("rotation %d",rotation);

        // open a new session of the blitter
        mBlt = blt_open();

        timestamp_end = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
        timestamp_init = (timestamp_end-timestamp_begin)/*/1000000*/;
        DBGT_EPILOG("");
    }

    SteHwConv::~SteHwConv()
    {
        DBGT_PROLOG("");
        blt_close( mBlt );
        DBGT_EPILOG("");
    }

    int SteHwConv::transform(
        sp<MemoryBase> inMem,
        sp<MemoryBase> outMem,
        int in_MemFd,
        int out_MemFd )
    {
        DBGT_PDEBUG("> transform");
        status_t err = NO_ERROR;
        ssize_t in_offset;
        ssize_t out_offset;
        size_t in_len;
        size_t out_len;

        inMem->getMemory(
            (ssize_t*)&in_offset,
            (size_t*)&in_len );

        outMem->getMemory(
            (ssize_t*)&out_offset,
            (size_t*)&out_len );

    int retval =  transform(
               in_MemFd,
               in_offset,
               in_len,
               out_MemFd,
               out_offset,
               out_len);
    DBGT_PTRACE("retval = %d", retval);
    DBGT_PDEBUG("< transform");
    return retval;
    }

    int SteHwConv::transform(
        int inMemFd,
        int inOffset,
        int inSize,
        int outMemFd,
        int outOffset,
        int outSize)
    {

        DBGT_PDEBUG("> transform");
        status_t err = NO_ERROR;
        ssize_t in_offset = inOffset;
        ssize_t out_offset = outOffset;
        size_t in_len = inSize;
        size_t out_len = outSize;
        int in_MemFd = inMemFd;
        int out_MemFd = outMemFd;

        struct blt_req req;
        int req_id;
        int ret = NO_ERROR;
        nsecs_t timestamp_Begin;
        nsecs_t timestamp_End;

        timestamp_Begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);

        memset(&req, 0, sizeof(req));
        req.size = sizeof(req);

        req.src_img.buf.type = BLT_PTR_HWMEM_BUF_NAME_OFFSET;
        req.src_img.buf.fd = in_MemFd;
        req.src_img.buf.hwmem_buf_name = ioctl(in_MemFd, HWMEM_EXPORT_IOC, 0);
        req.src_img.buf.offset = in_offset;
        req.src_img.buf.len = in_len;

        switch ((uint32_t)mInputColorFormat){
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            req.src_img.fmt = BLT_FMT_YUV420_PACKED_SEMIPLANAR_MB_STE;
            AlignPow2<int>::up(mInputWidth, 16);
            AlignPow2<int>::up(mInputHeight, 16);
            req.src_img.pitch = mInputWidth;
            break;
        case OMX_COLOR_FormatYUV420PackedSemiPlanar:
        case OMX_COLOR_FormatYUV420SemiPlanar:
            req.src_img.fmt = BLT_FMT_YUV420_PACKED_SEMI_PLANAR;
            AlignPow2<int>::up(mInputWidth, 16);
            AlignPow2<int>::up(mInputHeight, 16);
            req.src_img.pitch = mInputWidth;
            break;
        case OMX_COLOR_FormatYUV420Planar:
            req.src_img.fmt = BLT_FMT_YUV420_PACKED_PLANAR;
            req.src_img.pitch = mInputWidth;
            break;
        case OMX_COLOR_Format16bitRGB565:
            req.src_img.fmt = BLT_FMT_16_BIT_RGB565;
            req.src_img.pitch = mInputWidth*2;
            break;
        case OMX_COLOR_Format24bitRGB888:
            req.src_img.fmt = BLT_FMT_24_BIT_RGB888;
            req.src_img.pitch = mInputWidth*3;
            break;
        case OMX_COLOR_FormatCbYCrY:
            req.src_img.fmt = BLT_FMT_CB_Y_CR_Y;
            req.src_img.pitch = mInputWidth*2;
            break;
        default:
            DBGT_CRITICAL("%s: blt request failed, input format not implemented %x", __func__,mInputColorFormat);
            break;
        }

        req.dst_img.buf.type = BLT_PTR_HWMEM_BUF_NAME_OFFSET;
        req.dst_img.buf.fd = out_MemFd;
        req.dst_img.buf.hwmem_buf_name = ioctl(out_MemFd, HWMEM_EXPORT_IOC, 0);
        req.dst_img.buf.offset = out_offset;
        req.dst_img.buf.len = out_len;


        switch ((uint32_t)mOutputColorFormat){
        case OMX_COLOR_FormatYUV420SemiPlanar:
        case OMX_COLOR_FormatYUV420PackedSemiPlanar:
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV12:
            req.dst_img.fmt = BLT_FMT_YUV420_PACKED_SEMI_PLANAR;
            AlignPow2<int>::up(mOutputWidth, 2);
            AlignPow2<int>::up(mOutputHeight, 2);
            req.dst_img.pitch = mOutputWidth;
            break;
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV21:
            req.dst_img.fmt = BLT_FMT_YVU420_PACKED_SEMI_PLANAR;
            AlignPow2<int>::up(mOutputWidth, 2);
            AlignPow2<int>::up(mOutputHeight, 2);
            req.dst_img.pitch = mOutputWidth;
            break;
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            req.dst_img.fmt = BLT_FMT_YUV420_PACKED_SEMIPLANAR_MB_STE;
            AlignPow2<int>::up(mOutputWidth, 16);
            AlignPow2<int>::up(mOutputHeight, 16);
            req.dst_img.pitch = mOutputWidth;
            break;
        case OMX_COLOR_FormatYUV420Planar:
            req.dst_img.fmt = BLT_FMT_YUV420_PACKED_PLANAR;
            req.dst_img.pitch = mOutputWidth;
            break;
        case OMX_COLOR_Format16bitRGB565:
            req.dst_img.fmt = BLT_FMT_16_BIT_RGB565;
            req.dst_img.pitch = mOutputWidth*2;
            break;
        case OMX_COLOR_Format24bitRGB888:
            req.dst_img.fmt = BLT_FMT_24_BIT_RGB888;
            req.dst_img.pitch = mOutputWidth*3;
            break;
        default:
            DBGT_CRITICAL("%s: blt request failed, output format not implemented", __func__);
            break;
        }

        // set the cropping zone
        req.src_rect.x = mSrcRectX;
        req.src_rect.y = mSrcRectY;
        req.dst_rect.x = mDstRectX;
        req.dst_rect.y = mDstRectY;
        req.src_rect.width = mSrcRectWidth;
        req.src_rect.height = mSrcRectHeight;
        req.dst_rect.width = mDstRectWidth;
        req.dst_rect.height = mDstRectHeight;

        //set image size
        req.src_img.width = mInputWidth;
        req.src_img.height = mInputHeight;
        req.dst_img.width = mOutputWidth;
        req.dst_img.height = mOutputHeight;

        req.flags = (blt_flag)BLT_FLAG_DITHER;

        if(mRotation==90){
            req.transform = BLT_TRANSFORM_CCW_ROT_270;
        }else{
            if(mRotation==180){
                req.transform = BLT_TRANSFORM_CCW_ROT_180;
            }else{
                if(mRotation==270){
                    req.transform = BLT_TRANSFORM_CCW_ROT_90;
                }else{
                    req.transform = BLT_TRANSFORM_NONE;
                }
            }
        }

        req_id = blt_request(mBlt, &req);
        if (req_id < 0) {
            DBGT_CRITICAL("%s: blt request failed (%s)", __func__, strerror(errno));
        }else{
            ret = blt_synch(mBlt, ret);
            if(ret < 0) {
                if(0 != ret) {
                    DBGT_CRITICAL("blt_synch failed ret = %d", ret);
                }
            }
        }

        timestamp_End = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
        DBGT_PDEBUG("< transform %llu us ,Input %d %d %d %d %d %d, Output %d %d %d %d %d %d",
            (timestamp_End-timestamp_Begin)/1000,
            req.src_rect.x,req.src_rect.y,req.src_rect.width,req.src_rect.height,req.src_img.width,req.src_img.height,
            req.dst_rect.x,req.dst_rect.y,req.dst_rect.width,req.dst_rect.height,req.dst_img.width,req.dst_img.height );
        return ret;
    }

    int SteHwConv::setCropSrc(int left, int top, int width, int height)
    {
        DBGT_PROLOG(" left %d, top %d, width %d, height %d", left, top, width, height);
        // set the cropping zone
        mSrcRectX = left;
        mSrcRectY = top;
        mSrcRectWidth = width;
        mSrcRectHeight = height;
        DBGT_EPILOG();
        return 0;
    }

    int SteHwConv::setCropDst(int left, int top, int width, int height)
    {
        DBGT_PROLOG(" left %d, top %d, width %d, height %d", left, top, width, height);
        // set the cropping zone
        mDstRectX = left;
        mDstRectY = top;
        mDstRectWidth = width;
        mDstRectHeight = height;
        DBGT_EPILOG();
        return 0;
    }

}; //namespace android

