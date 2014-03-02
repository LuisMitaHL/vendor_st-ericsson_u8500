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
#ifndef _decoder_wrapper_hpp_
#define _decoder_wrapper_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_decoders_framealigned_wrapper : public Component, public hst_decoders_framealigned_wrapperTemplate
{
public:

    //Component virtual functions
    hst_decoders_framealigned_wrapper();

    // NMF life-cycle methods...
    virtual void stop(void);

    virtual void process() ;
    virtual void reset() ;
    virtual void disablePortIndication(t_uint32 portIdx) ;
    virtual void enablePortIndication(t_uint32 portIdx) ;
    virtual void flushPortIndication(t_uint32 portIdx) ;

    virtual void fsmInit(fsmInit_t initFsm);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
    virtual void processEvent(void)  		               { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBufferCheck(INPUT_PORT, buffer); } 
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)   { Component::deliverBufferCheck(OUTPUT_PORT, buffer); }

    virtual void setParameter();

private:

    typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
    void check_synchro(OMX_BUFFERHEADERTYPE_p bufIn);
    inline void count_frame(void);
    void check_output_format(void);
    void decode_frame(OMX_BUFFERHEADERTYPE_p bufIn, OMX_BUFFERHEADERTYPE_p bufOut);

    BS_STRUCT_T           mBSRead;
  
    // interface with decoder algo
    CODEC_INTERFACE_T     mInterfaceFE;
    bool                  mCodecInitialized;
    int                   mSampleFreq;
    int                   mNbChannel;
    int                   mSampleSize;
    int                   mFrameNb;
  
    Port                 mPorts[2];
};

#endif // _wrapperhst.hpp_
