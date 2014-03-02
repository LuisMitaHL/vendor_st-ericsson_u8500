/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifdef NMF_BUILD
#include <h264enc/arm_nmf/encoder_stub.nmf>
#endif

#include "../inc/h264_enc_inter_11.264.h"

#ifndef __SYMBIAN32__
//#define NMF_LOG(...) //comment this line to print the trace messages
#endif



t_nmf_error METH(construct)()
{
   mCounter = 0;
   mInit  = false;
   mReady = false;
   return NMF_OK;
}


void METH(stop)(void)
{

}

void METH(destroy)(void)
{

}

void METH(close)(void)
{

}

void METH(init)(void *mem_ctxt)
{
   mInit = true;
}

void METH(input)(void *p_sp, t_frameinfo *p_params, t_uint32 nFlags)
{
    if (!mReady)
        NMF_PANIC("encoder_stub[input]: not ready\n");

    mReady = false;

    int size = encode( (unsigned char*)p_sp, (unsigned char*)mpOutputBuffer);
    
    notify_input_buffer.notify_input_buffer(p_sp, 0, VALID_OUT,0, 0, nFlags);
    notify_output_buffer.notify_output_buffer(mpOutputBuffer, 0, size, VALID_OUT, 0, 0, nFlags);
}

void METH(output_buffer)(void *ptr,t_uint32 size16)
{
    mpOutputBuffer = ptr;
    mReady = true;
}


int METH(encode)(unsigned char* input_buffer,unsigned char* output_buffer)
{

    NMF_LOG("=== encoding ===\n");

    int bufferoffset = frameoffset[mCounter][0];
    int buffersize = frameoffset[mCounter][1];
    
    for(int i=0; i<buffersize; i++) 
    {
        output_buffer[i] = stream[bufferoffset+i];
    }
    
    mCounter++;
    if (mCounter == NBFRAME)
        mCounter = (NBFRAME % 16);
        
    return buffersize;
}



void METH(get_headers)(t_frameinfo* p_params, void* buffer1, t_uint16* size1, void* buffer2, t_uint16* size2, t_uint16 no_headers_stream)
{
    *size1 = 0;
    *size2 = 0;
}


