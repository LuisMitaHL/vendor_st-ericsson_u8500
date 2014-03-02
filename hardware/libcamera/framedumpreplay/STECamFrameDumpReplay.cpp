#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 2
#define DBGT_PREFIX "DmpRpy"

#include "STECamTrace.h"
#include "STECamFrameDumpReplay.h"
#include "STECamera.h"

namespace android {

CFrameDumpReplay::CFrameDumpReplay() {
    DBGT_PROLOG("");

    for(int i=0; i<EMaxDumpReplay; i++) {
        memset(&mDumpReplay[i],0,sizeof(Type));
        mDumpReplay[i].resetData();
    }

    DBGT_EPILOG("");
    return;
}

void CFrameDumpReplay::Type::setData(int aWidth, int aHeight, TColorFmt aColorFormat,
                                     STECamera* aCameraHal, int aAllocLen, MMHwBuffer* aMMHwBuffer) {
    DBGT_PROLOG("");

    mWidth = aWidth;
    mHeight = aHeight;
    mColorFormat = aColorFormat;
    mCameraHal = aCameraHal;
    mBufferAllocLen = aAllocLen;
    mMMHwBuffer = aMMHwBuffer;

    DBGT_EPILOG("");
    return;
}

void CFrameDumpReplay::Type::resetData() {
    DBGT_PROLOG("");

    mContinue = false;
    mFilename[0] = '\0';
    mWidth = 0;
    mHeight = 0;
    mColorFormat = EMaxColorFmt;
    if(mFile) {
        fclose(mFile);
        mFile = NULL;
    }
    mCameraHal = NULL;
    mBufferAllocLen = 0;
    mMMHwBuffer = NULL;

    DBGT_EPILOG("");
    return;
}

};
