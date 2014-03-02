/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   volctrl_powtab.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
Float const EXTERN coef_volctrl=FORMAT_FLOAT(0.921034040063792,MAXVAL); 
#define Exp_coef 11
#define MAXVAL_PLUS_ONE_OVER2 0x400000
#define SHIFTVAL_INV 20
#define numiter 8
Float const EXTERN volctrl_invtab[4] = {
	FORMAT_FLOAT(1.000000000000000,MAXVAL),  		// 0
	FORMAT_FLOAT(0.800000047683721,MAXVAL),  		// 1
	FORMAT_FLOAT(0.666666706403101,MAXVAL),  		// 2
	FORMAT_FLOAT(0.571428605488372,MAXVAL) };  		// 3
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl_include.h
 * \brief  
 * \author ST-Ericsson
 */