#ifndef __STECAMFRAMEDUMPER_H__
#define __STECAMFRAMEDUMPER_H__

#include "STECamFrameDumpReplay.h"

const char kFileLocationBase[] = "/data/local/";
const char kRecordingFilePfx[] = "record";
const char kPreviewFilePfx[] = "preview";

namespace android {

class CFrameDump: public CFrameDumpReplay {
    //constructor & destructor
    public:
        CFrameDump();
        ~CFrameDump();

    public:
        inline void process(void* pBuffer, int aLength, TDumpReplayType aDumpType);
        inline void process(void* pBuffer, TDumpReplayType aDumpType);
        inline bool start(TDumpReplayType aDumpType, int aWidth, int aHeight, TColorFmt aColorFormat,
                          STECamera* aCameraHal=NULL, int aAllocLen=0, MMHwBuffer* aMMHwBuffer=NULL);
        inline void stop(TDumpReplayType aDumpType);

    private:
        void dumpBuffer(void* pBuffer, int aLength, TDumpReplayType aDumpType);
        void dumpBuffer(void* pBuffer, TDumpReplayType aDumpType);
        bool startDump(TDumpReplayType aDumpType, int aWidth, int aHeight, TColorFmt aColorFormat,
                       STECamera* aCameraHal, int aAllocLen, MMHwBuffer* aMMHwBuffer);
        void stopDump(TDumpReplayType aDumpType);
        void getNextFileName(TDumpReplayType aDumpType, const char* aLocation);
};

#include "STECamFrameDumper.inl"

};

#endif //__STECAMFRAMEDUMPER_H__
