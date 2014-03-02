/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup Scalar Generic Scalar Services

 \detail This module provides an interface to the General Purpose Scalar. It is envisaged that
 		 a top level product will use its interface to build scalar functionalities corresponding
 		 to different pipes.
*/

/**
 \file Scalar.c
 \brief This file is a part of the scalar module release code. It implements a driver
        layer for the general purpose scalar.
 \ingroup Scalar
*/

#include "Platform.h"
#include "Scalar.h"
#include "Scalar_IPInterface.h"
#include "Scalar_OPInterface.h"
#include "ScalarPlatformSpecific.h"
#include "Zoom_IPInterface.h"

float_t const g_f_coslut[]={
1.000000,
0.999023,
0.995117,
0.989258,
0.980957,
0.970215,
0.957031,
0.941406,
0.923828,
0.903809,
0.881836,
0.857910,
0.831543,
0.803223,
0.772949,
0.740723,
0.707031,
0.671387,
0.634277,
0.595703,
0.555664,
0.514160,
0.471191,
0.427734,
0.382813,
0.336914,
0.290527,
0.243164,
0.195313,
0.146973,
0.098145,
0.048828,
0.000000
};

Scalar_StripeInternalParams_ts g_Scalar_StripeInternalParams[PIPE_COUNT] = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
Scalar_InternalParams_ts 	   g_Scalar_InternalParams[PIPE_COUNT];

void FilterResourceManager ( Scalar_UserParams_ts *UserParams, Scalar_InternalParams_ts *InternalParams, Scalar_Inputs_ts *Inputs, Scalar_Output_ts *Output );
extern void FilterResponseManager ( Scalar_UserParams_ts *UserParams, Scalar_InternalParams_ts *InternalParams, Scalar_Inputs_ts *Inputs, Scalar_Output_ts *Output );
void CropBorderManager 	   ( Scalar_UserParams_ts *UserParams, Scalar_InternalParams_ts *InternalParams, Scalar_Inputs_ts *Inputs, Scalar_Output_ts *Output, Scalar_Stripe_InputParams_ts *StripeInputs,bool_t bo_HValid);
void StripeBorderManager   ( Scalar_Stripe_InputParams_ts *StripeInputs,Scalar_StripeInternalParams_ts *StripeInternalParams, uint8_t DownScaling,uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;
void StripeParamsManager   ( Scalar_Inputs_ts *Inputs, Scalar_Output_ts *Output,Scalar_UserParams_ts *UserParams,Scalar_Stripe_InputParams_ts *StripeInputs,Scalar_Stripe_OutputParams_ts *StripeOutputs,Scalar_StripeInternalParams_ts *StripeInternalParams,uint8_t DownScaling,uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;
void StripeLineLength	   ( uint8_t DownScaling,Scalar_Inputs_ts *Inputs,Scalar_Stripe_OutputParams_ts *StripeOutputs,Scalar_StripeInternalParams_ts *StripeInternalParams,uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;


/**
  \fn void Scalar_Compute(Scalar_UserParams_ts *UserParams,
					Scalar_Inputs_ts *Inputs,
					Scalar_Output_ts *Output,
					Scalar_Stripe_InputParams_ts *StripeInputs,
					Scalar_Stripe_OutputParams_ts *StripeOutputs,
					uint8_t u8_PipeNo )
  \brief    Top level scalar module function to be called to
  			generate the scalar settings for a particular set
  			of inputs and outputs.
  \return 	void
  \param 	Scalar_UserParams_ts *UserParams			:	Pointer to user parameter structure
  \param 	Scalar_Inputs_ts *Inputs					:	Pointer to scalar inputs structure
  \param 	Scalar_Output_ts *Output					:	Pointer to scalar output structure
  \param 	Scalar_Stripe_InputParams_ts *StripeInputs	:	Pointer to scalar Stripe input structure
  \param 	Scalar_Stripe_OutputParams_ts *StripeInputs	:	Pointer to scalar Stripe output structure
  \param 	u8_PipeNo									: 	Variable to indicate Pipe No (0 or 1)

  \callgraph
  \callergraph
  \ingroup Scalar
*/
void Scalar_Compute(Scalar_UserParams_ts *UserParams,
					Scalar_Inputs_ts *Inputs,
					Scalar_Output_ts *Output,
					Scalar_Stripe_InputParams_ts *StripeInputs,
					Scalar_Stripe_OutputParams_ts *StripeOutputs,
					uint8_t u8_PipeNo
					)
{
	Scalar_InternalParams_ts		InternalParams;

	Scalar_StripeInternalParams_ts	*StripeInternalParams;

	StripeInternalParams = &g_Scalar_StripeInternalParams[u8_PipeNo];

	if(StripeInputs->u8_StripeIndex == 0){
		//If we have to do striping and stripe index is first then only we need to calculate coefficients n all
		//and for first stripe compute stripe borders and stripe size parameters
		FilterResourceManager(
					UserParams,
					&InternalParams,
					Inputs,
					Output);

		FilterResponseManager(
					UserParams,
		  			&InternalParams,
		  			Inputs,
		  			Output);

		CropBorderManager(
					UserParams,
		  			&InternalParams,
		  			Inputs,
		  			Output,
		  			StripeInputs,
		  			TRUE);

		CropBorderManager(
					UserParams,
					&InternalParams,
					Inputs,
					Output,
					StripeInputs,
					FALSE);
	}
	//Execution only if striping enable
	if(StripeInputs->u8_StripeCount >= 1){

	StripeBorderManager(StripeInputs,
					StripeInternalParams,
					Output->e_Flag_DownScaling,
					u8_PipeNo);


	StripeParamsManager(Inputs,
					Output,
					UserParams,
					StripeInputs,
					StripeOutputs,
					StripeInternalParams,
					Output->e_Flag_DownScaling,
					u8_PipeNo);

	StripeLineLength(Output->e_Flag_DownScaling,
					Inputs,
					StripeOutputs,
					StripeInternalParams,
					u8_PipeNo);
	}



	return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void FilterResourceManager ( Scalar_UserParams_ts *UserParams,
		        	 		 Scalar_InternalParams_ts *InternalParams,
							 Scalar_Inputs_ts *Inputs,
							 Scalar_Output_ts *Output )
  \brief
  \return 	void
  \param 	Scalar_UserParams_ts *UserParams			:	Pointer to user parameter structure
  \param 	Scalar_InternalParams_ts *InternalParams	:	Pointer to internal parameter structure
  \param 	Scalar_Inputs_ts *Inputs					:	Pointer to scalar inputs structure
  \param 	Scalar_Output_ts *Output					:	Pointer to scalar output structure
  \callgraph
  \callergraph
  \ingroup Scalar
  \endif
*/
void FilterResourceManager ( Scalar_UserParams_ts *UserParams,
		        	 		 Scalar_InternalParams_ts *InternalParams,
							 Scalar_Inputs_ts *Inputs,
							 Scalar_Output_ts *Output )
{
  uint8_t	u8_NMin;

  u8_NMin = (Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable) ? NMIN_AZ : NMIN;

  if ((1.0 == Inputs->f_Step))
  {
	  InternalParams->e_Flag_Transparent = Flag_e_TRUE;
  }
  else
  {
	  InternalParams->e_Flag_Transparent = Flag_e_FALSE;
  }

  if ((Flag_e_FALSE == InternalParams->e_Flag_Transparent) || (Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable))
  {
    Output->e_Flag_ScalarEnable = Flag_e_TRUE;
  }
/*
	if(*step >= 1.0) {
			nsize0 = *step * (double)(*bsize);
			*nsize = *no_overlap ? (int)nsize0 : (int)nsize0 + 1;
*/
  // since no_overlap is always false,
  // nsize = *step * (double)(*bsize) + 1
  if (Inputs->f_Step >= 1.0)
  {
    Output->u8_NSize = (Inputs->f_Step * UserParams->u8_BSize) + 1;
  }
  else
  {
	Output->u8_NSize = UserParams->u8_BSize + 1;
  }

  if (Inputs->f_Step >= 1.0)
  {
	Output->e_Flag_DownScaling = Flag_e_TRUE;
  }
  else
  {
	  Output->e_Flag_DownScaling = Flag_e_FALSE;
  }

/*
  if(*nsize > *phys_regsize) {
		*nsize = *phys_regsize;
*/
  if (Output->u8_NSize > UserParams->u16_PhysRegSize)
  {
	  Output->u8_NSize = UserParams->u16_PhysRegSize;
  }
  else if (Output->u8_NSize < u8_NMin)
  {
    if ((Flag_e_TRUE == InternalParams->e_Flag_Transparent) && (Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable))
    {
      Output->u8_NSize = NMIN;
      Output->u8_LogMSize = 1;
      InternalParams->u8_Overhang = 0;
    }
    else
    {
    	Output->u8_NSize = NMIN;
    }
  }

  /* trim the polyphases at large N */

/*
   *logmsize = LOGFTSIZE;
		while(((1 << *logmsize) * *nsize) > ((*phys_regsize) << 1)) --(*logmsize);
*/
  Output->u8_LogMSize = LOGFTSIZE;
  while (((1 << Output->u8_LogMSize) * Output->u8_NSize) > (UserParams->u16_PhysRegSize << 1))
  {
	  Output->u8_LogMSize -= 1;
  }

  if (Output->u8_LogMSize < 1)
  {
	  Output->u8_LogMSize = 1;
	  Output->u8_NSize = UserParams->u16_PhysRegSize;
  }

  /* overhang */
  if (Output->u8_NSize < 3)
  {
    InternalParams->u8_Overhang = 0;
  }
  else
  {
	if (Inputs->f_Step > 1.0)
	{
		/* *overhang = 1 + *nsize / (*phys_regwidth + 1); */
		InternalParams->u8_Overhang = 1 + Output->u8_NSize / (UserParams->u8_PhysRegWidth + 1);
	}
	else
	{
		InternalParams->u8_Overhang = 1;
	}
  }

  return;
}




/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void CropBorderManager ( Scalar_UserParams_ts *UserParams,
							 			  Scalar_InternalParams_ts *InternalParams,
							 			  Scalar_Inputs_ts *Inputs,
							 			  Scalar_Output_ts *Output,
							 			  bool_t bo_HValid )
  \brief
  \return 	void
  \param 	Scalar_UserParams_ts *UserParams			:	Pointer to user parameter structure
  \param 	Scalar_InternalParams_ts *InternalParams	:	Pointer to internal parameter structure
  \param 	Scalar_Inputs_ts *Inputs					:	Pointer to scalar inputs structure
  \param 	Scalar_Output_ts *Output					:	Pointer to scalar output structure
  \param 	bo_HValid									:   Variable to indicate H or V Crop Border Manager
  \callgraph
  \callergraph
  \ingroup Scalar
  \endif
*/
void CropBorderManager ( Scalar_UserParams_ts *UserParams,
						 Scalar_InternalParams_ts *InternalParams,
						 Scalar_Inputs_ts *Inputs,
						 Scalar_Output_ts *Output,
						 Scalar_Stripe_InputParams_ts *StripeInputs,
						 bool_t bo_HValid)
{

  int16_t  s16_LTmp[MAX_B_SIZE], s16_RTmp[MAX_B_SIZE], s16_LList[MAX_B_SIZE], s16_RList[MAX_B_SIZE];

  uint16_t u16_CropperInputSize, u16_ScalarOutputSize, u16_OSizeDim, u16_GrossCrop, u16_ScalarInputSize;

  uint8_t  u8_LSize, u8_RSize, u8_I, u8_BSizex4;

  float_t  f_Crop, f_OSizeDimMulStep, f_FarCrop, f_LocalCSize, f_CSizeDim, f_CropStt, f_NSizeD2;
  float_t  f_CropTmp, f_CropEnd, f_GpsCrop;

  int16_t  s16_ITmp, s16_SMod, s16_BorderShift, s16_BorderWidth, s16_NoBorder, s16_BorderSum;
  int16_t  s16_LeftBorder, s16_RightBorder;
  uint32_t 	u32_LBorderCopies;
  uint32_t  u32_RBorderCopies;

  //Initialization of Local Variables
  u32_LBorderCopies = 0;
  u32_RBorderCopies = 0;

  if(StripeInputs->u8_StripeCount >=1 ){
	  if(Flag_e_TRUE == Output->e_Flag_DownScaling){
		  if (bo_HValid)  {
			  u16_CropperInputSize = Inputs->u16_HCropperInputSize - (EXT_DS_PRE_LFT_BORDER + EXT_DS_PRE_RGT_BORDER);
			  u16_ScalarOutputSize = Inputs->u16_HScalarOutputSize;
			  f_Crop = Inputs->f_HCrop;
		   }
		  else {
			  u16_CropperInputSize = Inputs->u16_VCropperInputSize - (EXT_DS_PRE_TOP_BORDER + EXT_DS_PRE_BOT_BORDER);
			  u16_ScalarOutputSize = Inputs->u16_VScalarOutputSize;
			  f_Crop = Inputs->f_VCrop;
		  }
	  }//Downscaling
	  else {
		  if (bo_HValid)  {
			  u16_CropperInputSize = Inputs->u16_HCropperInputSize - (EXT_US_PRE_LFT_BORDER + EXT_US_PRE_RGT_BORDER);
		  	  u16_ScalarOutputSize = Inputs->u16_HScalarOutputSize;
		  	  f_Crop = Inputs->f_HCrop;
		  }
		  else {
			  u16_CropperInputSize = Inputs->u16_VCropperInputSize - (EXT_DS_PRE_TOP_BORDER + EXT_DS_PRE_BOT_BORDER);
			  u16_ScalarOutputSize = Inputs->u16_VScalarOutputSize;
			  f_Crop = Inputs->f_VCrop;
		  }

	  }//Upscaling

  }//Striping
  else {
	  if (bo_HValid)
	  {
		  u16_CropperInputSize = Inputs->u16_HCropperInputSize;
		  u16_ScalarOutputSize = Inputs->u16_HScalarOutputSize;
		  f_Crop = Inputs->f_HCrop;
	  }
	  else
	  {
		  u16_CropperInputSize = Inputs->u16_VCropperInputSize;
		  u16_ScalarOutputSize = Inputs->u16_VScalarOutputSize;
		  f_Crop = Inputs->f_VCrop;
	  }
  }//No Striping

  /* initial crop is distance from (cropped) array edge to centre of 1st o/p pixel */

  // osizedim = *scaler_output_size - 1;
  u16_OSizeDim = u16_ScalarOutputSize - 1;

  // far_crop = *crop + (double)osizedim * *step;
  f_OSizeDimMulStep = (float_t)u16_OSizeDim * Inputs->f_Step;
  f_FarCrop = f_Crop + f_OSizeDimMulStep;

  // local_csize = (double)*scaler_output_size * *step;
  f_LocalCSize = u16_ScalarOutputSize * Inputs->f_Step;

  // local_csize += *crop - (double)(int)*crop;
  f_LocalCSize += f_Crop - (uint32_t)f_Crop;

  // local_csize += (double)(*nsize - 1);
  f_LocalCSize += Output->u8_NSize - 1;

  // csizedim = local_csize - 1.0;
  f_CSizeDim = f_LocalCSize - 1.0;

  /* avoid cropping outside the original */
  if (f_FarCrop > u16_CropperInputSize)
  {
	// crop_stt = *crop + (double)*cropper_input_size - far_crop;
    f_CropStt = f_Crop + u16_CropperInputSize - f_FarCrop;

    // far_crop = crop_stt + (double)osizedim * *step;
    f_FarCrop = f_CropStt + f_OSizeDimMulStep;
  }
  else if (f_Crop < 0.0)
  {
	// crop_stt = 0.0;
	f_CropStt = 0.0;
  }
  else
  {
	// crop_stt = *crop;
	f_CropStt = f_Crop;
  }

  /* offset phase index compensation, so that -.5->+.5 becomes 0->1, and can lookup */
  // crop_stt += 0.5;
  f_CropStt += 0.5;

  /* fix to maxphase when cornered */
  // if(*nsize == 2 && *step == 1.0) crop_stt += 0.5;
  if (2 == Output->u8_NSize && 1.0 == Inputs->f_Step)
  {
	f_CropStt += 0.5;
  }

  /* jump back to filter start */
  // crop_stt -= (fnsized2 = (double)*nsize / 2.0);
  f_CropStt -= (f_NSizeD2 = (float_t)Output->u8_NSize/2.0);

  /* program borders */
  /* leading (top/left) */

  // lsize = 0;
  u8_LSize = 0;

  // crop_tmp = crop_stt;
  f_CropTmp = f_CropStt;

  if (Flag_e_TRUE == Output->e_Flag_DownScaling)
  {
    while (f_CropStt < 0.0)
    {
      // l_tmp[lsize++] = (int)(crop_stt + fnsized2 - .5);
      s16_LTmp[u8_LSize++] = (int16_t)(f_CropStt + f_NSizeD2 - 0.5);

      // crop_stt += *step;
      f_CropStt += Inputs->f_Step;

      // crop_tmp -= *step;
      f_CropTmp -= Inputs->f_Step;
    } // while (f_CropStt < 0.0)

    // *gross_crop = (crop_tmp < 0.0) ? 0 : (int)crop_tmp;
    u16_GrossCrop = (f_CropTmp < 0.0) ? 0 : (uint16_t)f_CropTmp;
  }
  else
  {
    if (f_CropStt < 0.0)
    {
      // l_tmp[lsize++] = 0;
    	s16_LTmp[u8_LSize++] = 0;
    	++f_CropStt;
    }

    // *gross_crop = crop_stt;
    u16_GrossCrop = (int16_t)f_CropStt;
  }

  // crop_end = far_crop + fnsized2 - .5;
  f_CropEnd = f_FarCrop + f_NSizeD2 - 0.5;

  // rsize = 0;
  u8_RSize = 0;

  if (Flag_e_TRUE == Output->e_Flag_DownScaling)
  {
    while (f_CropEnd >= u16_CropperInputSize)
    {
      // itmp = (int)(crop_end - (*gross_crop + csizedim + fnsized2));
      s16_ITmp = f_CropEnd - (u16_GrossCrop + f_CSizeDim + f_NSizeD2);

      // r_tmp[rsize++] = itmp > 0 ? itmp : 0;
      s16_RTmp[u8_RSize++] = s16_ITmp > 0 ? s16_ITmp : 0;

      // crop_end -= *step;
      f_CropEnd -= Inputs->f_Step;
    } // while (f_CropEnd >= u16_CropperInputSize)
  }
  else
  {
    if (f_CropEnd >= u16_CropperInputSize)
    {
      // r_tmp[rsize++] = 0;
      s16_RTmp[u8_RSize++] = 0;
    }
  }

  /* actual size used in h/w scaler (unless slaving, in which case it's already fixed) */
  // *scaler_input_size = 65535 - (int)(65535.0 - local_csize);
  u16_ScalarInputSize = 65535 - (uint16_t)(65535.0 - f_LocalCSize);

  if((u16_ScalarInputSize + u16_GrossCrop) > u16_CropperInputSize)
  {
	// *scaler_input_size = *cropper_input_size - *gross_crop;
	u16_ScalarInputSize = u16_CropperInputSize - u16_GrossCrop;
  }

  // crop_tmp = crop_stt - (double)*gross_crop;
  f_GpsCrop = f_CropStt - u16_GrossCrop;

  /* evaluate leading list */
  for(u8_I = 0; u8_I < UserParams->u8_BSize; u8_I++)
  {
  	// l_list[i] = NOT_BORDERING;
	s16_LList[u8_I] = NOT_BORDERING;

	// r_list[i] = NOT_BORDERING;
	s16_RList[u8_I] = NOT_BORDERING;

  }

  u8_BSizex4 = UserParams->u8_BSize << 2;

  for(u8_I = 0; u8_I < u8_LSize; u8_I++)
  {
	// l_list[((((*bsize)<<2)-lsize)+i) % (*bsize)] = l_tmp[i];
	s16_LList[(u8_BSizex4 - u8_LSize + u8_I) % UserParams->u8_BSize] = s16_LTmp[u8_I];
  }

  // smod = ((*scaler_output_size - (lsize + rsize)) % (*bsize));
  s16_SMod = (u16_ScalarOutputSize - (u8_LSize + u8_RSize)) % UserParams->u8_BSize;

  /* evaluate lagging list */
  for(u8_I = 0; u8_I < u8_RSize; u8_I++)
  {
  	// r_list[((*bsize<<2)+smod+(rsize-1)-i) % *bsize] = 1 + r_tmp[i];
  	s16_RList[(u8_BSizex4 + s16_SMod + (u8_RSize - 1) - u8_I) % UserParams->u8_BSize] = 1 + s16_RTmp[u8_I];
  }

  /* code the border lists as an integer */
  // border_shift = (*phys_regsize) - 1;
  s16_BorderShift = UserParams->u16_PhysRegSize - 1;

  // border_width = 1;
  s16_BorderWidth = 1;

  // while((border_shift >>= 1) > 0) ++border_width;
  while ((s16_BorderShift >>= 1) > 0)
  {
    ++s16_BorderWidth;
  }

  // no_border = ((1 << border_width) - 1);
  s16_NoBorder = ((1 << s16_BorderWidth) - 1);

  // border_shift = 0;
  s16_BorderShift = 0;

  // coded_l_list = 0;
  s16_BorderSum = 0;

  for(u8_I = 0; u8_I < UserParams->u8_BSize; u8_I++)
  {
    if (s16_LList[u8_I] < 0)
    {
      // *coded_l_list += (no_border << border_shift);
      s16_BorderSum += (s16_NoBorder << s16_BorderShift);
    }
    else
    {
      // *coded_l_list += (l_list[i] << border_shift);
      s16_BorderSum += (s16_LList[u8_I] << s16_BorderShift);
      //Added for Striping
      ++u32_LBorderCopies;

    }

    // border_shift = border_shift + border_width;
    s16_BorderShift += s16_BorderWidth;
  }

  s16_LeftBorder = s16_BorderSum;

  // border_shift = 0;
  s16_BorderShift = 0;

  // *coded_r_list = 0;
  s16_BorderSum = 0;

  for(u8_I = 0; u8_I < UserParams->u8_BSize; u8_I++)
  {
    if (s16_RList[u8_I] < 0)
    {
      // *coded_r_list += (no_border << border_shift);
      s16_BorderSum += (s16_NoBorder << s16_BorderShift);
    }
    else
    {
      // *coded_r_list += (r_list[i] << border_shift);
      s16_BorderSum += s16_RList[u8_I] << s16_BorderShift;

      //Added for Striping
      ++u32_RBorderCopies;
    }

    // border_shift = border_shift + border_width;
    s16_BorderShift += s16_BorderWidth;
  }

  s16_RightBorder = s16_BorderSum;

  if(StripeInputs->u8_StripeCount >=1 ){
  	  if(Flag_e_TRUE == Output->e_Flag_DownScaling){
  		  if (bo_HValid)  {
  			  Output->u16_HCropSize = u16_ScalarInputSize + (EXT_DS_PRE_LFT_BORDER + EXT_DS_PRE_RGT_BORDER);
  			  Output->u16_HCropStart = u16_GrossCrop;
  			  Output->f_HGPSCrop = f_GpsCrop;
  			  Output->u16_RightBorder = s16_RightBorder;
  			  Output->u16_LeftBorder = s16_LeftBorder;
  			  Output->u32_LBorderCopies = u32_LBorderCopies;
  			  Output->u32_RBorderCopies = u32_RBorderCopies;
  		   }
  		  else {
  			  Output->u16_VCropSize = u16_ScalarInputSize ;//No need to modify Vertical size,Striping only affects Horizontal Size
  			  Output->u16_VCropStart = u16_GrossCrop;
  			  Output->f_VGPSCrop = f_GpsCrop;
  			  Output->u16_TopBorder = s16_LeftBorder;
  			  Output->u16_BottomBorder = s16_RightBorder;
  		  }
  	  }//Downscaling
  	  else {
  		  if (bo_HValid)  {
  			  Output->u16_HCropSize = u16_ScalarInputSize + (EXT_US_PRE_LFT_BORDER + EXT_US_PRE_RGT_BORDER);
  			  Output->u16_HCropStart = u16_GrossCrop;
  			  Output->f_HGPSCrop = f_GpsCrop;
  			  Output->u16_RightBorder = s16_RightBorder;
  			  Output->u16_LeftBorder = s16_LeftBorder;
  			  Output->u32_LBorderCopies = u32_LBorderCopies;
  			  Output->u32_RBorderCopies = u32_RBorderCopies;
  		  }
  		  else {
  			  Output->u16_VCropSize = u16_ScalarInputSize ;//No need to modify Vertical size,Striping only affects Horizontal Size
  			  Output->u16_VCropStart = u16_GrossCrop;
  			  Output->f_VGPSCrop = f_GpsCrop;
  			  Output->u16_TopBorder = s16_LeftBorder;
  			  Output->u16_BottomBorder = s16_RightBorder;
  		  }
  	  }//Upscaling

  }//Striping
  else {
	  // Populate the outputs
	  if (bo_HValid)
	  {
	  	Output->u16_HCropSize = u16_ScalarInputSize;
	  	Output->u16_HCropStart = u16_GrossCrop;
	  	Output->f_HGPSCrop = f_GpsCrop;
	  	Output->u16_RightBorder = s16_RightBorder;
	  	Output->u16_LeftBorder = s16_LeftBorder;
	  	Output->u32_LBorderCopies = u32_LBorderCopies;
	  	Output->u32_RBorderCopies = u32_RBorderCopies;

	  }
	  else
	  {
	  	Output->u16_VCropSize 		= u16_ScalarInputSize;
	  	Output->u16_VCropStart = u16_GrossCrop;
	  	Output->f_VGPSCrop = f_GpsCrop;
	  	Output->u16_TopBorder = s16_LeftBorder;
	  	Output->u16_BottomBorder = s16_RightBorder;
	  }
  }//No Striping

  return ;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void StripeBorderManager ( Scalar_Stripe_InputParams_ts *StripeInputs,
						 		 StripeInternalParams *StripeInternalParams,
						 		 uint8_t u8_DownScaling,
						 		 uint8_t u8_PipeNo )
\brief
 \return 	void
 \param 	Scalar_Stripe_InputParams_ts *StripeInputs			:	Pointer to Stripe Input parameter structure
 \param 	StripeInternalParams *StripeInternalParams     		:	Pointer to Stripe Internal parameter structure
 \param 	u8_DownScaling										: 	Variable to indicate whether downscaling or not
 \param 	u8_PipeNo											: 	Variable to indicate Pipe No (0 or 1)

 \callgraph
 \callergraph
 \ingroup Scalar
 \endif
*/

void StripeBorderManager (Scalar_Stripe_InputParams_ts *StripeInputs,
						  Scalar_StripeInternalParams_ts *StripeInternalParams,
						  uint8_t u8_DownScaling,
						  uint8_t u8_PipeNo)
{


	uint8_t u8_ExtPreLeftBorder,u8_ExtPreRightBorder,u8_ExtPreTopBorder,u8_ExtPreBottomBorder;
	uint8_t u8_ExtPostLeftBorder,u8_ExtPostRightBorder,u8_ExtPostTopBorder,u8_ExtPostBottomBorder;

	uint8_t u8_IntPreLeftBorder,u8_IntPreRightBorder,u8_IntPreTopBorder,u8_IntPreBottomBorder;
	uint8_t u8_IntPostLeftBorder,u8_IntPostRightBorder,u8_IntPostTopBorder,u8_IntPostBottomBorder;

	/* Corresponding C Source Code
		ext_pre_top_border = do_downscale ? EXT_DS_PRE_TOP_BORDER : EXT_US_PRE_TOP_BORDER;
		ext_pre_bot_border = do_downscale ? EXT_DS_PRE_BOT_BORDER : EXT_US_PRE_BOT_BORDER;
		ext_pre_lft_border = do_downscale ? EXT_DS_PRE_LFT_BORDER : EXT_US_PRE_LFT_BORDER;
		ext_pre_rgt_border = do_downscale ? EXT_DS_PRE_RGT_BORDER : EXT_US_PRE_RGT_BORDER;
		ext_pst_top_border = do_downscale ? EXT_DS_PST_TOP_BORDER : EXT_US_PST_TOP_BORDER;
		ext_pst_bot_border = do_downscale ? EXT_DS_PST_BOT_BORDER : EXT_US_PST_BOT_BORDER;
		ext_pst_lft_border = do_downscale ? EXT_DS_PST_LFT_BORDER : EXT_US_PST_LFT_BORDER;
		ext_pst_rgt_border = do_downscale ? EXT_DS_PST_RGT_BORDER : EXT_US_PST_RGT_BORDER;
		int_pre_top_border = do_downscale ? INT_DS_PRE_TOP_BORDER : INT_US_PRE_TOP_BORDER;
		int_pre_bot_border = do_downscale ? INT_DS_PRE_BOT_BORDER : INT_US_PRE_BOT_BORDER;
		int_pre_lft_border = do_downscale ? INT_DS_PRE_LFT_BORDER : INT_US_PRE_LFT_BORDER;
		int_pre_rgt_border = do_downscale ? INT_DS_PRE_RGT_BORDER : INT_US_PRE_RGT_BORDER;
		int_pst_top_border = do_downscale ? INT_DS_PST_TOP_BORDER : INT_US_PST_TOP_BORDER;
		int_pst_bot_border = do_downscale ? INT_DS_PST_BOT_BORDER : INT_US_PST_BOT_BORDER;
		int_pst_lft_border = do_downscale ? INT_DS_PST_LFT_BORDER : INT_US_PST_LFT_BORDER;
		int_pst_rgt_border = do_downscale ? INT_DS_PST_RGT_BORDER : INT_US_PST_RGT_BORDER;
	*/

	u8_ExtPreTopBorder 		= u8_DownScaling ? EXT_DS_PRE_TOP_BORDER : EXT_US_PRE_TOP_BORDER;
	u8_ExtPreBottomBorder 	= u8_DownScaling ? EXT_DS_PRE_BOT_BORDER : EXT_US_PRE_BOT_BORDER;
	u8_ExtPreLeftBorder 	= u8_DownScaling ? EXT_DS_PRE_LFT_BORDER : EXT_US_PRE_LFT_BORDER;
	u8_ExtPreRightBorder 	= u8_DownScaling ? EXT_DS_PRE_RGT_BORDER : EXT_US_PRE_RGT_BORDER;
	u8_ExtPostTopBorder 	= u8_DownScaling ? EXT_DS_PST_TOP_BORDER : EXT_US_PST_TOP_BORDER;
	u8_ExtPostBottomBorder 	= u8_DownScaling ? EXT_DS_PST_BOT_BORDER : EXT_US_PST_BOT_BORDER;
	u8_ExtPostLeftBorder 	= u8_DownScaling ? EXT_DS_PST_LFT_BORDER : EXT_US_PST_LFT_BORDER;
	u8_ExtPostRightBorder 	= u8_DownScaling ? EXT_DS_PST_RGT_BORDER : EXT_US_PST_RGT_BORDER;

	u8_IntPreTopBorder 		= u8_DownScaling ? INT_DS_PRE_TOP_BORDER : INT_US_PRE_TOP_BORDER;
	u8_IntPreBottomBorder 	= u8_DownScaling ? INT_DS_PRE_BOT_BORDER : INT_US_PRE_BOT_BORDER;
	u8_IntPreLeftBorder 	= u8_DownScaling ? INT_DS_PRE_LFT_BORDER : INT_US_PRE_LFT_BORDER;
	u8_IntPreRightBorder 	= u8_DownScaling ? INT_DS_PRE_RGT_BORDER : INT_US_PRE_RGT_BORDER;
	u8_IntPostTopBorder 	= u8_DownScaling ? INT_DS_PST_TOP_BORDER : INT_US_PST_TOP_BORDER;
	u8_IntPostBottomBorder 	= u8_DownScaling ? INT_DS_PST_BOT_BORDER : INT_US_PST_BOT_BORDER;
	u8_IntPostLeftBorder 	= u8_DownScaling ? INT_DS_PST_LFT_BORDER : INT_US_PST_LFT_BORDER;
	u8_IntPostRightBorder 	= u8_DownScaling ? INT_DS_PST_RGT_BORDER : INT_US_PST_RGT_BORDER;

	if(StripeInputs->u8_StripeCount > 1) {
		/* Corresponding C Source Code
		  	*pre_top_border = ext_pre_top_border;
			*pre_bot_border = ext_pre_bot_border;
			*pst_top_border = ext_pst_top_border;
			*pst_bot_border = ext_pst_bot_border;
			if(stripe_index == 0) {
				*pre_lft_border = ext_pre_lft_border;
				*pre_rgt_border = int_pre_rgt_border;
				*pst_lft_border = ext_pst_lft_border;
				*pst_rgt_border = int_pst_rgt_border;
			}
			else if(stripe_index == stripe_count-1) {
				*pre_lft_border = int_pre_lft_border;
				*pre_rgt_border = ext_pre_rgt_border;
				*pst_lft_border = int_pst_lft_border;
				*pst_rgt_border = ext_pst_rgt_border;
			}
			else {
				*pre_lft_border = int_pre_lft_border;
				*pre_rgt_border = int_pre_rgt_border;
				*pst_lft_border = int_pst_lft_border;
				*pst_rgt_border = int_pst_rgt_border;
			}
		*/
		StripeInternalParams->u8_PreTopBorder 		  = u8_ExtPreTopBorder;
		StripeInternalParams->u8_PreBottomBorder 	  = u8_ExtPreBottomBorder;
		StripeInternalParams->u8_PostTopBorder		  = u8_ExtPostTopBorder;
		StripeInternalParams->u8_PostBottomBorder 	  = u8_ExtPostBottomBorder;

		if(StripeInputs->u8_StripeIndex == 0){
			StripeInternalParams->u8_PreLeftBorder   = u8_ExtPreLeftBorder;
			StripeInternalParams->u8_PreRightBorder  = u8_IntPreRightBorder;
			StripeInternalParams->u8_PostLeftBorder  = u8_ExtPostLeftBorder;
			StripeInternalParams->u8_PostRightBorder = u8_IntPostRightBorder;
		}
		else if(StripeInputs->u8_StripeIndex == StripeInputs->u8_StripeCount-1){
			StripeInternalParams->u8_PreLeftBorder   = u8_IntPreLeftBorder;
			StripeInternalParams->u8_PreRightBorder  = u8_ExtPreRightBorder;
			StripeInternalParams->u8_PostLeftBorder  = u8_IntPostLeftBorder;
			StripeInternalParams->u8_PostRightBorder = u8_ExtPostRightBorder;
		}
		else {
			StripeInternalParams->u8_PreLeftBorder   = u8_IntPreLeftBorder;
			StripeInternalParams->u8_PreRightBorder  = u8_IntPreRightBorder;
			StripeInternalParams->u8_PostLeftBorder  = u8_IntPostLeftBorder;
			StripeInternalParams->u8_PostRightBorder = u8_IntPostRightBorder;
		}
	}//if(StripeCount > 1)
	else {
		/* Corresponding C Source Code
		  *pre_lft_border = ext_pre_lft_border;
		  *pre_rgt_border = ext_pre_rgt_border;
		  *pre_top_border = ext_pre_top_border;
		  *pre_bot_border = ext_pre_bot_border;
		  *pst_lft_border = ext_pst_lft_border;
		  *pst_rgt_border = ext_pst_rgt_border;
		  *pst_top_border = ext_pst_top_border;
		  *pst_bot_border = ext_pst_bot_border;
		*/

		StripeInternalParams->u8_PreLeftBorder   	  = u8_ExtPreLeftBorder;
		StripeInternalParams->u8_PreRightBorder       = u8_ExtPreRightBorder;
		StripeInternalParams->u8_PreTopBorder 		  = u8_ExtPreTopBorder;
		StripeInternalParams->u8_PreBottomBorder 	  = u8_ExtPreBottomBorder;
		StripeInternalParams->u8_PostLeftBorder       = u8_ExtPostLeftBorder;
		StripeInternalParams->u8_PostRightBorder      = u8_ExtPostRightBorder;
		StripeInternalParams->u8_PostTopBorder		  = u8_ExtPostTopBorder;
		StripeInternalParams->u8_PostBottomBorder 	  = u8_ExtPostBottomBorder;
	}

	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PreLeftBorder   	 = StripeInternalParams->u8_PreLeftBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PreRightBorder       = StripeInternalParams->u8_PreRightBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PreTopBorder 		 = StripeInternalParams->u8_PreTopBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PreBottomBorder 	 = StripeInternalParams->u8_PreBottomBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PostLeftBorder       = StripeInternalParams->u8_PostLeftBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PostRightBorder      = StripeInternalParams->u8_PostRightBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PostTopBorder		 = StripeInternalParams->u8_PostTopBorder;
	g_Scalar_StripeInternalParams[u8_PipeNo].u8_PostBottomBorder 	 = StripeInternalParams->u8_PostBottomBorder;

	return ;
}//Function End



/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void StripeParamsManager ( Scalar_Inputs_ts *Inputs,
							 Scalar_Output_ts *Outputs,
							 Scalar_UserParams_ts *UserParams,
							 Scalar_Stripe_InputParams_ts *StripeInputs,
							 Scalar_Stripe_OutputParams_ts *StripeOutputs,
							 Scalar_StripeInternalParams_ts *StripeInternalParams,
							 uint8_t u8_DownScaling)

\brief
 \return 	void
 \param 	Scalar_Inputs_ts *Inputs					:	Pointer to Scalar Input parameter structure
 \param 	Scalar_Output_ts *Outputs					:	Pointer to Scalar Output parameter structure
 \param 	Scalar_UserParams_ts *UserParams     		:	Pointer to Scalar User Parameter structure
 \param 	Scalar_Stripe_InputParams_ts *StripeInputs  :	Pointer to Stripe Input parameter structure
 \param 	Scalar_Stripe_OutputParams_ts *StripeOutputs:	Pointer to Stripe Output parameter structure
 \param 	Scalar_StripeInternalParams_ts *StripeInternalParams: Pointer to Stripe internal parameter structure
 \param 	u8_DownScaling								: 	Variable to indicate whether downscaling or not
 \param 	u8_PipeNo									: 	Variable to indicate Pipe No (0 or 1)

 \callgraph
 \callergraph
 \ingroup Scalar
 \endif
*/
void StripeParamsManager   ( Scalar_Inputs_ts *Inputs,
							 Scalar_Output_ts *Outputs,
							 Scalar_UserParams_ts *UserParams,
							 Scalar_Stripe_InputParams_ts *StripeInputs,
							 Scalar_Stripe_OutputParams_ts *StripeOutputs,
							 Scalar_StripeInternalParams_ts *StripeInternalParams,
							 uint8_t u8_DownScaling,
							 uint8_t u8_PipeNo)
{

	uint32_t	u32_HCropAdvBulk,u32_HCropAdvFrac;

	uint32_t 	u32_HSizeBulk,u32_HSizeFrac;

	uint16_t	u16_VCropBulk;
	uint16_t	u16_VCropFrac;
	uint16_t	u16_HCropBulk;
	uint16_t	u16_HCropFrac;

	uint16_t	u16_HStepBulk;
	uint16_t	u16_HStepFrac;

	uint8_t     u8_Remainder, u8_UpStreamAdjust, u8_DnStreamAdjust;
	uint32_t 	u32_BorderCopies;
	uint16_t 	u16_HSizeShift = 0 ;


	//Initialization of Local Variables
	u32_BorderCopies = 0;

	u16_VCropBulk = (uint16_t)(Outputs->f_VGPSCrop);
	u16_VCropFrac = (uint16_t)((Outputs->f_VGPSCrop * 0x10000) + 0.5);

	u16_HCropBulk = (uint16_t)(Outputs->f_HGPSCrop);
	u16_HCropFrac = (uint16_t)((Outputs->f_HGPSCrop * 0x10000) + 0.5);

	u16_HStepBulk = (uint16_t)(Inputs->f_Step);
	u16_HStepFrac = (uint16_t)((Inputs->f_Step * 0x10000) + 0.5);


	if(StripeInputs->u8_StripeIndex == 0 ){
		/* Corresponding C Source Code
		   ss_hcrop_bulk = ir_hcrop.bulk;
		   ss_hcrop_frac = ir_hcrop.frac;
		   ss_vcrop_bulk = ir_vcrop.bulk;
		   ss_vcrop_frac = ir_vcrop.frac;
		 */
		StripeInternalParams->s32_StripeHCropBulk = u16_HCropBulk;
		StripeInternalParams->s32_StripeHCropFrac = u16_HCropFrac;
		StripeInternalParams->s32_StripeVCropBulk = u16_VCropBulk;
		StripeInternalParams->s32_StripeVCropFrac = u16_VCropFrac;
	}
	u8_UpStreamAdjust = (StripeInputs->u8_StripeIndex > 0) ? 0 :
						u8_DownScaling ? (INT_DS_PRE_LFT_BORDER - EXT_DS_PRE_LFT_BORDER) : (INT_US_PRE_LFT_BORDER - EXT_US_PRE_LFT_BORDER);
	u8_DnStreamAdjust = (StripeInputs->u8_StripeIndex > 0) ? 0 :
						u8_DownScaling ? (INT_DS_PST_LFT_BORDER - EXT_DS_PST_LFT_BORDER) : (INT_US_PST_LFT_BORDER - EXT_US_PST_LFT_BORDER);

	//Output->u16_HCropStart = u16_GrossCrop;//CropBorderManager Function

	/* Corresponding C Source Code
	 	*s_gross_hcrop = gross_hcrop + ((stripe_index == 0) ? 0 : ss_hcrop_bulk);
	 	ir_s_hcrop->frac = ss_hcrop_frac;
		ir_s_hcrop->bulk = (stripe_index == 0) ? ss_hcrop_bulk : 0;
		*s_system_output_hsize = (scaler_output_hsize / (stripe_count << 1)) << 1;
		remainder = (scaler_output_hsize - (stripe_count * *s_system_output_hsize)) >> 1;

		if(stripe_index < remainder) *s_system_output_hsize += 2;
		*s_scaler_output_hsize = *s_system_output_hsize + pst_lft_border + pst_rgt_border;
	*/
	StripeInternalParams->s32_StripeGrossHCrop     = Outputs->u16_HCropStart + ((StripeInputs->u8_StripeIndex == 0) ? 0 : StripeInternalParams->s32_StripeHCropBulk );
	StripeInternalParams->s32_IrStripeHCropFrac    = StripeInternalParams->s32_StripeHCropFrac;
	StripeInternalParams->s32_IrStripeHCropBulk    = (StripeInputs->u8_StripeIndex == 0) ? StripeInternalParams->s32_StripeHCropBulk : 0 ;
	//Ask to Rohit whether its needed or not,or Host will take care of this automatically
	StripeInternalParams->s16_SSystemOutputHSize	= (Inputs->u16_HScalarOutputSize / (StripeInputs->u8_StripeCount << 1)) << 1;
	u8_Remainder 									= (Inputs->u16_HScalarOutputSize - (StripeInputs->u8_StripeCount * StripeInternalParams->s16_SSystemOutputHSize)) >> 1;
	if(StripeInputs->u8_StripeIndex < u8_Remainder)
		StripeInternalParams->s16_SSystemOutputHSize += 2;
	//Till Here

	StripeInternalParams->s16_SScalerOutputHSize 	= StripeInternalParams->s16_SSystemOutputHSize + StripeInternalParams->u8_PostLeftBorder + StripeInternalParams->u8_PostRightBorder ;
	/* Corresponding C Source Code
	 	if(do_downscale) {
	 		if(stripe_index == 0)
	 			for(b=0; b<hbsize; b++)
	 				if(hl_list[b] >= 0) ++border_copies;
	 		if(stripe_index == (stripe_count-1))
	 			for(b=0; b<hbsize; b++)
					if(hr_list[b] >= 0) ++border_copies;
	}
	*/
	if(u8_DownScaling){
		if(StripeInputs->u8_StripeIndex == 0)
			u32_BorderCopies = Outputs->u32_LBorderCopies;
		if(StripeInputs->u8_StripeIndex == (StripeInputs->u8_StripeCount -1))
			u32_BorderCopies = Outputs->u32_RBorderCopies;
	}


	/* Corrreponding C Source Code
	 	hsize_frac = ((*s_scaler_output_hsize - border_copies) * ir_hstep.frac);
		hsize_bulk = ((*s_scaler_output_hsize - border_copies) * ir_hstep.bulk);

		hsize_bulk += (hsize_frac >> 16);
		hsize_frac &= (hsize_frac & 65535);
	*/
	u32_HSizeFrac =  (StripeInternalParams->s16_SScalerOutputHSize - u32_BorderCopies) * u16_HStepFrac;
	u32_HSizeBulk =  (StripeInternalParams->s16_SScalerOutputHSize - u32_BorderCopies) * u16_HStepBulk;

	u32_HSizeBulk += (u32_HSizeFrac >> 16);
	u32_HSizeFrac &= (u32_HSizeFrac & 65535);

	/* Corresponding C Source Code
	 	if(h_scaler_enable) {
			*s_scaler_input_hsize = hsize_bulk + hnsize;
    	if(!do_downscale) ++(*s_scaler_input_hsize); // provide internal RH border for upscale
    	if(!do_downscale && (ir_hstep.bulk == 1) && stripe_index>0) --(*s_gross_hcrop);
		}
		else
		*s_scaler_input_hsize = hsize_bulk;
 	*/
	if(Outputs->e_Flag_ScalarEnable)
	{
		StripeInternalParams->s16_SScalerInputHSize = u32_HSizeBulk + Outputs->u8_NSize;

		if(!u8_DownScaling)
			StripeInternalParams->s16_SScalerInputHSize += 1;

		if(!u8_DownScaling && (u16_HStepBulk == 1) && (StripeInputs->u8_StripeIndex > 0))
			StripeInternalParams->s32_StripeGrossHCrop -= 1;
	}//If H_scaler_enable
	else
		StripeInternalParams->s16_SScalerInputHSize = u32_HSizeBulk ;

	/* Corresponding C Source Code
	 	*s_cropper_output_hsize = *s_scaler_input_hsize + pre_lft_border + pre_rgt_border;
		if((stripe_count < 2) && do_downscale) {
			while((*s_gross_hcrop + *s_cropper_output_hsize) > cropper_output_hsize) {
				--(*s_scaler_input_hsize);
				--(*s_cropper_output_hsize);
			}
	  	}
	 */
	StripeInternalParams->s16_SCropperOutputHSize = StripeInternalParams->s16_SScalerInputHSize + StripeInternalParams->u8_PreLeftBorder + StripeInternalParams->u8_PreRightBorder ;
	if(StripeInputs->u8_StripeCount < 2 && u8_DownScaling){
		while((StripeInternalParams->s32_StripeGrossHCrop + StripeInternalParams->s16_SCropperOutputHSize) > ( Outputs->u16_HCropSize )){
			--(StripeInternalParams->s16_SScalerInputHSize);
			--(StripeInternalParams->s16_SCropperOutputHSize);
		}
	}
	/* Corresponding C Source Code
	 	hcrop_adv_frac = ((*s_system_output_hsize - border_copies - dnstream_adjustment) * ir_hstep.frac);
		hcrop_adv_bulk = ((*s_system_output_hsize - border_copies - dnstream_adjustment) * ir_hstep.bulk);
		hcrop_adv_bulk += (hcrop_adv_frac >> 16);
		hcrop_adv_bulk -= upstream_adjustment;
		hcrop_adv_frac &= 65535;
	*/
	u32_HCropAdvFrac = ((StripeInternalParams->s16_SSystemOutputHSize - u32_BorderCopies - u8_DnStreamAdjust) * u16_HStepFrac);
	u32_HCropAdvBulk = ((StripeInternalParams->s16_SSystemOutputHSize - u32_BorderCopies - u8_DnStreamAdjust) * u16_HStepBulk);
	u32_HCropAdvBulk += (u32_HCropAdvFrac >> 16);
	u32_HCropAdvBulk -= u8_UpStreamAdjust;
	u32_HCropAdvFrac &= 65535;

	/* Corresponding C Source Code
	   ss_hcrop_frac += hcrop_adv_frac;
	   ss_hcrop_bulk += hcrop_adv_bulk;
  	   ss_hcrop_bulk += ss_hcrop_frac >> 16;
	   ss_hcrop_frac &= 65535;
  */
	StripeInternalParams->s32_StripeHCropFrac += u32_HCropAdvFrac;
	StripeInternalParams->s32_StripeHCropBulk += u32_HCropAdvBulk;
	StripeInternalParams->s32_StripeHCropBulk += (StripeInternalParams->s32_StripeHCropFrac >> 16);
	StripeInternalParams->s32_StripeHCropFrac &= 65535;


   /*  Corresponding C Source Code
     	*s_system_output_vsize = scaler_output_vsize;
     	*s_scaler_output_vsize = scaler_output_vsize;
     	*s_scaler_input_vsize = scaler_input_vsize;
     	*s_cropper_output_vsize = cropper_output_vsize;
     	*s_gross_vcrop = gross_vcrop;
		ir_s_vcrop->frac = ss_vcrop_frac;
		ir_s_vcrop->bulk = ss_vcrop_bulk;
    */
	StripeInternalParams->s16_SSystemOutputVSize = Inputs->u16_VScalarOutputSize;
	StripeInternalParams->s16_SScalerOutputVSize = Inputs->u16_VScalarOutputSize;
	StripeInternalParams->s16_SScalerInputVSize  = Outputs->u16_VCropSize;
	StripeInternalParams->s16_SCropperOutputVSize= Outputs->u16_VCropSize;
	StripeInternalParams->s32_StripeGrossVCrop	  =	Outputs->u16_VCropStart;
	StripeInternalParams->s32_IrStripeVCropBulk  = u16_VCropBulk;
	StripeInternalParams->s32_IrStripeVCropFrac  = u16_VCropFrac;

	// final ouput data assignemnt (ie all *arg from initial sgs code)
	//Renaming of output variables in appropriate structure
	StripeOutputs->s16_StripeInCropHStart 		=	(int16_t)StripeInternalParams->s32_StripeGrossHCrop;
	StripeOutputs->s16_StripeInCropHSize  		=	StripeInternalParams->s16_SCropperOutputHSize;
	StripeOutputs->s16_StripeInCropVStart 		=	(int16_t)StripeInternalParams->s32_StripeGrossVCrop;
	StripeOutputs->s16_StripeInCropVSize  		=	StripeInternalParams->s16_SCropperOutputVSize;
	StripeOutputs->s16_StripeOutCropHStart		= 	StripeInternalParams->u8_PostLeftBorder;
	StripeOutputs->s16_StripeOutCropHSize 		= 	StripeInternalParams->s16_SSystemOutputHSize;
	StripeOutputs->s16_StripeOutCropVStart		= 	StripeInternalParams->u8_PostTopBorder;
	StripeOutputs->s16_StripeOutCropVSize 		= 	StripeInternalParams->s16_SSystemOutputVSize;
	StripeOutputs->s16_StripeScalarOutputHSize 	= 	StripeInternalParams->s16_SScalerOutputHSize;
	StripeOutputs->s16_StripeScalarOutputVSize 	= 	StripeInternalParams->s16_SScalerOutputVSize;
	StripeOutputs->s16_SScalerInputHSize 	   	= 	StripeInternalParams->s16_SScalerInputHSize;
	StripeOutputs->s16_SScalerInputVSize 	   	= 	StripeInternalParams->s16_SScalerInputVSize;

#if 0
	//Shift in Start
	if (( StripeOutputs->s16_StripeInCropHStart + StripeOutputs->s16_StripeInCropHSize ) >= Inputs->u16_HCropperInputSize)
	{
		u16_hStartShift = ( StripeOutputs->s16_StripeInCropHStart + StripeOutputs->s16_StripeInCropHSize - Inputs->u16_HCropperInputSize );
	}
	StripeOutputs->s16_StripeInCropHStart-= u16_hStartShift;
#endif

	//Shift in Size
	if (( StripeOutputs->s16_StripeInCropHStart + StripeOutputs->s16_StripeInCropHSize ) >= Inputs->u16_HCropperInputSize)
	{
		u16_HSizeShift = ( StripeOutputs->s16_StripeInCropHStart + StripeOutputs->s16_StripeInCropHSize ) - Inputs->u16_HCropperInputSize;
		StripeOutputs->s16_StripeInCropHSize  -= u16_HSizeShift;
		StripeOutputs->s16_SScalerInputHSize  -= u16_HSizeShift;
		StripeInternalParams->s16_SScalerInputHSize -= u16_HSizeShift;
		StripeInternalParams->s16_SCropperOutputHSize -= u16_HSizeShift;
	}

	g_Scalar_StripeInternalParams[u8_PipeNo].s32_StripeHCropBulk     = StripeInternalParams->s32_StripeHCropBulk;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_StripeHCropFrac     = StripeInternalParams->s32_StripeHCropFrac;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_StripeVCropBulk 	 = StripeInternalParams->s32_StripeVCropBulk;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_StripeVCropFrac 	 = StripeInternalParams->s32_StripeVCropFrac;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_StripeGrossHCrop    = StripeInternalParams->s32_StripeGrossHCrop;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_StripeGrossVCrop    = StripeInternalParams->s32_StripeGrossVCrop;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_IrStripeHCropBulk	  = StripeInternalParams->s32_IrStripeHCropBulk;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_IrStripeHCropFrac   = StripeInternalParams->s32_IrStripeHCropFrac;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_IrStripeVCropBulk	  = StripeInternalParams->s32_IrStripeVCropBulk;
	g_Scalar_StripeInternalParams[u8_PipeNo].s32_IrStripeVCropFrac   = StripeInternalParams->s32_IrStripeVCropFrac;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SSystemOutputHSize  = StripeInternalParams->s16_SSystemOutputHSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SSystemOutputVSize  = StripeInternalParams->s16_SSystemOutputVSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SScalerOutputHSize  = StripeInternalParams->s16_SScalerOutputHSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SScalerOutputVSize  = StripeInternalParams->s16_SScalerOutputVSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SScalerInputHSize   = StripeInternalParams->s16_SScalerInputHSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SScalerInputVSize   = StripeInternalParams->s16_SScalerInputVSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SCropperOutputHSize = StripeInternalParams->s16_SCropperOutputHSize;
	g_Scalar_StripeInternalParams[u8_PipeNo].s16_SCropperOutputVSize = StripeInternalParams->s16_SCropperOutputVSize;

	return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void StripeLineLength	   (uint8_t u8_DownScaling,
								Scalar_Inputs_ts *Inputs,
								Scalar_Stripe_OutputParams_ts *StripeOutputs,
								Scalar_StripeInternalParams_ts *StripeInternalParams,
								uint8_t PipeNo)

\brief
 \return 	void
 \param 	u8_DownScaling											: 	Variable to indicate whether downscaling or not
 \param 	Scalar_Inputs_ts									:	Pointer to Scalar Input parameter structure
 \param 	Scalar_Stripe_OutputParams_ts *StripeOutputs		:	Pointer to Stripe Output parameter structure
 \param 	Scalar_StripeInternalParams_ts *StripeInternalParams:   Pointer to Stripe internal parameter structure
 \param 	u8_PipeNo											:   Pipe Number (0 or 1)

 \callgraph
 \callergraph
 \ingroup Scalar
 \endif
*/
void StripeLineLength	   (uint8_t u8_DownScaling,
							Scalar_Inputs_ts *Inputs,
							Scalar_Stripe_OutputParams_ts *StripeOutputs,
							Scalar_StripeInternalParams_ts *StripeInternalParams,
							uint8_t u8_PipeNo)
{

	float_t		f_PipeStep;
	float_t		f_InvStepSize;
	float_t		f_PipeLineLength;
	float_t		f_PreScaleFactor;

	f_PipeStep = Inputs->f_Step;

	// make a request to set the line length if needed
	f_PreScaleFactor = Scalar_GetPendingPreScaleFactor();

	f_InvStepSize = f_PreScaleFactor/f_PipeStep;

	// min line length required for upscale = (g_PipeState.u16_OutputSizeX + min_line_blanking_beyond_scalar) * f_InvStepSize
	// min line length required for downscale = (g_PipeState.u16_OutputSizeX / f_InvStepSize) + min_line_blanking_beyond_scalar

	if (f_InvStepSize > 1.0)
	{
		if(0 == u8_PipeNo){
			// upscaling
			f_PipeLineLength = (StripeOutputs->s16_StripeScalarOutputHSize + Scalar_GetMinimumLineBlankingBeyondGPS0()) * Zoom_Ceiling(f_InvStepSize);
		}
		else {
			// upscaling
			f_PipeLineLength = (StripeOutputs->s16_StripeScalarOutputHSize + Scalar_GetMinimumLineBlankingBeyondGPS1()) * Zoom_Ceiling(f_InvStepSize);
		}

	}
	else
	{
		if(0 == u8_PipeNo){
			// downscaling
			f_PipeLineLength = (((float_t)StripeOutputs->s16_StripeScalarOutputHSize)/f_InvStepSize) + Scalar_GetMinimumLineBlankingBeyondGPS0();
		}
		else {
			// downscaling
			f_PipeLineLength = (((float_t)StripeOutputs->s16_StripeScalarOutputHSize)/f_InvStepSize) + Scalar_GetMinimumLineBlankingBeyondGPS1();
		}
	}


	StripeOutputs->s16_MinLineLength = Scalar_RoundUp(f_PipeLineLength);
	return ;
}

