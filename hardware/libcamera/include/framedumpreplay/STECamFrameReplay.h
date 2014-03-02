#ifndef __STECAMFRAMEREPLAY_H__
#define __STECAMFRAMEREPLAY_H__

#include "STECamFrameDumpReplay.h"

const char kFileRecording[] = "/sdcard/STECamera/record.yuv";
const char kFilePreview[] = "/sdcard/STECamera/preview.yuv";

namespace android {

class CFrameReplay: public CFrameDumpReplay {
    //constructor & destructor
    public:
        CFrameReplay();
        ~CFrameReplay();

    public:
        inline void process(void* pBuffer, int aLength, TDumpReplayType aReplayType);
        inline void process(void* pBuffer, TDumpReplayType aReplayType);
        inline bool start(TDumpReplayType aReplayType, int aWidth, int aHeight, TColorFmt aColorFormat,
                          STECamera* aCameraHal=NULL, int aAllocLen=0, MMHwBuffer* aMMHwBuffer=NULL);
        inline void stop(TDumpReplayType aReplayType);

    private:
        void replayBuffer(void* pBuffer, int aLength, TDumpReplayType aReplayType);
        void replayBuffer(void* pBuffer, TDumpReplayType aReplayType);
        bool startReplay(TDumpReplayType aReplayType, int aWidth, int aHeight, TColorFmt aColorFormat,
                         STECamera* aCameraHal, int aAllocLen, MMHwBuffer* aMMHwBuffer);
        void stopReplay(TDumpReplayType aReplayType);
};

#include "STECamFrameReplay.inl"

};

#endif //__STECAMFRAMEDUMPER_H__
