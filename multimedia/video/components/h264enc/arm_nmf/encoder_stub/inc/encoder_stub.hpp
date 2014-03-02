/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ENCODER_STUB_HPP
#define __ENCODER_STUB_HPP


class h264enc_arm_nmf_encoder_stub: public h264enc_arm_nmf_encoder_stubTemplate
{
    /* globals */
    
    /* methods private */
    
    /*constructor*/
public:
  //  h264enc_arm_nmf_encoder();
    
    /* methods itf */
public:
    virtual t_nmf_error construct(void);
    virtual void        stop(void);
    virtual void        destroy(void);
    virtual void        close(void);

    // Interfaces
    virtual void init(void *mem_ctxt);
    virtual void input(void *p_sp, t_frameinfo *p_params, t_uint32 nFlags);
    virtual void output_buffer(void *ptr,t_uint32 size16);
    virtual void get_headers(t_frameinfo* p_params, void* buffer1, t_uint16* size1, void* buffer2, t_uint16* size2, t_uint16 no_headers_stream);

private:
    int encode(unsigned char* input_buffer,unsigned char* output_buffer);

public:
    void    *vfm_memory_ctxt;

private:
    int mCounter;
    bool mInit;   // init has been called
    bool mReady;  // ready to encode (ie: output_buffer has been called and waiting for input to be called
    void *mpOutputBuffer;
};

#endif // __ENCODER_STUB_HPP

