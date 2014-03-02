/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef __NMF
#include <mpeg4dec/arm_nmf/algo_deblock.nmf>
#include "t1xhv_common.idt"	
#include "string.h"

#ifndef __ndk5500_a0__
  #include "stdio.h"
  #include "videocodecProfile_three_Filter.h"
#endif

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
/********************************************************************************
* Static variables
*********************************************************************************/

static const short qp_offset_vector[32] = {85,45,45,35,35,25,25,25,25,25,
                                           25,25,25,25,20,20,20,20,20,20,
                                           20,20,15,15,10,10,5,5,5,0,0,0};



// Annex T table for Chrominance QUANT
// NOTE this is also defined in vidSymbolDecH263
static const short ChrominanceQuantTableT[32] = { 0, 1, 2, 3, 4, 5, 6, 6, 7, 8,
                                                  9, 9,10,10,11,11,12,12,12,13,
                                                  13,13,14,14,14,14,14,15,15,15,
                                                  15,15 };

#define VID_PostFilterLevel_t unsigned int 
#define NRTAPSDIV2 2
#define DIVISORSHIFT 8
#define ROUNDVAL 128

#define MAX_INDEX_IMDIFFVECTORDR 38
#define MAX_INDEX_IMDIFFVECTOR 63

#define MABS(a)      (((a) < 0) ? -(a) : (a))
#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#define MIN(a,b)     ((a) < (b) ? (a) : (b))

						  
/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/
/*************************************************************************
* Declarations of internal functions
*************************************************************************/

static void mp4d_VidPostFilterDefault(const PostFilter_State_t *State_p,
                                 const t_uint8 *SrcY_p,
                                 t_uint8 *DstY_p,
                                 const t_uint8 *SrcCb_p,
                                 t_uint8 *DstCb_p,
                                 const t_uint8 *SrcCr_p,
                                 t_uint8 *DstCr_p,
                                 t_uint8 *PixelRow_p,
                                 VID_PostFilterLevel_t PostFilterLevel);

static void mp4d_PostFilterLineVertically(const PostFilter_State_t* State_p,
                                     const t_uint8 *ImageSrc_p,
                                     t_uint8 *ImageDst_p,
                                     const int row,
                                     const int width);


static void mp4d_PostFilterLineVerticallyChroma(const PostFilter_State_t* State_p,
                                           const t_uint8 *ImageSrc_p,
                                           t_uint8 *ImageDst_p,
                                           const int row,
                                           const int width);


static void mp4d_PostFilterLineVerticallyInternalPels(const PostFilter_State_t *State_p,
                                                 const t_uint8 *ImageSrc_p,
                                                 t_uint8 *ImageDst_p,
                                                 const int row,
                                                 const int width,
                                                 const int mb_size);


static void mp4d_PostFilterLineHorizontally(const PostFilter_State_t *State_p,
                                       const t_uint8 *ImageSrc_p,
                                       t_uint8 *ImageDst_p,
                                       const int row,
                                       const int width);


static void mp4d_PostFilterLineHorizontally2Pels(const PostFilter_State_t *State_p,
                                            const t_uint8 *ImageSrc_p,
                                            t_uint8 *ImageDst_p,
                                            const int row,
                                            const int width);


static void mp4d_PostFilterLineHorizontallyAllPels(const PostFilter_State_t *State_p,
                                              const t_uint8 *ImageSrc_p,
                                              t_uint8 *ImageDst_p,
                                              const int row,
                                              const int width,
                                              const int mb_size);

static void mp4d_VideoDecSetPostFilterStrength(PostFilter_State_t* State_p,
                              const int offset_border,
                              const int offset_internal);

static void mp4d_VideoDecSetPostFilter(PostFilter_State_t*   State_p,
                                  VID_PostFilterLevel_t PostFilterLevel);
/*------------------------------------------------------------------------
Methods of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  init
 *
 * Init jpeg Decoder Algo component. Implicitly called while instanciating
 *   this component.
 *   Unmask wanted internal interrupts.
 */
/*****************************************************************************/


t_nmf_error mpeg4dec_arm_nmf_algo_deblock::construct()
{
	return NMF_OK;
}

/*****************************************************************************/
/**
 * \brief  controlAlgo
 *
 * VP6 Decoder : Control Algo component.
 *
 * \param   sCommand
 * \param   param (n/a)
  */
/*****************************************************************************/
void mpeg4dec_arm_nmf_algo_deblock::controlAlgo(t_t1xhv_command command,t_uint16 param)
{
	t_uint32 Pixels = State_p->Width * State_p->Height;

	const t_uint8* SrcY_p  = State_p->Src_p;
	const t_uint8* SrcCb_p = ((const t_uint8* ) State_p->Src_p) + Pixels;
	const t_uint8* SrcCr_p = ((const t_uint8* ) State_p->Src_p) + Pixels + Pixels/4;

	t_uint8* DstY_p        = State_p->Dest_p;
	t_uint8* DstCb_p       = ((t_uint8*) State_p->Dest_p) + Pixels;
	t_uint8* DstCr_p       = ((t_uint8* ) State_p->Dest_p) + Pixels + Pixels/4;
	
	
#ifdef _DEBUG
printf("Hi mpeg4dec_arm_nmf_algo::controlAlgo with command = %d\n",command);
#endif //ifdef _DEBUG
	

	switch (command)
	{
		case CMD_RESET:
		case CMD_ABORT:
					/* (Not yet implemented) */
		break;
    case CMD_START:
#ifndef __ndk5500_a0__
    #ifdef NEW_DBLK
        if(param==2)
        {
          mp4d_DeblockingFilter_P3(&(State_p->Deblocked_Image), State_p);
        }
        else if(param == 1)
        {
          if (State_p->PostFilterLevel != 0)
          {
            mp4d_VidPostFilterDefault(State_p,
            SrcY_p, DstY_p,
            SrcCb_p, DstCb_p,
            SrcCr_p, DstCr_p,
            State_p->PixelRow_p,
            State_p->PostFilterLevel);
          }
          else
          {
            memcpy(DstY_p,  SrcY_p,  Pixels);
            memcpy(DstCb_p, SrcCb_p, Pixels/4);
            memcpy(DstCr_p, SrcCr_p, Pixels/4);
          }
        }
  #else
  			if (State_p->PostFilterLevel != 0)
  			{
  				mp4d_VidPostFilterDefault(State_p,
  				SrcY_p, DstY_p,
  				SrcCb_p, DstCb_p,
  				SrcCr_p, DstCr_p,
  				State_p->PixelRow_p,
  				State_p->PostFilterLevel);
  			}
  			else
  			{
  				memcpy(DstY_p,  SrcY_p,  Pixels);
  				memcpy(DstCb_p, SrcCb_p, Pixels/4);
  				memcpy(DstCr_p, SrcCr_p, Pixels/4);
  			}
  #endif
#else //#ifndef __ndk5500_a0__
			if (State_p->PostFilterLevel != 0)
			{
				mp4d_VidPostFilterDefault(State_p,
				SrcY_p, DstY_p,
				SrcCb_p, DstCb_p,
				SrcCr_p, DstCr_p,
				State_p->PixelRow_p,
				State_p->PostFilterLevel);
			}
			else
			{
				memcpy(DstY_p,  SrcY_p,  Pixels);
				memcpy(DstCb_p, SrcCb_p, Pixels/4);
				memcpy(DstCr_p, SrcCr_p, Pixels/4);
			}
#endif //#ifndef __ndk5500_a0__
		break;
		default:
			
		break;
	}

#ifndef __ndk5500_a0__
	oEndAlgoDeblock.endAlgoDeblock(STATUS_JOB_COMPLETE,(t_t1xhv_decoder_info)0,0,0);
#else //#ifndef __ndk5500_a0__
	oEndAlgoDeblock.endAlgo(STATUS_JOB_COMPLETE,(t_t1xhv_decoder_info)0,0,0);
#endif //#ifndef __ndk5500_a0__
	
} /* end of controlAlgo() function */


void mpeg4dec_arm_nmf_algo_deblock::configureAlgo(t_uint32 Width,
						 t_uint32 Height,
						 t_uint32 PostFilterLevel,
						 t_uint32 MQ_Mode,
						 t_uint32 QP_p,
						 t_uint32 Input_p,
						 t_uint32 Output_p,
						 t_uint32 Temp_p)
{
  State_p->Width  = Width;
	State_p->Height = Height;
	State_p->NrMB_Horizontal = Width / 16;
	State_p->MQ_Mode = MQ_Mode;
	State_p->QP_p    = (t_uint8*)QP_p;
	State_p->Src_p = (t_uint8*)Input_p;
  State_p->Dest_p = (t_uint8*)Output_p;
	State_p->PixelRow_p = (t_uint8 *)Temp_p;

#ifndef __ndk5500_a0__
  #ifdef NEW_DBLK
    State_p->NrMB =  (Width >>4) * (Height >>4);
    memcpy((t_uint8*)Output_p,(t_uint8*)Input_p,((Width * Height * 3)/2));
    State_p->NrMB_Horizontal = Width / 16;
    //State_p->Coding_type_p = (t_uint8*)(QP_p + State_p->NrMB);
  
    State_p->Deblocked_Image.Y_LineWidth = Width;   // Distance to next row
    State_p->Deblocked_Image.Y_DispWidth = Width;   // The number pixels on a line to display
    State_p->Deblocked_Image.Y_Height    = Height;
  
    State_p->Deblocked_Image.C_LineWidth = Width / 2;
    State_p->Deblocked_Image.C_DispWidth = Width/ 2;
    State_p->Deblocked_Image.C_Height    = Height / 2;
  
    State_p->Deblocked_Image.Y_p = (t_uint8*)Output_p;
    State_p->Deblocked_Image.Cb_p = State_p->Deblocked_Image.Y_p + (Width*Height);
    State_p->Deblocked_Image.Cr_p = State_p->Deblocked_Image.Cb_p + ((Width*Height)/4);
  #endif
#endif  //#ifndef __ndk5500_a0__

	mp4d_VideoDecSetPostFilter(State_p, PostFilterLevel);
	
	
} /* end of configureAlgo() function. */

/*************************************************************************
*
* Name:	        VideoDecSetPostFilter
*
* Parameters:		Decoder          [IN]
*               PostFilterType   [IN]
*               PostFilterLevel  [IN]
*
* Returns:
*
* Description:  This function updates the postfilter mode, if it is used
*
**************************************************************************/
static void mp4d_VideoDecSetPostFilter(PostFilter_State_t*   State_p,
                                  VID_PostFilterLevel_t PostFilterLevel)
{
  if (State_p->PostFilterLevel != PostFilterLevel)
  {
    switch (PostFilterLevel)
    {
      case 1:
      {
        mp4d_VideoDecSetPostFilterStrength(State_p,4,4);
        break;
      }
      case 2:
      {
        mp4d_VideoDecSetPostFilterStrength(State_p,4,4);
        break;
      }
      case 3:
      {
        mp4d_VideoDecSetPostFilterStrength(State_p,4,4);
        break;
      }
      case 4:
      {
        mp4d_VideoDecSetPostFilterStrength(State_p,4,4);
        break;
      }
    }

    State_p->PostFilterLevel = PostFilterLevel;
  }
}

/*************************************************************************
*
* Name:	        VideoDecSetPostFilterStrength
*
* Parameters:   Decoder         [IN]
*               offset_border   [IN]
*               offset_internal [IN]
*
* Returns:      void
*
* Description:	Sets postfilter strength
*
**************************************************************************/
static void mp4d_VideoDecSetPostFilterStrength(PostFilter_State_t*   State_p,
                                          const int offset_border,
                                          const int offset_internal)
{
  static const short start_i[10]  = {51, 51, 51, 37, 26, 21, 18, 16, 14, 1};
  static const short posadd[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  static const short negadd[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 0};

  static const short startDR[5]  = {51, 37, 28, 21, 1};
  static const short posaddDR[5] = {4, 3, 2, 1, 0};
  static const short negaddDR[5] = {1, 2, 3, 4, 0};

  short *ptr;
  int index;
  int val;

  ptr = State_p->PostFilterArrayBorderPixels_p;
  for(index = 0 ; index < 18 ; index++)
  {
    *ptr++ = 51;
  }

  if(offset_border > 0)
  {
    for(index = 0 ; index < 9 ; index++)
    {
      val = MAX(MIN(51,start_i[index]+posadd[index]*offset_border),1);
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
    }
  }
  else
  {
    for(index = 0 ; index < 9 ; index++)
    {
      val = MAX(MIN(51,start_i[index]+negadd[index]*offset_border),1);
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
    }
  }
  *ptr = 1;


  ptr = State_p->PostFilterArrayInternalPixels_p;
  for(index = 0 ; index < 18 ; index++)
  {
    *ptr++ = 51;
  }

  if(offset_internal>0)
  {
    for(index = 0 ; index < 4 ; index++)
    {
      val =  MAX(MIN(51,startDR[index]+posaddDR[index]*offset_internal),1);
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
    }
  }
  else
  {
    for(index = 0 ; index < 4 ; index++)
    {
      val = MAX(MIN(51,startDR[index]+negaddDR[index]*offset_internal),1);
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
      *ptr++ = val;
    }
  }
  *ptr = 1;
}

/*************************************************************************
*
* Name:         PostFilterLineVertically
*
* Parameters:   State_p     [IN]   Postfilter state
*               ImageSrc_p  [IN]   Pointer to source pixel line
*               ImageDst_p  [OUT]  Filtered pixel line is stored here
*               row         [IN]   What line number is filtered
*               width       [IN]   The width of the image
*
* Returns:      void
*
* Description:  Filters one line of luma pixels, stores the result in ImageDst_p.
*               First and last two pixels of the line are copied. The function
*               assumes that there are two lines of pixels both above and
*               below the line to be filtered. The filter used is a variant
*               of (1,1,-4,1,1)
*
**************************************************************************/
static void mp4d_PostFilterLineVertically(const PostFilter_State_t *State_p,
                                     const t_uint8 *ImageSrc_p,
                                     t_uint8 *ImageDst_p,
                                     const int row,
                                     const int width)
{
  int k;
  int kk;
  int sum;
  int imdiff;
  int imdifftmp;
  int mbY;
  int mbIndexstart;
  int QP_offset; /* quantization step for current MB */
  const int w2 = (width<<1);

  const short *FilterTableBorderPixels_p = State_p->PostFilterArrayBorderPixels_p;
  const t_uint8* QP_p;

  mbY = row >> 4;

  mbIndexstart = mbY*State_p->NrMB_Horizontal;//11;

  QP_p = &State_p->QP_p[mbIndexstart];

  QP_offset = qp_offset_vector[*QP_p];

  // Copy two leftmost pixels
  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;

  kk = 14;
  while(kk--)
  {
    imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
    imdiff = MIN(MABS(imdifftmp) + QP_offset, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }

  for (k=16; k<width-16;k+=16)
  {
    QP_p++;
    QP_offset = qp_offset_vector[*QP_p];
    kk = 16;
    while(kk--)
    {
      imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
      imdiff = MIN(MABS(imdifftmp) + QP_offset, MAX_INDEX_IMDIFFVECTOR);
      sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
      *ImageDst_p++ = *ImageSrc_p++ + sum;
    }
  }

  QP_p++;
  QP_offset = qp_offset_vector[*QP_p];

  kk = 14;
  while(kk--)
  {
    // Filter
    imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
    imdiff = MIN(MABS(imdifftmp) + QP_offset, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }

  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;
}

/*************************************************************************
*
* Name:         PostFilterLineVerticallyChroma
*
* Parameters:   State_p     [IN]   Postfilter state
*               ImageSrc_p  [IN]   Pointer to source pixel line
*               ImageDst_p  [OUT]  Filtered pixel line is stored here
*               row         [IN]   What line number is filtered
*               width       [IN]   The width of the image
*
* Returns:      void
*
* Description:  See PostFilterLineVertically. The difference is that a
*               chroma line is filtered instead of luma. Since images are
*               YUV a macroblock in for chroma is 8x8 instead of 16x16.
*
**************************************************************************/
static void mp4d_PostFilterLineVerticallyChroma(const PostFilter_State_t *State_p,
                                           const t_uint8 *ImageSrc_p,
                                           t_uint8 *ImageDst_p,
                                           const int row,
                                           const int width)
{
  int k;
  int kk;
  int sum;
  int imdiff;
  int imdifftmp;
  int mbY;
  int mbIndexstart;
  int QP_offset; /* quantization step for current MB */
  const int w2 = (width<<1);

  const short *FilterTableBorderPixels_p = State_p->PostFilterArrayBorderPixels_p;
  const t_uint8* QP_p;

  mbY = row >> 3;

  mbIndexstart = mbY*State_p->NrMB_Horizontal;//11;

  QP_p = &State_p->QP_p[mbIndexstart];

  if(State_p->MQ_Mode)
    QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
  else
    QP_offset = qp_offset_vector[*QP_p];

  // Copy two leftmost pixels
  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;

  kk=6;
  while(kk--)
  {
    imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }

  for (k=8; k<width-8;k+=8)
  {
    QP_p++;
    if(State_p->MQ_Mode)
      QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
    else
      QP_offset = qp_offset_vector[*QP_p];

    kk=8;
    while(kk--)
    {
      imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
      imdiff = MABS(imdifftmp) + QP_offset;
      imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
      sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
      *ImageDst_p++ = *ImageSrc_p++ + sum;
    }
  }


  QP_p++;
  if(State_p->MQ_Mode)
    QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
  else
    QP_offset = qp_offset_vector[*QP_p];
  kk=6;
  while(kk--)
  {
    imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }
  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;
}

/*************************************************************************
*
* Name:         PostFilterLineVerticallyInternalPels
*
* Parameters:   State_p     [IN]   Postfilter state
*               ImageSrc_p  [IN]   Pointer to source pixel line
*               ImageDst_p  [OUT]  Filtered pixel line is stored here
*               row         [IN]   What line number is filtered
*               width       [IN]   The width of the image
*               mb_size     [IN]   The size of a MB, luma->16, chroma->8
*
* Returns:      void
*
* Description:  See PostFilterLineVertically. The difference is that this
*               function filtes the pixels using a different table suitable
*               for pixels that does not lie on an 8x8 border. This function
*               can be used for both luma and chroma pixels.
*
**************************************************************************/
static void mp4d_PostFilterLineVerticallyInternalPels(const PostFilter_State_t *State_p,
                                                 const t_uint8 *ImageSrc_p,
                                                 t_uint8 *ImageDst_p,
                                                 const int row,
                                                 const int width,
                                                 const int mb_size)
{
  int k;
  int kk;
  int sum;
  int imdiff;
  int imdifftmp;
  int mbY;
  int mbIndexstart;
  int QP_offset; /* quantization step for current MB */
  const int w2 = (width<<1);

  const short *FilterTableInternalPixels_p = State_p->PostFilterArrayInternalPixels_p;
  const t_uint8* QP_p;

  int mask;

  mask = mb_size-1;
  if(mb_size == 16)
    mbY = row >> 4;
  else
    mbY = row >> 3;

  mbIndexstart = mbY*State_p->NrMB_Horizontal;//11;
  QP_p = &State_p->QP_p[mbIndexstart];

  if(mb_size == 8 && State_p->MQ_Mode)
    QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
  else
    QP_offset = qp_offset_vector[*QP_p];

  // Copy two leftmost pixels
  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;

  kk = 6;
  while(kk--)
  {
    imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdifftmp*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }
  for (k=8; k<width-8;k+=8)
  {
    // Check for new MB -> new QP
    if((k&mask) == 0)
    {
      QP_p++;
      if(mb_size == 8 && State_p->MQ_Mode)
        QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
      else
        QP_offset = qp_offset_vector[*QP_p];
    }

    kk = 8;
    while(kk--)
    {
      imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
      imdiff = MABS(imdifftmp) + QP_offset;
      imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
      sum = (imdifftmp*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
      *ImageDst_p++ = *ImageSrc_p++ + sum;
    }
  }
  if((k&mask) == 0)
  {
    QP_p++;
    if(mb_size == 8 && State_p->MQ_Mode)
      QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
    else
      QP_offset = qp_offset_vector[*QP_p];
  }
  kk = 6;
  while(kk--)
  {
    imdifftmp = *(ImageSrc_p-w2) + *(ImageSrc_p-width) - (*(ImageSrc_p)<<2) + *(ImageSrc_p+width) + *(ImageSrc_p+w2);
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdifftmp*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }

  // Copy two rightmost pixels
  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p   = *ImageSrc_p;
}

/*************************************************************************
*
* Name:         PostFilterLineHorizontally
*
* Parameters:   State_p     [IN]   Postfilter state
*               ImageSrc_p  [IN]   Pointer to source pixel line
*               ImageDst_p  [OUT]  Filtered pixel line is stored here
*               row         [IN]   What line number is filtered
*               width       [IN]   The width of the image
*
* Returns:      void
*
* Description:  Filters one line of luma pixels, stores the result in ImageDst_p.
*               Filters only the border pixels of 8x8 blocks, so every fourth
*               pixel is filtered, the rest are just copied. The filter used
*               is a variant of (1,1,-4,1,1)
*
**************************************************************************/
static void mp4d_PostFilterLineHorizontally(const PostFilter_State_t *State_p,
                                       const t_uint8 *ImageSrc_p,
                                       t_uint8 *ImageDst_p,
                                       const int row,
                                       const int width)
{
  int k;
  int sum;
  int imdiff;
  int imdifftmp;
  int mbY;
  int mbIndexstart;
  int QP_offset; /* quantization step for current MB */
  t_uint8 a,b,c,d,e,f;
  int imdiff2;

  const short *FilterTableBorderPixels_p = State_p->PostFilterArrayBorderPixels_p;
  const t_uint8* QP_p;

  mbY = row >> 4; // row/16

  mbIndexstart = mbY*State_p->NrMB_Horizontal;
  QP_p = &State_p->QP_p[mbIndexstart];

  QP_offset = qp_offset_vector[*QP_p];

  // Copy entire pixel line
  memcpy(ImageDst_p, ImageSrc_p, width);

  /////////////////////////////////////////
  // Filter first 8x8 border for the row //
  /////////////////////////////////////////
  ImageDst_p+=7;
  ImageSrc_p+=7;

  a = *(ImageSrc_p-2);
  b = *(ImageSrc_p-1);
  c = *(ImageSrc_p);
  d = *(ImageSrc_p+1);
  e = *(ImageSrc_p+2);
  f = *(ImageSrc_p+3);

  imdifftmp  = a+b-(c<<2)+d+e;
  imdiff2    = b+c-(d<<2)+e+f;

  imdiff = MABS(imdifftmp) + QP_offset;
  imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
  sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
  *ImageDst_p++ = *ImageSrc_p++ + sum;

  imdiff = MABS(imdiff2) + QP_offset;
  imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
  sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
  *ImageDst_p = *ImageSrc_p + sum;

  ImageDst_p+=7;
  ImageSrc_p+=7;

  /////////////////////////////////////////
  // Loop over the remaining macroblocks //
  /////////////////////////////////////////
  k = (width>>4) - 1;
  while(k--)
  {
    a = *(ImageSrc_p-2);
    b = *(ImageSrc_p-1);
    c = *(ImageSrc_p);
    d = *(ImageSrc_p+1);
    e = *(ImageSrc_p+2);
    f = *(ImageSrc_p+3);

    // Compute filter for two border pixels
    imdifftmp  = a+b-(c<<2)+d+e;
    imdiff2    = b+c-(d<<2)+e+f;

    // Filter first pixel
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    // Here we have a macroblock border
    QP_p++;
    QP_offset = qp_offset_vector[*QP_p];

    // Filter second pixel
    imdiff = MABS(imdiff2) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p = *ImageSrc_p + sum;

    ImageDst_p+=7;
    ImageSrc_p+=7;

    a = *(ImageSrc_p-2);
    b = *(ImageSrc_p-1);
    c = *(ImageSrc_p);
    d = *(ImageSrc_p+1);
    e = *(ImageSrc_p+2);
    f = *(ImageSrc_p+3);

    // Compute filter for two border pixels
    imdifftmp  = a+b-(c<<2)+d+e;
    imdiff2    = b+c-(d<<2)+e+f;

    // Filter first pixel
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    // Filter second pixel
    imdiff = MABS(imdiff2) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p = *ImageSrc_p + sum;

    ImageDst_p+=7;
    ImageSrc_p+=7;
  }
}

/*************************************************************************
*
* Name:         PostFilterLineHorizontally2Pels
*
* Parameters:   State_p     [IN]   Postfilter state
*               ImageSrc_p  [IN]   Pointer to source pixel line
*               ImageDst_p  [OUT]  Filtered pixel line is stored here
*               row         [IN]   What line number is filtered
*               width       [IN]   The width of the image
*
* Returns:      void
*
* Description:  See PostFilterLineHorizontally. The difference is that this
*               function filters two border pixels instead of one. This
*               means that 50% of the pixels are filtered, the rest are
*               copied.
*
**************************************************************************/
static void mp4d_PostFilterLineHorizontally2Pels(const PostFilter_State_t *State_p,
                                            const t_uint8 *ImageSrc_p,
                                            t_uint8 *ImageDst_p,
                                            const int row,
                                            const int width)
{
  int k;
  int sum;
  int imdiff;
  int imdifftmp;
  int mbY;
  int mbIndexstart;
  int QP_offset; /* quantization step for current MB */
  t_uint8 a,b,c,d,e,f,g,h;
  int imdiff2;
  int imdiff3;
  int imdiff4;

  const short *FilterTableBorderPixels_p = State_p->PostFilterArrayBorderPixels_p;
  const t_uint8* QP_p;

  mbY = row >> 4; // row/16

  mbIndexstart = mbY*State_p->NrMB_Horizontal;
  QP_p = &State_p->QP_p[mbIndexstart];

  QP_offset = qp_offset_vector[*QP_p];

  // Copy entire pixel line
  memcpy(ImageDst_p, ImageSrc_p, width);

  /////////////////////////////////////////
  // Filter first 8x8 border for the row //
  /////////////////////////////////////////
  ImageDst_p+=6;
  ImageSrc_p+=6;

  a = *(ImageSrc_p-2);
  b = *(ImageSrc_p-1);
  c = *(ImageSrc_p);
  d = *(ImageSrc_p+1);
  e = *(ImageSrc_p+2);
  f = *(ImageSrc_p+3);
  g = *(ImageSrc_p+4);
  h = *(ImageSrc_p+5);

  imdifftmp = a+b-(c<<2)+d+e;
  imdiff2   = b+c-(d<<2)+e+f;
  imdiff3   = c+d-(e<<2)+f+g;
  imdiff4   = d+e-(f<<2)+g+h;

  // Filter two border pixels
  imdiff = MABS(imdifftmp) + QP_offset;
  imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
  sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
  *ImageDst_p++ = *ImageSrc_p++ + sum;

  imdiff = MABS(imdiff2) + QP_offset;
  imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
  sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
  *ImageDst_p++ = *ImageSrc_p++ + sum;

  imdiff = MABS(imdiff3) + QP_offset;
  imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
  sum = (imdiff3*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
  *ImageDst_p++ = *ImageSrc_p++ + sum;

  imdiff = MABS(imdiff4) + QP_offset;
  imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
  sum = (imdiff4*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
  *ImageDst_p = *ImageSrc_p + sum;

  ImageDst_p+=5;
  ImageSrc_p+=5;

  /////////////////////////////////////////
  // Loop over the remaining macroblocks //
  /////////////////////////////////////////
  k = (width>>4) - 1;
  while(k--)
  {
    a = *(ImageSrc_p-2);
    b = *(ImageSrc_p-1);
    c = *(ImageSrc_p);
    d = *(ImageSrc_p+1);
    e = *(ImageSrc_p+2);
    f = *(ImageSrc_p+3);
    g = *(ImageSrc_p+4);
    h = *(ImageSrc_p+5);

    imdifftmp = a+b-(c<<2)+d+e;
    imdiff2   = b+c-(d<<2)+e+f;
    imdiff3   = c+d-(e<<2)+f+g;
    imdiff4   = d+e-(f<<2)+g+h;

    // Filter two border pixels
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff2) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    // Here we have a macroblock border
    QP_p++;
    QP_offset = qp_offset_vector[*QP_p];

    // Filter two border pixels
    imdiff = MABS(imdiff3) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff3*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff4) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff4*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p = *ImageSrc_p + sum;

    ImageDst_p+=5;
    ImageSrc_p+=5;

    a = *(ImageSrc_p-2);
    b = *(ImageSrc_p-1);
    c = *(ImageSrc_p);
    d = *(ImageSrc_p+1);
    e = *(ImageSrc_p+2);
    f = *(ImageSrc_p+3);
    g = *(ImageSrc_p+4);
    h = *(ImageSrc_p+5);

    imdifftmp = a+b-(c<<2)+d+e;
    imdiff2   = b+c-(d<<2)+e+f;
    imdiff3   = c+d-(e<<2)+f+g;
    imdiff4   = d+e-(f<<2)+g+h;

    // Filter four border pixels
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff2) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff3) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff3*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff4) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff4*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p = *ImageSrc_p + sum;

    ImageDst_p+=5;
    ImageSrc_p+=5;
  }
}

/*************************************************************************
*
* Name:         PostFilterLineHorizontallyAllPels
*
* Parameters:   State_p     [IN]   Postfilter state
*               ImageSrc_p  [IN]   Pointer to source pixel line
*               ImageDst_p  [OUT]  Filtered pixel line is stored here
*               row         [IN]   What line number is filtered
*               width       [IN]   The width of the image
*               mb_size     [IN]   The size of a MB, luma->16, chroma->8
*
* Returns:      void
*
* Description:  See PostFilterLineHorizontally. This function filters all
*               pixels, both border pixels and internal pixels to the 8x8
*               blocks.
*
**************************************************************************/
static void
mp4d_PostFilterLineHorizontallyAllPels(const PostFilter_State_t *State_p,
                                  const t_uint8 *ImageSrc_p,
                                  t_uint8 *ImageDst_p,
                                  const int row,
                                  const int width,
                                  const int mb_size)
{
  int kk;
  int sum;
  int imdiff;
  int imdifftmp;
  int mbY;
  int mbIndexstart;
  int QP_offset; /* quantization step for current MB */
  t_uint8 a,b,c,d,e,f,g,h,i,j,k,l;
  int imdiff2;
  int imdiff3;
  int imdiff4;
  int imdiff5;
  int imdiff6;
  int imdiff7;
  int imdiff8;
  int mask;

  const short *FilterTableBorderPixels_p = State_p->PostFilterArrayBorderPixels_p;
  const short *FilterTableInternalPixels_p = State_p->PostFilterArrayInternalPixels_p;
  const t_uint8* QP_p;

  if(mb_size == 16)
  {
    mask = 8;
    mbY = row >> 4;
  }
  else
  {
    mask = 0xff;
    mbY = row >> 3;
  }

  mbIndexstart = mbY*State_p->NrMB_Horizontal;
  QP_p = &State_p->QP_p[mbIndexstart];

  if(mb_size == 8 && State_p->MQ_Mode)
    QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
  else
    QP_offset = qp_offset_vector[*QP_p];

  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;

  // Filter first 4 internal pixels

  for(kk=0 ; kk<4 ; kk++)
  {
    imdifftmp = *(ImageSrc_p-2) +  *(ImageSrc_p-1) - (*ImageSrc_p << 2) + *(ImageSrc_p+1) + *(ImageSrc_p+2);
    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdifftmp*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }

  // Loop over the 8x8 border
  for(kk=7 ; kk<(width-1) ; kk+=8)
  {
    a = *(ImageSrc_p-2);
    b = *(ImageSrc_p-1);
    c = *(ImageSrc_p);
    d = *(ImageSrc_p+1);
    e = *(ImageSrc_p+2);
    f = *(ImageSrc_p+3);
    g = *(ImageSrc_p+4);
    h = *(ImageSrc_p+5);
    i = *(ImageSrc_p+6);
    j = *(ImageSrc_p+7);
    k = *(ImageSrc_p+8);
    l = *(ImageSrc_p+9);

    imdifftmp = a+b-(c<<2)+d+e;
    imdiff2   = b+c-(d<<2)+e+f;
    imdiff3   = c+d-(e<<2)+f+g;
    imdiff4   = d+e-(f<<2)+g+h;
    imdiff5   = e+f-(g<<2)+h+i;
    imdiff6   = f+g-(h<<2)+i+j;
    imdiff7   = g+h-(i<<2)+j+k;
    imdiff8   = h+i-(j<<2)+k+l;

    ///////////////////////////////
    // Filter four border pixels //
    ///////////////////////////////

    imdiff = MABS(imdifftmp) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdifftmp*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff2) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff2*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    // Check for macroblock border
    if(kk&mask)
    {
      QP_p++;
      if(mb_size == 8 && State_p->MQ_Mode)
        QP_offset = qp_offset_vector[ChrominanceQuantTableT[*QP_p]];
      else
        QP_offset = qp_offset_vector[*QP_p];
    }

    imdiff = MABS(imdiff3) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff3*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff4) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTOR);
    sum = (imdiff4*FilterTableBorderPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;


    /////////////////////////////////
    // Filter four internal pixels //
    /////////////////////////////////

    imdiff = MABS(imdiff5) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdiff5*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff6) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdiff6*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff7) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdiff7*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;

    imdiff = MABS(imdiff8) + QP_offset;
    imdiff = MIN(imdiff, MAX_INDEX_IMDIFFVECTORDR);
    sum = (imdiff8*FilterTableInternalPixels_p[imdiff]+ROUNDVAL) >> DIVISORSHIFT;
    *ImageDst_p++ = *ImageSrc_p++ + sum;
  }

  *ImageDst_p++ = *ImageSrc_p++;
  *ImageDst_p++ = *ImageSrc_p++;
}




static void mp4d_VidPostFilterDefault(const PostFilter_State_t *State_p,
						                     const t_uint8 *SrcY_p,
                                 t_uint8 *DstY_p,
                                 const t_uint8 *SrcCb_p,
                                 t_uint8 *DstCb_p,
                                 const t_uint8 *SrcCr_p,
                                 t_uint8 *DstCr_p,
                                 t_uint8 *PixelRow_p,
                                 VID_PostFilterLevel_t PostFilterLevel)
{
  //  Information about the coding parameters of the macroblocks
  //  are available in State_p->MB_DataArray_p[i] where i is the
  //  number of the macroblock (from 0 to 98 in a QCIF image).

  int size;
  t_uint32 row;
  t_uint32 width;
  const t_uint8 *Src_p;
  t_uint8 *Dst_p;

  Src_p = SrcY_p;
  Dst_p = DstY_p;

  width = State_p->Width;

  // Copy first two lines of pixels
  for(row=0;row<NRTAPSDIV2;row++)
  {
    // memory copy src to dst
    memcpy(Dst_p, Src_p, width);
    Src_p += width;
    Dst_p += width;
  }

  ///////////////////////////////
  // Filter luma part of image //
  ///////////////////////////////
  switch(PostFilterLevel)
  {
  case 1:
    for(row=NRTAPSDIV2;row<State_p->Height-NRTAPSDIV2;row++)
    {
      if((row%8==0)||(row%8==7))
      {
        mp4d_PostFilterLineVertically(State_p, Src_p, PixelRow_p, row, width);
        mp4d_PostFilterLineHorizontally(State_p, PixelRow_p, Dst_p, row, width);
      }
      else
      {
        mp4d_PostFilterLineHorizontally(State_p, Src_p, Dst_p, row, width);
      }
      Src_p += width;
      Dst_p += width;
    }
    break;
  case 2:
    for(row=NRTAPSDIV2;row<State_p->Height-NRTAPSDIV2;row++)
    {
      if((row%8 <= 1)||(row%8 >= 6))
      {
        mp4d_PostFilterLineVertically(State_p, Src_p, PixelRow_p, row, width);
        mp4d_PostFilterLineHorizontally2Pels(State_p, PixelRow_p, Dst_p, row, width);
      }
      else
      {
        mp4d_PostFilterLineHorizontally2Pels(State_p, Src_p, Dst_p, row, width);
      }
      Src_p += width;
      Dst_p += width;
    }
    break;
  case 3:
  case 4:
    for(row=NRTAPSDIV2;row<State_p->Height-NRTAPSDIV2;row++)
    {
      if((row%8 <= 1)||(row%8 >= 6))
      {
        mp4d_PostFilterLineVertically(State_p, Src_p, PixelRow_p, row, width);
      }
      else
      {
        mp4d_PostFilterLineVerticallyInternalPels(State_p, Src_p, PixelRow_p, row, width,16);
      }
      mp4d_PostFilterLineHorizontallyAllPels(State_p, PixelRow_p, Dst_p, row, width,16);
      Src_p += width;
      Dst_p += width;
    }
    break;
  default:
    break;
  }


  // Copy last two lines of pixels
  for(row=0;row<NRTAPSDIV2;row++)
  {
    // memory copy src to dst
    memcpy(Dst_p, Src_p, width);
    Src_p += width;
    Dst_p += width;
  }

  //////////////////////////////////////////////////////
  // Filter chroma part of image if PostFilterLevel 4 is used //
  //////////////////////////////////////////////////////
  if(PostFilterLevel == 4)
  {
    int chroma;
    width = width>>1;

    // Set up pointers for Cb
    Src_p = SrcCb_p;
    Dst_p = DstCb_p;

    // Do Cb and Cr
    for(chroma = 0 ; chroma<2 ; chroma++)
    {
      for(row=0;row<NRTAPSDIV2;row++)
      {
        // memory copy first two pixel lines
        memcpy(Dst_p, Src_p, width);
        Src_p += width;
        Dst_p += width;
      }

      // Do chroma filtering
      for(row=NRTAPSDIV2;row<State_p->Height/2-NRTAPSDIV2;row++)
      {
        if((row%8 <= 1)||(row%8 >= 6))
        {
          mp4d_PostFilterLineVerticallyChroma(State_p, Src_p, PixelRow_p, row, width);
        }
        else
        {
          mp4d_PostFilterLineVerticallyInternalPels(State_p, Src_p, PixelRow_p, row, width,8);
        }
        mp4d_PostFilterLineHorizontallyAllPels(State_p, PixelRow_p, Dst_p, row, width,8);
        Src_p += width;
        Dst_p += width;
      }

      for(row=0;row<NRTAPSDIV2;row++)
      {
        // memory copy last two pixel lines
        memcpy(Dst_p, Src_p, width);
        Src_p += width;
        Dst_p += width;
      }

      // Set up pointers for Cr
      Src_p = SrcCr_p;
      Dst_p = DstCr_p;
    }
  }
  else
  {
    size = State_p->Width * State_p->Height;
    memcpy(DstCb_p, SrcCb_p, size/4);
    memcpy(DstCr_p, SrcCr_p, size/4);
  }
}

#endif




