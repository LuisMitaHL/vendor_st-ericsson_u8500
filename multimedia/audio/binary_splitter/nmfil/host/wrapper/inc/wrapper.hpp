/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   binarysplitter.hpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _binarysplitter_nmfil_host_wrapper_hpp_
#define _binarysplitter_nmfil_host_wrapper_hpp_

#include "Component.h"
#include "common_interface.h"


#define NB_OUTPUT 2

class binary_splitter_nmfil_host_wrapper : public Component, public binary_splitter_nmfil_host_wrapperTemplate
{
public:
    binary_splitter_nmfil_host_wrapper();
    virtual ~binary_splitter_nmfil_host_wrapper();

    // NMF life-cycle methods...
    virtual t_nmf_error construct(void);
    virtual void destroy(void);
    virtual void start(void);
    virtual void stop(void);

    // Component virtual methods...
    virtual void reset();
    virtual void process();
    virtual void disablePortIndication(t_uint32 portIdx);
    virtual void enablePortIndication(t_uint32 portIdx);
    virtual void flushPortIndication(t_uint32 portIdx);

    virtual void fsmInit(fsmInit_t initFsm);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) {
        Component::sendCommand(cmd, param) ;
    }
    virtual void processEvent(void)  		               {
        Component::processEvent() ;
    }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 portIdx);
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 portIdx);

    virtual void setParameter(t_pcmprocessing_config_host config);
    virtual void setSynchronizedOutputPort(t_uint16 portIdx, BOOL bIsSynchronized);

    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint8 portIdx);

private:

    t_sample_freq     mSampleFreq[NB_OUTPUT];
    t_uint16          mNbChannels;
    t_uint16          mSampleBitSize;

    typedef enum {INPUT_PORT_IDX, OUTPUT_PORT_IDX} portname;

    typedef struct
    {
        OMX_BUFFERHEADERTYPE_p    BufferPtr;
        bool        bIsSynchronized;
        bool        bOutputRequired;
        bool        bEosReceived;
    }OutputBuffer;

    OutputBuffer                  mOutputBuf[NB_OUTPUT];

    OMX_BUFFERHEADERTYPE_p        mInputBufPtr;
    Port                          mPorts[NB_OUTPUT+1];
    t_pcmprocessing_config_host   mConfig;
};

#endif // _binarysplitter_nmfil_host_wrapper_hpp_
