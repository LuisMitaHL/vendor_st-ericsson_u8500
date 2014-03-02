/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Includes							       
 */

#define GBL_BPU_API
#include "bpu_api.h"
#include "mmdsp_api.h"
#include "mtf_api.h"

#ifdef __T1XHV_NMF_ENV
/* Include automatically built nmf file according to codec */
#include "local_nmf_codec.h"
#endif

/*****************************************************************************/
/**
 * \brief  Flush of the BPU at the end of a Video Packet
 * \author Maurizio Colombo
 * \param  bit_count2           :   number of bits in packer 2
 * \param  bit_count3           :   number of bits in packer 3
 * \param  bitstream_start_addr :   bitstream_start_address in encode task (used for cut B wa)
 * \param  offset               :   bitstream_offset in encode task (used for cut B wa)
 * \return flag to indicate if an overflow has  occured
 *  
 * Manage Flush of the BPU and verify if an overflow  has occured (Sum of bits 
 * from channel 2 & 3 > 8192).
 * Includes workaround for BPU regression on 8815B
 */
/*****************************************************************************/
t_uint16 bpu_flush_vp(t_uint16 bit_count2,t_uint16 bit_count3, t_ahb_address bitstream_start_addr, t_uint16 offset)
{
    t_uint16 overflow = 0; /* if sum of bit_count 2+3 > 8192 (VP max size) */
    t_uint16 align;		 /* Nb of bits to flush buffer of channel X 	   */
                     
    /* -- Verify if an overflow has occured -- */
    if((bit_count2+bit_count3)>8192) {
        overflow = 1;
    }


    /* -- Flush Channel 2 -- */
    if(bit_count2>0) {
        /* Compute Alignment of Bitstream to flush buffer */
        align = (16 - (bit_count2%16)) % 16;
        if(align>0) {
            /* Flush buffer  */
            BPU_FLUSH_HDR2(align,0);
        }
    }

    /* -- Flush Channel 3 -- */
    if(bit_count3>0) {
        /* Compute Alignment of Bitstream to flush buffer */
        align = (16 - (bit_count3%16)) % 16;
        if(align>0) {
            /* Flush buffer  */
            BPU_FLUSH_DATA(align,0);
        }
    }

	/* workaround of VI 5987 */
    if(bit_count2>0)
	  {
		/* Move buffer to main channel */
		BPU_MOVE_HDR2(bit_count2);
	  }
    if(bit_count3>0) 
	  {
        /* Move buffer to main channel */
        BPU_MOVE_DATA(bit_count3);
	  }
	
    /* Initialize SDRAM & buffers of channels 2 and 3. */
    BPU_INIT_HDR2_DATA();

    /* Return overflow. */
    return overflow;
}

/*****************************************************************************/
/**
 * \brief  Flush of the BPU 
 * \author RR
 * \param  bit_count2
 * \param  bit_count3
 * \return flag to indicate if an overflow has  occured
 *  
 * Manage Flush of the BPU and verify if an overflow  has occured (Sum of bits 
 * from channel 2 & 3 > 8192).
 *
 * Version adapted from previous version of bpu_flush_vp() 
 */
/*****************************************************************************/
t_uint16 bpu_flush_h264(t_uint16 bit_count2,t_uint16 bit_count3)
{
    t_uint16 overflow = 0; /* if sum of bit_count 2+3 > 8192 (VP max size) */
    t_uint16 align;		 /* Nb of bits to flush buffer of channel X 	   */
        
    /* -- Verify if an overflow has occured -- */
    if((bit_count2+bit_count3)>8192) {
        overflow = 1;
    }

    /* -- Flush Channel 2 -- */
    if(bit_count2>0) {
        /* Compute Alignment of Bitstream to flush buffer */
        align = (16 - (bit_count2%16)) % 16;
        if(align>0) {
            /* Flush buffer  */
            BPU_FLUSH_HDR2(align,0);
        }
    }

    /* -- Flush Channel 3 -- */
    if(bit_count3>0) {
        /* Compute Alignment of Bitstream to flush buffer */
        align = (16 - (bit_count3%16)) % 16;
        if(align>0) {
            /* Flush buffer  */
            BPU_FLUSH_DATA(align,0);
        }
    }

	/* workaround of VI 5987 */
    if(bit_count2>0)
	  {
		/* Move buffer to main channel */
		BPU_MOVE_HDR2(bit_count2);
	  }
    if(bit_count3>0) 
	  {
        /* Move buffer to main channel */
        BPU_MOVE_DATA(bit_count3);
	  }

    /* Initialize SDRAM & buffers of channels 2 and 3. */
    BPU_INIT_HDR2_DATA();

    /* Return overflow. */
    return overflow;
}

/*****************************************************************************/
/**
 * \brief  Flush of the BPU at the end of the frame
 * \author Philippe Rochette					
 * \param  previous_offset	Offset of preceding frame
 *		   (flushed=> not in number of bits. with BPU_GET_BIT_NUMBER_HDR1()
 * \return flag to indicate if an overflow has  occured
 *
 * Manage Flush of the BPU and verify if an overflow has occured (Sum of bits 
 * from channel 2 & 3 > 8192).
 * 
 */
/*****************************************************************************/
t_uint16 bpu_flush_frame(t_uint16 previous_offset)
{
    t_uint32 bit_count1;		  /* Nb of bits written into channel 1	*/
    t_uint16 align_burst;		  /* Nb of bits to align on a burst		*/
    t_uint16 offset;			  /* Nb of bits to align on a burst		*/

    /* -- Get Bit Number from channel -- */
    bit_count1 = BPU_GET_BIT_NUMBER_HDR1();
    bit_count1 += previous_offset;

    /* -- Flush Channel burst -- */
    /* Compute Alignment of Bitstream to flush buffer */
    offset      = bit_count1%128;
    align_burst = (128 - offset)%128;
    
    /* Flush buffer  */
    while(align_burst>= 16) 
    {
        BPU_FLUSH_HDR1(16,0);
        align_burst -= 16;
    }
    
    if(align_burst>0) {
        BPU_FLUSH_HDR1(align_burst,0);
    }

    /* Working with, but should not be useful. If a problem occurs uncomment. */
    /*     BPU_FLUSH_HDR1(align_burst,0); */

    /* Initialize SDRAM & buffers of channels 1. */
    BPU_INIT_HDR1();

    return offset;
}



/*****************************************************************************/
/**
 * \brief  Write 128-unaligned bits into packer1, read through MTF 
 * \author Maurizio Colombo
 * \param  offset_in_bits   : number of bits to be copied
 * \param  ahb_address      : the address where to read data
 * 
 */
/*****************************************************************************/

void bpu_align_offset(t_uint16 offset_in_bits, t_uint32 ahb_address)
{

  t_uint16 offset_in_16words, header_size;
  t_uint16 count;
  t_uint16 last_word;  
  t_uint16 a_temp_buffer[64]; /**<\brief To store Header read from MTF */
  t_uint16 cnt;

  offset_in_16words = offset_in_bits/16 + 1;
  
  for(cnt=0;cnt<offset_in_16words;cnt+=8) {
#ifdef __T1XHV_NMF_ENV
  iMtf.mtfReadTable(TRANSFERT_MTF_ONLY,
#else
  mtf_read_table(
#endif
				   ahb_address,
				   &a_temp_buffer[cnt],
				   8,
				   1,
				   MTF_SWAP_BYTE_NO_SWAP_WORD64);
	ahb_address+=16;
  } /* for */
  
  /** Write Header to BPU */
  header_size = offset_in_bits;
  count = 0;
  while(header_size!=0) {
	last_word = a_temp_buffer[count];
	if(header_size>=16) {
	  BPU_WRITE_CHANNEL_16(BPU_CHANNEL_HDR1,16,last_word);
	  header_size -= 16;
	} else {
	  last_word >>= 16 - header_size;
	  BPU_WRITE_CHANNEL_16(BPU_CHANNEL_HDR1,header_size,last_word);
	  header_size = 0;
	}
	count++;
  }
}

/* END of bpu_api.c */
