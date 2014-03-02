/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmout.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _shmout_hpp_
#define _shmout_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_bindings_shmpcm_shmout : public Component, public hst_bindings_shmpcm_shmoutTemplate
{
    public:
        hst_bindings_shmpcm_shmout();

        //Component virtual functions
        virtual void process() ;
        virtual void reset() ;
        virtual void disablePortIndication(t_uint32 portIdx) ;
        virtual void enablePortIndication(t_uint32 portIdx) ;
        virtual void flushPortIndication(t_uint32 portIdx) ;

        virtual void fsmInit(fsmInit_t initFsm);
        virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
        virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
        virtual void processEvent(void)  			       { Component::processEvent() ; }
        virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
        virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
        virtual void setParameter(ShmPcmConfig_t config, void *buffer, t_uint16 input_blocksize);
        virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);

    private:
        typedef enum {INPUT_PORT, OUTPUT_PORT} portname;

        //void copy(t_sint16 *in, t_sint16 *out, long size);
        
        static void copy_in16b_out16b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in16b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in16b_out32b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in16b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in32b_out16b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in32b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in32b_out32b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static void copy_in32b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        
        void (*Shmout_copy_routine)         (t_sint16 *in, t_sint16 *out, unsigned int size);
        
        void ReturnInputBuffer();
        void ReturnOutputBuffer(OMX_BUFFERHEADERTYPE * bufOut);

#define MAX_NB_BUFFERS 4

        ShmPcmConfig_t          mShmConfig;
        OMX_BUFFERHEADERTYPE  * mBufIn;
        Port                    mPorts[2];
        t_uint16                mDataConsumed;
        bool                    bInputBufJustArrived;
        OMX_S64                 mInitialTS;  
        OMX_S64                 mNbSamplesForTSComputation;
        bool                    bPropagateTS;
        // store any fillthisbuffer received before fsminit
        // This can happen in some specific case
        bool                    bFsmInitialized;
        OMX_BUFFERHEADERTYPE  * mPendingBuffer[MAX_NB_BUFFERS];
};

#endif // _shmout_hpp_
