/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "mmhwbuffer_c_wrapper.h"

extern "C"
{
    OMX_ERRORTYPE MMHwBuffer_Create_CWrapper(const MMHwBuffer::TBufferPoolCreationAttributes &
                                             aBuffPoolAttrs,
                                             OMX_HANDLETYPE aComponent,
                                             MMHwBuffer * &apMMHwBufferHandle)
    {
        return MMHwBuffer::Create(aBuffPoolAttrs, aComponent, apMMHwBufferHandle);
    }

    OMX_ERRORTYPE MMHwBuffer_Destroy_CWrapper(MMHwBuffer * &apMMHwBufferHandle)
    {
        return MMHwBuffer::Destroy(apMMHwBufferHandle);
    }

    MMNativeBuffer * create_mmnative_buffer(void *pBuffer)
    {
        return new MMNativeBuffer(pBuffer);
    }

    void destroy_mmnative_buffer(MMNativeBuffer *pNativeBuffer)
    {
        if (pNativeBuffer) {
            delete pNativeBuffer;
        }
    }

    int get_mm_file_handle(void *pBuffer, int *size)
    {
        return MMNativeBuffer::getFileHandle(pBuffer, size);
    }
}
