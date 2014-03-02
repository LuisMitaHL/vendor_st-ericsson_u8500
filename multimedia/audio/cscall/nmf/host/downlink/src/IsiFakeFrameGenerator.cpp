/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiFakeFrameGenerator.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "cscall/nmf/host/downlink/inc/IsiFakeFrameGenerator.hpp"
#include <string.h>

IsiFakeFrameGenerator::IsiFakeFrameGenerator(void):
    mCodec(CODEC_NONE),mNetwork(NOT_USED)
{
    memset(&mLatesetFrame, 0, sizeof(mLatesetFrame));
    mLatesetFrame.pBuffer              = mFrameBuffer;          
    mLatesetFrame.nAllocLen            = sizeof(mFrameBuffer);
};

void IsiFakeFrameGenerator::setConfiguration(CsCallCodec_t codecType, Cscall_Codec_Config_t* pCodecInfo) {
    mCodec   = codecType;
    switch (codecType) {
        case CODEC_AMR_NB:
            switch (pCodecInfo->amrnbDecConfig.ePayloadFormat) {
                case AMR_MB_MODEM_GSM_Payload:
                    mNetwork = GSM;
                    break;
                case AMR_MB_MODEM_3G_Payload:
                    if (mNetwork == GSM) {
                        *((int*)(mLatesetFrame.pBuffer)) &= 0x1FFF; // Reset RxType (not used in 3G)
                    }
                    mNetwork = WCDMA;
                    break;
                default:
                    ARMNMF_DBC_ASSERT_MSG(0, "Cscall/IsiFakeFrameGenerator: wrong AMRNB payload");
            }
            break;
        case CODEC_AMR_WB:
            switch (pCodecInfo->amrwbDecConfig.ePayloadFormat) {
                case AMRWB_MB_MODEM_GSM_Payload:
                    if (mNetwork == GSM) {
                        *((int*)(mLatesetFrame.pBuffer)) &= 0x1FFF; // Reset RxType (not used in 3G)
                    }
                    mNetwork = GSM;
                    break;
                case AMRWB_MB_MODEM_3G_Payload:
                    mNetwork = WCDMA;
                    break;
                default:
                    ARMNMF_DBC_ASSERT_MSG(0, "Cscall/IsiFakeFrameGenerator: wrong AMRWB payload");
            }
            break;
        default:
            mNetwork = GSM;
    }

}

void IsiFakeFrameGenerator::setLatestFrame(OMX_BUFFERHEADERTYPE* Frame)
{
    ARMNMF_DBC_ASSERT(Frame->nFilledLen <= mLatesetFrame.nAllocLen);
    memcpy((void *)mLatesetFrame.pBuffer, (void *)Frame->pBuffer, Frame->nFilledLen);
    mLatesetFrame.nFilledLen = Frame->nFilledLen;
}

OMX_BUFFERHEADERTYPE* IsiFakeFrameGenerator::generateFakeFrame() {
    
    *((int*)(mLatesetFrame.pBuffer)) |= 1; /// set BFI

    return &mLatesetFrame;

}


