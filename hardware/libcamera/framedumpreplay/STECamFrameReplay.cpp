#include <sys/stat.h>
#include <OMX_IVCommon.h>
#include <OMX_Symbian_IVCommonExt_Ste.h>


#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 2
#define DBGT_PREFIX "Replay"

#include "STECamTrace.h"

#include "STECamFrameReplay.h"
#include "STECamera.h"

namespace android {

CFrameReplay::CFrameReplay() {
    DBGT_PROLOG("");

    mDumpReplay[EPreview].mKey = DynSetting::EPreviewReplay;
    mDumpReplay[EPreview].mDefaultLocation = kFilePreview;
    mDumpReplay[ERecording].mKey = DynSetting::ERecordingReplay;
    mDumpReplay[ERecording].mDefaultLocation = kFileRecording;

    DBGT_EPILOG("");
    return;
}

CFrameReplay::~CFrameReplay() {
    DBGT_PROLOG("");

    DBGT_EPILOG("");
    return;
}

void CFrameReplay::replayBuffer(void* pBuffer, int aLength, TDumpReplayType aReplayType) {
    if(mDumpReplay[aReplayType].mContinue) {
        DBGT_PROLOG("aReplayType: %d, buffer: 0x%p, length: %d",aReplayType, pBuffer, aLength);

        DBGT_ASSERT(NULL != pBuffer, "pBuffer is NULL");
        DBGT_ASSERT(0 != aLength, "aLength is zero");
        DBGT_ASSERT(aReplayType<EMaxDumpReplay,"invalid Replay Request");

        DBGT_ASSERT(NULL != mDumpReplay[aReplayType].mFile, "file not open for reading");

        int readBufSize = fread(pBuffer,1,aLength,mDumpReplay[aReplayType].mFile);
        DBGT_PTRACE("readBufSize: %d",readBufSize);

        while(readBufSize < aLength ){
            if(!feof(mDumpReplay[aReplayType].mFile)) {
                DBGT_CRITICAL("file corrupt on card");
            }
            DBGT_PTRACE("EOF reached");

            fseek(mDumpReplay[aReplayType].mFile,0,SEEK_SET);
            readBufSize = fread(pBuffer,1,aLength,mDumpReplay[aReplayType].mFile);
            DBGT_PTRACE("readBufSize: %d",readBufSize);
        }

        if(mDumpReplay[aReplayType].mCameraHal && (mDumpReplay[aReplayType].mBufferAllocLen > 0)) {
            OMX_ERRORTYPE omxerr = mDumpReplay[aReplayType].mCameraHal->synchCBData(MMHwBuffer::ESyncBeforeReadHwOperation,
                                                                                    *mDumpReplay[aReplayType].mMMHwBuffer,
                                                                                    (OMX_U8*)pBuffer, mDumpReplay[aReplayType].mBufferAllocLen);
            DBGT_PTRACE("omxerr : %d",omxerr);
        }

        DBGT_EPILOG("");
        return;
    }
}

void CFrameReplay::replayBuffer(void* pBuffer, TDumpReplayType aReplayType) {
    replayBuffer(pBuffer,
               (mDumpReplay[aReplayType].mWidth*mDumpReplay[aReplayType].mHeight*g_ColorFmtInfo[mDumpReplay[aReplayType].mColorFormat].mBpp/8),
               aReplayType);
}

bool CFrameReplay::startReplay(TDumpReplayType aReplayType, int aWidth, int aHeight, TColorFmt aColorFormat,
                               STECamera* aCameraHal, int aAllocLen, MMHwBuffer* aMMHwBuffer) {
    DBGT_PROLOG("");

    DBGT_PTRACE("aReplayType: %d, aWidth: %d, aHeight: %d",aReplayType, aWidth, aHeight);
    DBGT_PTRACE("aColorFormat: %#x, aCameraHal: 0x%p, aAllocLen: %d",aColorFormat, aCameraHal, aAllocLen);

    DBGT_ASSERT(aReplayType<EMaxDumpReplay,"invalid Replay Request");
    DBGT_ASSERT(0 != aWidth, "pBuffer is NULL");
    DBGT_ASSERT(0 != aHeight, "aLength is zero");
    DBGT_ASSERT(aColorFormat<EMaxColorFmt,"invalid Color Format");

    mDumpReplay[aReplayType].resetData();
    mDumpReplay[aReplayType].setData(aWidth, aHeight, aColorFormat,aCameraHal,aAllocLen,aMMHwBuffer);

    DynSetting::get(mDumpReplay[aReplayType].mKey, mDumpReplay[aReplayType].mFilename);
    DBGT_PTRACE("key: %s",mDumpReplay[aReplayType].mFilename);

    if((!strcmp(mDumpReplay[aReplayType].mFilename,"0")) || (!strcmp(mDumpReplay[aReplayType].mFilename,""))) {
        mDumpReplay[aReplayType].resetData();

        DBGT_PTRACE("not dumping");
        DBGT_EPILOG("");
        return false;
    }

    if(!strcmp(mDumpReplay[aReplayType].mFilename,"1")) {
            strncpy(mDumpReplay[aReplayType].mFilename,mDumpReplay[aReplayType].mDefaultLocation,FILENAME_MAX);
    }

    mDumpReplay[aReplayType].mFile = fopen(mDumpReplay[aReplayType].mFilename,"rb");
    if(!mDumpReplay[aReplayType].mFile) {
        mDumpReplay[aReplayType].resetData();

        DBGT_PTRACE("not dumping");
        DBGT_EPILOG("");
        return false;
    }
    mDumpReplay[aReplayType].mContinue = true;

    DBGT_PTRACE("dumping");
    DBGT_EPILOG("");
    return true;
}

void CFrameReplay::stopReplay(TDumpReplayType aReplayType) {
    DBGT_PROLOG("");

    DBGT_ASSERT(aReplayType<EMaxDumpReplay,"invalid Replay Request");

    mDumpReplay[aReplayType].resetData();

    DBGT_EPILOG("");
    return;
}

};
