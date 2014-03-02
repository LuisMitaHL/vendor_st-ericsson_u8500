/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ALGO_DBLK_HPP_
#define _ALGO_DBLK_HPP_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "t1xhv_common.idt"	
/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
#ifndef __ndk5500_a0__
typedef struct 
{
  t_uint8*  Y_p;           // Luminance plane 
  t_uint8*  Cb_p;          // Cb plane        
  t_uint8*  Cr_p;          // Cr plane        
  t_uint16   Y_LineWidth;   // Distance to next row
  t_uint16    C_LineWidth;  
  t_uint16    Y_DispWidth;   // The number pixels on a line to display
  t_uint16    C_DispWidth;  
  t_uint16    Y_Height;
  t_uint16    C_Height;
  //boolean ReadOnly;      // Indication that the buffer is used as a reference and should not be written to
} YUV_Image_dblk;
#endif //#ifndef __ndk5500_a0__

typedef struct
{
  const t_uint8* QP_p;                /** Quantization array, one value for each macro block */
  short PostFilterArrayBorderPixels_p[64];   /** Table for filtering 8x8 border pixels */
  short PostFilterArrayInternalPixels_p[39]; /** Table for filtering 8x8 internal pixels */
  bool MQ_Mode;
  t_uint32 Width;                              /** Width of picture, in pixels */
  t_uint32 Height;                             /** Height of picture, in pixels */
  unsigned int PostFilterLevel;       /** 0 (disable) to 4 */
  t_uint8* PixelRow_p;                         /** Temporary buffer */
  const t_uint8* Src_p;
  t_uint8* Dest_p;
  // derived state
  int     NrMB_Horizontal;

#ifndef __ndk5500_a0__
  YUV_Image_dblk Deblocked_Image;
  t_uint32 NrMB;
  //t_uint8* Coding_type_p; 
#endif //#ifndef __ndk5500_a0__

} PostFilter_State_t;


class mpeg4dec_arm_nmf_algo_deblock: public mpeg4dec_arm_nmf_algo_deblockTemplate
{
	PostFilter_State_t* State_p;
	PostFilter_State_t State;
 // 'constructor' provided interface
  public:
    virtual t_nmf_error construct(void);
  

  // 'iAlgo' provided interface

    virtual void configureAlgo(t_uint32 Width,
						 t_uint32 Height,
						 t_uint32 PostFilterLevel,
						 t_uint32 MQ_Mode,
						 t_uint32 QP_p,
						 t_uint32 Input_p,
						 t_uint32 Output_p,
						 t_uint32 Temp_p);
						 
	virtual void controlAlgo(t_t1xhv_command command,t_uint16 param);
    
        mpeg4dec_arm_nmf_algo_deblock(){ State_p = &State;}
};
#endif /* _ALGO_H_ */

