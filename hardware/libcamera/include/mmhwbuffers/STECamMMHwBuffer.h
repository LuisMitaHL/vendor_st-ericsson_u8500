#ifndef __STECAMMMHWBUFFER_H__
#define __STECAMMMHWBUFFER_H__

#include <mmhwbuffer.h>
#include <mmhwbuffer_ext.h>
#include <mmhwbuffer_c_wrapper.h>

#include "STECamUtils.h"

namespace android {
class OmxBuffInfo;

class STECamMMHwBuffer {
    public:
        static int init();
        static void deinit();
        static OMX_ERRORTYPE allocateHwBuffer(OMX_U32 aCount, OMX_U32 aSize, bool aCached,
                                              OmxBuffInfo* aOwnerOmxBuffInfo, int aOwnerPort, sp<MemoryHeapBase>* aMemoryHeapBase,
                                              OmxBuffInfo* aShareOmxBuffInfo=NULL, int aSharePort=0);

        static OMX_ERRORTYPE freeHwBuffer(OmxBuffInfo* aOwnerOmxBuffInfo, bool aNativeWindowFlag=false);

        static OMX_ERRORTYPE allocateMMNativeBuf(MMNativeBuffer** aOwnerMMNativeBuf,
                                                 void* aBuf);
};
}
#endif //__STECAMMMHWBUFFER_H__
