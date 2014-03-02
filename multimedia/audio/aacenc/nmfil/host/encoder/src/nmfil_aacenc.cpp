/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
  * This code is ST-Ericsson proprietary and confidential.
   * Any use of the code for whatever purpose is subject to
   * specific written permission of ST-Ericsson SA.
    */

//#include <cpp.hpp>
//#include <ee/api/ee.h>
//#include <stdlib.h>
//#include "ENS_WrapperLog.h"
//#include <stdio.h>
//#include <utils/Log.h>
//#define LOG_TAG "SATISHENC"


#include <aacenc/nmfil/host/encoder.nmf>
#include <armnmf_dbc.h>


/////////////////////////////////////////////////////////////////////

#if 0
t_uint16
METH(encodeFrame)(void *interface) {
        //t_uint16 numOutBytes;
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        //HEAAC_ENCODER_INFO_STRUCT_T * nmf_heaac_info_config_ptr;

        return heaac_encode_frame(interface_fe);
        //heaac_info_config.numOutBytes;
        //nmf_heaac_info_config_ptr =  (HEAAC_ENCODER_INFO_STRUCT_T *)(interface_fe->codec_info_local_struct);
        //heaac_info_config.numOutBytes  = nmf_heaac_info_config_ptr->numOutBytes;
        //return heaac_info_config.numOutBytes;
        //return RETURN_STATUS_OK;
}


t_uint16
METH(encodeFrame)(void *interface) {
        int         status;
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        BS_STRUCT_T local_bs_struct, saved_bs_struct;
        unsigned int    local_bits_stream[ 385]; /*12288/32 word + 1*/
        t_uint16    framesize_bits;
        t_uint16    framesize_bytes;
        t_uint16    encoded_bit;
        SAMPLE_STRUCT_T  saved_sample_struct;



        if ( heaac_config.eAacEncProfile > HEAAC_PROFILE_AACLC)
        {   /* Dealing with HE and HE-SBR profile*/
                status =  heaac_encode_frame(interface_fe);
        }
        else
        {  /* Dealing with LC AAC profile*/
                /* Wrapper provide sample for 2 frames simultaneouly*/
                saved_bs_struct = (interface_fe->stream_struct).bits_struct;
                saved_sample_struct  = interface_fe->sample_struct;


                /* first frame handling */
                local_bs_struct.buf_add = local_bits_stream;
                local_bs_struct.buf_end = &local_bits_stream[384];
                local_bs_struct.pos_ptr = local_bits_stream;
                local_bs_struct.buf_size=385;
                local_bs_struct.nbit_available=32;
                local_bs_struct.word_size=32;

                (interface_fe->stream_struct).bits_struct = local_bs_struct;
                status = heaac_encode_frame(interface_fe);
                if (status != RETURN_STATUS_OK) {
                        return status;
                }
                framesize_bits = (interface_fe->stream_struct).real_size_frame_in_bit;
                encoded_bit = framesize_bits;
                //bs_fast_move_bit_arm(&local_bs_struct, framesize_bits);
                local_bs_struct.pos_ptr = local_bits_stream;
                bs_copy_bit_arm( &local_bs_struct ,&saved_bs_struct, framesize_bits);



                /* do for sencond frame*/
                local_bs_struct.buf_add = local_bits_stream;
                local_bs_struct.buf_end = &local_bits_stream[384];
                local_bs_struct.pos_ptr = local_bits_stream;
                local_bs_struct.buf_size=385;
                local_bs_struct.nbit_available=32;
                local_bs_struct.word_size=32;

                interface_fe->sample_struct = saved_sample_struct;
                (interface_fe->sample_struct).buf_add = (interface_fe->sample_struct).buf_add + AAC_SAMPLE_FRAME_SIZE_IN_WORD;
                (interface_fe->stream_struct).bits_struct = local_bs_struct;
                status = heaac_encode_frame(interface_fe);
                if (status != RETURN_STATUS_OK) {
                        return status;
                }
                framesize_bits = (interface_fe->stream_struct).real_size_frame_in_bit;
                encoded_bit += framesize_bits;
                //bs_fast_move_bit_arm(&local_bs_struct, framesize_bits);
                local_bs_struct.pos_ptr = local_bits_stream;


                bs_copy_bit_arm(&local_bs_struct ,&saved_bs_struct, framesize_bits);

                /* update the total encoded bit in "real_size_frame_in_bit" used at wrapper level*/
                (interface_fe->stream_struct).real_size_frame_in_bit = encoded_bit;

        }

        return status;

}

#endif


#ifndef OSI_TEST


extern const Word32 heaac_eaacenc_sampleRates[] ;

t_uint16
METH(encodeFrame)(void *interface) {
        int         status;
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        BS_STRUCT_T local_bs_struct, saved_bs_struct;
        t_uint16    framesize_bits, ifSbrProfile, ifMonoProfile, SamplesPerFrame, TotalNoOfFrameInInputBlockData;
        t_uint16    CurrentFrameNo;
        t_uint16    encoded_bit;
        int i;
        t_uint32    object_type, channel_nb, sample_freq,sampleRateIndex;

        if( interface_fe->codec_state.first_time  == true){

                if (heaac_config.eAacEncBsFormat== NMF_HEAAC_BSFORMAT_RAW)
                {
                        interface_fe->codec_state.mode = AAC_RAW;
                }

                if(heaac_config.eAacEncProfile == NMF_HEAAC_PROFILE_AACLC)
                {
                        object_type =0x2;
                }
                else if(heaac_config.eAacEncProfile == NMF_HEAAC_PROFILE_HE)
                {
                        object_type =0x5;
                }
                else if(heaac_config.eAacEncProfile == NMF_HEAAC_PROFILE_HE_PS)
                {
                        object_type =29;
                }
                else
                {
                        // return OMX_ErrorBadParameter;  // to be updated once the parameter is supported by AFM
                        return -1;
                }


                channel_nb= heaac_config.bEncodeMono?1 : heaac_config.nChannels;

                sampleRateIndex=0;
                while(heaac_eaacenc_sampleRates[sampleRateIndex] > heaac_config.sampleRate)
                        sampleRateIndex++;
                if(heaac_eaacenc_sampleRates[sampleRateIndex] == -1)
                        // return(OMX_ErrorBadParameter); // to be updated once the parameter is supported by AFM
                        return( -1 );

                bs_write_bit_32b(&interface_fe->stream_struct.bits_struct,object_type, 5);
                bs_write_bit_32b(&interface_fe->stream_struct.bits_struct,sampleRateIndex, 4);
                bs_write_bit_32b(&interface_fe->stream_struct.bits_struct,channel_nb, 4);
                bs_write_bit_32b(&interface_fe->stream_struct.bits_struct,0x0, 3);
                //bs_write_bit_32b(&interface_fe->stream_struct.bits_struct,0xbb, 8);
                //bs_write_bit_32b(&interface_fe->stream_struct.bits_struct,0xaa, 8);


                (interface_fe->stream_struct).real_size_frame_in_bit =16;

                return RETURN_STATUS_OK;

        }
        else
        {

                status = heaac_encode_frame(interface_fe);
                if (status != RETURN_STATUS_OK) {
                        return status;
                }

        }

        return status;

}

#else  /*OSI_TEST*/

static unsigned int    local_bits_stream[ 385]; /*12288/32 word + 1*/

t_uint16
METH(encodeFrame)(void *interface) {
        int         status;
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        BS_STRUCT_T local_bs_struct, saved_bs_struct;
        t_uint16    framesize_bits, ifSbrProfile, ifMonoProfile, SamplesPerFrame, TotalNoOfFrameInInputBlockData;
        t_uint16    CurrentFrameNo;
        t_uint16    encoded_bit;




        /* Dealing with LC AAC profile*/
        /* Wrapper provide sample for 2 frames simultaneouly*/
        saved_bs_struct = (interface_fe->stream_struct).bits_struct;

        ifSbrProfile = ( heaac_config.eAacEncProfile > HEAAC_PROFILE_AACLC)? 1 : 2;
        ifMonoProfile = ( heaac_config.nChannels == 1) ? 2 : 1;
        TotalNoOfFrameInInputBlockData = ifSbrProfile * ifMonoProfile;
        SamplesPerFrame = getMaxSamples()/TotalNoOfFrameInInputBlockData;
        encoded_bit = 0;

        for( CurrentFrameNo =0 ; CurrentFrameNo < TotalNoOfFrameInInputBlockData; CurrentFrameNo++)
        {
                /* reset local struct */
                local_bs_struct.buf_add = local_bits_stream;
                local_bs_struct.buf_end = &local_bits_stream[384];
                local_bs_struct.pos_ptr = local_bits_stream;
                local_bs_struct.buf_size=385;
                local_bs_struct.nbit_available=32;
                local_bs_struct.word_size=32;

                (interface_fe->stream_struct).bits_struct = local_bs_struct;
                status = heaac_encode_frame(interface_fe);
                if (status != RETURN_STATUS_OK) {
                        return status;
                }
                framesize_bits = (interface_fe->stream_struct).real_size_frame_in_bit;
                encoded_bit += framesize_bits;
                local_bs_struct.pos_ptr = local_bits_stream;
                bs_copy_bit_arm( &local_bs_struct ,&saved_bs_struct, framesize_bits);
                /* update the pointer*/
                (interface_fe->sample_struct).buf_add = (interface_fe->sample_struct).buf_add + SamplesPerFrame;

        }


        /* update the total encoded bit in "real_size_frame_in_bit" used at wrapper level*/
        (interface_fe->stream_struct).real_size_frame_in_bit = encoded_bit;

        return status;

}
#endif /*OSI_TEST*/


void
METH(setParameter)(AacEncParam_t params) {
        heaac_config.bitRate               = params.nBitRate;
        heaac_config.sampleRate             = params.sampleRate;
        heaac_config.nChannels                      = params.nChannels;
        heaac_config.nBits                                                = params.nBits;      // need to be 16 bit
        heaac_config.eAacEncProfile   = (HEAAC_PROFILE)params.eAacEncProfile;
        heaac_config.eAacEncBsFormat  = (HEAAC_BS_FORMAT)params.eAacEncBsFormat;
        heaac_config.bEncodeMono                        = params.bEncodeMono;

}
/*
void
METH(setConfig)(AacEncConfig_t config) {
//heaac_config.bitRate   = config.nBitRate;
heaac_config.bitRate   = 128000;
}
*/

void
METH(open)(void *interface) {
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        int status;

        interface_fe->codec_config_local_struct = &heaac_config;
        interface_fe->codec_info_local_struct   = &heaac_info_config;
        interface_fe->codec_local_struct = NULL;

#ifndef OSI_TEST
        heaac_config.nChannels=2;
#endif

        status = heaac_open(interface_fe);
        ARMNMF_DBC_POSTCONDITION(status == INIT_OK);


}

void
METH(reset)(void *interface) {
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        heaac_reset(interface_fe);
        /*Reset should set the value, used by wrapper to set valuse of mByteBufSize */
        /*need to be check with final version*/
        interface_fe->sample_struct.block_len = getMaxSamples();
        interface_fe->sample_struct.chans_nb = EAACPLUS_MAX_CHANNELS;
}

t_uint16
METH(getMaxFrameSize)(void) {
        //return AAC_MAX_BITSTREAM_SIZE_IN_BIT;
        return 24576;    // 12288*2
}

t_uint16
METH(getMaxSamples)(void) {
#ifndef OSI_TEST
        return AAC_SAMPLE_FRAME_SIZE_IN_WORD;
#else
        return  EAACPLUS_SAMPLE_FRAME_SIZE_IN_WORD;
#endif
}

t_uint16
METH(getSampleBitSize)(void) {
        return AAC_BITS_PER_SAMPLE;
}

t_uint32
METH(close)(void *interface) {
        CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
        heaac_close( interface_fe );
        return RETURN_STATUS_OK;
}


