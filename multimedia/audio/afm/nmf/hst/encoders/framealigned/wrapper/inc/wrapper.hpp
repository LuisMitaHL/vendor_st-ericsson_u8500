/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _encoder_wrapper_hpp_
#define _encoder_wrapper_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_encoders_framealigned_wrapper : public Component, public hst_encoders_framealigned_wrapperTemplate
{
public:

    hst_encoders_framealigned_wrapper(void);

    // NMF life-cycle methods...
    virtual void stop(void);

    //Component virtual functions
    virtual void process(void);
    virtual void reset(void) ;
    virtual void disablePortIndication(t_uint32 portIdx) ;
    virtual void enablePortIndication(t_uint32 portIdx) ;
    virtual void flushPortIndication(t_uint32 portIdx) ;
  
    virtual void fsmInit(fsmInit_t initFsm);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
    virtual void processEvent(void)  			       { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBufferCheck(INPUT_PORT, buffer); } 
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBufferCheck(OUTPUT_PORT, buffer); }
  
    virtual void setParameter();
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_bitsize);

private:

    typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
    void encode_frame(OMX_BUFFERHEADERTYPE * bufIn, OMX_BUFFERHEADERTYPE * bufOut);

    // interface with encoder algo
    CODEC_INTERFACE_T     mInterfaceFE;
    bool                  mCodecInitialized;
    int                   mFrameNb;
    int                   mByteBufSize;
    Port                  mPorts[2];
};

#endif // _wrapperhst.hpp_
