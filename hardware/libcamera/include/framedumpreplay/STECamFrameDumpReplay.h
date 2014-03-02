#ifndef __STECAMFRAMEDUMPREPLAY_H__
#define __STECAMFRAMEDUMPREPLAY_H__
//using namespace android;

#include <stdio.h>
#include "STECamDynSetting.h"
#include <utils/threads.h>

#include <mmhwbuffer.h>
#include <mmhwbuffer_ext.h>
#include <mmhwbuffer_c_wrapper.h>

namespace android {

class STECamera;

#define COLOR_FMT_NAME_EXT_MAX 20

struct ColorFmtInfo {
    ColorFmtInfo() : mBpp(0) {
    mColorFmtExtStr[0] = '\0';
    }

    ColorFmtInfo(const char* aColorFmtExtStr,
             int aBpp) :
             mBpp(aBpp) {
        strncpy(mColorFmtExtStr,aColorFmtExtStr,COLOR_FMT_NAME_EXT_MAX);
    }

    char mColorFmtExtStr[COLOR_FMT_NAME_EXT_MAX+1];
    int mBpp;
};

static const ColorFmtInfo g_ColorFmtInfo[] = {
    ColorFmtInfo("CbYCrY.yuv",16), //CbYCrY
    ColorFmtInfo("16bitRGB565.raw",16), //16bitRGB565
    ColorFmtInfo("YUV420MBPSP.yuv",12), //YUV420MBPSP
    ColorFmtInfo()
};

class CFrameDumpReplay {
    public:
        CFrameDumpReplay();
        virtual ~CFrameDumpReplay() {}
    public:
        enum TColorFmt {
            ECbYCrY,
            E16bitRGB565,
            EYUV420MBPackedSemiPlanar,
            EMaxColorFmt
        };

        enum TDumpReplayType {
            EPreview = 0,
            ERecording,
            EMaxDumpReplay
        };

        struct Type {
            Type(): mFile(NULL),
                    mKey(DynSetting::EMax),
                    mDefaultLocation(NULL),
                    mPrefix(NULL) {
                resetData();
            }

            void setData(int aWidth, int aHeight, TColorFmt aColorFormat,
                         STECamera* aCameraHal, int aAllocLen, MMHwBuffer* aMMHwBuffer);
            void resetData();

            bool mContinue;
            char mFilename[FILENAME_MAX + 1];
            int mWidth;
            int mHeight;
            TColorFmt mColorFormat;
            FILE* mFile;
            DynSetting::Type mKey;
            const char* mDefaultLocation;
            const char* mPrefix;
            STECamera* mCameraHal;
            int mBufferAllocLen;
            MMHwBuffer* mMMHwBuffer;
        };

    public:
        Type mDumpReplay[EMaxDumpReplay];
};

};

#endif //__STECAMFRAMEDUMPREPLAY_H__
