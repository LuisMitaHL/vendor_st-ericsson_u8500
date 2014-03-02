/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#if 0
#include "types.h"
typedef unsigned long long t_uint64;

#define SWAP(a) (((a&0xFF)<<24)|((a&0xFF00)<<8)|((a&0xFF0000)>>8)|((a&0xFF000000)>>24))
void PlaneToMBtiled(t_uint32 aWidth, t_uint32 aHeight,
    t_uint8 *p_im_in, t_uint8 *p_im_in_u, t_uint8 *p_im_in_v,
    t_uint8 *p_im_out)
{
	t_uint32 nb_mb_width = aWidth / 16;     /** Number of Macroblocs in width **/
	t_uint32 nb_mb_height = aHeight / 16;   /** Number of Macroblocs in height **/
	t_uint32 x, y, nb_of_block, index_column, index_burst;
	t_uint8 *p_mb_luma, *p_mb_chroma_U, *p_mb_chroma_V;
	t_uint8 *p_word_luma=0, *p_word_chroma_U, *p_word_chroma_V;
	t_uint8 *p_burst_luma, *p_burst_chroma_U, *p_burst_chroma_V;

	t_uint32 *p_current_buffer;
	t_uint32 word;
	t_uint32 luma_size = 0;
	luma_size = aWidth * aHeight;


	/* Pointers initialisations  */
	p_mb_luma = p_im_in;
	p_mb_chroma_U = p_im_in_u;
	p_mb_chroma_V = p_im_in_v;

	p_current_buffer = (t_uint32 *) p_im_out;

	/* Luma Convert */
	for (y = 0; y < nb_mb_height; y++) {
		for (x = 0; x < nb_mb_width; x++) {
			/* conversion of a macroblock */
			for (nb_of_block = 0; nb_of_block < 2; nb_of_block++) {
				switch (nb_of_block) {
				case 0:
					p_word_luma = p_mb_luma;
					break;
				case 1:
					p_word_luma = p_mb_luma + 8;
					break;
				}
				p_burst_luma = p_word_luma;

				for (index_burst = 0; index_burst < 16; index_burst++) {
					*(p_current_buffer++) = *(t_uint32 *) p_burst_luma;
					p_burst_luma += 4;
					*(p_current_buffer++) = *(t_uint32 *) p_burst_luma;
					p_burst_luma += (nb_mb_width * 16) - 4;
				}
			}					/*End of a macroblock of luma */
			p_mb_luma += 16;
		}
		p_mb_luma += nb_mb_width * 16 * 15;
	}							/* End of Luma Convert */

	p_current_buffer = (t_uint32 *) (p_im_out + luma_size);

	/* Chroma Conversion */
	for (y = 0; y < nb_mb_height; y++) {
		for (x = 0; x < nb_mb_width; x++) {

			/* ======== Burst Chroma ======== */
			p_word_chroma_U = p_mb_chroma_U;
			p_word_chroma_V = p_mb_chroma_V;
			for (index_column = 0; index_column < 2; index_column++) {

				p_burst_chroma_U = p_word_chroma_U;
				p_burst_chroma_V = p_word_chroma_V;

				for (index_burst = 0; index_burst < 8; index_burst++) {

					word = (*p_burst_chroma_U & 0xFF);
					word <<= 8;
					word |= (*p_burst_chroma_V & 0xFF);
					word <<= 8;
					word |= (*(p_burst_chroma_U + 1) & 0xFF);
					word <<= 8;
					word |= (*(p_burst_chroma_V + 1) & 0xFF);
					*(p_current_buffer++) = SWAP(word);
					word = (*(p_burst_chroma_U + 2) & 0xFF);
					word <<= 8;
					word |= (*(p_burst_chroma_V + 2) & 0xFF);
					word <<= 8;
					word |= (*(p_burst_chroma_U + 3) & 0xFF);
					word <<= 8;
					word |= (*(p_burst_chroma_V + 3) & 0xFF);
					*(p_current_buffer++) = SWAP(word);

					p_burst_chroma_U += (nb_mb_width * 16) >> 1;
					p_burst_chroma_V += (nb_mb_width * 16) >> 1;
				}
				p_word_chroma_U += 4;	/* Change column */
				p_word_chroma_V += 4;	/* Change column */
			}					/*End of a Macroblock */

			p_mb_chroma_U += 8;
			p_mb_chroma_V += 8;

		}

		p_mb_chroma_U += (nb_mb_width * 8) * 7;
		p_mb_chroma_V += (nb_mb_width * 8) * 7;
	}
}

void VAL_YuvMb64ToPlane( unsigned char *ap_mb_tiled_image ,
                         unsigned char *ap_plane_image    , 
                         unsigned int   a_width           , 
                         unsigned int   a_height          )
{
    int nb_mb_width  = a_width / 16;  /** Frame width  in Macroblocks **/
    int nb_mb_height = a_height / 16; /** Frame height in Macroblocks **/
// int i, index_char_in_word;
    int x, y, nb_of_block,index_column, index_2bytes_in_word, index_burst;
    t_uint8 *p_mb_luma;
    t_uint8 *p_mb_chroma_U;
    t_uint8 *p_mb_chroma_V;
    t_uint8 *p_word_luma;
    t_uint8 *p_word_chroma_U;
    t_uint8 *p_word_chroma_V;
    t_uint8 *p_burst_luma;
    t_uint64 *p_burst_luma64, *p_im_in64;
//   t_uint64 *p_burst_chroma_UV64;
//   t_uint32 *p_burst_chroma_U32, *p_burst_chroma_V32;
    t_uint8 *p_burst_chroma_U;
    t_uint8 *p_burst_chroma_V;
    t_uint8 *p_im_in;
    t_uint8 *p_im_out;
//    t_uint32 start_time, end_luma_time, end_time;

/* FOR DEBUG */
//    start_time = VAL_GetTimestamp();
    p_im_in  = ap_mb_tiled_image;
    p_im_out = ap_plane_image;

    /* pointers initializations */
    p_mb_luma     = p_im_out;
    p_mb_chroma_U = p_im_out + a_width * a_height;
    p_mb_chroma_V = p_mb_chroma_U + (a_width * a_height)/4;

    /* convert Luma */
    for (y = 0; y < nb_mb_height; y++)
    {
        for (x = 0 ; x < nb_mb_width; x++)
        {
            /* conversion of a macroblock */
            for (nb_of_block = 0 ; nb_of_block < 2 ; nb_of_block ++)
            {
                switch(nb_of_block)
                {
                case 0:
                    p_word_luma = p_mb_luma;
                    break;
                case 1:
                    p_word_luma = p_mb_luma+8;
                    break;
                }
                p_burst_luma = p_word_luma;
                /* parse half a macroblock of luma */
                for (index_burst = 0; index_burst < 16; index_burst++) 
                {
                    /* get current word !!! word for 8820 is a composed of 64 bits */
                    /* To avoid endianess pb we'll get 8 chars one by one to have a word */
/*                    for (index_char_in_word = 0 ; index_char_in_word < 8; index_char_in_word++)
                    {
                        *p_burst_luma = *p_im_in;
                        p_burst_luma++; 
                        p_im_in++;
                    }
                    p_burst_luma += (nb_mb_width*16 - 8);
*/
                    /* NEW IMPLEMENTATION : assume that endianness 
                     *                      is the same in input and output */
                    p_burst_luma64  = (t_uint64 *)p_burst_luma;
                    p_im_in64       = (t_uint64 *)p_im_in;
                    *p_burst_luma64 = *p_im_in64;
                    p_im_in      += 8;
                    p_burst_luma += nb_mb_width*16;
                    /* ------ */    
                }
            } /* End of processing for a luma macroblock */
            p_mb_luma += 16;
        } /* End of processing for a line of luma macroblocks */ 
        p_mb_luma += nb_mb_width*16*15;
    } /* end of luma conversion */

   /* FOR DEBUG */
//   end_luma_time = VAL_GetTimestamp();
    
    /* Interleaved Chroma conversion */
    for (y = 0; y < nb_mb_height; y++) 
    {
        for(x = 0; x < nb_mb_width; x++) 
        {
            /* ======== Burst Chroma ======== */
            p_word_chroma_U = p_mb_chroma_U;
            p_word_chroma_V = p_mb_chroma_V;
            for(index_column = 0; index_column < 2; index_column++) 
            {               
                p_burst_chroma_U = p_word_chroma_U;
                p_burst_chroma_V = p_word_chroma_V;
                for (index_burst = 0; index_burst < 8; index_burst++) 
                {
/* OLD IMPLEMENTATION : TO BE KEPT UNTIL CACHE IS ACTIVABLE */
                    for (index_2bytes_in_word = 0 ; index_2bytes_in_word < 4 ; index_2bytes_in_word ++)
                    {
                        *p_burst_chroma_U = *p_im_in;
                        p_burst_chroma_U++;
                        p_im_in++;
                        *p_burst_chroma_V = *p_im_in;
                        p_burst_chroma_V++;
                        p_im_in++;
                    }
                    /* End of a word (64 bits) */
                    p_burst_chroma_U += (((nb_mb_width*16)>>1)-4);
                    p_burst_chroma_V += (((nb_mb_width*16)>>1)-4);
/************************** OLD IMPLEMENTATION *******************************/

/* NEW IMPLEMENTATION : TO BE USED WHEN CACHE IS ACTIVABLE */
                    /* NEW IMPLEMENTATION : assume that endianness 
                     *                      is the same in input and output */
                    /* Read a 64-bit input data pointed at p_im_in */
//                    p_burst_chroma_UV64 = (t_uint64 *)p_im_in;
//                    glp_sw_yc_data->chroma_uv64.uv64 = *p_burst_chroma_UV64;
                    /* Reorder data */
//                    for (i = 0 ; i < 4 ; i++)
//                    {
//                       glp_sw_yc_data->chroma_u32.u[i] = glp_sw_yc_data->chroma_uv64.uv[2*i];
//                       glp_sw_yc_data->chroma_v32.v[i] = glp_sw_yc_data->chroma_uv64.uv[2*i+1];
//                    }
                    /* Output reordered data */
//                    p_burst_chroma_U32  = (t_uint32 *)p_burst_chroma_U;
//                    p_burst_chroma_V32  = (t_uint32 *)p_burst_chroma_V;
//                    *p_burst_chroma_U32 = glp_sw_yc_data->chroma_u32.u32;
//                    *p_burst_chroma_V32 = glp_sw_yc_data->chroma_v32.v32;                   
                    /* Update pointers */
//                    p_im_in += 8;
//                    p_burst_chroma_U += (nb_mb_width*16)>>1;
//                    p_burst_chroma_V += (nb_mb_width*16)>>1;
                }
                p_word_chroma_U += 4; /* Change column */
                p_word_chroma_V += 4; /* Change column */
            }/* End of a column */
            p_mb_chroma_U += 8;
            p_mb_chroma_V += 8;
        }/* End of a line of Chroma */
        p_mb_chroma_U += (nb_mb_width*8)*7;
        p_mb_chroma_V += (nb_mb_width*8)*7;
   }
   /* FOR DEBUG */
//   end_time = VAL_GetTimestamp();
//   VAL_Log( TEST_DEBUG, PRINT_ONLY_IN_LOGFILE,
//            "VAL_YuvMb64ToPlane total duration is %d tics (%d tics for Luma)", 
//            end_time-start_time, end_luma_time-start_time);
}
#endif
