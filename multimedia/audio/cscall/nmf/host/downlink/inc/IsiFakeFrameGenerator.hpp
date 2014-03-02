/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiFakeFrameGenerator.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef __isiFakeFrameGeneratorhpp__
#define __isiFakeFrameGeneratorhpp__

#include "cscall/nmf/host/downlink/inc/FakeFrameGenerator.hpp"

class IsiFakeFrameGenerator : public FakeFrameGenerator {
    public:
        IsiFakeFrameGenerator(void);
        virtual ~IsiFakeFrameGenerator() {};

        /**
         * if this specific configuration can handle the creation of fake Frame
         * By default, returns false (downlink component must do it by itself)
         * */
        virtual bool codecCanHandleFakeFrame() {return ((mCodec != CODEC_PCM8) && (mCodec != CODEC_PCM16));};

        /**
         * set the codec configuration. 
         * Called when a new codec configuration is received from the modem
         * */
        virtual void setConfiguration(CsCallCodec_t codecType, Cscall_Codec_Config_t* pCodecInfo);

        /**
         * Generate a fake Frame according to current configuration
         * */
        virtual OMX_BUFFERHEADERTYPE* generateFakeFrame();

        /**
         * Called to store latest decoded Frame (32b) to help generating new Frame
         * Frame is stored in protected member mLatesetFrame
         * */
        virtual void setLatestFrame(OMX_BUFFERHEADERTYPE* Frame);

    protected:
        typedef enum {NOT_USED, GSM, WCDMA} network_idx;
        
    private:
        OMX_BUFFERHEADERTYPE mLatesetFrame;
	    OMX_U8               mFrameBuffer[100];

        CsCallCodec_t mCodec;
        network_idx   mNetwork;

};

#endif
