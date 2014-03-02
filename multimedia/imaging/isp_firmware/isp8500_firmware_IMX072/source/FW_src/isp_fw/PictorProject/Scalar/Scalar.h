/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef SCALAR_
#define SCALAR_

/**
 \file 	Scalar.h
 \brief	This file is a part of the scalar module release code.
 		It contains definition of APIs and objects exported WITHIN the Scalar module.
 \ingroup Scalar
*/

#define NMIN					2						/* min filter length */
#define NMIN_AZ					3 						/* min filter length in anti-zipper mode */
#define LOGFTSIZE				16						/* address width of virtual polyphase table (i.e. inc. fines) */
#define FW_FP_UNITY 			64.0					/* 1.0 ref FP f/w cosine arg space */
#define AZ_SAFEZONE 			1.5						/* maximum inverse cutoff when in A-Z mode */
#define SCALED_INV_AZ_SAFEZONE	(64.0 / AZ_SAFEZONE)	/* scaled maximum cutoff when in A-Z mode */
#define SCALED_UNITY 			64.0					/* unity in cutoff context */
#define FW_FP_AHALF 			32.0					/* 0.5 ref FP f/w cosine arg space */

#define FW_CLUT_WIDTH 			14						/* width (in bits) of f/w cosine LUT */
#define FW_CLUT_UNITY 			(1 << (FW_CLUT_WIDTH))	/* unity in f/w cosine LUT */
#define FW_CLUT_AHALF 			(FW_CLUT_UNITY>>1)		/* half-unity in f/w cosine LUT */

#define FW_FP_TWO 				128.0					/* 2.0 ref FP f/w cosine arg space */
#define PI						3.141593
#define NOT_BORDERING			-1

/**
 \if INCLUDE_IN_HTML_ONLY
 \struct Scalar_InternalParams_ts
 \brief Internal parameters used by the Scalar module.
 \ingroup Scalar
 \endif
*/

typedef struct
{
  /// Overhang control
  uint8_t  u8_Overhang;
  
  /// Indicates a no (up/down)scale setup
  uint8_t	e_Flag_Transparent;
  
} Scalar_InternalParams_ts;

typedef struct {
	
	//ss_hcrop_bulk
	int32_t s32_StripeHCropBulk;
	
	//ss_hcrop_frac
	int32_t s32_StripeHCropFrac;
	
	//ss_vcrop_bulk
	int32_t s32_StripeVCropBulk;
	
	//ss_vcrop_frac
	int32_t s32_StripeVCropFrac;
	  
	//s_gross_hcrop
	int32_t s32_StripeGrossHCrop;
	  
	//s_gross_vcrop
	int32_t s32_StripeGrossVCrop;
	  
	//ir_s_hcrop_bulk
	int32_t s32_IrStripeHCropBulk;

	//ir_s_hcrop_frac
	int32_t s32_IrStripeHCropFrac;
	    
	//ir_s_vcrop_bulk
	int32_t s32_IrStripeVCropBulk;

	//ir_s_vcrop_frac
	int32_t s32_IrStripeVCropFrac;
	  
	//s_system_output_hsize
	int16_t s16_SSystemOutputHSize;

	//s_system_output_vsize
	int16_t s16_SSystemOutputVSize;  
	    
	//s_scaler_output_hsize
	int16_t s16_SScalerOutputHSize;

	//s_scaler_output_vsize
	int16_t s16_SScalerOutputVSize;
	    
	//s_scaler_input_hsize
	int16_t s16_SScalerInputHSize;

	//s_scaler_input_vsize
	int16_t s16_SScalerInputVSize;
	    
	//s_cropper_output_hsize
	int16_t s16_SCropperOutputHSize;

	//s_cropper_output_vsize
	int16_t s16_SCropperOutputVSize;
	
	//Pre Top Border of Stripe
	uint8_t u8_PreTopBorder;
	
	//Pre Bottom Border of Stripe
	uint8_t u8_PreBottomBorder;
  
	//Pre Left Border of Stripe
	uint8_t u8_PreLeftBorder;
  
	//Pre Right Border of Stripe
	uint8_t u8_PreRightBorder;
  
	//Post Top Border of Stripe
	uint8_t u8_PostTopBorder;
    
	//Post Bottom Border of Stripe
	uint8_t u8_PostBottomBorder;
    
	//Post Left Border of Stripe
	uint8_t u8_PostLeftBorder;
    
	//Post Right Border of Stripe
	uint8_t u8_PostRightBorder;
  
    
}Scalar_StripeInternalParams_ts;



#endif// SCALAR_
