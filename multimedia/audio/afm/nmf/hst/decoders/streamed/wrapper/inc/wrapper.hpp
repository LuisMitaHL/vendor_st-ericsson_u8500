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
#ifndef _decoder_wrapper_streamed_hpp_
#define _decoder_wrapper_streamed_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_decoders_streamed_wrapper : public Component, public hst_decoders_streamed_wrapperTemplate
{
public:

    //Component virtual functions
    hst_decoders_streamed_wrapper(void);  
 
    // NMF life-cycle methods...
    virtual void stop(void);

    virtual void process(void) ;
    virtual void reset(void) ;
    virtual void disablePortIndication(t_uint32 portIdx) ;
    virtual void enablePortIndication(t_uint32 portIdx) ;
    virtual void flushPortIndication(t_uint32 portIdx) ;

    virtual void fsmInit(fsmInit_t initFsm);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
    virtual void processEvent(void)  		               { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBufferCheck(INPUT_PORT, buffer); } 
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)   { Component::deliverBufferCheck(OUTPUT_PORT, buffer); }

    virtual void setParameter(void *buffer_in, t_uint32 size_in, void *buffer_out, t_uint32 size_out,  t_uint32 payLoad);

private:

    typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
    void framebuffer_reset(void);
    int read_array_and_bitstream(unsigned int num1, unsigned int num2, unsigned int arrayIndex);
    void asf_read_payload_header(void);
    void asf_reformatted_payload(unsigned int nbits_to_write);
    void framebuffer_write(OMX_BUFFERHEADERTYPE_p buffer);
    void framebuffer_consume(unsigned int nbits);
    bool find_first_sync_word(void);
    bool check_next_sync_word(bool *not_enough_data);
    bool parse_header_first_frame(void);
    bool parse_header_at_each_frame(void);
    bool parse_no_header_varying_size(void);
    bool parse_header_packet(void);
    bool parse_one_frame(void);
    bool parse_framealigned_data(void);
    void count_frame(void);
    void wrapper_reset(void);
    void copy_output_buffer_to_buffer(OMX_BUFFERHEADERTYPE_p targetBuf);
    void process_input_buffer(OMX_BUFFERHEADERTYPE_p bufIn);
    void check_output_format(void);
    void decode_frame();

    // interface with decoder algo
    CODEC_INTERFACE_T    mInterfaceFE;
    t_sample_freq        mSampleFreq;
    int                  mNbChannel;
    int                  mSampleSize;

    ////////////////////////////////////////
    BS_STRUCT_T         mBSWrite;
    BS_STRUCT_T         mBSRead;
    BS_STRUCT_T         mBSInput;

    OMX_BUFFERHEADERTYPE mBuffer_out;

    unsigned int        mAvailableRoom;
    unsigned int        mAvailableBits;
    unsigned int        mInputBufBits;
    int                 mFrameSizeBit;
    int                 mFrameNb;
    bool                mFrameReady;
    bool                mEosReceived;
    bool                bInputBufJustArrived;
    bool                bOutputBufferFilled;
    bool                bStopEmittingBuffers;

    unsigned int        mPayloadDataBitsRead;
    unsigned int        mPayloadDataBitsLeft;
    unsigned int        mPayloadHeaderBitsRead;
    unsigned int        mPartialHeaderArray[40];
    unsigned int        mPartialHeaderByteCount;
    bool                bPartialAsfHeaderPresent;
    Port                mPorts[2];
    unsigned int        mPayLoadPresent;
    bool                mPropagateTS;
};

#endif // _decoder_wrapper_streamed_hpp_
