/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/decoder.nmf>
#endif
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h" /* for low level parsing functions */
#include "vc1d_ht_sequence_header_parser.h"

/*------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------*/


#if  0
#define PRINTF(value) if (1) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)
#endif


/****************************************************************************/
/**
 * \brief 	parse the sequence header in a rcv file
 * \author 	jean-marc volle
 * \param 	p_bitstream: pointer on bitstream struct
 * \param 	p_seq_param: pointer on sequence parameters to update
 * \note        See VC1 standard Annex L and J
 * \return 	 bitstream reading error code
 *  Return type changed For ER 344032 
 * 
 **/
/****************************************************************************/

OMX_ERRORTYPE  parse_sequence_header(tps_bitstream_buffer p_bitstream,
                            tps_sequence_parameters p_seq_param)
{
    t_uint16 read_bits;
  
    /* We are parsing the so called struct C */

    /* First 4 bits are the profile  */
    /* ----------------------------- */
    read_bits = arm_btst_getbits(p_bitstream,4);
    p_seq_param->profile = (t_vc1_profile) (read_bits >> 2); 
    if (read_bits != 0 && read_bits != 4)
    {
        printf("ERROR: unsupported profile:%u\n",read_bits);
        return OMX_ErrorFormatNotDetected ; // For ER 344032 
    }
    PRINTF(("\tPROFILE: %s\n",p_seq_param->profile==0 ? "SIMPLE":"MAIN")); 
   
                                                       
    /* Next 3 bits are FRMRTQ_POSTPROC */
    /* ------------------------------- */

    read_bits= arm_btst_getbits(p_bitstream,3);
    p_seq_param->q_framerate_for_postproc =read_bits ;

#if defined(PRINT_SEQ_HEADER_PARSER) || defined(PRINT_ALL)
    PRINTF(("\tFRMRTQ_POSTPROC: %d ",p_seq_param->q_framerate_for_postproc));
    if (p_seq_param->q_framerate_for_postproc == 7)
    {
        printf("(frame rate is around 30 frames/s)\n"); 
    }
    else
    {
        printf("(frame rate is around %d frames/s)\n",(2+p_seq_param->q_framerate_for_postproc*4)); 
    }   
#endif

    /* Next 5 bits are BITRTQ_POSTPROC */
    /* ------------------------------- */
    read_bits = arm_btst_getbits(p_bitstream,5);
    p_seq_param->q_bitrate_for_postproc = read_bits ;


#if defined(PRINT_SEQ_HEADER_PARSER) || defined(PRINT_ALL)
    PRINTF(("\tBITRTQ_POSTPROC: %d ",p_seq_param->q_bitrate_for_postproc));
    if (p_seq_param->q_bitrate_for_postproc == 31)
    {
        printf("(bit rate is around 2016 kbps or more)\n"); 
    }
    else
    {
        printf("(bit rate is around %d kbps/s)\n",(32+p_seq_param->q_bitrate_for_postproc*64)); 
    }   
#endif
    
    /* Next bit is LOOPFILTER */
    /* ---------------------- */
    read_bits= arm_btst_getbits(p_bitstream,1);
    p_seq_param->loop_filter_enabled = read_bits ;
    PRINTF(("\tLOOPFILTER: %d\n",p_seq_param->loop_filter_enabled));

    
    /* Next bit is reserved */
    /* -------------------- */
    arm_btst_flushbits(p_bitstream,1);

    /* Next bit is MULTIRES */
    /* -------------------- */

    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->multires_coding_enabled = read_bits;
    PRINTF(("\tMULTIRES: %d\n",p_seq_param->multires_coding_enabled));

    /* Next bit is reserved */
    /* -------------------- */
    arm_btst_flushbits(p_bitstream,1);

    /* Next bit is FASTUVMC */
    /* -------------------- */
    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->fast_uvmc_enabled = read_bits;
    PRINTF(("\tFASTUVMC: %d\n",p_seq_param->fast_uvmc_enabled));
    
    /* Next bit is EXTENDED_MV */
    /* ----------------------- */
    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->extended_mv_enabled = read_bits;
    PRINTF(("\tEXTENDED_MV: %d\n",p_seq_param->extended_mv_enabled));
    
    /* Next 2 bits are DQUANT */
    /* ----------------------- */
    read_bits = arm_btst_getbits(p_bitstream,2);
    p_seq_param->dquant = read_bits;
    PRINTF(("\tDQUANT: %d\n",p_seq_param->dquant));

    /* Next  bit is VSTRANSFORM */
    /* ------------------------ */
    read_bits =arm_btst_getbits(p_bitstream,1);
    p_seq_param->variable_size_transform_enabled = read_bits;
    PRINTF(("\tVSTRANSFORM: %d\n",p_seq_param->variable_size_transform_enabled));

    /* Next  bit is reserved */
    /* --------------------- */
    arm_btst_flushbits(p_bitstream,1);

    /* Next  bit is OVERLAP */
    /* -------------------- */
    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->overlap_transform_enabled = read_bits;
    PRINTF(("\tOVERLAP: %d\n",p_seq_param->overlap_transform_enabled));

    /* Next  bit is SYNCMARKER */
    /* ----------------------- */
    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->syncmarker_enabled = read_bits;
    PRINTF(("\tSYNCMARKER: %d\n",p_seq_param->syncmarker_enabled));

    /* Next  bit is RANGERED */
    /* --------------------- */
    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->rangered_enabled = read_bits;
    PRINTF(("\tRANGERED: %d\n",p_seq_param->rangered_enabled));

    /* Next 3 bits are MAXBFRAMES */
    /* -------------------------- */
    read_bits = arm_btst_getbits(p_bitstream,3);
    p_seq_param->max_b_frames = read_bits;
    PRINTF(("\tMAXBFRAMES: %d\n",p_seq_param->max_b_frames));
    
    /* Next 2 bits are QUANTIZER */
    /* ------------------------- */
    read_bits = arm_btst_getbits(p_bitstream,2);
    p_seq_param->quantizer = (t_vc1_quantizer)read_bits;
    PRINTF(("\tQUANTIZER: %d\n",p_seq_param->quantizer));

    /* Next  bit is FINTERPFLAG */
    /* ------------------------ */
    read_bits = arm_btst_getbits(p_bitstream,1);
    p_seq_param->frame_interpolation_enabled = read_bits;
    PRINTF(("\tFINTERPFLAG: %d\n",p_seq_param->frame_interpolation_enabled));
    
    /* Last  bit is reserved */
    /* --------------------- */
    arm_btst_flushbits(p_bitstream,1);
    return OMX_ErrorNone; //For ER 344032 
}


