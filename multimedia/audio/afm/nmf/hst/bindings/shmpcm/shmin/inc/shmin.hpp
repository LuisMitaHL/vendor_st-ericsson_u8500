/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmin.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _shmpcm_shmin_hpp_
#define _shmpcm_shmin_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_bindings_shmpcm_shmin : public Component, public hst_bindings_shmpcm_shminTemplate
{

    public:

        hst_bindings_shmpcm_shmin();

        //Component virtual functions
        virtual void process() ;
        virtual void reset() ;
        virtual void disablePortIndication(t_uint32 portIdx) ;
        virtual void enablePortIndication(t_uint32 portIdx) ;
        virtual void flushPortIndication(t_uint32 portIdx) ;

        virtual void fsmInit(fsmInit_t init);
        virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
        virtual void setParameter(ShmPcmConfig_t config ,
                void *buffer, 
                t_uint16 input_blocksize);

        virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
        virtual void processEvent(void)  			       { Component::processEvent() ; }
        virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
        virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

    private:
        typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
        
        //int copy(t_sint16 *in, t_sint16 *out, int size);
        //int copy_downmix(t_sint16 *in, t_sint16 *out, int size);
        //int copy_upmix(t_sint16 *in, t_sint16 *out, int size);
        
        static int copy_downmix_in16b_out16b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in16b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in16b_out32b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in16b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in32b_out16b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in32b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in32b_out32b_swap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_downmix_in32b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in16b_out16b_swap    (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in16b_out16b_noswap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in16b_out32b_swap    (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in16b_out32b_noswap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in32b_out16b_swap    (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in32b_out16b_noswap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in32b_out32b_swap    (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_upmix_in32b_out32b_noswap  (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in16b_out16b_swap          (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in16b_out16b_noswap        (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in16b_out32b_swap          (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in16b_out32b_noswap        (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in32b_out16b_swap          (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in32b_out16b_noswap        (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in32b_out32b_swap          (t_sint16 *in, t_sint16 *out, unsigned int size);
        static int copy_in32b_out32b_noswap        (t_sint16 *in, t_sint16 *out, unsigned int size);

        int (*Shmin_copy_downmix_routine)          (t_sint16 *in, t_sint16 *out, unsigned int size);
        int (*Shmin_copy_upmix_routine)            (t_sint16 *in, t_sint16 *out, unsigned int size);
        int (*Shmin_copy_routine)                  (t_sint16 *in, t_sint16 *out, unsigned int size);
        
        void ReturnInputBuffer();
        void ReturnOutputBuffer(OMX_BUFFERHEADERTYPE * bufOut);
        void processDownMix();
        void processUpMix();
        void updateTimestamp(OMX_BUFFERHEADERTYPE* bufOut, ShmPcmConfig_t ShmConfig, OMX_S64 initial, OMX_S64 * nbSamples);
        void fillBufWithZeros(OMX_BUFFERHEADERTYPE*  buf);

#define MAX_NB_BUFFERS 4

        ShmPcmConfig_t          mShmConfig;
        OMX_BUFFERHEADERTYPE  * mBufOut;
        t_uint16                mDataConsumed;
        Port                    mPorts[2];
        bool                    mSendNewFormat;
        bool                    bInputBufJustArrived;
        OMX_S64                 mInitialTS;  
        OMX_S64                 mNbSamplesForTSComputation;
        bool                    bPropagateTS;
        bool                    mInputBufAlreadyReceived;

        // store any emptythisbuffer received before fsminit
        // This can happen in some specific case
        bool                    bFsmInitialized;
        OMX_BUFFERHEADERTYPE  * mPendingBuffer[MAX_NB_BUFFERS];
};

#endif // _shmpcm_shmin_hpp_
