/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FakeFrameGenerator.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef __fakeFrameGeneratorhpp__
#define __fakeFrameGeneratorhpp__

#include <armnmf_dbc.h>
#include "TraceObject.h"
#include "OMX_Core.h"
#include "cscall/nmf/host/protocolhandler/codecreq.idt.h"
#include "cscall_codec.h"

class FakeFrameGenerator : public TraceObject
{
    public:
        FakeFrameGenerator(void) {};
        virtual ~FakeFrameGenerator() {};
        static FakeFrameGenerator* createFakeFrameGenerator(CsCallProtocol_t protocol);

        /**
         * if this specific configuration can handle the creation of fake Frame
         * By default, returns false (downlink component must do it by itself)
         * */
        virtual bool codecCanHandleFakeFrame() {return false;};

        /**
         * set the codec configuration. 
         * Called when a new codec configuration is received from the modem
         * */
        virtual void setConfiguration(CsCallCodec_t codecType, Cscall_Codec_Config_t* codecInfo) {};

        /**
         * Generate a fake Frame according to current configuration
         * */
        virtual OMX_BUFFERHEADERTYPE* generateFakeFrame() {return (OMX_BUFFERHEADERTYPE*)0;};

        /**
         * Called to store latest decoded Frame (32b) to help generating new Frame
         * Frame is stored in protected member mLatesetFrame
         * */
        virtual void setLatestFrame(OMX_BUFFERHEADERTYPE* Frame) {};
 
};

#endif
